#include "esp/runtime.hpp"
#include "esp/ciberseguridad.hpp"
#include "esp/codegen.hpp"
#include "esp/graficos.hpp"
#include "esp/os_mod.hpp"
#include "esp/redes_ip.hpp"
#include "esp/regex_mod.hpp"
#include "esp/realtime.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <mutex>
#include <random>
#include <set>
#include <sstream>
#include <thread>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winhttp.lib")
#else
#include <arpa/inet.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace esp {
namespace {

Valor nativa_modulo(std::function<Valor(std::vector<Valor>&, Ubicacion)> f, const std::string& nombre) {
    auto fn = std::make_shared<Funcion>();
    fn->nombre = nombre;
    fn->es_nativa = true;
    fn->nativa = std::move(f);
    Valor v;
    v.tipo = TipoValor::Funcion;
    v.funcion = fn;
    return v;
}

void fn(Entorno& e, const std::string& n, std::function<Valor(std::vector<Valor>&, Ubicacion)> f) {
    e.definir(n, nativa_modulo(std::move(f), n), Visibilidad::Publico, true, true);
}

std::shared_ptr<Modulo> crear(const std::string& nombre) {
    auto m = std::make_shared<Modulo>();
    m->nombre = nombre;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();
    return m;
}

#ifdef _WIN32
using SocketNativo = SOCKET;
constexpr SocketNativo SOCKET_INVALIDO = INVALID_SOCKET;
void asegurar_red() {
    static bool ok = false;
    if (!ok) {
        WSADATA w;
        WSAStartup(MAKEWORD(2, 2), &w);
        ok = true;
    }
}
void cerrar_socket(SocketNativo s) { closesocket(s); }
#else
using SocketNativo = int;
constexpr SocketNativo SOCKET_INVALIDO = -1;
void asegurar_red() {}
void cerrar_socket(SocketNativo s) { close(s); }
#endif

std::string base64_codificar(const std::string& in) {
    static const char* tabla = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(tabla[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(tabla[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::string base64_decodificar(const std::string& in) {
    std::array<int, 256> T{};
    T.fill(-1);
    const std::string tabla = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < 64; ++i) T[static_cast<unsigned char>(tabla[i])] = i;
    std::string out;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

std::string escapar_html(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '&') out += "&amp;";
        else if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else if (c == '"') out += "&quot;";
        else out.push_back(c);
    }
    return out;
}

// ============================================================================
// MOTOR SQL EMBEBIDO CON SINTAXIS EN ESPAÑOL E INGLÉS
// ============================================================================

struct Tabla {
    std::vector<std::string> columnas;
    std::vector<std::vector<std::string>> filas;
    std::map<std::string, std::map<std::string, std::vector<std::size_t>>> indices;
};

struct BaseSQL {
    std::map<std::string, Tabla> tablas;
    std::map<std::string, Tabla> snapshot_tx;
    bool en_transaccion = false;
    std::string ruta;
};

std::mutex sql_mutex;
std::map<std::string, std::shared_ptr<BaseSQL>> bases;

std::vector<std::string> partir(const std::string& s, char c) {
    std::vector<std::string> r;
    std::string a;
    bool q = false;
    for (char ch : s) {
        if (ch == '\'') q = !q;
        if (ch == c && !q) {
            if (!a.empty()) r.push_back(a);
            a.clear();
        } else {
            a += ch;
        }
    }
    if (!a.empty()) r.push_back(a);
    return r;
}

std::string recortar(std::string s) {
    auto a = s.find_first_not_of(" \t\r\n");
    auto b = s.find_last_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

std::string mayusculas(std::string s) {
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::string quitar_comillas_sql(std::string s) {
    s = recortar(std::move(s));
    if (s.size() >= 2 && ((s.front() == '\'' && s.back() == '\'') || (s.front() == '"' && s.back() == '"'))) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

std::string normalizar_sql_espanol(const std::string& sql_raw) {
    std::string res;
    res.reserve(sql_raw.size() + 32);
    std::size_t i = 0;
    const std::size_t n = sql_raw.size();
    while (i < n) {
        char c = sql_raw[i];
        if (c == '\'' || c == '"') {
            char q = c;
            res.push_back(sql_raw[i++]);
            while (i < n && sql_raw[i] != q) {
                if (sql_raw[i] == '\\' && i + 1 < n) res.push_back(sql_raw[i++]);
                res.push_back(sql_raw[i++]);
            }
            if (i < n) res.push_back(sql_raw[i++]);
            continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            std::size_t ini = i;
            while (i < n && (std::isalnum(static_cast<unsigned char>(sql_raw[i])) || sql_raw[i] == '_')) ++i;
            std::string tok = sql_raw.substr(ini, i - ini);
            std::string up = mayusculas(tok);
            if (up == "CREAR") res += "CREATE";
            else if (up == "TABLA") res += "TABLE";
            else if (up == "INDICE") res += "INDEX";
            else if (up == "ELIMINAR") res += "DELETE";
            else if (up == "BORRAR") res += "DROP";
            else if (up == "INSERTAR") res += "INSERT";
            else if (up == "EN") res += "INTO";
            else if (up == "VALORES") res += "VALUES";
            else if (up == "SELECCIONAR") res += "SELECT";
            else if (up == "DE" || up == "DESDE") res += "FROM";
            else if (up == "DONDE") res += "WHERE";
            else if (up == "ACTUALIZAR") res += "UPDATE";
            else if (up == "PONER" || up == "ESTABLECER") res += "SET";
            else if (up == "ORDENAR") res += "ORDER";
            else if (up == "POR") res += "BY";
            else if (up == "LIMITE") res += "LIMIT";
            else if (up == "COMO") res += "LIKE";
            else if (up == "INICIAR") res += "BEGIN";
            else if (up == "TRANSACCION") res += "TRANSACTION";
            else if (up == "CONFIRMAR") res += "COMMIT";
            else if (up == "REVERTIR") res += "ROLLBACK";
            else if (up == "SI") res += "IF";
            else if (up == "NO") res += "NOT";
            else if (up == "EXISTE") res += "EXISTS";
            else res += tok;
            continue;
        }
        res.push_back(sql_raw[i++]);
    }
    return res;
}

void reconstruir_indices(Tabla& t) {
    for (auto& kv : t.indices) {
        const std::string& col = kv.first;
        kv.second.clear();
        auto it_col = std::find(t.columnas.begin(), t.columnas.end(), col);
        if (it_col == t.columnas.end()) continue;
        std::size_t idx = static_cast<std::size_t>(std::distance(t.columnas.begin(), it_col));
        for (std::size_t r = 0; r < t.filas.size(); ++r) {
            if (idx < t.filas[r].size()) kv.second[t.filas[r][idx]].push_back(r);
        }
    }
}

void guardar_base_sql(const BaseSQL& db) {
    if (db.ruta.empty() || db.ruta == ":memoria:") return;
    std::ostringstream out;
    out << "ESPSQL1\n";
    for (const auto& kv : db.tablas) {
        const auto& nombre = kv.first;
        const auto& t = kv.second;
        out << "T\t" << nombre << "\t" << t.columnas.size();
        for (const auto& c : t.columnas) out << "\t" << c;
        out << "\n";
        for (const auto& fila : t.filas) {
            out << "R";
            for (const auto& celda : fila) {
                std::string esc;
                for (char ch : celda) {
                    if (ch == '\\') esc += "\\\\";
                    else if (ch == '\t') esc += "\\t";
                    else if (ch == '\n') esc += "\\n";
                    else if (ch == '\r') esc += "\\r";
                    else esc += ch;
                }
                out << "\t" << esc;
            }
            out << "\n";
        }
    }
    escribir_archivo(db.ruta, out.str());
}

void cargar_base_sql(BaseSQL& db) {
    if (db.ruta.empty() || db.ruta == ":memoria:") return;
    std::error_code ec;
    if (!std::filesystem::is_regular_file(db.ruta, ec) || ec) return;
    std::string contenido = leer_archivo(db.ruta);
    std::istringstream in(contenido);
    std::string linea;
    if (!std::getline(in, linea) || recortar(linea) != "ESPSQL1") return;
    Tabla* actual = nullptr;
    while (std::getline(in, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;
        std::vector<std::string> partes;
        std::size_t inicio = 0;
        while (true) {
            std::size_t tab = linea.find('\t', inicio);
            if (tab == std::string::npos) {
                partes.push_back(linea.substr(inicio));
                break;
            }
            partes.push_back(linea.substr(inicio, tab - inicio));
            inicio = tab + 1;
        }
        if (partes[0] == "T" && partes.size() >= 3) {
            Tabla t;
            for (std::size_t i = 3; i < partes.size(); ++i) t.columnas.push_back(partes[i]);
            db.tablas[partes[1]] = std::move(t);
            actual = &db.tablas[partes[1]];
        } else if (partes[0] == "R" && actual != nullptr) {
            std::vector<std::string> fila;
            for (std::size_t i = 1; i < partes.size(); ++i) {
                std::string val;
                for (std::size_t k = 0; k < partes[i].size(); ++k) {
                    if (partes[i][k] == '\\' && k + 1 < partes[i].size()) {
                        char nx = partes[i][++k];
                        if (nx == 't') val += '\t';
                        else if (nx == 'n') val += '\n';
                        else if (nx == 'r') val += '\r';
                        else val += nx;
                    } else {
                        val += partes[i][k];
                    }
                }
                fila.push_back(std::move(val));
            }
            actual->filas.push_back(std::move(fila));
        }
    }
}

struct CondicionWhere {
    bool activa = false;
    std::string columna;
    std::string op;
    std::string valor;
};

CondicionWhere parsear_where(const std::string& clausula) {
    CondicionWhere w;
    std::string c = recortar(clausula);
    if (c.empty()) return w;
    const char* ops[] = {"!=", "<=", ">=", "=", "<", ">"};
    for (const char* op : ops) {
        std::size_t pos = c.find(op);
        if (pos != std::string::npos) {
            w.activa = true;
            w.columna = recortar(c.substr(0, pos));
            w.op = op;
            w.valor = quitar_comillas_sql(c.substr(pos + std::strlen(op)));
            return w;
        }
    }
    std::string up = mayusculas(c);
    std::size_t like_pos = up.find(" LIKE ");
    if (like_pos != std::string::npos) {
        w.activa = true;
        w.columna = recortar(c.substr(0, like_pos));
        w.op = "LIKE";
        w.valor = quitar_comillas_sql(c.substr(like_pos + 6));
    }
    return w;
}

bool evaluar_where(const Tabla& t, const std::vector<std::string>& fila, const CondicionWhere& w) {
    if (!w.activa) return true;
    auto it = std::find(t.columnas.begin(), t.columnas.end(), w.columna);
    if (it == t.columnas.end()) throw ErrorEjecucion("columna no encontrada en WHERE/DONDE: " + w.columna);
    std::size_t idx = static_cast<std::size_t>(std::distance(t.columnas.begin(), it));
    const std::string& actual = idx < fila.size() ? fila[idx] : "";

    auto es_num = [](const std::string& s, double& out) -> bool {
        if (s.empty()) return false;
        char* end = nullptr;
        out = std::strtod(s.c_str(), &end);
        return end != s.c_str() && *end == '\0';
    };

    double num_a = 0, num_b = 0;
    bool ambos_num = es_num(actual, num_a) && es_num(w.valor, num_b);

    if (w.op == "=") return ambos_num ? (num_a == num_b) : (actual == w.valor);
    if (w.op == "!=") return ambos_num ? (num_a != num_b) : (actual != w.valor);
    if (w.op == "<") return ambos_num ? (num_a < num_b) : (actual < w.valor);
    if (w.op == ">") return ambos_num ? (num_a > num_b) : (actual > w.valor);
    if (w.op == "<=") return ambos_num ? (num_a <= num_b) : (actual <= w.valor);
    if (w.op == ">=") return ambos_num ? (num_a >= num_b) : (actual >= w.valor);
    if (w.op == "LIKE") {
        std::string pat = w.valor;
        if (pat.size() >= 2 && pat.front() == '%' && pat.back() == '%') {
            return actual.find(pat.substr(1, pat.size() - 2)) != std::string::npos;
        }
        if (!pat.empty() && pat.back() == '%') {
            std::string pref = pat.substr(0, pat.size() - 1);
            return actual.compare(0, pref.size(), pref) == 0;
        }
        return actual == pat;
    }
    return false;
}

std::string aplicar_params(std::string sql, const std::vector<Valor>& params) {
    std::size_t p = 0;
    std::string out;
    for (char c : sql) {
        if (c == '?' && p < params.size()) {
            auto v = params[p++];
            if (v.tipo == TipoValor::Texto) {
                std::string t = v.texto;
                for (char& x : t)
                    if (x == '\'') x = '`';
                out += "'" + t + "'";
            } else if (v.tipo == TipoValor::Nulo) {
                out += "NULL";
            } else {
                out += v.a_texto();
            }
        } else {
            out += c;
        }
    }
    return out;
}

Valor sql_ejecutar(std::shared_ptr<BaseSQL> db, std::string sql_entrada) {
    std::string sql = normalizar_sql_espanol(recortar(sql_entrada));
    while (!sql.empty() && sql.back() == ';') sql.pop_back();
    sql = recortar(sql);
    std::string up = mayusculas(sql);

    if (up == "BEGIN" || up == "BEGIN TRANSACTION") {
        if (db->en_transaccion) throw ErrorEjecucion("ya existe una transaccion SQL activa");
        db->snapshot_tx = db->tablas;
        db->en_transaccion = true;
        return Valor::de_bool(true);
    }
    if (up == "COMMIT") {
        if (!db->en_transaccion) throw ErrorEjecucion("no hay transaccion SQL activa para confirmar");
        db->snapshot_tx.clear();
        db->en_transaccion = false;
        guardar_base_sql(*db);
        return Valor::de_bool(true);
    }
    if (up == "ROLLBACK") {
        if (!db->en_transaccion) throw ErrorEjecucion("no hay transaccion SQL activa para revertir");
        db->tablas = std::move(db->snapshot_tx);
        db->en_transaccion = false;
        return Valor::de_bool(true);
    }

    if (up.rfind("CREATE TABLE", 0) == 0) {
        auto par = sql.find('(');
        if (par == std::string::npos) throw ErrorEjecucion("CREAR TABLA requiere lista de columnas");
        auto nombre = recortar(sql.substr(12, par - 12));
        if (mayusculas(nombre).rfind("IF NOT EXISTS ", 0) == 0) {
            nombre = recortar(nombre.substr(14));
            if (db->tablas.count(nombre)) return Valor::de_bool(true);
        }
        for (char& c : nombre)
            if (c == '"' || c == '`') c = 0;
        nombre.erase(std::remove(nombre.begin(), nombre.end(), '\0'), nombre.end());
        auto cols_s = sql.substr(par + 1, sql.rfind(')') - par - 1);
        Tabla t;
        auto partes = partir(cols_s, ',');
        for (auto& p : partes) {
            auto bits = partir(recortar(p), ' ');
            if (!bits.empty()) t.columnas.push_back(recortar(bits[0]));
        }
        db->tablas[nombre] = std::move(t);
        if (!db->en_transaccion) guardar_base_sql(*db);
        return Valor::de_bool(true);
    }

    if (up.rfind("DROP TABLE", 0) == 0 || up.rfind("DELETE TABLE", 0) == 0) {
        std::size_t off = (up.rfind("DROP TABLE", 0) == 0) ? 10 : 12;
        std::string nombre = recortar(sql.substr(off));
        if (mayusculas(nombre).rfind("IF EXISTS ", 0) == 0) nombre = recortar(nombre.substr(10));
        db->tablas.erase(nombre);
        if (!db->en_transaccion) guardar_base_sql(*db);
        return Valor::de_bool(true);
    }

    if (up.rfind("CREATE INDEX", 0) == 0) {
        auto on_pos = up.find(" ON ");
        if (on_pos == std::string::npos) on_pos = up.find(" INTO ");
        auto par = sql.find('(', on_pos == std::string::npos ? 0 : on_pos);
        if (on_pos == std::string::npos || par == std::string::npos)
            throw ErrorEjecucion("sintaxis esperada: CREAR INDICE idx EN tabla(columna)");
        std::size_t kw_len = (up.find(" ON ") != std::string::npos) ? 4 : 6;
        std::string tabla_nombre = recortar(sql.substr(on_pos + kw_len, par - (on_pos + kw_len)));
        std::string col_nombre = recortar(sql.substr(par + 1, sql.rfind(')') - par - 1));
        auto it = db->tablas.find(tabla_nombre);
        if (it == db->tablas.end()) throw ErrorEjecucion("tabla no existe: " + tabla_nombre);
        it->second.indices[col_nombre] = {};
        reconstruir_indices(it->second);
        return Valor::de_bool(true);
    }

    if (up.rfind("INSERT", 0) == 0) {
        auto into = up.find("INTO");
        auto par = sql.find('(');
        if (into == std::string::npos || par == std::string::npos)
            throw ErrorEjecucion("INSERTAR EN requiere tabla(columnas) VALORES(...)");
        auto nombre = recortar(sql.substr(into + 4, par - (into + 4)));
        auto it_tabla = db->tablas.find(nombre);
        if (it_tabla == db->tablas.end()) throw ErrorEjecucion("tabla no existe: " + nombre);

        auto cols_str = sql.substr(par + 1, sql.find(')', par) - par - 1);
        auto cols_insert = partir(cols_str, ',');
        for (auto& c : cols_insert) c = recortar(c);

        auto vals_pos = up.find("VALUES");
        if (vals_pos == std::string::npos) throw ErrorEjecucion("INSERTAR requiere VALORES");
        auto vp = sql.find('(', vals_pos);
        auto vs = sql.substr(vp + 1, sql.rfind(')') - vp - 1);
        auto valores = partir(vs, ',');

        auto& t = it_tabla->second;
        std::vector<std::string> fila(t.columnas.size());
        for (std::size_t i = 0; i < cols_insert.size() && i < valores.size(); ++i) {
            auto it_col = std::find(t.columnas.begin(), t.columnas.end(), cols_insert[i]);
            std::size_t idx = (it_col != t.columnas.end())
                                  ? static_cast<std::size_t>(std::distance(t.columnas.begin(), it_col))
                                  : i;
            if (idx < fila.size()) fila[idx] = quitar_comillas_sql(valores[i]);
        }
        t.filas.push_back(std::move(fila));
        reconstruir_indices(t);
        if (!db->en_transaccion) guardar_base_sql(*db);
        return Valor::de_entero(1);
    }

    if (up.rfind("SELECT", 0) == 0) {
        auto from = up.find(" FROM ");
        if (from == std::string::npos) throw ErrorEjecucion("SELECCIONAR sin DE/FROM");
        std::string proj_str = recortar(sql.substr(6, from - 6));
        std::string rest = recortar(sql.substr(from + 6));
        std::string up_rest = mayusculas(rest);

        std::size_t where_pos = up_rest.find(" WHERE ");
        std::size_t order_pos = up_rest.find(" ORDER BY ");
        std::size_t limit_pos = up_rest.find(" LIMIT ");

        std::size_t fin_tabla = std::min({where_pos, order_pos, limit_pos, rest.size()});
        std::string nombre = recortar(rest.substr(0, fin_tabla));
        auto it = db->tablas.find(nombre);
        if (it == db->tablas.end()) throw ErrorEjecucion("tabla no existe: " + nombre);
        const Tabla& t = it->second;

        CondicionWhere where;
        if (where_pos != std::string::npos) {
            std::size_t fin_where = std::min({order_pos, limit_pos, rest.size()});
            where = parsear_where(rest.substr(where_pos + 7, fin_where - (where_pos + 7)));
        }

        std::vector<std::vector<std::string>> seleccionadas;
        for (const auto& fila : t.filas) {
            if (evaluar_where(t, fila, where)) seleccionadas.push_back(fila);
        }

        if (order_pos != std::string::npos) {
            std::size_t fin_order = std::min({limit_pos, rest.size()});
            std::string ord_str = recortar(rest.substr(order_pos + 10, fin_order - (order_pos + 10)));
            bool desc = false;
            std::string up_ord = mayusculas(ord_str);
            if (up_ord.size() > 5 && up_ord.substr(up_ord.size() - 5) == " DESC") {
                desc = true;
                ord_str = recortar(ord_str.substr(0, ord_str.size() - 5));
            } else if (up_ord.size() > 4 && up_ord.substr(up_ord.size() - 4) == " ASC") {
                ord_str = recortar(ord_str.substr(0, ord_str.size() - 4));
            }
            auto it_col = std::find(t.columnas.begin(), t.columnas.end(), ord_str);
            if (it_col != t.columnas.end()) {
                std::size_t idx = static_cast<std::size_t>(std::distance(t.columnas.begin(), it_col));
                std::sort(seleccionadas.begin(), seleccionadas.end(),
                          [idx, desc](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                              const std::string& va = idx < a.size() ? a[idx] : "";
                              const std::string& vb = idx < b.size() ? b[idx] : "";
                              return desc ? (va > vb) : (va < vb);
                          });
            }
        }

        if (limit_pos != std::string::npos) {
            std::string lim_str = recortar(rest.substr(limit_pos + 7));
            std::size_t lim = static_cast<std::size_t>(std::stoull(lim_str));
            if (seleccionadas.size() > lim) seleccionadas.resize(lim);
        }

        std::vector<std::string> cols_salida = t.columnas;
        if (proj_str != "*") {
            cols_salida.clear();
            for (auto& c : partir(proj_str, ',')) cols_salida.push_back(recortar(c));
        }

        std::vector<Valor> filas;
        for (const auto& fila : seleccionadas) {
            std::map<std::string, Valor> m;
            for (const auto& col_name : cols_salida) {
                auto it_c = std::find(t.columnas.begin(), t.columnas.end(), col_name);
                if (it_c != t.columnas.end()) {
                    std::size_t idx = static_cast<std::size_t>(std::distance(t.columnas.begin(), it_c));
                    m[col_name] = Valor::de_texto(idx < fila.size() ? fila[idx] : "");
                }
            }
            filas.push_back(Valor::de_mapa(std::move(m)));
        }
        return Valor::de_lista(std::move(filas));
    }

    if (up.rfind("UPDATE", 0) == 0) {
        auto set_pos = up.find(" SET ");
        if (set_pos == std::string::npos) throw ErrorEjecucion("ACTUALIZAR requiere PONER/SET");
        std::string nombre = recortar(sql.substr(6, set_pos - 6));
        auto it_tabla = db->tablas.find(nombre);
        if (it_tabla == db->tablas.end()) throw ErrorEjecucion("tabla no existe: " + nombre);
        Tabla& t = it_tabla->second;

        std::string rest = sql.substr(set_pos + 5);
        std::string up_rest = mayusculas(rest);
        auto where_pos = up_rest.find(" WHERE ");
        std::string set_clause = where_pos == std::string::npos ? rest : rest.substr(0, where_pos);
        CondicionWhere where = where_pos == std::string::npos ? CondicionWhere{} : parsear_where(rest.substr(where_pos + 7));

        std::vector<std::pair<std::size_t, std::string>> asignaciones;
        for (auto& asig : partir(set_clause, ',')) {
            auto eq = asig.find('=');
            if (eq == std::string::npos) throw ErrorEjecucion("asignacion invalida en ACTUALIZAR: " + asig);
            std::string col = recortar(asig.substr(0, eq));
            std::string val = quitar_comillas_sql(asig.substr(eq + 1));
            auto it_c = std::find(t.columnas.begin(), t.columnas.end(), col);
            if (it_c == t.columnas.end()) throw ErrorEjecucion("columna no existe: " + col);
            asignaciones.emplace_back(static_cast<std::size_t>(std::distance(t.columnas.begin(), it_c)), val);
        }

        std::int64_t afectadas = 0;
        for (auto& fila : t.filas) {
            if (evaluar_where(t, fila, where)) {
                for (const auto& kv : asignaciones) {
                    if (kv.first < fila.size()) fila[kv.first] = kv.second;
                }
                ++afectadas;
            }
        }
        reconstruir_indices(t);
        if (!db->en_transaccion) guardar_base_sql(*db);
        return Valor::de_entero(afectadas);
    }

    if (up.rfind("DELETE", 0) == 0) {
        auto from = up.find("FROM");
        if (from == std::string::npos) throw ErrorEjecucion("ELIMINAR sin DE/FROM");
        std::string rest = recortar(sql.substr(from + 4));
        std::string up_rest = mayusculas(rest);
        auto where_pos = up_rest.find(" WHERE ");
        std::string nombre = recortar(where_pos == std::string::npos ? rest : rest.substr(0, where_pos));
        auto it_tabla = db->tablas.find(nombre);
        if (it_tabla == db->tablas.end()) throw ErrorEjecucion("tabla no existe: " + nombre);
        Tabla& t = it_tabla->second;

        if (where_pos == std::string::npos) {
            std::int64_t total = static_cast<std::int64_t>(t.filas.size());
            t.filas.clear();
            reconstruir_indices(t);
            if (!db->en_transaccion) guardar_base_sql(*db);
            return Valor::de_entero(total);
        }
        CondicionWhere where = parsear_where(rest.substr(where_pos + 7));
        std::int64_t eliminadas = 0;
        std::vector<std::vector<std::string>> restantes;
        for (auto& fila : t.filas) {
            if (evaluar_where(t, fila, where)) ++eliminadas;
            else restantes.push_back(std::move(fila));
        }
        t.filas = std::move(restantes);
        reconstruir_indices(t);
        if (!db->en_transaccion) guardar_base_sql(*db);
        return Valor::de_entero(eliminadas);
    }

    throw ErrorEjecucion("consulta SQL no soportada: " + sql_entrada);
}

Valor ejecutar_peticion_http_nativa(const std::string& metodo, const std::string& url,
                                    const std::string& cuerpo_req,
                                    const std::map<std::string, std::string>& cabeceras,
                                    Ubicacion u) {
#ifdef _WIN32
    std::wstring wurl(url.begin(), url.end());
    URL_COMPONENTS uc{};
    uc.dwStructSize = sizeof(uc);
    wchar_t host[256], path[2048];
    uc.lpszHostName = host;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = 2048;
    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc)) throw ErrorEjecucion("URL HTTP no valida: " + url, u);

    HINTERNET s = WinHttpOpen(L"ESP/2.0.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, nullptr, nullptr, 0);
    if (!s) throw ErrorEjecucion("no se pudo iniciar sesion WinHTTP", u);
    HINTERNET c = WinHttpConnect(s, host, uc.nPort, 0);
    std::wstring wmetodo(metodo.begin(), metodo.end());
    HINTERNET r = WinHttpOpenRequest(c, wmetodo.c_str(), path, nullptr, nullptr, nullptr,
                                     uc.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);

    std::wstring wheaders;
    for (const auto& kv : cabeceras) {
        std::string hline = kv.first + ": " + kv.second + "\r\n";
        wheaders += std::wstring(hline.begin(), hline.end());
    }
    if (!wheaders.empty()) {
        WinHttpAddRequestHeaders(r, wheaders.c_str(), static_cast<DWORD>(wheaders.size()), WINHTTP_ADDREQ_FLAG_ADD);
    }

    WinHttpSendRequest(r, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                       cuerpo_req.empty() ? WINHTTP_NO_REQUEST_DATA : const_cast<char*>(cuerpo_req.data()),
                       static_cast<DWORD>(cuerpo_req.size()), static_cast<DWORD>(cuerpo_req.size()), 0);
    WinHttpReceiveResponse(r, nullptr);

    DWORD status_code = 0;
    DWORD status_sz = sizeof(status_code);
    WinHttpQueryHeaders(r, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &status_code, &status_sz, WINHTTP_NO_HEADER_INDEX);

    std::string body;
    DWORD n = 0;
    do {
        DWORD sz = 0;
        WinHttpQueryDataAvailable(r, &sz);
        if (!sz) break;
        std::string buf(sz, 0);
        WinHttpReadData(r, buf.data(), sz, &n);
        body.append(buf.data(), n);
    } while (n);

    WinHttpCloseHandle(r);
    WinHttpCloseHandle(c);
    WinHttpCloseHandle(s);

    return Valor::de_mapa({
        {"estado", Valor::de_entero(static_cast<std::int64_t>(status_code))},
        {"ok", Valor::de_bool(status_code >= 200 && status_code < 300)},
        {"cuerpo", Valor::de_texto(std::move(body))}
    });
#else
    std::string cmd = "curl --silent --show-error --location -X " + citar_argumento_proceso(metodo);
    for (const auto& kv : cabeceras) {
        cmd += " -H " + citar_argumento_proceso(kv.first + ": " + kv.second);
    }
    if (!cuerpo_req.empty()) {
        cmd += " --data " + citar_argumento_proceso(cuerpo_req);
    }
    cmd += " -w \"\\n%{http_code}\" " + citar_argumento_proceso(url);
    std::string out, err;
    int code = ejecutar_proceso(cmd, "", &out, &err, 35000);
    if (code != 0) throw ErrorEjecucion("peticion HTTP fallo: " + err, u);
    int http_code = 200;
    auto nl = out.rfind('\n');
    if (nl != std::string::npos) {
        http_code = std::atoi(out.substr(nl + 1).c_str());
        out = out.substr(0, nl);
    }
    return Valor::de_mapa({
        {"estado", Valor::de_entero(http_code)},
        {"ok", Valor::de_bool(http_code >= 200 && http_code < 300)},
        {"cuerpo", Valor::de_texto(std::move(out))}
    });
#endif
}

}  // namespace

std::shared_ptr<Modulo> Interprete::cargar_modulo_estandar(const std::string& nombre) {
    // --- MODULOS EXTERNOS DEDICADOS ---
    if (nombre == "re" || nombre == "regex") return crear_modulo_re(nombre);
    if (nombre == "os" || nombre == "kernel") return crear_modulo_os(nombre);
    if (nombre == "binario" || nombre == "struct") return crear_modulo_binario(nombre);
    if (nombre == "redes.ip") return crear_modulo_redes_ip();
    if (nombre == "redes.sockets" || nombre == "sockets") return crear_modulo_sockets(nombre);
    if (nombre == "cripto" || nombre == "seguridad") return crear_modulo_cripto(nombre);
    if (nombre == "forense") return crear_modulo_forense();
    if (nombre == "ast") return crear_modulo_ast();
    if (nombre == "motor2d" || nombre == "motor3d" || nombre == "graficos") return crear_modulo_graficos(nombre);
    if (nombre == "gui" || nombre == "interfaz") return crear_modulo_gui();
    if (nombre == "audio") return crear_modulo_audio();
    if (nombre == "websockets" || nombre == "ws") return crear_modulo_websockets();

    // --- MODULOS INTEGRADOS ---
    if (nombre == "archivos") {
        auto m = crear("archivos");
        fn(*m->entorno, "leer", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("archivos.leer(ruta)", u);
            return Valor::de_texto(leer_archivo(a[0].a_texto()));
        });
        fn(*m->entorno, "escribir", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("archivos.escribir(ruta, texto)", u);
            escribir_archivo(a[0].a_texto(), a[1].a_texto());
            return Valor::de_bool(true);
        });
        fn(*m->entorno, "anexar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("archivos.anexar(ruta, texto)", u);
            std::ofstream out(a[0].a_texto(), std::ios::binary | std::ios::app);
            if (!out) throw ErrorEjecucion("no se pudo abrir para anexar: " + a[0].a_texto(), u);
            out << a[1].a_texto();
            return Valor::de_bool(true);
        });
        fn(*m->entorno, "existe", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("archivos.existe(ruta)", u);
            std::error_code ec;
            return Valor::de_bool(std::filesystem::exists(a[0].a_texto(), ec) && !ec);
        });
        fn(*m->entorno, "eliminar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("archivos.eliminar(ruta)", u);
            std::error_code ec;
            return Valor::de_bool(std::filesystem::remove(a[0].a_texto(), ec) && !ec);
        });
        fn(*m->entorno, "crear_directorio", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("archivos.crear_directorio(ruta)", u);
            std::error_code ec;
            std::filesystem::create_directories(a[0].a_texto(), ec);
            return Valor::de_bool(!ec);
        });
        fn(*m->entorno, "listar", [](std::vector<Valor>& a, Ubicacion u) {
            std::string dir = a.empty() ? "." : a[0].a_texto();
            std::error_code ec;
            std::vector<Valor> items;
            for (std::filesystem::directory_iterator it(dir, ec), fin; it != fin && !ec; it.increment(ec)) {
                items.push_back(Valor::de_texto(it->path().filename().string()));
            }
            if (ec) throw ErrorEjecucion("no se pudo listar el directorio: " + dir, u);
            return Valor::de_lista(std::move(items));
        });
        return m;
    }
    if (nombre == "rutas") {
        auto m = crear(nombre);
        fn(*m->entorno, "unir", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("rutas.unir(p1, p2, ...)", u);
            std::filesystem::path p(a[0].a_texto());
            for (std::size_t i = 1; i < a.size(); ++i) p /= a[i].a_texto();
            return Valor::de_texto(p.string());
        });
        fn(*m->entorno, "absoluta", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("rutas.absoluta(ruta)", u);
            return Valor::de_texto(std::filesystem::absolute(a[0].a_texto()).string());
        });
        fn(*m->entorno, "padre", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("rutas.padre(ruta)", u);
            return Valor::de_texto(std::filesystem::path(a[0].a_texto()).parent_path().string());
        });
        fn(*m->entorno, "nombre", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("rutas.nombre(ruta)", u);
            return Valor::de_texto(std::filesystem::path(a[0].a_texto()).filename().string());
        });
        fn(*m->entorno, "extension", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("rutas.extension(ruta)", u);
            return Valor::de_texto(std::filesystem::path(a[0].a_texto()).extension().string());
        });
        fn(*m->entorno, "tamano", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("rutas.tamano(ruta)", u);
            std::error_code ec;
            auto sz = std::filesystem::file_size(a[0].a_texto(), ec);
            if (ec) throw ErrorEjecucion("no se pudo leer tamano de: " + a[0].a_texto(), u);
            return Valor::de_entero(static_cast<std::int64_t>(sz));
        });
        fn(*m->entorno, "recorrer", [](std::vector<Valor>& a, Ubicacion u) {
            std::string dir = a.empty() ? "." : a[0].a_texto();
            std::error_code ec;
            std::vector<Valor> lista;
            for (std::filesystem::recursive_directory_iterator it(dir, ec), fin; it != fin && !ec; it.increment(ec)) {
                lista.push_back(Valor::de_texto(it->path().string()));
            }
            if (ec) throw ErrorEjecucion("no se pudo recorrer directorio: " + dir, u);
            return Valor::de_lista(std::move(lista));
        });
        return m;
    }
    if (nombre == "sistema" || nombre == "sistema.os") {
        auto m = crear(nombre);
        fn(*m->entorno, "nombre", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
            return Valor::de_texto("windows");
#elif defined(__APPLE__)
            return Valor::de_texto("macos");
#else
            return Valor::de_texto("linux");
#endif
        });
        fn(*m->entorno, "variable", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("sistema.variable(nombre)", u);
            const char* v = std::getenv(a[0].a_texto().c_str());
            return v ? Valor::de_texto(v) : Valor::nulo();
        });
        fn(*m->entorno, "prioridad", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("sistema.prioridad(alta|normal|baja)", u);
            std::string p = a[0].a_texto();
#ifdef _WIN32
            DWORD clase = NORMAL_PRIORITY_CLASS;
            if (p == "alta") clase = HIGH_PRIORITY_CLASS;
            else if (p == "baja") clase = IDLE_PRIORITY_CLASS;
            return Valor::de_bool(SetPriorityClass(GetCurrentProcess(), clase) != 0);
#else
            (void)p;
            return Valor::de_bool(true);
#endif
        });
        return m;
    }
    if (nombre == "sistema.procesos" || nombre == "subprocess") {
        auto m = crear(nombre);
        fn(*m->entorno, "ejecutar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("procesos.ejecutar(comando, timeout_ms?)", u);
            std::string out, err;
            int code = ejecutar_proceso(a[0].a_texto(), "", &out, &err, a.size() > 1 ? static_cast<int>(a[1].numero()) : 0);
            std::map<std::string, Valor> r;
            r["codigo"] = Valor::de_entero(code);
            r["salida"] = Valor::de_texto(out);
            r["error"] = Valor::de_texto(err);
            return Valor::de_mapa(std::move(r));
        });
        return m;
    }
    if (nombre == "hardware") {
        auto m = crear(nombre);
        fn(*m->entorno, "nucleos", [](std::vector<Valor>&, Ubicacion) {
            unsigned int n = std::thread::hardware_concurrency();
            return Valor::de_entero(n > 0 ? static_cast<std::int64_t>(n) : 1);
        });
        fn(*m->entorno, "arquitectura", [](std::vector<Valor>&, Ubicacion) {
#if defined(__x86_64__) || defined(_M_X64)
            return Valor::de_texto("x86_64");
#elif defined(__aarch64__) || defined(_M_ARM64)
            return Valor::de_texto("arm64");
#else
            return Valor::de_texto("x86");
#endif
        });
        fn(*m->entorno, "memoria_total_mb", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
            MEMORYSTATUSEX st{};
            st.dwLength = sizeof(st);
            if (GlobalMemoryStatusEx(&st)) {
                return Valor::de_entero(static_cast<std::int64_t>(st.ullTotalPhys / (1024ULL * 1024ULL)));
            }
#else
            std::ifstream mem("/proc/meminfo");
            std::string clave;
            std::uint64_t kb = 0;
            while (mem >> clave >> kb) {
                if (clave == "MemTotal:") return Valor::de_entero(static_cast<std::int64_t>(kb / 1024ULL));
            }
#endif
            return Valor::de_entero(0);
        });
        fn(*m->entorno, "memoria_libre_mb", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
            MEMORYSTATUSEX st{};
            st.dwLength = sizeof(st);
            if (GlobalMemoryStatusEx(&st)) {
                return Valor::de_entero(static_cast<std::int64_t>(st.ullAvailPhys / (1024ULL * 1024ULL)));
            }
#else
            std::ifstream mem("/proc/meminfo");
            std::string clave;
            std::uint64_t kb = 0;
            while (mem >> clave >> kb) {
                if (clave == "MemAvailable:") return Valor::de_entero(static_cast<std::int64_t>(kb / 1024ULL));
            }
#endif
            return Valor::de_entero(0);
        });
        return m;
    }
    if (nombre == "tiempo") {
        auto m = crear(nombre);
        fn(*m->entorno, "ahora_ms", [](std::vector<Valor>&, Ubicacion) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
            return Valor::de_entero(static_cast<std::int64_t>(ms));
        });
        fn(*m->entorno, "ahora_us", [](std::vector<Valor>&, Ubicacion) {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch())
                          .count();
            return Valor::de_entero(static_cast<std::int64_t>(us));
        });
        fn(*m->entorno, "esperar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("tiempo.esperar(milisegundos)", u);
            std::int64_t ms = static_cast<std::int64_t>(a[0].numero());
            if (ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            return Valor::de_bool(true);
        });
        return m;
    }
    if (nombre == "matematicas") {
        auto m = crear(nombre);
        fn(*m->entorno, "raiz", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("matematicas.raiz(x)", u);
            return Valor::de_decimal(std::sqrt(a[0].numero()));
        });
        fn(*m->entorno, "abs", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("matematicas.abs(x)", u);
            return Valor::de_decimal(std::fabs(a[0].numero()));
        });
        fn(*m->entorno, "seno", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("matematicas.seno(x)", u);
            return Valor::de_decimal(std::sin(a[0].numero()));
        });
        fn(*m->entorno, "coseno", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("matematicas.coseno(x)", u);
            return Valor::de_decimal(std::cos(a[0].numero()));
        });
        fn(*m->entorno, "log", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("matematicas.log(x)", u);
            return Valor::de_decimal(std::log(a[0].numero()));
        });
        fn(*m->entorno, "redondear", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("matematicas.redondear(x)", u);
            return Valor::de_entero(static_cast<std::int64_t>(std::llround(a[0].numero())));
        });
        fn(*m->entorno, "min", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("matematicas.min(a,b)", u);
            return Valor::de_decimal(std::min(a[0].numero(), a[1].numero()));
        });
        fn(*m->entorno, "max", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("matematicas.max(a,b)", u);
            return Valor::de_decimal(std::max(a[0].numero(), a[1].numero()));
        });
        m->entorno->definir("PI", Valor::de_decimal(3.14159265358979323846), Visibilidad::Publico, true, true);
        m->entorno->definir("E", Valor::de_decimal(2.71828182845904523536), Visibilidad::Publico, true, true);
        return m;
    }
    if (nombre == "aleatorio") {
        static std::mt19937_64 rng(std::random_device{}());
        auto m = crear(nombre);
        fn(*m->entorno, "semilla", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("aleatorio.semilla(numero)", u);
            rng.seed(static_cast<std::uint64_t>(a[0].numero()));
            return Valor::de_bool(true);
        });
        fn(*m->entorno, "entero", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("aleatorio.entero(min, max)", u);
            std::int64_t lo = static_cast<std::int64_t>(a[0].numero());
            std::int64_t hi = static_cast<std::int64_t>(a[1].numero());
            if (lo > hi) std::swap(lo, hi);
            std::uniform_int_distribution<std::int64_t> dist(lo, hi);
            return Valor::de_entero(dist(rng));
        });
        fn(*m->entorno, "decimal", [](std::vector<Valor>&, Ubicacion) {
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            return Valor::de_decimal(dist(rng));
        });
        fn(*m->entorno, "elegir", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Lista || a[0].lista->empty())
                throw ErrorEjecucion("aleatorio.elegir(lista_no_vacia)", u);
            std::uniform_int_distribution<std::size_t> dist(0, a[0].lista->size() - 1);
            return (*a[0].lista)[dist(rng)];
        });
        fn(*m->entorno, "mezclar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Lista)
                throw ErrorEjecucion("aleatorio.mezclar(lista)", u);
            std::vector<Valor> copia = *a[0].lista;
            std::shuffle(copia.begin(), copia.end(), rng);
            return Valor::de_lista(std::move(copia));
        });
        return m;
    }
    if (nombre == "estadistica") {
        auto m = crear(nombre);
        fn(*m->entorno, "media", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Lista || a[0].lista->empty())
                throw ErrorEjecucion("estadistica.media(lista_no_vacia)", u);
            double s = 0;
            for (auto& x : *a[0].lista) s += x.numero();
            return Valor::de_decimal(s / static_cast<double>(a[0].lista->size()));
        });
        return m;
    }
    if (nombre == "base64") {
        auto m = crear(nombre);
        fn(*m->entorno, "codificar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("base64.codificar(texto)", u);
            return Valor::de_texto(base64_codificar(a[0].a_texto()));
        });
        fn(*m->entorno, "decodificar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("base64.decodificar(texto_base64)", u);
            return Valor::de_texto(base64_decodificar(a[0].a_texto()));
        });
        return m;
    }
    if (nombre == "hilos") {
        auto m = crear(nombre);
        fn(*m->entorno, "ejecutar_paralelo", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Lista)
                throw ErrorEjecucion("hilos.ejecutar_paralelo(lista_codigos_esp)", u);
            const auto& tareas = *a[0].lista;
            std::vector<Valor> resultados(tareas.size());
            std::vector<std::string> errores(tareas.size());
            std::vector<std::thread> workers;
            for (std::size_t i = 0; i < tareas.size(); ++i) {
                std::string src = tareas[i].a_texto();
                workers.emplace_back([&, i, src]() {
                    try {
                        Interprete interp_local;
                        resultados[i] = interp_local.ejecutar_fuente(src, "hilo_" + std::to_string(i) + ".esp");
                    } catch (const std::exception& ex) {
                        errores[i] = ex.what();
                    }
                });
            }
            for (auto& w : workers) if (w.joinable()) w.join();
            for (std::size_t i = 0; i < errores.size(); ++i) {
                if (!errores[i].empty()) throw ErrorEjecucion("error en hilo #" + std::to_string(i) + ": " + errores[i], u);
            }
            return Valor::de_lista(std::move(resultados));
        });
        return m;
    }
    if (nombre == "tensores") {
        auto m = crear(nombre);
        fn(*m->entorno, "producto_punto", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2 || a[0].tipo != TipoValor::Lista || a[1].tipo != TipoValor::Lista)
                throw ErrorEjecucion("tensores.producto_punto(v1, v2)", u);
            const auto& v1 = *a[0].lista;
            const auto& v2 = *a[1].lista;
            if (v1.size() != v2.size()) throw ErrorEjecucion("los vectores deben tener igual longitud", u);
            double suma = 0.0;
            for (std::size_t i = 0; i < v1.size(); ++i) suma += v1[i].numero() * v2[i].numero();
            return Valor::de_decimal(suma);
        });
        fn(*m->entorno, "multiplicar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2 || a[0].tipo != TipoValor::Lista || a[1].tipo != TipoValor::Lista)
                throw ErrorEjecucion("tensores.multiplicar(matriz_a, matriz_b)", u);
            const auto& A = *a[0].lista;
            const auto& B = *a[1].lista;
            if (A.empty() || B.empty() || A[0].tipo != TipoValor::Lista || B[0].tipo != TipoValor::Lista)
                throw ErrorEjecucion("matrices invalidas", u);
            std::size_t filas_a = A.size();
            std::size_t cols_a = A[0].lista->size();
            std::size_t filas_b = B.size();
            std::size_t cols_b = B[0].lista->size();
            if (cols_a != filas_b) throw ErrorEjecucion("dimensiones incompatibles para multiplicacion de matrices", u);

            std::vector<Valor> C;
            C.reserve(filas_a);
            for (std::size_t i = 0; i < filas_a; ++i) {
                std::vector<Valor> fila(cols_b);
                for (std::size_t j = 0; j < cols_b; ++j) {
                    double acc = 0.0;
                    for (std::size_t k = 0; k < cols_a; ++k) {
                        acc += (*A[i].lista)[k].numero() * (*B[k].lista)[j].numero();
                    }
                    fila[j] = Valor::de_decimal(acc);
                }
                C.push_back(Valor::de_lista(std::move(fila)));
            }
            return Valor::de_lista(std::move(C));
        });
        return m;
    }
    if (nombre == "datos.sql" || nombre == "sql") {
        auto m = crear(nombre);
        fn(*m->entorno, "abrir", [](std::vector<Valor>& a, Ubicacion) {
            std::string ruta = a.empty() ? ":memoria:" : a[0].a_texto();
            std::lock_guard<std::mutex> lock(sql_mutex);
            auto& db = bases[ruta];
            if (!db) {
                db = std::make_shared<BaseSQL>();
                db->ruta = ruta;
                cargar_base_sql(*db);
            }
            std::map<std::string, Valor> info;
            info["ruta"] = Valor::de_texto(ruta);
            info["motor"] = Valor::de_texto(ruta == ":memoria:" ? "esp-sql-memoria" : "esp-sql-persistente");
            return Valor::de_mapa(std::move(info));
        });
        fn(*m->entorno, "consultar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("sql.consultar(ruta, sql_en_espanol_o_ingles, [params])", u);
            std::string sql = a[1].a_texto();
            std::vector<Valor> params;
            if (a.size() > 2 && a[2].tipo == TipoValor::Lista) params = *a[2].lista;
            sql = aplicar_params(sql, params);
            std::lock_guard<std::mutex> lock(sql_mutex);
            auto it = bases.find(a[0].a_texto());
            if (it == bases.end()) throw ErrorEjecucion("base no abierta; use sql.abrir primero", u);
            return sql_ejecutar(it->second, sql);
        });
        fn(*m->entorno, "conectar_externo", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("sql.conectar_externo(motor)", u);
            std::string motor = a[0].a_texto();
#ifdef _WIN32
            std::string dll;
            if (motor == "postgresql") dll = "libpq.dll";
            else if (motor == "mysql" || motor == "mariadb") dll = "libmysql.dll";
            else if (motor == "sqlserver") dll = "sqlncli11.dll";
            else if (motor == "sqlite") dll = "sqlite3.dll";
            else throw ErrorEjecucion("motor desconocido: " + motor, u);
            HMODULE h = LoadLibraryA(dll.c_str());
            if (!h)
                throw ErrorEjecucion("no hay cliente nativo de " + motor + " (" + dll +
                                     "). El motor embebido local si esta disponible via sql.abrir",
                                     u);
            FreeLibrary(h);
            return Valor::de_bool(true);
#elif defined(__APPLE__)
            std::string dylib;
            if (motor == "postgresql") dylib = "libpq.dylib";
            else if (motor == "mysql" || motor == "mariadb") dylib = "libmysqlclient.dylib";
            else if (motor == "sqlserver") dylib = "libodbc.dylib";
            else if (motor == "sqlite") dylib = "libsqlite3.dylib";
            else throw ErrorEjecucion("motor desconocido: " + motor, u);
            void* h = dlopen(dylib.c_str(), RTLD_LAZY);
            if (!h) throw ErrorEjecucion("no hay cliente nativo de " + motor + " (" + dylib + ")", u);
            dlclose(h);
            return Valor::de_bool(true);
#else
            std::string so;
            if (motor == "postgresql") so = "libpq.so";
            else if (motor == "mysql" || motor == "mariadb") so = "libmysqlclient.so";
            else if (motor == "sqlserver") so = "libodbc.so";
            else if (motor == "sqlite") so = "libsqlite3.so";
            else throw ErrorEjecucion("motor desconocido: " + motor, u);
            void* h = dlopen(so.c_str(), RTLD_LAZY);
            if (!h) throw ErrorEjecucion("no hay cliente nativo de " + motor + " (" + so + ")", u);
            dlclose(h);
            return Valor::de_bool(true);
#endif
        });
        return m;
    }
    if (nombre == "redes.dns" || nombre == "dns") {
        auto m = crear(nombre);
        fn(*m->entorno, "resolver", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("dns.resolver(dominio)", u);
            asegurar_red();
            std::string dominio = a[0].a_texto();
            addrinfo hints{}, *res = nullptr;
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            if (getaddrinfo(dominio.c_str(), nullptr, &hints, &res) != 0 || !res) {
                throw ErrorEjecucion("no se pudo resolver el dominio DNS: " + dominio, u);
            }
            std::vector<Valor> ips;
            for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
                char ipstr[INET6_ADDRSTRLEN]{};
                void* addr_ptr = nullptr;
                if (p->ai_family == AF_INET) addr_ptr = &(reinterpret_cast<sockaddr_in*>(p->ai_addr)->sin_addr);
                else if (p->ai_family == AF_INET6) addr_ptr = &(reinterpret_cast<sockaddr_in6*>(p->ai_addr)->sin6_addr);
                if (addr_ptr && inet_ntop(p->ai_family, addr_ptr, ipstr, sizeof(ipstr))) {
                    std::string s(ipstr);
                    bool dup = false;
                    for (const auto& ex : ips) if (ex.texto == s) { dup = true; break; }
                    if (!dup) ips.push_back(Valor::de_texto(std::move(s)));
                }
            }
            freeaddrinfo(res);
            return Valor::de_lista(std::move(ips));
        });
        return m;
    }
    if (nombre == "redes.http" || nombre == "web.http" || nombre == "http" || nombre == "requests") {
        auto m = crear(nombre);
        fn(*m->entorno, "obtener", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("http.obtener(url, cabeceras?)", u);
            std::map<std::string, std::string> headers;
            if (a.size() > 1 && a[1].tipo == TipoValor::Mapa && a[1].mapa) {
                for (const auto& kv : *a[1].mapa) headers[kv.first] = kv.second.a_texto();
            }
            Valor resp = ejecutar_peticion_http_nativa("GET", a[0].a_texto(), "", headers, u);
            return (*resp.mapa)["cuerpo"];
        });
        fn(*m->entorno, "peticion", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("http.peticion(metodo, url, cuerpo?, cabeceras?)", u);
            std::string metodo = mayusculas(a[0].a_texto());
            std::string url = a[1].a_texto();
            std::string cuerpo = a.size() > 2 ? a[2].a_texto() : "";
            std::map<std::string, std::string> headers;
            if (a.size() > 3 && a[3].tipo == TipoValor::Mapa && a[3].mapa) {
                for (const auto& kv : *a[3].mapa) headers[kv.first] = kv.second.a_texto();
            }
            return ejecutar_peticion_http_nativa(metodo, url, cuerpo, headers, u);
        });
        fn(*m->entorno, "enviar_post", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("http.enviar_post(url, cuerpo, cabeceras?)", u);
            std::map<std::string, std::string> headers;
            if (a.size() > 2 && a[2].tipo == TipoValor::Mapa && a[2].mapa) {
                for (const auto& kv : *a[2].mapa) headers[kv.first] = kv.second.a_texto();
            }
            return ejecutar_peticion_http_nativa("POST", a[0].a_texto(), a[1].a_texto(), headers, u);
        });
        fn(*m->entorno, "descargar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("http.descargar(url, ruta_destino)", u);
            Valor resp = ejecutar_peticion_http_nativa("GET", a[0].a_texto(), "", {}, u);
            escribir_archivo(a[1].a_texto(), (*resp.mapa)["cuerpo"].texto);
            return (*resp.mapa)["ok"];
        });
        return m;
    }
    if (nombre == "web") {
        auto m = crear(nombre);

        fn(*m->entorno, "pagina", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("web.pagina(titulo, contenido_html, css?, js?)", u);
            std::string titulo = escapar_html(a[0].a_texto());
            std::string cuerpo = a[1].a_texto();
            std::string css_extra = a.size() > 2 ? a[2].a_texto() : "";
            std::string js_extra = a.size() > 3 ? a[3].a_texto() : "";

            std::ostringstream html;
            html << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
                 << "  <meta charset=\"UTF-8\">\n"
                 << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                 << "  <title>" << titulo << "</title>\n"
                 << "  <style>\n"
                 << "    :root { --bg: #0b1020; --card: #121a30; --text: #edf2ff; --accent: #54e5d0; --border: rgba(255,255,255,0.1); }\n"
                 << "    * { box-sizing: border-box; }\n"
                 << "    body { margin: 0; font-family: system-ui, -apple-system, sans-serif; background: var(--bg); color: var(--text); line-height: 1.6; padding: 2rem; }\n"
                 << "    .contenedor { max-width: 960px; margin: 0 auto; }\n"
                 << "    .tarjeta { background: var(--card); border: 1px solid var(--border); border-radius: 14px; padding: 1.5rem; margin-bottom: 1.25rem; }\n"
                 << "    .boton { display: inline-block; background: var(--accent); color: #061018; font-weight: 700; padding: 0.65rem 1.2rem; border-radius: 10px; text-decoration: none; border: none; cursor: pointer; }\n"
                 << "    table { width: 100%; border-collapse: collapse; margin: 1rem 0; }\n"
                 << "    th, td { padding: 0.75rem; border-bottom: 1px solid var(--border); text-align: left; }\n"
                 << "    input, textarea { width: 100%; padding: 0.65rem; margin: 0.4rem 0 1rem; border-radius: 8px; border: 1px solid var(--border); background: #090d1a; color: var(--text); }\n"
                 << css_extra << "\n"
                 << "  </style>\n</head>\n<body>\n  <main class=\"contenedor\">\n"
                 << cuerpo << "\n  </main>\n";
            if (!js_extra.empty()) {
                html << "  <script>\n" << js_extra << "\n  </script>\n";
            }
            html << "</body>\n</html>\n";
            return Valor::de_texto(html.str());
        });

        fn(*m->entorno, "tarjeta", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("web.tarjeta(titulo, descripcion, texto_boton?, enlace?)", u);
            std::ostringstream out;
            out << "<section class=\"tarjeta\"><h2>" << escapar_html(a[0].a_texto()) << "</h2><p>"
                << escapar_html(a[1].a_texto()) << "</p>";
            if (a.size() > 2 && !a[2].a_texto().empty()) {
                std::string href = a.size() > 3 ? escapar_html(a[3].a_texto()) : "#";
                out << "<a class=\"boton\" href=\"" << href << "\">" << escapar_html(a[2].a_texto()) << "</a>";
            }
            out << "</section>";
            return Valor::de_texto(out.str());
        });

        fn(*m->entorno, "tabla", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2 || a[0].tipo != TipoValor::Lista || a[1].tipo != TipoValor::Lista)
                throw ErrorEjecucion("web.tabla(columnas, filas)", u);
            std::ostringstream out;
            out << "<div class=\"tarjeta\"><table><thead><tr>";
            for (const auto& col : *a[0].lista) out << "<th>" << escapar_html(col.a_texto()) << "</th>";
            out << "</tr></thead><tbody>";
            for (const auto& fila : *a[1].lista) {
                out << "<tr>";
                if (fila.tipo == TipoValor::Lista && fila.lista) {
                    for (const auto& celda : *fila.lista) out << "<td>" << escapar_html(celda.a_texto()) << "</td>";
                } else if (fila.tipo == TipoValor::Mapa && fila.mapa) {
                    for (const auto& col : *a[0].lista) {
                        auto it = fila.mapa->find(col.a_texto());
                        out << "<td>" << (it != fila.mapa->end() ? escapar_html(it->second.a_texto()) : "") << "</td>";
                    }
                }
                out << "</tr>";
            }
            out << "</tbody></table></div>";
            return Valor::de_texto(out.str());
        });

        fn(*m->entorno, "formulario", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 3 || a[2].tipo != TipoValor::Lista)
                throw ErrorEjecucion("web.formulario(accion, metodo, lista_campos, texto_enviar?)", u);
            std::string accion = escapar_html(a[0].a_texto());
            std::string metodo = escapar_html(a[1].a_texto());
            std::string enviar_txt = a.size() > 3 ? escapar_html(a[3].a_texto()) : "Enviar";
            std::ostringstream out;
            out << "<form class=\"tarjeta\" action=\"" << accion << "\" method=\"" << metodo << "\">";
            for (const auto& campo : *a[2].lista) {
                std::string nom = escapar_html(campo.a_texto());
                out << "<label>" << nom << "<input name=\"" << nom << "\" required></label>";
            }
            out << "<button class=\"boton\" type=\"submit\">" << enviar_txt << "</button></form>";
            return Valor::de_texto(out.str());
        });

        fn(*m->entorno, "servidor", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2 || a[1].tipo != TipoValor::Mapa || !a[1].mapa)
                throw ErrorEjecucion("web.servidor(puerto, mapa_rutas, max_peticiones?)", u);
            asegurar_red();
            int puerto = static_cast<int>(a[0].numero());
            int max_peticiones = a.size() > 2 ? static_cast<int>(a[2].numero()) : 10;
            if (max_peticiones < 1) max_peticiones = 1;

            SocketNativo ls = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (ls == SOCKET_INVALIDO) throw ErrorEjecucion("no se pudo crear socket web", u);
            int opt = 1;
            setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(static_cast<u_short>(puerto));
            if (bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
                cerrar_socket(ls);
                throw ErrorEjecucion("puerto ocupado: " + std::to_string(puerto), u);
            }
            listen(ls, 32);

            for (int i = 0; i < max_peticiones; ++i) {
                SocketNativo cs = accept(ls, nullptr, nullptr);
                if (cs == SOCKET_INVALIDO) break;
                char buf[8192]{};
                int n = recv(cs, buf, sizeof(buf) - 1, 0);
                std::string req(n > 0 ? buf : "", n > 0 ? static_cast<std::size_t>(n) : 0);

                std::string ruta = "/";
                auto sp1 = req.find(' ');
                if (sp1 != std::string::npos) {
                    auto sp2 = req.find(' ', sp1 + 1);
                    if (sp2 != std::string::npos) {
                        ruta = req.substr(sp1 + 1, sp2 - (sp1 + 1));
                        auto qm = ruta.find('?');
                        if (qm != std::string::npos) ruta = ruta.substr(0, qm);
                    }
                }

                std::string status = "HTTP/1.1 200 OK";
                std::string body;
                std::string ctype = "text/html; charset=utf-8";

                auto it = a[1].mapa->find(ruta);
                if (it != a[1].mapa->end()) {
                    body = it->second.a_texto();
                    std::string trim = recortar(body);
                    if (!trim.empty() && (trim.front() == '{' || trim.front() == '[')) {
                        ctype = "application/json; charset=utf-8";
                    } else if (trim.find("<html") == std::string::npos && trim.find("<!DOCTYPE") == std::string::npos &&
                               trim.find('<') == std::string::npos) {
                        ctype = "text/plain; charset=utf-8";
                    }
                } else {
                    status = "HTTP/1.1 404 Not Found";
                    body = "<h1>404 - Pagina no encontrada</h1>";
                }

                std::ostringstream resp;
                resp << status << "\r\nContent-Type: " << ctype
                     << "\r\nContent-Length: " << body.size()
                     << "\r\nConnection: close\r\n\r\n"
                     << body;
                std::string raw = resp.str();
                send(cs, raw.c_str(), static_cast<int>(raw.size()), 0);
                cerrar_socket(cs);
            }
            cerrar_socket(ls);
            return Valor::de_bool(true);
        });

        return m;
    }
    if (nombre == "servidor") {
        auto m = crear(nombre);
        fn(*m->entorno, "http", [](std::vector<Valor>& a, Ubicacion u) {
            asegurar_red();
            int puerto = a.empty() ? 8080 : static_cast<int>(a[0].numero());
            bool es_mapa_rutas = (a.size() > 1 && a[1].tipo == TipoValor::Mapa && a[1].mapa);
            std::string cuerpo_fijo = (!es_mapa_rutas && a.size() > 1) ? a[1].a_texto() : "ESP 2.0.0";
            int max_peticiones = a.size() > 2 ? static_cast<int>(a[2].numero()) : 1;
            if (max_peticiones < 1) max_peticiones = 1;

            SocketNativo ls = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (ls == SOCKET_INVALIDO) throw ErrorEjecucion("no se pudo crear el socket del servidor", u);

            int opt = 1;
            setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(static_cast<u_short>(puerto));

            if (bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
                cerrar_socket(ls);
                throw ErrorEjecucion("no se pudo enlazar el puerto " + std::to_string(puerto), u);
            }
            listen(ls, 16);

            for (int atendidas = 0; atendidas < max_peticiones; ++atendidas) {
                SocketNativo cs = accept(ls, nullptr, nullptr);
                if (cs == SOCKET_INVALIDO) break;

                char buf[4096]{};
                int leidos = recv(cs, buf, sizeof(buf) - 1, 0);
                std::string req(leidos > 0 ? buf : "", leidos > 0 ? static_cast<std::size_t>(leidos) : 0);

                std::string ruta_req = "/";
                auto sp1 = req.find(' ');
                if (sp1 != std::string::npos) {
                    auto sp2 = req.find(' ', sp1 + 1);
                    if (sp2 != std::string::npos) ruta_req = req.substr(sp1 + 1, sp2 - (sp1 + 1));
                }

                std::string status_line = "HTTP/1.1 200 OK";
                std::string cuerpo_resp = cuerpo_fijo;
                std::string content_type = "text/plain; charset=utf-8";

                if (es_mapa_rutas) {
                    auto it_ruta = a[1].mapa->find(ruta_req);
                    if (it_ruta != a[1].mapa->end()) {
                        cuerpo_resp = it_ruta->second.a_texto();
                        if (!cuerpo_resp.empty() && (cuerpo_resp.front() == '{' || cuerpo_resp.front() == '[')) {
                            content_type = "application/json; charset=utf-8";
                        } else if (cuerpo_resp.find("<html") != std::string::npos ||
                                   cuerpo_resp.find("<!DOCTYPE") != std::string::npos) {
                            content_type = "text/html; charset=utf-8";
                        }
                    } else {
                        status_line = "HTTP/1.1 404 Not Found";
                        cuerpo_resp = "Ruta no encontrada: " + ruta_req;
                    }
                }

                std::ostringstream resp;
                resp << status_line << "\r\nContent-Type: " << content_type
                     << "\r\nContent-Length: " << cuerpo_resp.size()
                     << "\r\nConnection: close\r\n\r\n"
                     << cuerpo_resp;
                auto s = resp.str();
                send(cs, s.c_str(), static_cast<int>(s.size()), 0);
                cerrar_socket(cs);
            }
            cerrar_socket(ls);
            return Valor::de_bool(true);
        });
        return m;
    }
    if (nombre == "csv") {
        auto m = crear(nombre);
        fn(*m->entorno, "parsear", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Texto)
                throw ErrorEjecucion("csv.parsear(texto, delimitador?)", u);
            char delim = (a.size() > 1 && a[1].tipo == TipoValor::Texto && !a[1].texto.empty())
                             ? a[1].texto[0]
                             : ',';
            const std::string& src = a[0].texto;
            std::vector<Valor> filas;
            std::vector<Valor> fila_actual;
            std::string campo;
            bool en_comillas = false;
            for (std::size_t i = 0; i < src.size(); ++i) {
                char c = src[i];
                if (en_comillas) {
                    if (c == '"' && i + 1 < src.size() && src[i + 1] == '"') {
                        campo.push_back('"');
                        ++i;
                    } else if (c == '"') {
                        en_comillas = false;
                    } else {
                        campo.push_back(c);
                    }
                } else {
                    if (c == '"') {
                        en_comillas = true;
                    } else if (c == delim) {
                        fila_actual.push_back(Valor::de_texto(std::move(campo)));
                        campo.clear();
                    } else if (c == '\n') {
                        fila_actual.push_back(Valor::de_texto(std::move(campo)));
                        campo.clear();
                        filas.push_back(Valor::de_lista(std::move(fila_actual)));
                        fila_actual.clear();
                    } else if (c != '\r') {
                        campo.push_back(c);
                    }
                }
            }
            if (!campo.empty() || !fila_actual.empty()) {
                fila_actual.push_back(Valor::de_texto(std::move(campo)));
                filas.push_back(Valor::de_lista(std::move(fila_actual)));
            }
            return Valor::de_lista(std::move(filas));
        });
        fn(*m->entorno, "texto", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Lista)
                throw ErrorEjecucion("csv.texto(filas, delimitador?)", u);
            char delim = (a.size() > 1 && a[1].tipo == TipoValor::Texto && !a[1].texto.empty())
                             ? a[1].texto[0]
                             : ',';
            std::ostringstream out;
            for (std::size_t r = 0; r < a[0].lista->size(); ++r) {
                const auto& fila = (*a[0].lista)[r];
                if (fila.tipo != TipoValor::Lista) throw ErrorEjecucion("cada fila CSV debe ser una lista", u);
                for (std::size_t c = 0; c < fila.lista->size(); ++c) {
                    if (c > 0) out << delim;
                    std::string celda = (*fila.lista)[c].a_texto();
                    if (celda.find(delim) != std::string::npos || celda.find('"') != std::string::npos ||
                        celda.find('\n') != std::string::npos) {
                        out << '"';
                        for (char ch : celda) {
                            if (ch == '"') out << "\"\"";
                            else out << ch;
                        }
                        out << '"';
                    } else {
                        out << celda;
                    }
                }
                out << '\n';
            }
            return Valor::de_texto(out.str());
        });
        return m;
    }
    if (nombre == "lenguajes") {
        struct DefinicionLenguaje {
            std::string nombre;
            std::map<std::string, std::string> alias_palabras;
            std::string abrir_bloque;
            std::string cerrar_bloque;
        };
        static std::mutex lang_mutex;
        static std::map<std::string, DefinicionLenguaje> registro_lenguajes;

        auto traducir_a_esp = [](const DefinicionLenguaje& def, const std::string& codigo_fuente) -> std::string {
            std::string salida;
            salida.reserve(codigo_fuente.size() + 64);
            std::size_t i = 0;
            const std::size_t n = codigo_fuente.size();

            auto es_id_char = [](unsigned char c) {
                return std::isalnum(c) || c == '_' || c >= 128;
            };

            while (i < n) {
                char c = codigo_fuente[i];
                if (c == '"' || c == '\'') {
                    char q = c;
                    salida.push_back(codigo_fuente[i++]);
                    while (i < n && codigo_fuente[i] != q) {
                        if (codigo_fuente[i] == '\\' && i + 1 < n) salida.push_back(codigo_fuente[i++]);
                        salida.push_back(codigo_fuente[i++]);
                    }
                    if (i < n) salida.push_back(codigo_fuente[i++]);
                    continue;
                }
                if (c == '#' || (c == '/' && i + 1 < n && codigo_fuente[i + 1] == '/')) {
                    while (i < n && codigo_fuente[i] != '\n') salida.push_back(codigo_fuente[i++]);
                    continue;
                }
                if (!def.abrir_bloque.empty() && codigo_fuente.compare(i, def.abrir_bloque.size(), def.abrir_bloque) == 0) {
                    salida += "\n";
                    i += def.abrir_bloque.size();
                    continue;
                }
                if (!def.cerrar_bloque.empty() && codigo_fuente.compare(i, def.cerrar_bloque.size(), def.cerrar_bloque) == 0) {
                    salida += "\nfin\n";
                    i += def.cerrar_bloque.size();
                    continue;
                }
                if (es_id_char(static_cast<unsigned char>(c)) && !std::isdigit(static_cast<unsigned char>(c))) {
                    std::size_t inicio = i;
                    while (i < n && es_id_char(static_cast<unsigned char>(codigo_fuente[i]))) ++i;
                    std::string palabra = codigo_fuente.substr(inicio, i - inicio);
                    auto it = def.alias_palabras.find(palabra);
                    if (it != def.alias_palabras.end()) salida += it->second;
                    else salida += palabra;
                    continue;
                }
                salida.push_back(codigo_fuente[i++]);
            }
            return salida;
        };

        auto m = crear(nombre);

        fn(*m->entorno, "crear", [](std::vector<Valor>& a, Ubicacion) {
            std::string id = a.empty() ? "LenguajePersonalizado" : a[0].a_texto();
            std::lock_guard<std::mutex> lk(lang_mutex);
            registro_lenguajes[id] = DefinicionLenguaje{id, {}, "", ""};
            return Valor::de_texto(id);
        });

        fn(*m->entorno, "bloque", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 3) throw ErrorEjecucion("lenguajes.bloque(lenguaje, simbolo_abrir, simbolo_cerrar)", u);
            std::string lang_id = a[0].a_texto();
            std::lock_guard<std::mutex> lk(lang_mutex);
            auto it = registro_lenguajes.find(lang_id);
            if (it == registro_lenguajes.end()) throw ErrorEjecucion("lenguaje no registrado: " + lang_id, u);
            it->second.abrir_bloque = a[1].a_texto();
            it->second.cerrar_bloque = a[2].a_texto();
            return Valor::de_bool(true);
        });

        auto crear_configurador_token = [&](const std::string& palabra_esp_objetivo) -> Valor {
            std::map<std::string, Valor> obj;
            obj["nombre"] = nativa_modulo(
                [palabra_esp_objetivo](std::vector<Valor>& a, Ubicacion u) {
                    if (a.size() < 2)
                        throw ErrorEjecucion("lenguajes." + palabra_esp_objetivo + ".nombre(lenguaje, nueva_palabra)", u);
                    std::string lang_id = a[0].a_texto();
                    std::string nueva_palabra = a[1].a_texto();
                    std::lock_guard<std::mutex> lk(lang_mutex);
                    auto it = registro_lenguajes.find(lang_id);
                    if (it == registro_lenguajes.end())
                        throw ErrorEjecucion("lenguaje no registrado: " + lang_id, u);
                    it->second.alias_palabras[nueva_palabra] = palabra_esp_objetivo;
                    return Valor::de_bool(true);
                },
                "nombre");
            return Valor::de_mapa(std::move(obj));
        };

        const std::vector<std::string> constructos = {
            "variable", "constante", "funcion", "clase", "hereda", "si", "sino",
            "mientras", "para", "hasta", "paso", "en", "devolver", "fin",
            "intenta", "si_no_puede", "conseguir", "exportar", "romper", "continuar",
            "mostrar", "verdadero", "falso", "nulo", "este", "y", "o", "no"
        };
        for (const auto& kw : constructos) {
            m->entorno->definir(kw, crear_configurador_token(kw), Visibilidad::Publico, true, true);
        }

        fn(*m->entorno, "transpilar", [traducir_a_esp](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("lenguajes.transpilar(lenguaje, codigo)", u);
            std::string lang_id = a[0].a_texto();
            std::string codigo = a[1].a_texto();
            DefinicionLenguaje def;
            {
                std::lock_guard<std::mutex> lk(lang_mutex);
                auto it = registro_lenguajes.find(lang_id);
                if (it == registro_lenguajes.end()) throw ErrorEjecucion("lenguaje no registrado: " + lang_id, u);
                def = it->second;
            }
            return Valor::de_texto(traducir_a_esp(def, codigo));
        });

        fn(*m->entorno, "ejecutar", [traducir_a_esp](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("lenguajes.ejecutar(lenguaje, codigo)", u);
            std::string lang_id = a[0].a_texto();
            std::string codigo = a[1].a_texto();
            DefinicionLenguaje def;
            {
                std::lock_guard<std::mutex> lk(lang_mutex);
                auto it = registro_lenguajes.find(lang_id);
                if (it == registro_lenguajes.end()) throw ErrorEjecucion("lenguaje no registrado: " + lang_id, u);
                def = it->second;
            }
            std::string codigo_esp = traducir_a_esp(def, codigo);
            Interprete interp_lang;
            return interp_lang.ejecutar_fuente(codigo_esp, lang_id + ".esp");
        });

        fn(*m->entorno, "compilar", [traducir_a_esp](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 3) throw ErrorEjecucion("lenguajes.compilar(lenguaje, codigo, salida, formato?)", u);
            std::string lang_id = a[0].a_texto();
            std::string codigo = a[1].a_texto();
            std::string salida = a[2].a_texto();
            std::string formato = a.size() > 3 ? a[3].a_texto() : "exe";
            DefinicionLenguaje def;
            {
                std::lock_guard<std::mutex> lk(lang_mutex);
                auto it = registro_lenguajes.find(lang_id);
                if (it == registro_lenguajes.end()) throw ErrorEjecucion("lenguaje no registrado: " + lang_id, u);
                def = it->second;
            }
            std::string codigo_esp = traducir_a_esp(def, codigo);
            OpcionesCompilacion opt;
            opt.entrada = lang_id + ".esp";
            opt.salida = salida;
            opt.formato = formato;
            auto res = compilar_fuente_esp(codigo_esp, opt);
            if (!res.ok) throw ErrorEjecucion("error compilando lenguaje '" + lang_id + "': " + res.mensaje, u);
            return Valor::de_texto(res.archivo_generado);
        });

        fn(*m->entorno, "empaquetar_lenguaje", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("lenguajes.empaquetar_lenguaje(lenguaje, ruta_exe_salida)", u);
            std::string lang_id = a[0].a_texto();
            std::string ruta_exe = a[1].a_texto();
            DefinicionLenguaje def;
            {
                std::lock_guard<std::mutex> lk(lang_mutex);
                auto it = registro_lenguajes.find(lang_id);
                if (it == registro_lenguajes.end()) throw ErrorEjecucion("lenguaje no registrado: " + lang_id, u);
                def = it->second;
            }
            std::ostringstream esp_src;
            esp_src << "conseguir lenguajes\n"
                    << "conseguir archivos\n"
                    << "variable L = lenguajes.crear(\"" << def.nombre << "\")\n";
            for (const auto& kv : def.alias_palabras) {
                esp_src << "lenguajes." << kv.second << ".nombre(L, \"" << kv.first << "\")\n";
            }
            if (!def.abrir_bloque.empty()) {
                esp_src << "lenguajes.bloque(L, \"" << def.abrir_bloque << "\", \"" << def.cerrar_bloque << "\")\n";
            }
            esp_src << "si archivos.existe(\"programa." << def.nombre << "\")\n"
                    << "    lenguajes.ejecutar(L, archivos.leer(\"programa." << def.nombre << "\"))\n"
                    << "sino\n"
                    << "    mostrar(\"Compilador autonomo " << def.nombre << " listo. Crea programa." << def.nombre << "\")\n"
                    << "fin\n";

            OpcionesCompilacion opt;
            opt.entrada = def.nombre + "_cli.esp";
            opt.salida = ruta_exe;
            opt.formato = "exe";
            auto res = compilar_fuente_esp(esp_src.str(), opt);
            if (!res.ok) throw ErrorEjecucion("no se pudo empaquetar compilador: " + res.mensaje, u);
            return Valor::de_texto(res.archivo_generado);
        });

        return m;
    }
    if (nombre == "nativo" || nombre == "ffi" || nombre == "ctypes") {
        static std::mutex ffi_mutex;
        static std::int64_t next_lib_id = 1;
#ifdef _WIN32
        static std::map<std::int64_t, HMODULE> libs_abiertas;
#else
        static std::map<std::int64_t, void*> libs_abiertas;
#endif
        auto m = crear(nombre);
        fn(*m->entorno, "abrir", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("nativo.abrir(ruta_dll_o_so)", u);
            std::string ruta = a[0].a_texto();
#ifdef _WIN32
            HMODULE h = LoadLibraryA(ruta.c_str());
            if (!h) throw ErrorEjecucion("no se pudo cargar libreria nativa: " + ruta, u);
            std::lock_guard<std::mutex> lk(ffi_mutex);
            std::int64_t id = next_lib_id++;
            libs_abiertas[id] = h;
            return Valor::de_entero(id);
#else
            void* h = dlopen(ruta.c_str(), RTLD_LAZY);
            if (!h) throw ErrorEjecucion("no se pudo cargar libreria nativa: " + ruta, u);
            std::lock_guard<std::mutex> lk(ffi_mutex);
            std::int64_t id = next_lib_id++;
            libs_abiertas[id] = h;
            return Valor::de_entero(id);
#endif
        });
        fn(*m->entorno, "llamar_entero", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2) throw ErrorEjecucion("nativo.llamar_entero(lib_id, funcion, arg1?, arg2?)", u);
            std::int64_t id = static_cast<std::int64_t>(a[0].numero());
            std::string sym = a[1].a_texto();
            std::lock_guard<std::mutex> lk(ffi_mutex);
            auto it = libs_abiertas.find(id);
            if (it == libs_abiertas.end()) throw ErrorEjecucion("libreria nativa no abierta", u);
#ifdef _WIN32
            FARPROC proc = GetProcAddress(it->second, sym.c_str());
#else
            void* proc = dlsym(it->second, sym.c_str());
#endif
            if (!proc) throw ErrorEjecucion("simbolo nativo no encontrado: " + sym, u);
            if (a.size() == 2) {
                using Fn0 = int (*)();
                return Valor::de_entero(reinterpret_cast<Fn0>(proc)());
            }
            if (a.size() == 3) {
                using Fn1 = int (*)(int);
                return Valor::de_entero(reinterpret_cast<Fn1>(proc)(static_cast<int>(a[2].numero())));
            }
            using Fn2 = int (*)(int, int);
            return Valor::de_entero(reinterpret_cast<Fn2>(proc)(
                static_cast<int>(a[2].numero()), static_cast<int>(a[3].numero())));
        });
        fn(*m->entorno, "cerrar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("nativo.cerrar(lib_id)", u);
            std::int64_t id = static_cast<std::int64_t>(a[0].numero());
            std::lock_guard<std::mutex> lk(ffi_mutex);
            auto it = libs_abiertas.find(id);
            if (it == libs_abiertas.end()) return Valor::de_bool(false);
#ifdef _WIN32
            FreeLibrary(it->second);
#else
            dlclose(it->second);
#endif
            libs_abiertas.erase(it);
            return Valor::de_bool(true);
        });
        return m;
    }
    if (nombre == "__herramientas__") {
        auto m = crear(nombre);
        const auto analizar_codigo = [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Texto)
                throw ErrorEjecucion("__herramientas__.analizar(codigo, archivo?) requiere codigo como texto", u);
            if (a.size() > 2 || (a.size() == 2 && a[1].tipo != TipoValor::Texto))
                throw ErrorEjecucion("archivo debe ser texto en __herramientas__.analizar(codigo, archivo?)", u);
            const std::string archivo = a.size() > 1 ? a[1].texto : "codigo.esp";
            std::map<std::string, Valor> resultado;
            resultado["archivo"] = Valor::de_texto(archivo);
            try {
                Lexer lexer(a[0].texto, archivo);
                Parser parser(lexer.analizar());
                const auto ast = parser.analizar();
                resultado["valido"] = Valor::de_bool(true);
                resultado["sentencias"] =
                    Valor::de_entero(static_cast<std::int64_t>(ast ? ast->sentencias.size() : 0));
                resultado["diagnostico"] = Valor::de_texto("");
                resultado["linea"] = Valor::de_entero(0);
                resultado["columna"] = Valor::de_entero(0);
            } catch (const ErrorESP& error) {
                resultado["valido"] = Valor::de_bool(false);
                resultado["sentencias"] = Valor::de_entero(0);
                resultado["diagnostico"] = Valor::de_texto(formatear_error(error));
                resultado["linea"] = Valor::de_entero(error.ubicacion.linea);
                resultado["columna"] = Valor::de_entero(error.ubicacion.columna);
            }
            return Valor::de_mapa(std::move(resultado));
        };
        fn(*m->entorno, "analizar", analizar_codigo);
        fn(*m->entorno, "diagnosticar", analizar_codigo);
        fn(*m->entorno, "compilar", [this](std::vector<Valor>& a, Ubicacion u) {
            if (a.size() < 2 || a.size() > 4 || a[0].tipo != TipoValor::Texto ||
                a[1].tipo != TipoValor::Texto)
                throw ErrorEjecucion(
                    "__herramientas__.compilar(codigo, salida, formato?, optimizacion?) requiere codigo y salida "
                    "como texto",
                    u);
            if (a.size() > 2 && a[2].tipo != TipoValor::Texto)
                throw ErrorEjecucion("formato debe ser texto en __herramientas__.compilar", u);
            if (a.size() > 3 && a[3].tipo != TipoValor::Texto)
                throw ErrorEjecucion("optimizacion debe ser texto en __herramientas__.compilar", u);
            OpcionesCompilacion opciones;
            opciones.formato = a.size() > 2 ? a[2].texto : "exe";
            if (opciones.formato == "ejecutable") opciones.formato = "exe";
            else if (opciones.formato == "llvm") opciones.formato = "ll";
            else if (opciones.formato == "static" || opciones.formato == "biblioteca_estatica")
                opciones.formato = "estatica";
            opciones.nivel = a.size() > 3 ? a[3].texto : "O2";
            std::filesystem::path salida(a[1].texto);
            if (salida.extension().empty()) {
#ifdef _WIN32
                if (opciones.formato == "exe") salida += ".exe";
                else if (opciones.formato == "dll") salida += ".dll";
                else if (opciones.formato == "estatica") salida += ".lib";
#elif defined(__APPLE__)
                if (opciones.formato == "dylib") salida += ".dylib";
                else if (opciones.formato == "estatica") salida += ".a";
#else
                if (opciones.formato == "so") salida += ".so";
                else if (opciones.formato == "estatica") salida += ".a";
#endif
                if (opciones.formato == "cpp") salida += ".cpp";
                else if (opciones.formato == "ll") salida += ".ll";
            }
            if (salida.is_relative()) salida = std::filesystem::path(directorio_trabajo) / salida;
            opciones.entrada = salida.string();
            opciones.salida = salida.string();
            const auto resultado = compilar_fuente_esp(a[0].texto, opciones);
            if (!resultado.ok)
                throw ErrorEjecucion("no se pudo compilar el programa ESP: " + resultado.mensaje, u);
            return Valor::de_mapa({
                {"archivo", Valor::de_texto(resultado.archivo_generado)},
                {"formato", Valor::de_texto(opciones.formato)},
                {"mensaje", Valor::de_texto(resultado.mensaje)},
                {"cache", Valor::de_bool(resultado.uso_cache)},
                {"llvm", Valor::de_bool(resultado.uso_llvm)},
            });
        });
        return m;
    }
    if (nombre == "json") {
        auto m = crear(nombre);
        fn(*m->entorno, "parsear", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || a[0].tipo != TipoValor::Texto)
                throw ErrorEjecucion("json.parsear(texto) requiere un texto", u);
            struct JSONParser {
                const std::string& s;
                Ubicacion u;
                std::size_t p = 0;
                void ws() { while (p < s.size() && (s[p] == ' ' || s[p] == '\t' || s[p] == '\r' || s[p] == '\n')) ++p; }
                Valor parse(int d = 0) {
                    if (d > 256) throw ErrorEjecucion("JSON: profundidad maxima excedida", u);
                    ws();
                    if (p >= s.size()) throw ErrorEjecucion("JSON: inesperado fin", u);
                    char c = s[p];
                    if (c == '"') return Valor::de_texto(str());
                    if (c == '-' || (c >= '0' && c <= '9')) return num();
                    if (s.compare(p, 4, "true") == 0) { p += 4; return Valor::de_bool(true); }
                    if (s.compare(p, 5, "false") == 0) { p += 5; return Valor::de_bool(false); }
                    if (s.compare(p, 4, "null") == 0) { p += 4; return Valor::nulo(); }
                    if (c == '[') {
                        ++p; ws();
                        std::vector<Valor> arr;
                        if (p < s.size() && s[p] == ']') { ++p; return Valor::de_lista(std::move(arr)); }
                        while (true) {
                            arr.push_back(parse(d + 1)); ws();
                            if (p < s.size() && s[p] == ']') { ++p; break; }
                            if (p >= s.size() || s[p] != ',') throw ErrorEjecucion("JSON: se esperaba ','", u);
                            ++p;
                        }
                        return Valor::de_lista(std::move(arr));
                    }
                    if (c == '{') {
                        ++p; ws();
                        std::map<std::string, Valor> obj;
                        if (p < s.size() && s[p] == '}') { ++p; return Valor::de_mapa(std::move(obj)); }
                        while (true) {
                            ws();
                            if (p >= s.size() || s[p] != '"') throw ErrorEjecucion("JSON: clave debe ser texto", u);
                            std::string k = str(); ws();
                            if (p >= s.size() || s[p] != ':') throw ErrorEjecucion("JSON: se esperaba ':'", u);
                            ++p;
                            obj[std::move(k)] = parse(d + 1); ws();
                            if (p < s.size() && s[p] == '}') { ++p; break; }
                            if (p >= s.size() || s[p] != ',') throw ErrorEjecucion("JSON: se esperaba ','", u);
                            ++p;
                        }
                        return Valor::de_mapa(std::move(obj));
                    }
                    throw ErrorEjecucion("JSON: token no valido", u);
                }
                std::string str() {
                    ++p; std::string out;
                    while (p < s.size() && s[p] != '"') {
                        if (s[p] == '\\') {
                            ++p; if (p >= s.size()) throw ErrorEjecucion("JSON: escape incompleto", u);
                            char e = s[p++];
                            if (e == 'n') out += '\n'; else if (e == 't') out += '\t';
                            else if (e == 'r') out += '\r'; else out += e;
                        } else out += s[p++];
                    }
                    if (p >= s.size()) throw ErrorEjecucion("JSON: cadena sin cerrar", u);
                    ++p; return out;
                }
                Valor num() {
                    std::size_t ini = p;
                    if (s[p] == '-') ++p;
                    while (p < s.size() && std::isdigit(static_cast<unsigned char>(s[p]))) ++p;
                    bool dec = false;
                    if (p < s.size() && s[p] == '.') { dec = true; ++p; while (p < s.size() && std::isdigit(static_cast<unsigned char>(s[p]))) ++p; }
                    if (p < s.size() && (s[p] == 'e' || s[p] == 'E')) { dec = true; ++p; if (p < s.size() && (s[p] == '+' || s[p] == '-')) ++p; while (p < s.size() && std::isdigit(static_cast<unsigned char>(s[p]))) ++p; }
                    std::string sub = s.substr(ini, p - ini);
                    return dec ? Valor::de_decimal(std::stod(sub)) : Valor::de_entero(std::stoll(sub));
                }
            } parser{a[0].texto, u};
            Valor res = parser.parse();
            parser.ws();
            if (parser.p != a[0].texto.size()) throw ErrorEjecucion("JSON: contenido extra", u);
            return res;
        });
        Valor fn_p;
        m->entorno->obtener("parsear", &fn_p);
        m->entorno->definir("analizar", fn_p, Visibilidad::Publico, true, true);
        m->entorno->definir("deserializar", fn_p, Visibilidad::Publico, true, true);

        std::function<void(const Valor&, std::string&, Ubicacion)> ser = [&](const Valor& v, std::string& out, Ubicacion u) {
            if (v.tipo == TipoValor::Nulo) out += "null";
            else if (v.tipo == TipoValor::Booleano) out += v.booleano ? "true" : "false";
            else if (v.tipo == TipoValor::Entero) out += std::to_string(v.entero);
            else if (v.tipo == TipoValor::Decimal) { std::ostringstream ss; ss << v.decimal; out += ss.str(); }
            else if (v.tipo == TipoValor::Texto) {
                out += '"';
                for (unsigned char c : v.texto) {
                    if (c == '"') out += "\\\""; else if (c == '\\') out += "\\\\";
                    else if (c == '\n') out += "\\n"; else if (c == '\r') out += "\\r";
                    else if (c == '\t') out += "\\t"; else out += static_cast<char>(c);
                }
                out += '"';
            } else if (v.tipo == TipoValor::Lista) {
                out += '[';
                if (v.lista) for (std::size_t i = 0; i < v.lista->size(); ++i) { if (i) out += ','; ser((*v.lista)[i], out, u); }
                out += ']';
            } else if (v.tipo == TipoValor::Mapa) {
                out += '{'; bool first = true;
                if (v.mapa) for (const auto& kv : *v.mapa) { if (!first) out += ','; first = false; ser(Valor::de_texto(kv.first), out, u); out += ':'; ser(kv.second, out, u); }
                out += '}';
            } else throw ErrorEjecucion("tipo no serializable a JSON", u);
        };
        fn(*m->entorno, "texto", [ser](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty()) throw ErrorEjecucion("json.texto(valor)", u);
            std::string out; ser(a[0], out, u); return Valor::de_texto(std::move(out));
        });
        Valor fn_t;
        m->entorno->obtener("texto", &fn_t);
        m->entorno->definir("serializar", fn_t, Visibilidad::Publico, true, true);
        m->entorno->definir("codificar", fn_t, Visibilidad::Publico, true, true);
        return m;
    }
    if (nombre == "pruebas") {
        auto m = crear(nombre);
        fn(*m->entorno, "afirmar", [](std::vector<Valor>& a, Ubicacion u) {
            if (a.empty() || !a[0].verdad())
                throw ErrorEjecucion(a.size() > 1 ? a[1].a_texto() : "prueba fallida", u);
            return Valor::de_bool(true);
        });
        return m;
    }
    if (nombre == "compilacion") {
        auto m = crear(nombre);
        fn(*m->entorno, "compilador", [](std::vector<Valor>&, Ubicacion) { return Valor::de_texto(detectar_compilador()); });
        return m;
    }
    return nullptr;
}

}  // namespace esp