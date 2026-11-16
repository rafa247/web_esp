#include "esp/redes_ip.hpp"
#include "esp/runtime.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
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
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace esp {
namespace {

#ifdef _WIN32
using SocketIP = SOCKET;
constexpr SocketIP SOCKET_IP_INVALIDO = INVALID_SOCKET;
void red_ip_init() {
    static bool ok = false;
    if (!ok) {
        WSADATA w;
        WSAStartup(MAKEWORD(2, 2), &w);
        ok = true;
    }
}
void red_ip_cerrar(SocketIP s) { closesocket(s); }
#else
using SocketIP = int;
constexpr SocketIP SOCKET_IP_INVALIDO = -1;
void red_ip_init() {}
void red_ip_cerrar(SocketIP s) { close(s); }
#endif

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

bool parsear_ipv4(const std::string& texto, std::uint32_t& out_host) {
    in_addr addr{};
    if (inet_pton(AF_INET, texto.c_str(), &addr) != 1) return false;
    out_host = ntohl(addr.s_addr);
    return true;
}

std::string formatear_ipv4(std::uint32_t ip_host) {
    in_addr addr{};
    addr.s_addr = htonl(ip_host);
    char buf[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &addr, buf, sizeof(buf));
    return std::string(buf);
}

bool parsear_ipv6(const std::string& texto, std::array<std::uint8_t, 16>& bytes) {
    in6_addr addr{};
    if (inet_pton(AF_INET6, texto.c_str(), &addr) != 1) return false;
    std::memcpy(bytes.data(), &addr, 16);
    return true;
}

std::string formatear_ipv6_canonica(const std::array<std::uint8_t, 16>& bytes) {
    in6_addr addr{};
    std::memcpy(&addr, bytes.data(), 16);
    char buf[INET6_ADDRSTRLEN]{};
    inet_ntop(AF_INET6, &addr, buf, sizeof(buf));
    return std::string(buf);
}

std::string formatear_ipv6_expandida(const std::array<std::uint8_t, 16>& bytes) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 16; i += 2) {
        if (i > 0) ss << ':';
        std::uint16_t grupo = (static_cast<std::uint16_t>(bytes[i]) << 8) | bytes[i + 1];
        ss << std::setw(4) << grupo;
    }
    return ss.str();
}

struct RedIPv4 {
    std::uint32_t red = 0;
    std::uint32_t mascara = 0;
    std::uint32_t broadcast = 0;
    int prefijo = 0;
};

bool parsear_cidr_v4(const std::string& cidr, RedIPv4& out, bool estricto = false) {
    auto slash = cidr.find('/');
    std::string ip_part = (slash == std::string::npos) ? cidr : cidr.substr(0, slash);
    int prefijo = 32;
    if (slash != std::string::npos) {
        std::string pref_str = cidr.substr(slash + 1);
        if (pref_str.empty()) return false;
        for (unsigned char c : pref_str) if (!std::isdigit(c)) return false;
        prefijo = std::stoi(pref_str);
        if (prefijo < 0 || prefijo > 32) return false;
    }
    std::uint32_t ip = 0;
    if (!parsear_ipv4(ip_part, ip)) return false;
    std::uint32_t mask = (prefijo == 0) ? 0U : (~0U << (32 - prefijo));
    std::uint32_t net = ip & mask;
    if (estricto && ip != net) return false;
    out.red = net;
    out.mascara = mask;
    out.broadcast = net | (~mask);
    out.prefijo = prefijo;
    return true;
}

double sonda_tcp_ip_ms(const std::string& ip, int puerto, int timeout_ms) {
    red_ip_init();
    SocketIP s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == SOCKET_IP_INVALIDO) return -1.0;

#ifdef _WIN32
    u_long modo = 1;
    ioctlsocket(s, FIONBIO, &modo);
#else
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(puerto));
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        red_ip_cerrar(s);
        return -1.0;
    }

    auto t0 = std::chrono::steady_clock::now();
    int rc = connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    bool conectado = (rc == 0);
    if (!conectado) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(s, &wfds);
        timeval tv{};
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        if (select(static_cast<int>(s) + 1, nullptr, &wfds, nullptr, &tv) > 0) {
            int err = 0;
            socklen_t len = sizeof(err);
            getsockopt(s, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &len);
            conectado = (err == 0);
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    red_ip_cerrar(s);
    if (!conectado) return -1.0;
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

}  // namespace

std::shared_ptr<Modulo> crear_modulo_redes_ip() {
    auto m = std::make_shared<Modulo>();
    m->nombre = "redes.ip";
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    // Equivalente a socket.gethostname()
    registrar_fn(*m->entorno, "local", [](std::vector<Valor>&, Ubicacion) {
        red_ip_init();
        char host[256]{};
        if (gethostname(host, sizeof(host) - 1) == 0 && host[0] != '\0') return Valor::de_texto(host);
        return Valor::de_texto("localhost");
    });

    // Equivalente a ipaddress.ip_address(direccion)
    registrar_fn(*m->entorno, "direccion", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.direccion(cadena_ip)", u);
        std::string s = a[0].a_texto();
        std::uint32_t v4 = 0;
        if (parsear_ipv4(s, v4)) {
            bool privada = ((v4 & 0xFF000000U) == 0x0A000000U) ||
                           ((v4 & 0xFFF00000U) == 0xAC100000U) ||
                           ((v4 & 0xFFFF0000U) == 0xC0A80000U);
            bool loopback = ((v4 & 0xFF000000U) == 0x7F000000U);
            bool link_local = ((v4 & 0xFFFF0000U) == 0xA9FE0000U);
            bool multicast = ((v4 & 0xF0000000U) == 0xE0000000U);
            bool reservada = ((v4 & 0xF0000000U) == 0xF0000000U);

            std::ostringstream bin;
            for (int bit = 31; bit >= 0; --bit) {
                bin << ((v4 >> bit) & 1U);
                if (bit > 0 && bit % 8 == 0) bin << '.';
            }

            return Valor::de_mapa({
                {"ip", Valor::de_texto(formatear_ipv4(v4))},
                {"version", Valor::de_entero(4)},
                {"entero", Valor::de_entero(static_cast<std::int64_t>(v4))},
                {"binario", Valor::de_texto(bin.str())},
                {"es_privada", Valor::de_bool(privada)},
                {"es_global", Valor::de_bool(!privada && !loopback && !link_local && !multicast && !reservada && v4 != 0)},
                {"es_loopback", Valor::de_bool(loopback)},
                {"es_link_local", Valor::de_bool(link_local)},
                {"es_multicast", Valor::de_bool(multicast)},
                {"es_reservada", Valor::de_bool(reservada)}
            });
        }
        std::array<std::uint8_t, 16> v6{};
        if (parsear_ipv6(s, v6)) {
            std::uint16_t g0 = (static_cast<std::uint16_t>(v6[0]) << 8) | v6[1];
            bool loopback = true;
            for (int i = 0; i < 15; ++i) if (v6[i] != 0) loopback = false;
            loopback = loopback && (v6[15] == 1);
            bool privada = (g0 >= 0xFC00 && g0 <= 0xFDFF);
            bool link_local = (g0 >= 0xFE80 && g0 <= 0xFEBF);
            bool multicast = (v6[0] == 0xFF);
            bool global = (g0 >= 0x2000 && g0 <= 0x3FFF);

            return Valor::de_mapa({
                {"ip", Valor::de_texto(formatear_ipv6_canonica(v6))},
                {"expandida", Valor::de_texto(formatear_ipv6_expandida(v6))},
                {"version", Valor::de_entero(6)},
                {"es_privada", Valor::de_bool(privada)},
                {"es_global", Valor::de_bool(global)},
                {"es_loopback", Valor::de_bool(loopback)},
                {"es_link_local", Valor::de_bool(link_local)},
                {"es_multicast", Valor::de_bool(multicast)}
            });
        }
        throw ErrorEjecucion("direccion IPv4 o IPv6 no valida: " + s, u);
    });

    // Equivalente a ipaddress.ip_network(cidr, strict=False)
    registrar_fn(*m->entorno, "red", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.red(cidr, estricto?)", u);
        std::string cidr = a[0].a_texto();
        bool estricto = a.size() > 1 ? a[1].verdad() : false;
        RedIPv4 r{};
        if (!parsear_cidr_v4(cidr, r, estricto))
            throw ErrorEjecucion("red CIDR IPv4 no valida (o tiene bits de host activos en modo estricto): " + cidr, u);

        std::uint64_t total = 1ULL << (32 - r.prefijo);
        std::uint64_t utilizables = (r.prefijo < 31) ? (total - 2ULL) : total;
        std::uint32_t primera = (r.prefijo < 31) ? (r.red + 1) : r.red;
        std::uint32_t ultima = (r.prefijo < 31) ? (r.broadcast - 1) : r.broadcast;

        return Valor::de_mapa({
            {"cidr", Valor::de_texto(formatear_ipv4(r.red) + "/" + std::to_string(r.prefijo))},
            {"direccion_red", Valor::de_texto(formatear_ipv4(r.red))},
            {"mascara", Valor::de_texto(formatear_ipv4(r.mascara))},
            {"wildcard", Valor::de_texto(formatear_ipv4(~r.mascara))},
            {"broadcast", Valor::de_texto(formatear_ipv4(r.broadcast))},
            {"prefijo", Valor::de_entero(r.prefijo)},
            {"primera_ip", Valor::de_texto(formatear_ipv4(primera))},
            {"ultima_ip", Valor::de_texto(formatear_ipv4(ultima))},
            {"total_direcciones", Valor::de_entero(static_cast<std::int64_t>(total))},
            {"hosts_utilizables", Valor::de_entero(static_cast<std::int64_t>(utilizables))}
        });
    });

    // Equivalente a list(net.hosts()) en Python
    registrar_fn(*m->entorno, "hosts", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.hosts(cidr, limite?)", u);
        RedIPv4 r{};
        if (!parsear_cidr_v4(a[0].a_texto(), r, false))
            throw ErrorEjecucion("red CIDR no valida: " + a[0].a_texto(), u);

        std::uint64_t limite = a.size() > 1 ? static_cast<std::uint64_t>(a[1].numero()) : 65536ULL;
        std::uint32_t inicio = (r.prefijo < 31) ? (r.red + 1) : r.red;
        std::uint32_t fin = (r.prefijo < 31) ? (r.broadcast - 1) : r.broadcast;

        std::vector<Valor> lista;
        if (inicio <= fin) {
            std::uint64_t cantidad = static_cast<std::uint64_t>(fin - inicio) + 1ULL;
            if (cantidad > limite) cantidad = limite;
            lista.reserve(static_cast<std::size_t>(cantidad));
            for (std::uint64_t i = 0; i < cantidad; ++i) {
                lista.push_back(Valor::de_texto(formatear_ipv4(inicio + static_cast<std::uint32_t>(i))));
            }
        }
        return Valor::de_lista(std::move(lista));
    });

    // Equivalente a list(net.subnets(new_prefix=...)) en Python
    registrar_fn(*m->entorno, "subredes", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("ip.subredes(cidr, nuevo_prefijo)", u);
        RedIPv4 r{};
        if (!parsear_cidr_v4(a[0].a_texto(), r, false))
            throw ErrorEjecucion("red CIDR no valida: " + a[0].a_texto(), u);
        int nuevo_pref = static_cast<int>(a[1].numero());
        if (nuevo_pref < r.prefijo || nuevo_pref > 32)
            throw ErrorEjecucion("el nuevo prefijo debe estar entre " + std::to_string(r.prefijo) + " y 32", u);

        int diff = nuevo_pref - r.prefijo;
        if (diff > 16) throw ErrorEjecucion("demasiadas subredes generadas de una vez (max 65536)", u);
        std::uint32_t num_subredes = 1U << diff;
        std::uint32_t paso = 1U << (32 - nuevo_pref);

        std::vector<Valor> resultado;
        resultado.reserve(num_subredes);
        for (std::uint32_t i = 0; i < num_subredes; ++i) {
            std::uint32_t sub_net = r.red + (i * paso);
            resultado.push_back(Valor::de_texto(formatear_ipv4(sub_net) + "/" + std::to_string(nuevo_pref)));
        }
        return Valor::de_lista(std::move(resultado));
    });

    // Equivalente a net.supernet(new_prefix=...) en Python
    registrar_fn(*m->entorno, "superred", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("ip.superred(cidr, nuevo_prefijo)", u);
        RedIPv4 r{};
        if (!parsear_cidr_v4(a[0].a_texto(), r, false))
            throw ErrorEjecucion("red CIDR no valida: " + a[0].a_texto(), u);
        int nuevo_pref = static_cast<int>(a[1].numero());
        if (nuevo_pref < 0 || nuevo_pref > r.prefijo)
            throw ErrorEjecucion("el prefijo de superred debe ser menor o igual a " + std::to_string(r.prefijo), u);

        std::uint32_t mask = (nuevo_pref == 0) ? 0U : (~0U << (32 - nuevo_pref));
        std::uint32_t super_net = r.red & mask;
        return Valor::de_texto(formatear_ipv4(super_net) + "/" + std::to_string(nuevo_pref));
    });

    // Equivalente a net1.overlaps(net2) en Python
    registrar_fn(*m->entorno, "se_solapan", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("ip.se_solapan(cidr1, cidr2)", u);
        RedIPv4 r1{}, r2{};
        if (!parsear_cidr_v4(a[0].a_texto(), r1, false) || !parsear_cidr_v4(a[1].a_texto(), r2, false))
            throw ErrorEjecucion("redes CIDR no validas en se_solapan", u);
        bool solapan = (r1.red <= r2.broadcast) && (r2.red <= r1.broadcast);
        return Valor::de_bool(solapan);
    });

    // Equivalente a ip in net en Python
    registrar_fn(*m->entorno, "contiene", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("ip.contiene(cidr, ip_o_subred)", u);
        RedIPv4 r{};
        if (!parsear_cidr_v4(a[0].a_texto(), r, false)) return Valor::de_bool(false);
        std::string objetivo = a[1].a_texto();
        if (objetivo.find('/') != std::string::npos) {
            RedIPv4 sub{};
            if (!parsear_cidr_v4(objetivo, sub, false)) return Valor::de_bool(false);
            return Valor::de_bool(sub.red >= r.red && sub.broadcast <= r.broadcast);
        }
        std::uint32_t ip = 0;
        if (!parsear_ipv4(objetivo, ip)) return Valor::de_bool(false);
        return Valor::de_bool(ip >= r.red && ip <= r.broadcast);
    });

    // Equivalente a ip.reverse_pointer en Python (.in-addr.arpa / .ip6.arpa)
    registrar_fn(*m->entorno, "puntero_inverso", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.puntero_inverso(direccion)", u);
        std::string s = a[0].a_texto();
        std::uint32_t v4 = 0;
        if (parsear_ipv4(s, v4)) {
            std::ostringstream out;
            out << (v4 & 0xFF) << '.' << ((v4 >> 8) & 0xFF) << '.' << ((v4 >> 16) & 0xFF) << '.'
                << ((v4 >> 24) & 0xFF) << ".in-addr.arpa";
            return Valor::de_texto(out.str());
        }
        std::array<std::uint8_t, 16> v6{};
        if (parsear_ipv6(s, v6)) {
            static const char* hex = "0123456789abcdef";
            std::ostringstream out;
            for (int i = 15; i >= 0; --i) {
                out << hex[v6[i] & 0x0F] << '.' << hex[(v6[i] >> 4) & 0x0F] << '.';
            }
            out << "ip6.arpa";
            return Valor::de_texto(out.str());
        }
        throw ErrorEjecucion("direccion IP no valida: " + s, u);
    });

    registrar_fn(*m->entorno, "validar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.validar(direccion)", u);
        std::string s = a[0].a_texto();
        std::uint32_t v4 = 0;
        std::array<std::uint8_t, 16> v6{};
        return Valor::de_bool(parsear_ipv4(s, v4) || parsear_ipv6(s, v6));
    });

    registrar_fn(*m->entorno, "a_entero", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.a_entero(ipv4)", u);
        std::uint32_t v4 = 0;
        if (!parsear_ipv4(a[0].a_texto(), v4)) return Valor::nulo();
        return Valor::de_entero(static_cast<std::int64_t>(v4));
    });

    registrar_fn(*m->entorno, "desde_entero", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("ip.desde_entero(numero)", u);
        std::int64_t val = static_cast<std::int64_t>(a[0].numero());
        if (val < 0 || val > 0xFFFFFFFFLL) return Valor::nulo();
        return Valor::de_texto(formatear_ipv4(static_cast<std::uint32_t>(val)));
    });

    registrar_fn(*m->entorno, "latencia_tcp", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("ip.latencia_tcp(ip, puerto, timeout_ms?)", u);
        return Valor::de_decimal(sonda_tcp_ip_ms(a[0].a_texto(), static_cast<int>(a[1].numero()),
                                                 a.size() > 2 ? static_cast<int>(a[2].numero()) : 800));
    });

    registrar_fn(*m->entorno, "enrutar_mas_rapida", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2 || a[0].tipo != TipoValor::Lista)
            throw ErrorEjecucion("ip.enrutar_mas_rapida(lista_ips, puerto, timeout_ms?)", u);
        int puerto = static_cast<int>(a[1].numero());
        int timeout = a.size() > 2 ? static_cast<int>(a[2].numero()) : 800;

        struct Sonda { std::string ip; double ms; };
        std::vector<std::thread> hilos;
        std::vector<Sonda> sondas(a[0].lista->size());
        for (std::size_t i = 0; i < a[0].lista->size(); ++i) {
            std::string ip_actual = (*a[0].lista)[i].a_texto();
            sondas[i] = {ip_actual, -1.0};
            hilos.emplace_back([&, i, ip_actual]() {
                sondas[i].ms = sonda_tcp_ip_ms(ip_actual, puerto, timeout);
            });
        }
        for (auto& h : hilos) if (h.joinable()) h.join();

        const Sonda* mejor = nullptr;
        std::vector<Valor> ranking;
        for (const auto& s : sondas) {
            ranking.push_back(Valor::de_mapa({
                {"ip", Valor::de_texto(s.ip)},
                {"activa", Valor::de_bool(s.ms >= 0.0)},
                {"latencia_ms", Valor::de_decimal(s.ms)}
            }));
            if (s.ms >= 0.0 && (!mejor || s.ms < mejor->ms)) mejor = &s;
        }
        return Valor::de_mapa({
            {"mejor_ip", mejor ? Valor::de_texto(mejor->ip) : Valor::nulo()},
            {"mejor_latencia_ms", mejor ? Valor::de_decimal(mejor->ms) : Valor::de_decimal(-1.0)},
            {"resultados", Valor::de_lista(std::move(ranking))}
        });
    });

    // Escaneo paralelo de subred IPv4 estilo nmap/ipaddress
    registrar_fn(*m->entorno, "escanear_red", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("ip.escanear_red(cidr, puerto, timeout_ms?)", u);
        RedIPv4 r{};
        if (!parsear_cidr_v4(a[0].a_texto(), r, false))
            throw ErrorEjecucion("red CIDR IPv4 no valida: " + a[0].a_texto(), u);
        int puerto = static_cast<int>(a[1].numero());
        int timeout = a.size() > 2 ? static_cast<int>(a[2].numero()) : 250;

        std::uint32_t inicio = (r.prefijo < 31) ? (r.red + 1) : r.red;
        std::uint32_t fin = (r.prefijo < 31) ? (r.broadcast - 1) : r.broadcast;
        if (inicio > fin) return Valor::de_lista({});

        std::uint64_t total = static_cast<std::uint64_t>(fin - inicio) + 1ULL;
        if (total > 1024ULL) throw ErrorEjecucion("maximo 1024 hosts por escaneo paralelo", u);

        struct HostInfo { std::string ip; double ms; };
        std::vector<HostInfo> resultados(static_cast<std::size_t>(total));
        std::vector<std::thread> hilos;
        hilos.reserve(static_cast<std::size_t>(total));

        for (std::uint64_t i = 0; i < total; ++i) {
            std::string ip_actual = formatear_ipv4(inicio + static_cast<std::uint32_t>(i));
            resultados[static_cast<std::size_t>(i)] = {ip_actual, -1.0};
            hilos.emplace_back([&, i, ip_actual]() {
                resultados[static_cast<std::size_t>(i)].ms = sonda_tcp_ip_ms(ip_actual, puerto, timeout);
            });
        }
        for (auto& h : hilos) if (h.joinable()) h.join();

        std::vector<Valor> activos;
        for (const auto& h : resultados) {
            if (h.ms >= 0.0) {
                activos.push_back(Valor::de_mapa({
                    {"ip", Valor::de_texto(h.ip)},
                    {"puerto", Valor::de_entero(puerto)},
                    {"latencia_ms", Valor::de_decimal(h.ms)}
                }));
            }
        }
        return Valor::de_lista(std::move(activos));
    });

    return m;
}

}  // namespace esp