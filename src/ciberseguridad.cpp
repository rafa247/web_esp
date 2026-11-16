#include "esp/ciberseguridad.hpp"
#include "esp/lexer.hpp"
#include "esp/parser.hpp"
#include "esp/runtime.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <random>
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
#include <bcrypt.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
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

#ifdef _WIN32
using SocketNativo = SOCKET;
constexpr SocketNativo SOCKET_INVALIDO = INVALID_SOCKET;
void asegurar_red_ciber() {
    static bool ok = false;
    if (!ok) {
        WSADATA w;
        WSAStartup(MAKEWORD(2, 2), &w);
        ok = true;
    }
}
void cerrar_socket_ciber(SocketNativo s) { closesocket(s); }
#else
using SocketNativo = int;
constexpr SocketNativo SOCKET_INVALIDO = -1;
void asegurar_red_ciber() {}
void cerrar_socket_ciber(SocketNativo s) { close(s); }
#endif

std::string hex_encode_ciber(const std::vector<std::uint8_t>& in) {
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.reserve(in.size() * 2);
    for (std::uint8_t c : in) {
        out.push_back(hex[c >> 4]);
        out.push_back(hex[c & 0x0F]);
    }
    return out;
}

std::vector<std::uint8_t> hex_decode_ciber(const std::string& in, Ubicacion u) {
    if (in.size() % 2 != 0) throw ErrorEjecucion("cadena hexadecimal impar", u);
    auto val = [&](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw ErrorEjecucion("caracter hexadecimal invalido", u);
    };
    std::vector<std::uint8_t> out;
    out.reserve(in.size() / 2);
    for (std::size_t i = 0; i < in.size(); i += 2) {
        out.push_back(static_cast<std::uint8_t>((val(in[i]) << 4) | val(in[i + 1])));
    }
    return out;
}

std::string generar_uuid_v4_ciber() {
    unsigned char bytes[16]{};
#ifdef _WIN32
    BCryptGenRandom(nullptr, bytes, sizeof(bytes), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
    std::ifstream urandom("/dev/urandom", std::ios::binary);
    if (urandom) urandom.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
    else {
        std::random_device rd;
        for (auto& b : bytes) b = static_cast<unsigned char>(rd());
    }
#endif
    bytes[6] = static_cast<unsigned char>((bytes[6] & 0x0F) | 0x40);
    bytes[8] = static_cast<unsigned char>((bytes[8] & 0x3F) | 0x80);
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10) ss << '-';
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

}  // namespace

// ============================================================================
// SOCKETS PERSISTENTES TCP / UDP + BANNER GRABBING (estilo socket de Python)
// ============================================================================

std::shared_ptr<Modulo> crear_modulo_sockets(const std::string& nombre_alias) {
    static std::mutex sock_mutex;
    static std::int64_t next_sock_id = 1;
    static std::map<std::int64_t, SocketNativo> sockets_abiertos;

    auto m = std::make_shared<Modulo>();
    m->nombre = nombre_alias;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "tcp_conectar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("sockets.tcp_conectar(host, puerto, timeout_ms?)", u);
        asegurar_red_ciber();
        std::string host = a[0].a_texto();
        int puerto = static_cast<int>(a[1].numero());
        int timeout_ms = a.size() > 2 ? static_cast<int>(a[2].numero()) : 3000;

        SocketNativo s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == SOCKET_INVALIDO) throw ErrorEjecucion("no se pudo crear socket TCP", u);

#ifdef _WIN32
        DWORD tv = static_cast<DWORD>(timeout_ms);
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
#else
        timeval tv{};
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<u_short>(puerto));
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
            addrinfo hints{}, *res = nullptr;
            hints.ai_family = AF_INET;
            if (getaddrinfo(host.c_str(), nullptr, &hints, &res) == 0 && res) {
                addr.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;
                freeaddrinfo(res);
            } else {
                cerrar_socket_ciber(s);
                throw ErrorEjecucion("no se pudo resolver host: " + host, u);
            }
        }

        if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            cerrar_socket_ciber(s);
            throw ErrorEjecucion("conexion TCP rechazada o expirada hacia " + host + ":" + std::to_string(puerto), u);
        }

        std::lock_guard<std::mutex> lk(sock_mutex);
        std::int64_t id = next_sock_id++;
        sockets_abiertos[id] = s;
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "enviar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("sockets.enviar(socket_id, texto_o_bytes)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        SocketNativo s = SOCKET_INVALIDO;
        {
            std::lock_guard<std::mutex> lk(sock_mutex);
            auto it = sockets_abiertos.find(id);
            if (it == sockets_abiertos.end()) throw ErrorEjecucion("socket no valido o cerrado", u);
            s = it->second;
        }
        std::string payload;
        if (a[1].tipo == TipoValor::Lista && a[1].lista) {
            for (const auto& b : *a[1].lista) payload.push_back(static_cast<char>(static_cast<int>(b.numero()) & 0xFF));
        } else {
            payload = a[1].a_texto();
        }
        int enviados = send(s, payload.data(), static_cast<int>(payload.size()), 0);
        return Valor::de_entero(enviados);
    });

    registrar_fn(*m->entorno, "recibir", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("sockets.recibir(socket_id, max_bytes?)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        int max_b = a.size() > 1 ? std::clamp(static_cast<int>(a[1].numero()), 1, 65536) : 4096;
        SocketNativo s = SOCKET_INVALIDO;
        {
            std::lock_guard<std::mutex> lk(sock_mutex);
            auto it = sockets_abiertos.find(id);
            if (it == sockets_abiertos.end()) throw ErrorEjecucion("socket no valido o cerrado", u);
            s = it->second;
        }
        std::string buf(static_cast<std::size_t>(max_b), '\0');
        int n = recv(s, buf.data(), max_b, 0);
        if (n <= 0) return Valor::de_texto("");
        buf.resize(static_cast<std::size_t>(n));
        return Valor::de_texto(std::move(buf));
    });

    registrar_fn(*m->entorno, "cerrar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("sockets.cerrar(socket_id)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(sock_mutex);
        auto it = sockets_abiertos.find(id);
        if (it == sockets_abiertos.end()) return Valor::de_bool(false);
        cerrar_socket_ciber(it->second);
        sockets_abiertos.erase(it);
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "udp_enviar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("sockets.udp_enviar(host, puerto, texto)", u);
        asegurar_red_ciber();
        SocketNativo s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (s == SOCKET_INVALIDO) throw ErrorEjecucion("no se pudo crear socket UDP", u);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<u_short>(a[1].numero()));
        inet_pton(AF_INET, a[0].a_texto().c_str(), &addr.sin_addr);
        auto msg = a[2].a_texto();
        int n = sendto(s, msg.c_str(), static_cast<int>(msg.size()), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        cerrar_socket_ciber(s);
        return Valor::de_entero(n);
    });

    registrar_fn(*m->entorno, "tcp_enviar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("sockets.tcp_enviar(host, puerto, texto)", u);
        asegurar_red_ciber();
        SocketNativo s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == SOCKET_INVALIDO) throw ErrorEjecucion("no se pudo crear socket TCP", u);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<u_short>(a[1].numero()));
        inet_pton(AF_INET, a[0].a_texto().c_str(), &addr.sin_addr);
        if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            cerrar_socket_ciber(s);
            throw ErrorEjecucion("no se pudo conectar por TCP a " + a[0].a_texto(), u);
        }
        auto msg = a[2].a_texto();
        send(s, msg.c_str(), static_cast<int>(msg.size()), 0);
        char buf[4096]{};
        int n = recv(s, buf, sizeof(buf) - 1, 0);
        cerrar_socket_ciber(s);
        return Valor::de_texto(n > 0 ? std::string(buf, static_cast<std::size_t>(n)) : "");
    });

    return m;
}

// ============================================================================
// CRIPTOGRAFIA AVANZADA (SHA-256, HMAC-SHA256, AES-256, TOKENS Y UUIDv4)
// ============================================================================

std::shared_ptr<Modulo> crear_modulo_cripto(const std::string& nombre_alias) {
    auto m = std::make_shared<Modulo>();
    m->nombre = nombre_alias;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "sha256", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("cripto.sha256(texto)", u);
        return Valor::de_texto(sha256_hex(a[0].a_texto()));
    });

    registrar_fn(*m->entorno, "hmac_sha256", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("cripto.hmac_sha256(clave, mensaje)", u);
        std::string key = a[0].a_texto();
        std::string msg = a[1].a_texto();
        if (key.size() > 64) key = key.substr(0, 64);
        key.resize(64, '\0');
        std::string o_key_pad(64, '\0'), i_key_pad(64, '\0');
        for (std::size_t i = 0; i < 64; ++i) {
            o_key_pad[i] = static_cast<char>(key[i] ^ 0x5c);
            i_key_pad[i] = static_cast<char>(key[i] ^ 0x36);
        }
        std::string inner_hex = sha256_hex(i_key_pad + msg);
        auto inner_bytes = hex_decode_ciber(inner_hex, u);
        std::string inner_raw(reinterpret_cast<const char*>(inner_bytes.data()), inner_bytes.size());
        return Valor::de_texto(sha256_hex(o_key_pad + inner_raw));
    });

    registrar_fn(*m->entorno, "token_seguro", [](std::vector<Valor>& a, Ubicacion) {
        int nbytes = a.empty() ? 32 : std::clamp(static_cast<int>(a[0].numero()), 8, 256);
        std::vector<std::uint8_t> buf(static_cast<std::size_t>(nbytes), 0);
#ifdef _WIN32
        BCryptGenRandom(nullptr, buf.data(), static_cast<ULONG>(buf.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
        std::ifstream urandom("/dev/urandom", std::ios::binary);
        if (urandom) urandom.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
#endif
        return Valor::de_texto(hex_encode_ciber(buf));
    });

    registrar_fn(*m->entorno, "uuid_v4", [](std::vector<Valor>&, Ubicacion) {
        return Valor::de_texto(generar_uuid_v4_ciber());
    });

    registrar_fn(*m->entorno, "hex_codificar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("cripto.hex_codificar(texto)", u);
        std::string s = a[0].a_texto();
        std::vector<std::uint8_t> v(s.begin(), s.end());
        return Valor::de_texto(hex_encode_ciber(v));
    });

    registrar_fn(*m->entorno, "hex_decodificar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("cripto.hex_decodificar(hex)", u);
        auto v = hex_decode_ciber(a[0].a_texto(), u);
        return Valor::de_texto(std::string(reinterpret_cast<const char*>(v.data()), v.size()));
    });

#ifdef _WIN32
    registrar_fn(*m->entorno, "aes_cifrar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("cripto.aes_cifrar(clave, texto_plano)", u);
        auto key_bytes = hex_decode_ciber(sha256_hex(a[0].a_texto()), u);
        std::string plain = a[1].a_texto();

        BCRYPT_ALG_HANDLE hAlg = nullptr;
        BCRYPT_KEY_HANDLE hKey = nullptr;
        if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0) != 0)
            throw ErrorEjecucion("error iniciando AES", u);
        BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                          reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_CBC)),
                          sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
        BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, key_bytes.data(), 32, 0);

        std::vector<std::uint8_t> iv(16, 0);
        BCryptGenRandom(nullptr, iv.data(), 16, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        std::vector<std::uint8_t> iv_copy = iv;

        DWORD cbCipher = 0;
        BCryptEncrypt(hKey, reinterpret_cast<PUCHAR>(plain.data()), static_cast<ULONG>(plain.size()),
                      nullptr, iv_copy.data(), 16, nullptr, 0, &cbCipher, BCRYPT_BLOCK_PADDING);
        std::vector<std::uint8_t> cipher(cbCipher);
        BCryptEncrypt(hKey, reinterpret_cast<PUCHAR>(plain.data()), static_cast<ULONG>(plain.size()),
                      nullptr, iv_copy.data(), 16, cipher.data(), cbCipher, &cbCipher, BCRYPT_BLOCK_PADDING);

        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);

        std::vector<std::uint8_t> combinado = iv;
        combinado.insert(combinado.end(), cipher.begin(), cipher.end());
        return Valor::de_texto(hex_encode_ciber(combinado));
    });

    registrar_fn(*m->entorno, "aes_descifrar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("cripto.aes_descifrar(clave, hex_cifrado)", u);
        auto key_bytes = hex_decode_ciber(sha256_hex(a[0].a_texto()), u);
        auto combinado = hex_decode_ciber(a[1].a_texto(), u);
        if (combinado.size() <= 16) throw ErrorEjecucion("datos cifrados AES invalidos", u);

        std::vector<std::uint8_t> iv(combinado.begin(), combinado.begin() + 16);
        std::vector<std::uint8_t> cipher(combinado.begin() + 16, combinado.end());

        BCRYPT_ALG_HANDLE hAlg = nullptr;
        BCRYPT_KEY_HANDLE hKey = nullptr;
        BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0);
        BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                          reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_CBC)),
                          sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
        BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, key_bytes.data(), 32, 0);

        DWORD cbPlain = 0;
        BCryptDecrypt(hKey, cipher.data(), static_cast<ULONG>(cipher.size()), nullptr, iv.data(), 16,
                      nullptr, 0, &cbPlain, BCRYPT_BLOCK_PADDING);
        std::vector<std::uint8_t> plain(cbPlain);
        NTSTATUS st = BCryptDecrypt(hKey, cipher.data(), static_cast<ULONG>(cipher.size()), nullptr, iv.data(), 16,
                                    plain.data(), cbPlain, &cbPlain, BCRYPT_BLOCK_PADDING);
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        if (st != 0) throw ErrorEjecucion("clave incorrecta o datos corruptos al descifrar AES", u);
        return Valor::de_texto(std::string(reinterpret_cast<const char*>(plain.data()), cbPlain));
    });
#endif

    return m;
}

// ============================================================================
// FORENSE Y AUDITORIA BINARIA (PE/ELF, ENTROPIA SHANNON, STRINGS, PUERTOS)
// ============================================================================

std::shared_ptr<Modulo> crear_modulo_forense() {
    auto m = std::make_shared<Modulo>();
    m->nombre = "forense";
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "analizar_ejecutable", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("forense.analizar_ejecutable(ruta_binario)", u);
        std::string ruta = a[0].a_texto();
        std::ifstream f(ruta, std::ios::binary);
        if (!f) throw ErrorEjecucion("no se pudo abrir archivo: " + ruta, u);
        std::vector<std::uint8_t> buf((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        // Entropia Shannon
        std::array<std::size_t, 256> freq{};
        for (std::uint8_t b : buf) ++freq[b];
        double ent = 0.0;
        for (std::size_t c : freq) {
            if (c > 0) {
                double p = static_cast<double>(c) / static_cast<double>(buf.size());
                ent -= p * std::log2(p);
            }
        }

        std::string formato = "desconocido";
        std::string arquitectura = "desconocida";
        if (buf.size() >= 64 && buf[0] == 'M' && buf[1] == 'Z') {
            formato = "PE (Windows)";
            std::uint32_t pe_off = *reinterpret_cast<std::uint32_t*>(&buf[60]);
            if (pe_off + 6 < buf.size() && buf[pe_off] == 'P' && buf[pe_off + 1] == 'E') {
                std::uint16_t mach = *reinterpret_cast<std::uint16_t*>(&buf[pe_off + 4]);
                if (mach == 0x8664) arquitectura = "x86_64";
                else if (mach == 0x014c) arquitectura = "x86 (32-bit)";
                else if (mach == 0xAA64) arquitectura = "ARM64";
            }
        } else if (buf.size() >= 20 && buf[0] == 0x7F && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'F') {
            formato = "ELF (Linux/Unix)";
            arquitectura = (buf[4] == 2) ? "64-bit" : "32-bit";
        }

        return Valor::de_mapa({
            {"tamano_bytes", Valor::de_entero(static_cast<std::int64_t>(buf.size()))},
            {"formato", Valor::de_texto(formato)},
            {"arquitectura", Valor::de_texto(arquitectura)},
            {"entropia_shannon", Valor::de_decimal(ent)},
            {"posible_empaquetado", Valor::de_bool(ent > 7.2)},
            {"sha256", Valor::de_texto(sha256_hex(std::string(reinterpret_cast<const char*>(buf.data()), buf.size())))}
        });
    });

    registrar_fn(*m->entorno, "extraer_strings", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("forense.extraer_strings(ruta_archivo, longitud_minima?)", u);
        std::size_t min_len = a.size() > 1 ? static_cast<std::size_t>(std::max(3, static_cast<int>(a[1].numero()))) : 5;
        std::ifstream f(a[0].a_texto(), std::ios::binary);
        if (!f) throw ErrorEjecucion("no se pudo abrir archivo: " + a[0].a_texto(), u);
        std::string contenido((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        std::vector<Valor> cadenas;
        std::string actual;
        for (unsigned char c : contenido) {
            if (c >= 32 && c <= 126) {
                actual.push_back(static_cast<char>(c));
            } else {
                if (actual.size() >= min_len) cadenas.push_back(Valor::de_texto(actual));
                actual.clear();
            }
        }
        if (actual.size() >= min_len) cadenas.push_back(Valor::de_texto(actual));
        return Valor::de_lista(std::move(cadenas));
    });

    registrar_fn(*m->entorno, "escanear_puertos", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2 || a[1].tipo != TipoValor::Lista)
            throw ErrorEjecucion("forense.escanear_puertos(ip, lista_puertos, timeout_ms?)", u);
        asegurar_red_ciber();
        std::string ip = a[0].a_texto();
        int timeout_ms = a.size() > 2 ? static_cast<int>(a[2].numero()) : 250;

        std::vector<int> puertos;
        for (const auto& p : *a[1].lista) puertos.push_back(static_cast<int>(p.numero()));

        struct ResultadoPuerto { int puerto; bool abierto; std::string banner; };
        std::vector<ResultadoPuerto> resultados(puertos.size());
        std::vector<std::thread> hilos;

        for (std::size_t i = 0; i < puertos.size(); ++i) {
            int p = puertos[i];
            resultados[i] = {p, false, ""};
            hilos.emplace_back([&, i, p]() {
                SocketNativo s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (s == SOCKET_INVALIDO) return;
#ifdef _WIN32
                DWORD tv = static_cast<DWORD>(timeout_ms);
                setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
                setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
#else
                timeval tv{timeout_ms / 1000, (timeout_ms % 1000) * 1000};
                setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif
                sockaddr_in addr{};
                addr.sin_family = AF_INET;
                addr.sin_port = htons(static_cast<u_short>(p));
                inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
                if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
                    resultados[i].abierto = true;
                    char banner[256]{};
                    int n = recv(s, banner, sizeof(banner) - 1, 0);
                    if (n > 0) resultados[i].banner = std::string(banner, static_cast<std::size_t>(n));
                }
                cerrar_socket_ciber(s);
            });
        }
        for (auto& h : hilos) if (h.joinable()) h.join();

        std::vector<Valor> abiertos;
        for (const auto& r : resultados) {
            if (r.abierto) {
                abiertos.push_back(Valor::de_mapa({
                    {"puerto", Valor::de_entero(r.puerto)},
                    {"abierto", Valor::de_bool(true)},
                    {"banner", Valor::de_texto(r.banner)}
                }));
            }
        }
        return Valor::de_lista(std::move(abiertos));
    });

    return m;
}

// ============================================================================
// INTROSPECCION DEL COMPILADOR: MODULO AST Y LEXER NATIVO
// ============================================================================

std::shared_ptr<Modulo> crear_modulo_ast() {
    auto m = std::make_shared<Modulo>();
    m->nombre = "ast";
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "tokens", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ast.tokens(codigo_esp)", u);
        Lexer lexer(a[0].a_texto(), "<ast>");
        auto toks = lexer.analizar();
        std::vector<Valor> lista;
        for (const auto& t : toks) {
            if (t.tipo == TipoToken::FinArchivo) continue;
            lista.push_back(Valor::de_mapa({
                {"lexema", Valor::de_texto(t.lexema)},
                {"linea", Valor::de_entero(t.ubicacion.linea)},
                {"columna", Valor::de_entero(t.ubicacion.columna)}
            }));
        }
        return Valor::de_lista(std::move(lista));
    });

    registrar_fn(*m->entorno, "analizar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ast.analizar(codigo_esp)", u);
        Lexer lexer(a[0].a_texto(), "<ast>");
        Parser parser(lexer.analizar());
        auto prog = parser.analizar();
        std::vector<Valor> nodos;
        for (const auto& s : prog->sentencias) {
            nodos.push_back(Valor::de_mapa({
                {"tipo_id", Valor::de_entero(static_cast<std::int64_t>(s->tipo))},
                {"linea", Valor::de_entero(s->ubicacion.linea)},
                {"columna", Valor::de_entero(s->ubicacion.columna)}
            }));
        }
        return Valor::de_mapa({
            {"total_sentencias", Valor::de_entero(static_cast<std::int64_t>(prog->sentencias.size()))},
            {"nodos", Valor::de_lista(std::move(nodos))}
        });
    });

    return m;
}

}  // namespace esp