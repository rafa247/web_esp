#include "esp/os_mod.hpp"
#include "esp/runtime.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <process.h>
#else
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace esp {
namespace {

void registrar_fn(Entorno& e, const std::string& n, std::function<Valor(std::vector<Valor>&, Ubicacion)> f) {
    auto fn = std::make_shared<Funcion>();
    fn->nombre = n;
    fn->es_nativa = true;
    fn->nativa = std::move(f);
    Valor v;
    v.tipo = TipoValor::Funcion;
    v.funcion = fn;
    e.definir(n, std::move(v), Visibilidad::Publico, true, true);
}

std::vector<std::uint8_t> valor_a_bytes(const Valor& v, Ubicacion u) {
    std::vector<std::uint8_t> out;
    if (v.tipo == TipoValor::Texto) {
        out.assign(v.texto.begin(), v.texto.end());
        return out;
    }
    if (v.tipo == TipoValor::Lista && v.lista) {
        out.reserve(v.lista->size());
        for (const auto& item : *v.lista) {
            out.push_back(static_cast<std::uint8_t>(static_cast<int>(item.numero()) & 0xFF));
        }
        return out;
    }
    throw ErrorEjecucion("se esperaba lista de bytes o texto", u);
}

Valor bytes_a_lista_valor(const std::vector<std::uint8_t>& bytes) {
    std::vector<Valor> lista;
    lista.reserve(bytes.size());
    for (std::uint8_t b : bytes) lista.push_back(Valor::de_entero(b));
    return Valor::de_lista(std::move(lista));
}

}  // namespace

std::shared_ptr<Modulo> crear_modulo_os(const std::string& nombre_alias) {
    struct BloqueMemoria {
        std::vector<std::uint8_t> bytes;
    };
    static std::mutex mem_mutex;
    static std::int64_t next_mem_id = 1;
    static std::map<std::int64_t, BloqueMemoria> memoria_virtual;

    auto m = std::make_shared<Modulo>();
    m->nombre = nombre_alias;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "nombre", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
        return Valor::de_texto("windows");
#elif defined(__APPLE__)
        return Valor::de_texto("macos");
#else
        return Valor::de_texto("linux");
#endif
    });

    registrar_fn(*m->entorno, "pid", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
        return Valor::de_entero(static_cast<std::int64_t>(GetCurrentProcessId()));
#else
        return Valor::de_entero(static_cast<std::int64_t>(getpid()));
#endif
    });

    registrar_fn(*m->entorno, "directorio_actual", [](std::vector<Valor>&, Ubicacion) {
        return Valor::de_texto(std::filesystem::current_path().string());
    });

    registrar_fn(*m->entorno, "cambiar_directorio", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.cambiar_directorio(ruta)", u);
        std::error_code ec;
        std::filesystem::current_path(a[0].a_texto(), ec);
        if (ec) throw ErrorEjecucion("no se pudo cambiar al directorio: " + a[0].a_texto(), u);
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "variable", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.variable(nombre)", u);
        const char* v = std::getenv(a[0].a_texto().c_str());
        return v ? Valor::de_texto(v) : Valor::nulo();
    });

    registrar_fn(*m->entorno, "establecer_variable", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("os.establecer_variable(clave, valor)", u);
#ifdef _WIN32
        return Valor::de_bool(_putenv_s(a[0].a_texto().c_str(), a[1].a_texto().c_str()) == 0);
#else
        return Valor::de_bool(setenv(a[0].a_texto().c_str(), a[1].a_texto().c_str(), 1) == 0);
#endif
    });

    registrar_fn(*m->entorno, "listar", [](std::vector<Valor>& a, Ubicacion u) {
        std::string dir = a.empty() ? "." : a[0].a_texto();
        std::error_code ec;
        std::vector<Valor> items;
        for (std::filesystem::directory_iterator it(dir, ec), fin; it != fin && !ec; it.increment(ec)) {
            items.push_back(Valor::de_texto(it->path().filename().string()));
        }
        if (ec) throw ErrorEjecucion("no se pudo listar: " + dir, u);
        return Valor::de_lista(std::move(items));
    });

    registrar_fn(*m->entorno, "recorrer", [](std::vector<Valor>& a, Ubicacion u) {
        std::string dir = a.empty() ? "." : a[0].a_texto();
        std::error_code ec;
        std::vector<Valor> items;
        for (std::filesystem::recursive_directory_iterator it(dir, ec), fin; it != fin && !ec; it.increment(ec)) {
            items.push_back(Valor::de_texto(it->path().string()));
        }
        if (ec) throw ErrorEjecucion("no se pudo recorrer: " + dir, u);
        return Valor::de_lista(std::move(items));
    });

    registrar_fn(*m->entorno, "unir_ruta", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.unir_ruta(p1, p2, ...)", u);
        std::filesystem::path p(a[0].a_texto());
        for (std::size_t i = 1; i < a.size(); ++i) p /= a[i].a_texto();
        return Valor::de_texto(p.string());
    });

    registrar_fn(*m->entorno, "existe", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.existe(ruta)", u);
        std::error_code ec;
        return Valor::de_bool(std::filesystem::exists(a[0].a_texto(), ec) && !ec);
    });

    registrar_fn(*m->entorno, "crear_directorio", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.crear_directorio(ruta)", u);
        std::error_code ec;
        std::filesystem::create_directories(a[0].a_texto(), ec);
        return Valor::de_bool(!ec);
    });

    registrar_fn(*m->entorno, "eliminar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.eliminar(ruta)", u);
        std::error_code ec;
        return Valor::de_bool(std::filesystem::remove_all(a[0].a_texto(), ec) > 0 && !ec);
    });

    registrar_fn(*m->entorno, "copiar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("os.copiar(origen, destino)", u);
        std::error_code ec;
        std::filesystem::copy(a[0].a_texto(), a[1].a_texto(),
                              std::filesystem::copy_options::overwrite_existing |
                                  std::filesystem::copy_options::recursive,
                              ec);
        if (ec) throw ErrorEjecucion("no se pudo copiar: " + ec.message(), u);
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "mover", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("os.mover(origen, destino)", u);
        std::error_code ec;
        std::filesystem::rename(a[0].a_texto(), a[1].a_texto(), ec);
        if (ec) throw ErrorEjecucion("no se pudo mover: " + ec.message(), u);
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "ejecutar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.ejecutar(comando, timeout_ms?)", u);
        std::string out, err;
        int code = ejecutar_proceso(a[0].a_texto(), "", &out, &err, a.size() > 1 ? static_cast<int>(a[1].numero()) : 0);
        return Valor::de_mapa({
            {"codigo", Valor::de_entero(code)},
            {"salida", Valor::de_texto(out)},
            {"error", Valor::de_texto(err)}
        });
    });

    registrar_fn(*m->entorno, "asignar_memoria", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.asignar_memoria(tamano_bytes)", u);
        std::size_t tam = static_cast<std::size_t>(a[0].numero());
        if (tam == 0 || tam > 64 * 1024 * 1024)
            throw ErrorEjecucion("tamano de memoria invalido (1B - 64MB)", u);
        std::lock_guard<std::mutex> lk(mem_mutex);
        std::int64_t id = next_mem_id++;
        memoria_virtual[id] = BloqueMemoria{std::vector<std::uint8_t>(tam, 0)};
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "escribir_byte", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("os.escribir_byte(bloque_id, direccion, valor_0_255)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::size_t dir = static_cast<std::size_t>(a[1].numero());
        std::uint8_t val = static_cast<std::uint8_t>(static_cast<int>(a[2].numero()) & 0xFF);
        std::lock_guard<std::mutex> lk(mem_mutex);
        auto it = memoria_virtual.find(id);
        if (it == memoria_virtual.end() || dir >= it->second.bytes.size())
            throw ErrorEjecucion("violacion de segmento (escritura fuera de rango)", u);
        it->second.bytes[dir] = val;
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "leer_byte", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("os.leer_byte(bloque_id, direccion)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::size_t dir = static_cast<std::size_t>(a[1].numero());
        std::lock_guard<std::mutex> lk(mem_mutex);
        auto it = memoria_virtual.find(id);
        if (it == memoria_virtual.end() || dir >= it->second.bytes.size())
            throw ErrorEjecucion("violacion de segmento (lectura fuera de rango)", u);
        return Valor::de_entero(it->second.bytes[dir]);
    });

    registrar_fn(*m->entorno, "liberar_memoria", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("os.liberar_memoria(bloque_id)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(mem_mutex);
        return Valor::de_bool(memoria_virtual.erase(id) > 0);
    });

    registrar_fn(*m->entorno, "generar_boot_sector", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("os.generar_boot_sector(ruta_bin, mensaje_arranque)", u);
        std::string ruta = a[0].a_texto();
        std::string msg = a[1].a_texto();
        std::vector<std::uint8_t> sector(512, 0);
        std::size_t p = 0;
        sector[p++] = 0xFA;  // cli
        sector[p++] = 0xFC;  // cld
        for (unsigned char ch : msg) {
            if (p + 6 >= 510) break;
            sector[p++] = 0xB4; sector[p++] = 0x0E;  // mov ah, 0x0E
            sector[p++] = 0xB0; sector[p++] = ch;    // mov al, ch
            sector[p++] = 0xCD; sector[p++] = 0x10;  // int 0x10
        }
        sector[p++] = 0xF4;  // hlt
        sector[p++] = 0xEB; sector[p++] = 0xFE;  // jmp $
        sector[510] = 0x55;
        sector[511] = 0xAA;

        std::ofstream out(ruta, std::ios::binary);
        if (!out) throw ErrorEjecucion("no se pudo escribir imagen de arranque: " + ruta, u);
        out.write(reinterpret_cast<const char*>(sector.data()), static_cast<std::streamsize>(sector.size()));
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "generar_kernel_32", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("os.generar_kernel_32(ruta_bin, mensaje_vga)", u);
        std::string ruta = a[0].a_texto();
        std::string msg = a[1].a_texto();
        std::vector<std::uint8_t> img(1024, 0);
        std::size_t p = 0;
        img[p++] = 0xB8; img[p++] = 0x03; img[p++] = 0x00;  // mov ax, 0x0003
        img[p++] = 0xCD; img[p++] = 0x10;                    // int 0x10
        img[p++] = 0xB8; img[p++] = 0x00; img[p++] = 0xB8;  // mov ax, 0xB800
        img[p++] = 0x8E; img[p++] = 0xC0;                    // mov es, ax
        std::uint16_t vga_off = 0;
        for (unsigned char ch : msg) {
            if (p + 10 >= 510) break;
            img[p++] = 0x26; img[p++] = 0xC7; img[p++] = 0x06;
            img[p++] = static_cast<std::uint8_t>(vga_off & 0xFF);
            img[p++] = static_cast<std::uint8_t>((vga_off >> 8) & 0xFF);
            img[p++] = ch;
            img[p++] = 0x0F;
            vga_off += 2;
        }
        img[p++] = 0xFA;
        img[p++] = 0xF4;
        img[p++] = 0xEB; img[p++] = 0xFE;
        img[510] = 0x55;
        img[511] = 0xAA;

        std::ofstream out(ruta, std::ios::binary);
        if (!out) throw ErrorEjecucion("no se pudo escribir imagen kernel 32: " + ruta, u);
        out.write(reinterpret_cast<const char*>(img.data()), static_cast<std::streamsize>(img.size()));
        return Valor::de_bool(true);
    });

    return m;
}

std::shared_ptr<Modulo> crear_modulo_binario(const std::string& nombre_alias) {
    auto m = std::make_shared<Modulo>();
    m->nombre = nombre_alias;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    // Equivalente a struct.pack(formato, valores...) en Python (< = Little Endian, > = Big Endian; B, H, I, Q)
    registrar_fn(*m->entorno, "empaquetar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("binario.empaquetar(formato, val1, val2, ...)", u);
        std::string fmt = a[0].a_texto();
        bool big_endian = false;
        std::size_t idx_fmt = 0;
        if (!fmt.empty() && (fmt[0] == '<' || fmt[0] == '>' || fmt[0] == '!')) {
            big_endian = (fmt[0] == '>' || fmt[0] == '!');
            idx_fmt = 1;
        }

        std::vector<std::uint8_t> buf;
        std::size_t arg_idx = 1;
        for (; idx_fmt < fmt.size(); ++idx_fmt) {
            char code = fmt[idx_fmt];
            if (code == ' ') continue;
            if (arg_idx >= a.size()) throw ErrorEjecucion("faltan valores para formato binario", u);
            std::uint64_t val = static_cast<std::uint64_t>(static_cast<std::int64_t>(a[arg_idx++].numero()));

            int bytes = 0;
            if (code == 'B' || code == 'b') bytes = 1;
            else if (code == 'H' || code == 'h') bytes = 2;
            else if (code == 'I' || code == 'i') bytes = 4;
            else if (code == 'Q' || code == 'q') bytes = 8;
            else throw ErrorEjecucion(std::string("especificador binario no soportado: ") + code, u);

            if (big_endian) {
                for (int b = bytes - 1; b >= 0; --b) {
                    buf.push_back(static_cast<std::uint8_t>((val >> (b * 8)) & 0xFF));
                }
            } else {
                for (int b = 0; b < bytes; ++b) {
                    buf.push_back(static_cast<std::uint8_t>((val >> (b * 8)) & 0xFF));
                }
            }
        }
        return bytes_a_lista_valor(buf);
    });

    // Equivalente a struct.unpack(formato, bytes) en Python
    registrar_fn(*m->entorno, "desempaquetar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("binario.desempaquetar(formato, lista_bytes_o_texto)", u);
        std::string fmt = a[0].a_texto();
        std::vector<std::uint8_t> buf = valor_a_bytes(a[1], u);
        bool big_endian = false;
        std::size_t idx_fmt = 0;
        if (!fmt.empty() && (fmt[0] == '<' || fmt[0] == '>' || fmt[0] == '!')) {
            big_endian = (fmt[0] == '>' || fmt[0] == '!');
            idx_fmt = 1;
        }

        std::vector<Valor> salida;
        std::size_t pos = 0;
        for (; idx_fmt < fmt.size(); ++idx_fmt) {
            char code = fmt[idx_fmt];
            if (code == ' ') continue;
            int bytes = 0;
            if (code == 'B' || code == 'b') bytes = 1;
            else if (code == 'H' || code == 'h') bytes = 2;
            else if (code == 'I' || code == 'i') bytes = 4;
            else if (code == 'Q' || code == 'q') bytes = 8;
            else throw ErrorEjecucion(std::string("especificador binario no soportado: ") + code, u);

            if (pos + static_cast<std::size_t>(bytes) > buf.size())
                throw ErrorEjecucion("buffer binario demasiado corto para desempaquetar", u);

            std::uint64_t val = 0;
            if (big_endian) {
                for (int b = 0; b < bytes; ++b) val = (val << 8) | buf[pos + static_cast<std::size_t>(b)];
            } else {
                for (int b = bytes - 1; b >= 0; --b) val = (val << 8) | buf[pos + static_cast<std::size_t>(b)];
            }
            pos += static_cast<std::size_t>(bytes);
            salida.push_back(Valor::de_entero(static_cast<std::int64_t>(val)));
        }
        return Valor::de_lista(std::move(salida));
    });

    registrar_fn(*m->entorno, "xor_bytes", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("binario.xor_bytes(datos, clave)", u);
        auto datos = valor_a_bytes(a[0], u);
        auto clave = valor_a_bytes(a[1], u);
        if (clave.empty()) throw ErrorEjecucion("la clave XOR no puede estar vacia", u);
        for (std::size_t i = 0; i < datos.size(); ++i) {
            datos[i] ^= clave[i % clave.size()];
        }
        return bytes_a_lista_valor(datos);
    });

    registrar_fn(*m->entorno, "a_hex", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("binario.a_hex(lista_bytes_o_texto)", u);
        auto bytes = valor_a_bytes(a[0], u);
        static const char* hex = "0123456789abcdef";
        std::string out;
        out.reserve(bytes.size() * 2);
        for (std::uint8_t b : bytes) {
            out.push_back(hex[b >> 4]);
            out.push_back(hex[b & 0x0F]);
        }
        return Valor::de_texto(std::move(out));
    });

    registrar_fn(*m->entorno, "de_hex", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("binario.de_hex(cadena_hex)", u);
        std::string s = a[0].a_texto();
        if (s.size() % 2 != 0) throw ErrorEjecucion("longitud hexadecimal impar", u);
        auto nibble = [&](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            throw ErrorEjecucion("digito hexadecimal no valido", u);
        };
        std::vector<std::uint8_t> out;
        out.reserve(s.size() / 2);
        for (std::size_t i = 0; i < s.size(); i += 2) {
            out.push_back(static_cast<std::uint8_t>((nibble(s[i]) << 4) | nibble(s[i + 1])));
        }
        return bytes_a_lista_valor(out);
    });

    registrar_fn(*m->entorno, "leer_archivo_bytes", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("binario.leer_archivo_bytes(ruta)", u);
        std::ifstream f(a[0].a_texto(), std::ios::binary);
        if (!f) throw ErrorEjecucion("no se pudo abrir archivo binario: " + a[0].a_texto(), u);
        std::vector<std::uint8_t> buf((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        return bytes_a_lista_valor(buf);
    });

    registrar_fn(*m->entorno, "escribir_archivo_bytes", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("binario.escribir_archivo_bytes(ruta, lista_bytes)", u);
        auto bytes = valor_a_bytes(a[1], u);
        std::ofstream f(a[0].a_texto(), std::ios::binary);
        if (!f) throw ErrorEjecucion("no se pudo escribir archivo binario: " + a[0].a_texto(), u);
        f.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "entropia_shannon", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("binario.entropia_shannon(lista_bytes_o_texto)", u);
        auto bytes = valor_a_bytes(a[0], u);
        if (bytes.empty()) return Valor::de_decimal(0.0);
        std::array<std::size_t, 256> freq{};
        for (std::uint8_t b : bytes) ++freq[b];
        double ent = 0.0;
        double n = static_cast<double>(bytes.size());
        for (std::size_t count : freq) {
            if (count == 0) continue;
            double p = static_cast<double>(count) / n;
            ent -= p * std::log2(p);
        }
        return Valor::de_decimal(ent);
    });

    return m;
}

}  // namespace esp