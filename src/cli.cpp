#include "esp/cli.hpp"
#include "esp/codegen.hpp"
#include "esp/lexer.hpp"
#include "esp/parser.hpp"
#include "esp/runtime.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
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
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace esp {
namespace {

#ifdef _WIN32
using SocketNodo = SOCKET;
constexpr SocketNodo NODO_SOCK_INV = INVALID_SOCKET;
void nodo_red_init() {
    static bool ok = false;
    if (!ok) { WSADATA w; WSAStartup(MAKEWORD(2, 2), &w); ok = true; }
}
void nodo_cerrar(SocketNodo s) { closesocket(s); }
#else
using SocketNodo = int;
constexpr SocketNodo NODO_SOCK_INV = -1;
void nodo_red_init() {}
void nodo_cerrar(SocketNodo s) { close(s); }
#endif

bool enviar_todo(SocketNodo s, const void* data, std::size_t len) {
    const char* ptr = static_cast<const char*>(data);
    std::size_t sent = 0;
    while (sent < len) {
        int n = send(s, ptr + sent, static_cast<int>(len - sent), 0);
        if (n <= 0) return false;
        sent += static_cast<std::size_t>(n);
    }
    return true;
}

bool recibir_todo(SocketNodo s, void* data, std::size_t len) {
    char* ptr = static_cast<char*>(data);
    std::size_t recvd = 0;
    while (recvd < len) {
        int n = recv(s, ptr + recvd, static_cast<int>(len - recvd), 0);
        if (n <= 0) return false;
        recvd += static_cast<std::size_t>(n);
    }
    return true;
}

bool enviar_paquete_nodo(SocketNodo s, std::uint8_t tipo, const std::string& payload) {
    std::uint8_t hdr[5]{};
    hdr[0] = tipo;
    std::uint32_t len = htonl(static_cast<std::uint32_t>(payload.size()));
    std::memcpy(hdr + 1, &len, 4);
    if (!enviar_todo(s, hdr, 5)) return false;
    return payload.empty() ? true : enviar_todo(s, payload.data(), payload.size());
}

bool recibir_paquete_nodo(SocketNodo s, std::uint8_t& tipo, std::string& payload) {
    std::uint8_t hdr[5]{};
    if (!recibir_todo(s, hdr, 5)) return false;
    tipo = hdr[0];
    std::uint32_t len_net = 0;
    std::memcpy(&len_net, hdr + 1, 4);
    std::uint32_t len = ntohl(len_net);
    if (len > 32 * 1024 * 1024) return false;
    payload.assign(len, '\0');
    return len == 0 ? true : recibir_todo(s, payload.data(), len);
}

// ============================================================================
// FORMATEADOR AUTOMATICO DE CODIGO ESP (.esp)
// ============================================================================

std::string formatear_codigo_esp(const std::string& src) {
    std::istringstream in(src);
    std::ostringstream out;
    std::string linea;
    int nivel = 0;

    auto trim = [](std::string s) {
        auto a = s.find_first_not_of(" \t\r\n");
        auto b = s.find_last_not_of(" \t\r\n");
        return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
    };

    auto empieza_con = [](const std::string& s, const std::string& pref) {
        if (s.compare(0, pref.size(), pref) != 0) return false;
        if (s.size() == pref.size()) return true;
        unsigned char c = static_cast<unsigned char>(s[pref.size()]);
        return !std::isalnum(c) && c != '_';
    };

    while (std::getline(in, linea)) {
        std::string t = trim(linea);
        if (t.empty()) {
            out << "\n";
            continue;
        }
        bool cierra = empieza_con(t, "fin") || empieza_con(t, "sino") ||
                      empieza_con(t, "si_no_si") || empieza_con(t, "si_no_puede");
        if (cierra && nivel > 0) --nivel;

        for (int i = 0; i < nivel; ++i) out << "    ";
        out << t << "\n";

        bool abre = empieza_con(t, "funcion") || empieza_con(t, "clase") ||
                    empieza_con(t, "si") || empieza_con(t, "sino") ||
                    empieza_con(t, "si_no_si") || empieza_con(t, "mientras") ||
                    empieza_con(t, "para") || empieza_con(t, "intenta") ||
                    empieza_con(t, "si_no_puede") || empieza_con(t, "pagina") ||
                    empieza_con(t, "estilo") || empieza_con(t, "script") ||
                    empieza_con(t, "contenedor") || (t.size() >= 7 && t.substr(t.size() - 7) == "al_clic");
        if (abre) ++nivel;
    }
    return out.str();
}

// ============================================================================
// LANGUAGE SERVER PROTOCOL (LSP) PARA VS CODE Y EDITORES
// ============================================================================

int ejecutar_lsp_stdio() {
    std::string linea;
    while (std::getline(std::cin, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.rfind("Content-Length:", 0) != 0) continue;
        std::size_t len = static_cast<std::size_t>(std::stoull(linea.substr(15)));
        std::getline(std::cin, linea);  // linea vacia
        std::string body(len, '\0');
        std::cin.read(body.data(), static_cast<std::streamsize>(len));

        if (body.find("\"method\":\"initialize\"") != std::string::npos ||
            body.find("\"method\": \"initialize\"") != std::string::npos) {
            std::string resp =
                R"({"jsonrpc":"2.0","id":1,"result":{"capabilities":{"textDocumentSync":1,"completionProvider":{"triggerCharacters":["."]}}}})";
            std::cout << "Content-Length: " << resp.size() << "\r\n\r\n" << resp << std::flush;
        } else if (body.find("\"method\":\"textDocument/completion\"") != std::string::npos ||
                   body.find("\"method\": \"textDocument/completion\"") != std::string::npos) {
            std::string resp =
                R"({"jsonrpc":"2.0","id":2,"result":[{"label":"funcion","kind":14},{"label":"variable","kind":14},{"label":"pagina","kind":14},{"label":"estilo","kind":14},{"label":"script","kind":14},{"label":"crear_tabla","kind":14},{"label":"seleccionar","kind":14},{"label":"conseguir","kind":14},{"label":"mostrar","kind":3}]})";
            std::cout << "Content-Length: " << resp.size() << "\r\n\r\n" << resp << std::flush;
        } else if (body.find("\"method\":\"shutdown\"") != std::string::npos) {
            break;
        }
    }
    return 0;
}

// ============================================================================
// GESTOR DE PAQUETES CON VERIFICACION SHA-256
// ============================================================================

int ejecutar_instalar_paquete(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Uso: esp instalar <nombre_paquete> [--url <url>] [--sha256 <hash>]\n";
        return 1;
    }
    std::string nombre = args[0];
    std::string url;
    std::string sha_esperado;
    for (std::size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--url" && i + 1 < args.size()) url = args[++i];
        else if (args[i] == "--sha256" && i + 1 < args.size()) sha_esperado = args[++i];
    }
    if (url.empty()) {
        url = "https://raw.githubusercontent.com/esp-lang/registro/main/paquetes/" + nombre + "/latest/" + nombre + ".esp";
    }
    std::string contenido, stderr_curl;
    int rc = ejecutar_proceso("curl --silent --show-error --fail --location --max-time 30 " +
                                  citar_argumento_proceso(url),
                              "", &contenido, &stderr_curl, 35000);
    if (rc != 0) {
        std::cerr << "[ErrorPaquete] No se pudo descargar '" << nombre << "': " << stderr_curl << "\n";
        return 1;
    }
    std::string hash_actual = sha256_hex(contenido);
    if (!sha_esperado.empty()) {
        for (char& c : sha_esperado) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (hash_actual != sha_esperado) {
            std::cerr << "[ErrorSeguridad] SHA-256 invalido. Esperado: " << sha_esperado << ", Obtenido: " << hash_actual << "\n";
            return 1;
        }
    }
    std::filesystem::path dir_pkg = std::filesystem::path(directorio_home_esp()) / "paquetes" / nombre;
    std::error_code ec;
    std::filesystem::create_directories(dir_pkg, ec);
    escribir_archivo((dir_pkg / (nombre + ".esp")).string(), contenido);
    std::cout << "Paquete '" << nombre << "' instalado correctamente.\n"
              << "SHA-256: " << hash_actual << "\n";
    return 0;
}

// ============================================================================
// NODOS DE RED IP (CLUSTER DISTRIBUIDO)
// ============================================================================

int ejecutar_nodo_escuchar(int puerto, const std::string& token) {
    nodo_red_init();
    SocketNodo ls = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ls == NODO_SOCK_INV) {
        std::cerr << "[ErrorNodo] No se pudo crear el socket de escucha\n";
        return 1;
    }
    int opt = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<u_short>(puerto));

    if (bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        nodo_cerrar(ls);
        std::cerr << "[ErrorNodo] No se pudo enlazar el puerto IP " << puerto << "\n";
        return 1;
    }
    listen(ls, 32);
    std::cout << "[ESP Nodo 2.0.0] Escuchando conexiones IP en 0.0.0.0:" << puerto
              << " (token=" << (token.empty() ? "abierto" : "protegido") << ")\n";

    while (true) {
        sockaddr_in cli_addr{};
#ifdef _WIN32
        int cli_len = sizeof(cli_addr);
#else
        socklen_t cli_len = sizeof(cli_addr);
#endif
        SocketNodo cs = accept(ls, reinterpret_cast<sockaddr*>(&cli_addr), &cli_len);
        if (cs == NODO_SOCK_INV) continue;

        char ip_remota[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &cli_addr.sin_addr, ip_remota, sizeof(ip_remota));

        std::thread([cs, token, ip_str = std::string(ip_remota)]() {
            std::uint8_t tipo = 0;
            std::string payload;
            if (!recibir_paquete_nodo(cs, tipo, payload)) {
                nodo_cerrar(cs);
                return;
            }
            if (tipo == 0x01) {
                if (!token.empty() && payload != token) {
                    enviar_paquete_nodo(cs, 0xFF, "Token de nodo IP invalido");
                } else {
                    enviar_paquete_nodo(cs, 0x02, "OK:NodoESP-" + std::string(VERSION));
                }
            } else if (tipo == 0x10) {
                std::size_t nl = payload.find('\n');
                std::string tok_rec = nl == std::string::npos ? "" : payload.substr(0, nl);
                std::string codigo = nl == std::string::npos ? "" : payload.substr(nl + 1);
                if (!token.empty() && tok_rec != token) {
                    enviar_paquete_nodo(cs, 0xFF, "Acceso denegado: token incorrecto");
                } else {
                    std::cout << "[ESP Nodo] Ejecutando tarea remota desde IP " << ip_str << "...\n";
                    try {
                        Interprete interp;
                        Valor res = interp.ejecutar_fuente(codigo, "<nodo_remoto>");
                        enviar_paquete_nodo(cs, 0x11, res.a_texto());
                    } catch (const ErrorESP& e) {
                        enviar_paquete_nodo(cs, 0xFF, formatear_error(e));
                    } catch (const std::exception& e) {
                        enviar_paquete_nodo(cs, 0xFF, e.what());
                    }
                }
            }
            nodo_cerrar(cs);
        }).detach();
    }
    nodo_cerrar(ls);
    return 0;
}

SocketNodo conectar_ip_nodo(const std::string& ip_puerto, std::string& ip_out, int& puerto_out) {
    auto colon = ip_puerto.find(':');
    ip_out = colon == std::string::npos ? ip_puerto : ip_puerto.substr(0, colon);
    puerto_out = colon == std::string::npos ? 9500 : std::stoi(ip_puerto.substr(colon + 1));

    nodo_red_init();
    SocketNodo s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == NODO_SOCK_INV) return NODO_SOCK_INV;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(puerto_out));
    if (inet_pton(AF_INET, ip_out.c_str(), &addr.sin_addr) != 1) {
        addrinfo hints{}, *res = nullptr;
        hints.ai_family = AF_INET;
        if (getaddrinfo(ip_out.c_str(), nullptr, &hints, &res) == 0 && res) {
            addr.sin_addr = reinterpret_cast<sockaddr_in*>(res->sin_addr)->sin_addr;
            freeaddrinfo(res);
        } else {
            nodo_cerrar(s);
            return NODO_SOCK_INV;
        }
    }
    if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        nodo_cerrar(s);
        return NODO_SOCK_INV;
    }
    return s;
}

int ejecutar_subcomando_nodo(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Uso:\n"
                  << "  esp nodo escuchar [--puerto 9500] [--token clave]\n"
                  << "  esp nodo conectar <ip:puerto> [--token clave]\n"
                  << "  esp nodo ejecutar <ip:puerto> <archivo.esp> [--token clave]\n";
        return 1;
    }
    std::string accion = args[0];
    int puerto = 9500;
    std::string token;
    for (std::size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--puerto" && i + 1 < args.size()) puerto = std::stoi(args[++i]);
        else if (args[i] == "--token" && i + 1 < args.size()) token = args[++i];
    }

    if (accion == "escuchar") return ejecutar_nodo_escuchar(puerto, token);

    if (accion == "conectar") {
        if (args.size() < 2) { std::cerr << "Falta <ip:puerto>\n"; return 1; }
        std::string ip; int p = 0;
        auto t0 = std::chrono::steady_clock::now();
        SocketNodo s = conectar_ip_nodo(args[1], ip, p);
        if (s == NODO_SOCK_INV) { std::cerr << "[ErrorNodo] No se pudo conectar a " << args[1] << "\n"; return 1; }
        enviar_paquete_nodo(s, 0x01, token);
        std::uint8_t tipo = 0; std::string resp;
        bool ok = recibir_paquete_nodo(s, tipo, resp);
        nodo_cerrar(s);
        double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
        if (!ok || tipo != 0x02) { std::cerr << "[ErrorNodo] " << resp << "\n"; return 1; }
        std::cout << "Conectado a " << ip << ":" << p << " -> " << resp << " (" << ms << " ms)\n";
        return 0;
    }

    if (accion == "ejecutar") {
        if (args.size() < 3) { std::cerr << "Falta <ip:puerto> <archivo.esp>\n"; return 1; }
        std::string codigo = leer_archivo(args[2]);
        std::string ip; int p = 0;
        SocketNodo s = conectar_ip_nodo(args[1], ip, p);
        if (s == NODO_SOCK_INV) { std::cerr << "[ErrorNodo] No se pudo conectar a " << args[1] << "\n"; return 1; }
        enviar_paquete_nodo(s, 0x10, token + "\n" + codigo);
        std::uint8_t tipo = 0; std::string resp;
        bool ok = recibir_paquete_nodo(s, tipo, resp);
        nodo_cerrar(s);
        if (!ok || tipo != 0x11) { std::cerr << "[ErrorRemoto] " << resp << "\n"; return 1; }
        std::cout << resp << "\n";
        return 0;
    }

    std::cerr << "Accion de nodo desconocida: " << accion << "\n";
    return 1;
}

void imprimir_ayuda() {
    std::cout << "ESP " << VERSION << " - Lenguaje de Programacion General, Web, SQL y Sistemas\n\n"
              << "Uso:\n"
              << "  esp                          Inicia el modo interactivo (REPL)\n"
              << "  esp <archivo.esp>            Ejecuta un programa ESP\n"
              << "  esp compilar <archivo.esp> [-o salida] [--formato exe|dll|so|dylib|estatica|ll|cpp] [-O0|-O1|-O2|-O3]\n"
              << "  esp formatear <archivo.esp>  Formatea e indenta automaticamente un archivo ESP\n"
              << "  esp instalar <paquete>       Instala un paquete verificando su firma SHA-256\n"
              << "  esp lsp                      Inicia el servidor de lenguaje (LSP) para editores\n"
              << "  esp nodo <escuchar|conectar|ejecutar> ...\n"
              << "  esp verificar <archivo.esp>  Verifica sintaxis sin ejecutar\n"
              << "  esp version                  Muestra la version del lenguaje\n"
              << "  esp ayuda                    Muestra esta ayuda\n";
}

int iniciar_repl() {
    Interprete interp;
    std::cout << "ESP " << VERSION << " - Consola interactiva (escribe 'salir' para terminar)\n";
    std::string linea;
    std::string buffer;
    int profundidad = 0;

    auto ajustar_profundidad = [](const std::string& l, int& prof) {
        std::istringstream ss(l);
        std::string tok;
        while (ss >> tok) {
            if (tok == "funcion" || tok == "clase" || tok == "si" || tok == "mientras" ||
                tok == "para" || tok == "intenta" || tok == "pagina" || tok == "estilo" ||
                tok == "script" || tok == "contenedor" || tok == "al_clic") {
                ++prof;
            } else if (tok == "fin") {
                if (prof > 0) --prof;
            }
        }
    };

    while (true) {
        std::cout << (profundidad > 0 ? "... " : "esp> ") << std::flush;
        if (!std::getline(std::cin, linea)) break;
        if (profundidad == 0 && (linea == "salir" || linea == "exit")) break;
        if (linea.empty() && profundidad == 0) continue;

        buffer += linea + "\n";
        ajustar_profundidad(linea, profundidad);
        if (profundidad > 0) continue;

        try {
            Valor r = interp.evaluar_repl(buffer);
            if (r.tipo != TipoValor::Nulo) std::cout << r.a_texto() << '\n';
        } catch (const ErrorESP& e) {
            std::cerr << formatear_error(e) << '\n';
        } catch (const std::exception& e) {
            std::cerr << "[Error] " << e.what() << '\n';
        }
        buffer.clear();
    }
    return 0;
}

}  // namespace

int ejecutar_cli(const std::vector<std::string>& args) {
    if (args.empty()) return iniciar_repl();

    const std::string& cmd = args[0];
    if (cmd == "ayuda" || cmd == "--ayuda" || cmd == "-h" || cmd == "--help") {
        imprimir_ayuda();
        return 0;
    }
    if (cmd == "version" || cmd == "--version" || cmd == "-v") {
        std::cout << "ESP " << VERSION << " (compilador nativo: "
                  << (detectar_compilador().empty() ? "no detectado" : detectar_compilador()) << ")\n";
        return 0;
    }
    if (cmd == "lsp") {
        return ejecutar_lsp_stdio();
    }
    if (cmd == "formatear") {
        if (args.size() < 2) { std::cerr << "Uso: esp formatear <archivo.esp>\n"; return 1; }
        std::string orig = leer_archivo(args[1]);
        std::string fmt = formatear_codigo_esp(orig);
        escribir_archivo(args[1], fmt);
        std::cout << "Archivo formateado: " << args[1] << "\n";
        return 0;
    }
    if (cmd == "instalar") {
        std::vector<std::string> sub(args.begin() + 1, args.end());
        return ejecutar_instalar_paquete(sub);
    }
    if (cmd == "nodo") {
        std::vector<std::string> sub_args(args.begin() + 1, args.end());
        return ejecutar_subcomando_nodo(sub_args);
    }
    if (cmd == "verificar") {
        if (args.size() < 2) {
            std::cerr << "Uso: esp verificar <archivo.esp>\n";
            return 1;
        }
        try {
            Lexer lex(leer_archivo(args[1]), args[1]);
            Parser par(lex.analizar());
            (void)par.analizar();
            std::cout << "Sintaxis correcta: " << args[1] << '\n';
            return 0;
        } catch (const ErrorESP& e) {
            std::cerr << formatear_error(e) << '\n';
            return 1;
        }
    }
    if (cmd == "compilar") {
        if (args.size() < 2) {
            std::cerr << "Uso: esp compilar <archivo.esp> [-o salida] [--formato exe|dll|so|dylib|estatica|ll|cpp] [-O0|-O1|-O2|-O3]\n";
            return 1;
        }
        OpcionesCompilacion opt;
        opt.entrada = args[1];
        for (std::size_t i = 2; i < args.size(); ++i) {
            if ((args[i] == "-o" || args[i] == "--salida") && i + 1 < args.size()) {
                opt.salida = args[++i];
            } else if ((args[i] == "--formato" || args[i] == "-f") && i + 1 < args.size()) {
                opt.formato = args[++i];
            } else if (args[i] == "-O0" || args[i] == "-O1" || args[i] == "-O2" || args[i] == "-O3") {
                opt.nivel = args[i].substr(1);
            } else if ((args[i] == "--optimizacion" || args[i] == "-O") && i + 1 < args.size()) {
                opt.nivel = args[++i];
            } else {
                std::cerr << "Opcion desconocida para compilar: " << args[i] << '\n';
                return 1;
            }
        }
        try {
            auto r = compilar_fuente_esp(leer_archivo(opt.entrada), opt);
            if (!r.ok) {
                std::cerr << r.mensaje << '\n';
                return 1;
            }
            std::cout << r.mensaje << '\n';
            return 0;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    try {
        Interprete interp;
        (void)interp.ejecutar_archivo(cmd);
        return 0;
    } catch (const ErrorESP& e) {
        std::cerr << formatear_error(e) << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << '\n';
        return 1;
    }
}

}  // namespace esp