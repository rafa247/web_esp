#include "esp/realtime.hpp"
#include "esp/runtime.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
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
using SocketWS = SOCKET;
constexpr SocketWS WS_SOCK_INV = INVALID_SOCKET;
void ws_red_init() {
    static bool ok = false;
    if (!ok) { WSADATA w; WSAStartup(MAKEWORD(2, 2), &w); ok = true; }
}
void ws_cerrar(SocketWS s) { closesocket(s); }
#else
using SocketWS = int;
constexpr SocketWS WS_SOCK_INV = -1;
void ws_red_init() {}
void ws_cerrar(SocketWS s) { close(s); }
#endif

std::string sha1_base64_ws(const std::string& input) {
    // Implementacion SHA-1 estandar RFC 3174 para handshake WebSocket RFC 6455
    std::uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;
    std::vector<std::uint8_t> data(input.begin(), input.end());
    std::uint64_t bit_len = static_cast<std::uint64_t>(data.size()) * 8ULL;
    data.push_back(0x80);
    while ((data.size() % 64) != 56) data.push_back(0x00);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<std::uint8_t>((bit_len >> (i * 8)) & 0xFF));

    auto rotl = [](std::uint32_t x, int n) { return (x << n) | (x >> (32 - n)); };
    for (std::size_t chunk = 0; chunk < data.size(); chunk += 64) {
        std::uint32_t w[80]{};
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<std::uint32_t>(data[chunk + i * 4]) << 24) |
                   (static_cast<std::uint32_t>(data[chunk + i * 4 + 1]) << 16) |
                   (static_cast<std::uint32_t>(data[chunk + i * 4 + 2]) << 8) |
                   static_cast<std::uint32_t>(data[chunk + i * 4 + 3]);
        }
        for (int i = 16; i < 80; ++i) w[i] = rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        std::uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; ++i) {
            std::uint32_t f = 0, k = 0;
            if (i < 20) { f = (b & c) | ((~b) & d); k = 0x5A827999; }
            else if (i < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
            else { f = b ^ c ^ d; k = 0xCA62C1D6; }
            std::uint32_t temp = rotl(a, 5) + f + e + k + w[i];
            e = d; d = c; c = rotl(b, 30); b = a; a = temp;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }

    std::uint8_t digest[20]{};
    std::uint32_t hs[5] = {h0, h1, h2, h3, h4};
    for (int i = 0; i < 5; ++i) {
        digest[i * 4] = static_cast<std::uint8_t>((hs[i] >> 24) & 0xFF);
        digest[i * 4 + 1] = static_cast<std::uint8_t>((hs[i] >> 16) & 0xFF);
        digest[i * 4 + 2] = static_cast<std::uint8_t>((hs[i] >> 8) & 0xFF);
        digest[i * 4 + 3] = static_cast<std::uint8_t>(hs[i] & 0xFF);
    }

    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (std::uint8_t c : digest) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(b64[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(b64[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

void ws_enviar_trama_texto(SocketWS s, const std::string& msg) {
    std::vector<std::uint8_t> frame;
    frame.push_back(0x81);  // FIN + Opcode Text (0x1)
    if (msg.size() <= 125) {
        frame.push_back(static_cast<std::uint8_t>(msg.size()));
    } else if (msg.size() <= 65535) {
        frame.push_back(126);
        frame.push_back(static_cast<std::uint8_t>((msg.size() >> 8) & 0xFF));
        frame.push_back(static_cast<std::uint8_t>(msg.size() & 0xFF));
    }
    frame.insert(frame.end(), msg.begin(), msg.end());
    send(s, reinterpret_cast<const char*>(frame.data()), static_cast<int>(frame.size()), 0);
}

std::string ws_recibir_trama_texto(SocketWS s) {
    std::uint8_t hdr[2]{};
    if (recv(s, reinterpret_cast<char*>(hdr), 2, 0) != 2) return "";
    bool masked = (hdr[1] & 0x80) != 0;
    std::size_t len = hdr[1] & 0x7F;
    if (len == 126) {
        std::uint8_t ext[2]{};
        recv(s, reinterpret_cast<char*>(ext), 2, 0);
        len = (static_cast<std::size_t>(ext[0]) << 8) | ext[1];
    }
    std::uint8_t mask[4]{};
    if (masked) recv(s, reinterpret_cast<char*>(mask), 4, 0);
    std::string payload(len, '\0');
    if (len > 0) recv(s, payload.data(), static_cast<int>(len), 0);
    if (masked) {
        for (std::size_t i = 0; i < len; ++i) payload[i] ^= static_cast<char>(mask[i % 4]);
    }
    return payload;
}

}  // namespace

std::shared_ptr<Modulo> crear_modulo_websockets() {
    static std::mutex ws_mtx;
    static std::int64_t next_ws_id = 1;
    static std::map<std::int64_t, SocketWS> ws_clientes;

    auto m = std::make_shared<Modulo>();
    m->nombre = "websockets";
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "escuchar_uno", [](std::vector<Valor>& a, Ubicacion u) {
        int puerto = a.empty() ? 8765 : static_cast<int>(a[0].numero());
        ws_red_init();
        SocketWS ls = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ls == WS_SOCK_INV) throw ErrorEjecucion("no se pudo crear socket WebSocket", u);
        int opt = 1;
        setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(static_cast<u_short>(puerto));
        if (bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            ws_cerrar(ls);
            throw ErrorEjecucion("puerto WebSocket ocupado: " + std::to_string(puerto), u);
        }
        listen(ls, 16);
        SocketWS cs = accept(ls, nullptr, nullptr);
        ws_cerrar(ls);
        if (cs == WS_SOCK_INV) throw ErrorEjecucion("error aceptando cliente WebSocket", u);

        char buf[4096]{};
        int n = recv(cs, buf, sizeof(buf) - 1, 0);
        std::string req(n > 0 ? buf : "", n > 0 ? static_cast<std::size_t>(n) : 0);
        std::string key;
        auto kpos = req.find("Sec-WebSocket-Key:");
        if (kpos != std::string::npos) {
            kpos += 18;
            while (kpos < req.size() && req[kpos] == ' ') ++kpos;
            auto kend = req.find("\r\n", kpos);
            key = req.substr(kpos, kend - kpos);
        }
        std::string accept_key = sha1_base64_ws(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        std::ostringstream resp;
        resp << "HTTP/1.1 101 Switching Protocols\r\n"
             << "Upgrade: websocket\r\n"
             << "Connection: Upgrade\r\n"
             << "Sec-WebSocket-Accept: " << accept_key << "\r\n\r\n";
        std::string r = resp.str();
        send(cs, r.c_str(), static_cast<int>(r.size()), 0);

        std::lock_guard<std::mutex> lk(ws_mtx);
        std::int64_t id = next_ws_id++;
        ws_clientes[id] = cs;
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "enviar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("websockets.enviar(cliente_id, mensaje)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(ws_mtx);
        auto it = ws_clientes.find(id);
        if (it == ws_clientes.end()) throw ErrorEjecucion("conexion WebSocket no valida", u);
        ws_enviar_trama_texto(it->second, a[1].a_texto());
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "recibir", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("websockets.recibir(cliente_id)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        SocketWS cs = WS_SOCK_INV;
        {
            std::lock_guard<std::mutex> lk(ws_mtx);
            auto it = ws_clientes.find(id);
            if (it == ws_clientes.end()) throw ErrorEjecucion("conexion WebSocket no valida", u);
            cs = it->second;
        }
        return Valor::de_texto(ws_recibir_trama_texto(cs));
    });

    registrar_fn(*m->entorno, "cerrar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("websockets.cerrar(cliente_id)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(ws_mtx);
        auto it = ws_clientes.find(id);
        if (it == ws_clientes.end()) return Valor::de_bool(false);
        ws_cerrar(it->second);
        ws_clientes.erase(it);
        return Valor::de_bool(true);
    });

    return m;
}

}  // namespace esp