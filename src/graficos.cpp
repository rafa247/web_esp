#include "esp/graficos.hpp"
#include "esp/runtime.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
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
#include <mmsystem.h>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "winmm.lib")
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

constexpr std::array<std::uint8_t, 8> glifo_8x8(char c) {
    unsigned char u = static_cast<unsigned char>(std::toupper(static_cast<unsigned char>(c)));
    switch (u) {
        case '0': return {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00};
        case '1': return {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00};
        case '2': return {0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E, 0x00};
        case '3': return {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00};
        case '4': return {0x0C, 0x1C, 0x2C, 0x4C, 0x7E, 0x0C, 0x0C, 0x00};
        case '5': return {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00};
        case '6': return {0x3C, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x3C, 0x00};
        case '7': return {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00};
        case '8': return {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00};
        case '9': return {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00};
        case 'A': return {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00};
        case 'B': return {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00};
        case 'C': return {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00};
        case 'D': return {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00};
        case 'E': return {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00};
        case 'F': return {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00};
        case 'G': return {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3E, 0x00};
        case 'H': return {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00};
        case 'I': return {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00};
        case 'J': return {0x3E, 0x0C, 0x0C, 0x0C, 0x6C, 0x6C, 0x38, 0x00};
        case 'K': return {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00};
        case 'L': return {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00};
        case 'M': return {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00};
        case 'N': return {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00};
        case 'O': return {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00};
        case 'P': return {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00};
        case 'Q': return {0x3C, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x0E, 0x00};
        case 'R': return {0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00};
        case 'S': return {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00};
        case 'T': return {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00};
        case 'U': return {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00};
        case 'V': return {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00};
        case 'W': return {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00};
        case 'X': return {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00};
        case 'Y': return {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00};
        case 'Z': return {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00};
        case ':': return {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00};
        case '.': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00};
        case '-': return {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00};
        case '+': return {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00};
        case '/': return {0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00, 0x00};
        case '!': return {0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00};
        case '(': return {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00};
        case ')': return {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00};
        case ' ': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        default:  return {0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00};
    }
}

struct Textura {
    int ancho = 0;
    int alto = 0;
    std::vector<std::uint32_t> pixeles;
};

bool cargar_bmp_portable(const std::string& ruta, Textura& out) {
    std::ifstream f(ruta, std::ios::binary);
    if (!f) return false;
    std::uint8_t hdr[54]{};
    f.read(reinterpret_cast<char*>(hdr), 54);
    if (f.gcount() < 54 || hdr[0] != 'B' || hdr[1] != 'M') return false;

    std::uint32_t offset = *reinterpret_cast<std::uint32_t*>(&hdr[10]);
    std::int32_t ancho = *reinterpret_cast<std::int32_t*>(&hdr[18]);
    std::int32_t alto_raw = *reinterpret_cast<std::int32_t*>(&hdr[22]);
    std::uint16_t bpp = *reinterpret_cast<std::uint16_t*>(&hdr[28]);
    if (ancho <= 0 || alto_raw == 0 || (bpp != 24 && bpp != 32)) return false;

    bool top_down = alto_raw < 0;
    int alto = std::abs(alto_raw);
    out.ancho = ancho;
    out.alto = alto;
    out.pixeles.assign(static_cast<std::size_t>(ancho * alto), 0xFFFFFFFF);

    f.seekg(offset, std::ios::beg);
    int bytes_por_pixel = bpp / 8;
    int fila_stride = ((ancho * bytes_por_pixel + 3) / 4) * 4;
    std::vector<std::uint8_t> fila_buf(static_cast<std::size_t>(fila_stride));

    for (int y = 0; y < alto; ++y) {
        f.read(reinterpret_cast<char*>(fila_buf.data()), fila_stride);
        int dest_y = top_down ? y : (alto - 1 - y);
        for (int x = 0; x < ancho; ++x) {
            std::size_t off = static_cast<std::size_t>(x * bytes_por_pixel);
            std::uint8_t b = fila_buf[off];
            std::uint8_t g = fila_buf[off + 1];
            std::uint8_t r = fila_buf[off + 2];
            std::uint8_t a = (bpp == 32) ? fila_buf[off + 3] : 255;
            out.pixeles[static_cast<std::size_t>(dest_y * ancho + x)] =
                (static_cast<std::uint32_t>(a) << 24) |
                (static_cast<std::uint32_t>(r) << 16) |
                (static_cast<std::uint32_t>(g) << 8) |
                static_cast<std::uint32_t>(b);
        }
    }
    return true;
}

bool cargar_imagen_nativa(const std::string& ruta, Textura& out) {
#ifdef _WIN32
    static bool com_ok = false;
    if (!com_ok) {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        com_ok = true;
    }
    IWICImagingFactory* factory = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&factory))) && factory) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, ruta.c_str(), -1, nullptr, 0);
        std::wstring wruta(static_cast<std::size_t>(wlen), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, ruta.c_str(), -1, wruta.data(), wlen);

        IWICBitmapDecoder* decoder = nullptr;
        if (SUCCEEDED(factory->CreateDecoderFromFilename(wruta.c_str(), nullptr, GENERIC_READ,
                                                         WICDecodeMetadataCacheOnLoad, &decoder)) && decoder) {
            IWICBitmapFrameDecode* frame = nullptr;
            if (SUCCEEDED(decoder->GetFrame(0, &frame)) && frame) {
                IWICFormatConverter* converter = nullptr;
                if (SUCCEEDED(factory->CreateFormatConverter(&converter)) && converter) {
                    if (SUCCEEDED(converter->Initialize(frame, GUID_WICPixelFormat32bppBGRA,
                                                        WICBitmapDitherTypeNone, nullptr, 0.0,
                                                        WICBitmapPaletteTypeCustom))) {
                        UINT w = 0, h = 0;
                        converter->GetSize(&w, &h);
                        if (w > 0 && h > 0) {
                            out.ancho = static_cast<int>(w);
                            out.alto = static_cast<int>(h);
                            out.pixeles.resize(static_cast<std::size_t>(w * h));
                            converter->CopyPixels(nullptr, w * 4, static_cast<UINT>(out.pixeles.size() * 4),
                                                  reinterpret_cast<BYTE*>(out.pixeles.data()));
                            converter->Release();
                            frame->Release();
                            decoder->Release();
                            factory->Release();
                            return true;
                        }
                    }
                    converter->Release();
                }
                frame->Release();
            }
            decoder->Release();
        }
        factory->Release();
    }
#endif
    return cargar_bmp_portable(ruta, out);
}

struct Vec2 {
    float u = 0.0f;
    float v = 0.0f;
};

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Triangulo {
    Vec3 a, b, c;
    Vec2 uva{0.0f, 1.0f}, uvb{0.0f, 0.0f}, uvc{1.0f, 0.0f};
    std::uint32_t color = 0xFFFFFFFF;
};

struct Objeto3D {
    std::vector<Triangulo> malla;
    Vec3 pos{0.0f, 0.0f, 5.0f};
    Vec3 rot{0.0f, 0.0f, 0.0f};
    Vec3 escala{1.0f, 1.0f, 1.0f};
    std::int64_t textura_id = 0;
    bool visible = true;
};

struct EstadoRender {
    int ancho = 800;
    int alto = 600;
    std::vector<std::uint32_t> color;
    std::vector<float> zbuffer;
    bool abierta = false;
    std::chrono::steady_clock::time_point ultimo_frame = std::chrono::steady_clock::now();
    double delta_seg = 0.016;

    Vec3 cam_pos{0.0f, 0.0f, 0.0f};
    Vec3 cam_rot{0.0f, 0.0f, 0.0f};
    float fov_grados = 70.0f;

    bool luz_puntual_activa = false;
    Vec3 luz_pos{0.0f, 3.0f, 1.0f};
    float luz_intensidad = 1.5f;

    bool niebla_activa = false;
    float niebla_dist = 25.0f;
    std::uint32_t niebla_color = 0xFF101522U;

    std::int64_t siguiente_obj = 1;
    std::map<std::int64_t, Objeto3D> objetos;

    std::int64_t siguiente_tex = 1;
    std::map<std::int64_t, Textura> texturas;

#ifdef _WIN32
    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    BITMAPINFO bmi{};
    bool clase_registrada = false;
#endif
};

std::mutex g_gfx_mutex;
EstadoRender g_gfx;

inline void poner_pixel_seguro(int x, int y, std::uint32_t col) {
    if (x >= 0 && x < g_gfx.ancho && y >= 0 && y < g_gfx.alto) {
        std::uint8_t a = static_cast<std::uint8_t>((col >> 24) & 0xFF);
        if (a == 0) return;
        std::size_t idx = static_cast<std::size_t>(y * g_gfx.ancho + x);
        if (a == 255) {
            g_gfx.color[idx] = col;
        } else {
            std::uint32_t dst = g_gfx.color[idx];
            std::uint32_t inv = 255U - a;
            std::uint32_t r = (((col >> 16) & 0xFF) * a + ((dst >> 16) & 0xFF) * inv) >> 8;
            std::uint32_t g = (((col >> 8) & 0xFF) * a + ((dst >> 8) & 0xFF) * inv) >> 8;
            std::uint32_t b = ((col & 0xFF) * a + (dst & 0xFF) * inv) >> 8;
            g_gfx.color[idx] = 0xFF000000U | (r << 16) | (g << 8) | b;
        }
    }
}

void dibujar_linea_2d(int x0, int y0, int x1, int y1, std::uint32_t col) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        poner_pixel_seguro(x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void dibujar_texto_2d(int x, int y, const std::string& texto, std::uint32_t col, int escala = 2) {
    int cursor_x = x;
    int cursor_y = y;
    for (char ch : texto) {
        if (ch == '\n') {
            cursor_x = x;
            cursor_y += 9 * escala;
            continue;
        }
        auto bitmap = glifo_8x8(ch);
        for (int row = 0; row < 8; ++row) {
            for (int bit = 0; bit < 8; ++bit) {
                if (bitmap[static_cast<std::size_t>(row)] & (0x80 >> bit)) {
                    for (int sy = 0; sy < escala; ++sy) {
                        for (int sx = 0; sx < escala; ++sx) {
                            poner_pixel_seguro(cursor_x + bit * escala + sx, cursor_y + row * escala + sy, col);
                        }
                    }
                }
            }
        }
        cursor_x += 8 * escala;
    }
}

std::uint32_t aplicar_luz_y_niebla(std::uint32_t col, float luz, float z) {
    float r = static_cast<float>((col >> 16) & 0xFF) * luz;
    float g = static_cast<float>((col >> 8) & 0xFF) * luz;
    float b = static_cast<float>(col & 0xFF) * luz;

    if (g_gfx.niebla_activa && g_gfx.niebla_dist > 0.1f) {
        float f = std::clamp(z / g_gfx.niebla_dist, 0.0f, 1.0f);
        float nr = static_cast<float>((g_gfx.niebla_color >> 16) & 0xFF);
        float ng = static_cast<float>((g_gfx.niebla_color >> 8) & 0xFF);
        float nb = static_cast<float>(g_gfx.niebla_color & 0xFF);
        r = r * (1.0f - f) + nr * f;
        g = g * (1.0f - f) + ng * f;
        b = b * (1.0f - f) + nb * f;
    }

    std::uint32_t ri = static_cast<std::uint32_t>(std::clamp(r, 0.0f, 255.0f));
    std::uint32_t gi = static_cast<std::uint32_t>(std::clamp(g, 0.0f, 255.0f));
    std::uint32_t bi = static_cast<std::uint32_t>(std::clamp(b, 0.0f, 255.0f));
    return 0xFF000000U | (ri << 16) | (gi << 8) | bi;
}

void rasterizar_triangulo_z(Vec3 p0, Vec3 p1, Vec3 p2, Vec2 uv0, Vec2 uv1, Vec2 uv2,
                            std::uint32_t col_base, float luz, const Textura* tex) {
    if (p0.z <= 0.05f || p1.z <= 0.05f || p2.z <= 0.05f) return;

    int min_x = std::max(0, static_cast<int>(std::floor(std::min({p0.x, p1.x, p2.x}))));
    int max_x = std::min(g_gfx.ancho - 1, static_cast<int>(std::ceil(std::max({p0.x, p1.x, p2.x}))));
    int min_y = std::max(0, static_cast<int>(std::floor(std::min({p0.y, p1.y, p2.y}))));
    int max_y = std::min(g_gfx.alto - 1, static_cast<int>(std::ceil(std::max({p0.y, p1.y, p2.y}))));

    if (min_x > max_x || min_y > max_y) return;

    float area = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
    if (std::fabs(area) < 1e-4f) return;
    float inv_area = 1.0f / area;

    float inv_z0 = 1.0f / p0.z;
    float inv_z1 = 1.0f / p1.z;
    float inv_z2 = 1.0f / p2.z;

    float u0_z = uv0.u * inv_z0, v0_z = uv0.v * inv_z0;
    float u1_z = uv1.u * inv_z1, v1_z = uv1.v * inv_z1;
    float u2_z = uv2.u * inv_z2, v2_z = uv2.v * inv_z2;

    for (int y = min_y; y <= max_y; ++y) {
        float py = static_cast<float>(y) + 0.5f;
        std::size_t row_offset = static_cast<std::size_t>(y * g_gfx.ancho);
        for (int x = min_x; x <= max_x; ++x) {
            float px = static_cast<float>(x) + 0.5f;
            float w0 = ((p1.x - px) * (p2.y - py) - (p1.y - py) * (p2.x - px)) * inv_area;
            float w1 = ((p2.x - px) * (p0.y - py) - (p2.y - py) * (p0.x - px)) * inv_area;
            float w2 = 1.0f - w0 - w1;
            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
                float inv_z = w0 * inv_z0 + w1 * inv_z1 + w2 * inv_z2;
                float z = 1.0f / inv_z;
                std::size_t idx = row_offset + static_cast<std::size_t>(x);
                if (z < g_gfx.zbuffer[idx]) {
                    g_gfx.zbuffer[idx] = z;
                    std::uint32_t sample = col_base;
                    if (tex && tex->ancho > 0 && tex->alto > 0) {
                        float u = std::clamp((w0 * u0_z + w1 * u1_z + w2 * u2_z) * z, 0.0f, 0.9999f);
                        float v = std::clamp((w0 * v0_z + w1 * v1_z + w2 * v2_z) * z, 0.0f, 0.9999f);
                        int tx = static_cast<int>(u * static_cast<float>(tex->ancho));
                        int ty = static_cast<int>(v * static_cast<float>(tex->alto));
                        sample = tex->pixeles[static_cast<std::size_t>(ty * tex->ancho + tx)];
                    }
                    g_gfx.color[idx] = aplicar_luz_y_niebla(sample, luz, z);
                }
            }
        }
    }
}

Vec3 rotar_vec3(Vec3 p, Vec3 r) {
    float cx = std::cos(r.x), sx = std::sin(r.x);
    float cy = std::cos(r.y), sy = std::sin(r.y);
    float cz = std::cos(r.z), sz = std::sin(r.z);
    float y1 = p.y * cx - p.z * sx;
    float z1 = p.y * sx + p.z * cx;
    float x2 = p.x * cy + z1 * sy;
    float z2 = -p.x * sy + z1 * cy;
    float x3 = x2 * cz - y1 * sz;
    float y3 = x2 * sz + y1 * cz;
    return {x3, y3, z2};
}

Vec3 rotar_inverso_camara(Vec3 p, Vec3 r) {
    float cx = std::cos(-r.x), sx = std::sin(-r.x);
    float cy = std::cos(-r.y), sy = std::sin(-r.y);
    float cz = std::cos(-r.z), sz = std::sin(-r.z);
    float x1 = p.x * cz - p.y * sz;
    float y1 = p.x * sz + p.y * cz;
    float x2 = x1 * cy + p.z * sy;
    float z2 = -x1 * sy + p.z * cy;
    float y3 = y1 * cx - z2 * sx;
    float z3 = y1 * sx + z2 * cx;
    return {x2, y3, z3};
}

#ifdef _WIN32
LRESULT CALLBACK esp_gfx_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CLOSE || msg == WM_DESTROY) {
        g_gfx.abierta = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}
#endif

}  // namespace

std::shared_ptr<Modulo> crear_modulo_graficos(const std::string& nombre_alias) {
    auto m = std::make_shared<Modulo>();
    m->nombre = nombre_alias;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "iniciar", [](std::vector<Valor>& a, Ubicacion u) {
        int w = a.size() > 0 ? static_cast<int>(a[0].numero()) : 800;
        int h = a.size() > 1 ? static_cast<int>(a[1].numero()) : 600;
        std::string titulo = a.size() > 2 ? a[2].a_texto() : "ESP 2.0.0 - Motor Grafico 2D/3D";

        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        g_gfx.ancho = std::max(64, w);
        g_gfx.alto = std::max(64, h);
        g_gfx.color.assign(static_cast<std::size_t>(g_gfx.ancho * g_gfx.alto), 0xFF101522);
        g_gfx.zbuffer.assign(static_cast<std::size_t>(g_gfx.ancho * g_gfx.alto), std::numeric_limits<float>::infinity());
        g_gfx.ultimo_frame = std::chrono::steady_clock::now();

#ifdef _WIN32
        HINSTANCE hInst = GetModuleHandleA(nullptr);
        if (!g_gfx.clase_registrada) {
            WNDCLASSA wc{};
            wc.lpfnWndProc = esp_gfx_wndproc;
            wc.hInstance = hInst;
            wc.lpszClassName = "ESPMotorGfx2D3D_V4";
            wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
            RegisterClassA(&wc);
            g_gfx.clase_registrada = true;
        }
        if (g_gfx.hwnd) {
            if (g_gfx.hdc) ReleaseDC(g_gfx.hwnd, g_gfx.hdc);
            DestroyWindow(g_gfx.hwnd);
            g_gfx.hwnd = nullptr;
            g_gfx.hdc = nullptr;
        }
        RECT rc{0, 0, g_gfx.ancho, g_gfx.alto};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        g_gfx.hwnd = CreateWindowExA(0, "ESPMotorGfx2D3D_V4", titulo.c_str(),
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                                     rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
        if (!g_gfx.hwnd) throw ErrorEjecucion("no se pudo crear la ventana grafica", u);
        g_gfx.hdc = GetDC(g_gfx.hwnd);
        std::memset(&g_gfx.bmi, 0, sizeof(g_gfx.bmi));
        g_gfx.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        g_gfx.bmi.bmiHeader.biWidth = g_gfx.ancho;
        g_gfx.bmi.bmiHeader.biHeight = -g_gfx.alto;
        g_gfx.bmi.bmiHeader.biPlanes = 1;
        g_gfx.bmi.bmiHeader.biBitCount = 32;
        g_gfx.bmi.bmiHeader.biCompression = BI_RGB;
#endif
        g_gfx.abierta = true;
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "cerrar", [](std::vector<Valor>&, Ubicacion) {
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
#ifdef _WIN32
        if (g_gfx.hwnd) {
            if (g_gfx.hdc) ReleaseDC(g_gfx.hwnd, g_gfx.hdc);
            DestroyWindow(g_gfx.hwnd);
            g_gfx.hwnd = nullptr;
            g_gfx.hdc = nullptr;
        }
#endif
        g_gfx.abierta = false;
        g_gfx.objetos.clear();
        g_gfx.texturas.clear();
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "activa", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
        MSG msg{};
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) g_gfx.abierta = false;
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
#endif
        return Valor::de_bool(g_gfx.abierta);
    });

    registrar_fn(*m->entorno, "delta_tiempo", [](std::vector<Valor>&, Ubicacion) {
        return Valor::de_decimal(g_gfx.delta_seg);
    });

    registrar_fn(*m->entorno, "tecla", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("graficos.tecla(codigo_tecla)", u);
#ifdef _WIN32
        int vk = static_cast<int>(a[0].numero());
        return Valor::de_bool((GetAsyncKeyState(vk) & 0x8000) != 0);
#else
        return Valor::de_bool(false);
#endif
    });

    registrar_fn(*m->entorno, "raton", [](std::vector<Valor>&, Ubicacion) {
        int rx = 0, ry = 0;
        bool izq = false, der = false;
#ifdef _WIN32
        POINT pt{};
        if (g_gfx.hwnd && GetCursorPos(&pt)) {
            ScreenToClient(g_gfx.hwnd, &pt);
            rx = pt.x;
            ry = pt.y;
        }
        izq = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        der = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
#endif
        return Valor::de_mapa({
            {"x", Valor::de_entero(rx)},
            {"y", Valor::de_entero(ry)},
            {"izquierdo", Valor::de_bool(izq)},
            {"derecho", Valor::de_bool(der)}
        });
    });

    registrar_fn(*m->entorno, "color_rgb", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("graficos.color_rgb(r, g, b, a?)", u);
        std::uint32_t r = static_cast<std::uint32_t>(std::clamp(static_cast<int>(a[0].numero()), 0, 255));
        std::uint32_t g = static_cast<std::uint32_t>(std::clamp(static_cast<int>(a[1].numero()), 0, 255));
        std::uint32_t b = static_cast<std::uint32_t>(std::clamp(static_cast<int>(a[2].numero()), 0, 255));
        std::uint32_t al = a.size() > 3 ? static_cast<std::uint32_t>(std::clamp(static_cast<int>(a[3].numero()), 0, 255)) : 255U;
        return Valor::de_entero(static_cast<std::int64_t>((al << 24) | (r << 16) | (g << 8) | b));
    });

    registrar_fn(*m->entorno, "limpiar", [](std::vector<Valor>& a, Ubicacion) {
        std::uint32_t col = a.empty() ? 0xFF101522U : static_cast<std::uint32_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::fill(g_gfx.color.begin(), g_gfx.color.end(), col);
        std::fill(g_gfx.zbuffer.begin(), g_gfx.zbuffer.end(), std::numeric_limits<float>::infinity());
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "cargar_imagen", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("graficos.cargar_imagen(ruta_png_jpg_bmp)", u);
        std::string ruta = a[0].a_texto();
        Textura tex;
        if (!cargar_imagen_nativa(ruta, tex))
            throw ErrorEjecucion("no se pudo cargar la imagen (PNG/JPG/BMP): " + ruta, u);
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::int64_t id = g_gfx.siguiente_tex++;
        g_gfx.texturas[id] = std::move(tex);
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "info_imagen", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("graficos.info_imagen(imagen_id)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto it = g_gfx.texturas.find(id);
        if (it == g_gfx.texturas.end()) throw ErrorEjecucion("imagen no encontrada", u);
        return Valor::de_mapa({
            {"ancho", Valor::de_entero(it->second.ancho)},
            {"alto", Valor::de_entero(it->second.alto)}
        });
    });

    registrar_fn(*m->entorno, "dibujar_imagen", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("motor2d.dibujar_imagen(imagen_id, x, y, ancho?, alto?)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        int dst_x = static_cast<int>(a[1].numero());
        int dst_y = static_cast<int>(a[2].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto it = g_gfx.texturas.find(id);
        if (it == g_gfx.texturas.end()) throw ErrorEjecucion("imagen no encontrada", u);
        const Textura& tex = it->second;
        int dst_w = a.size() > 3 ? static_cast<int>(a[3].numero()) : tex.ancho;
        int dst_h = a.size() > 4 ? static_cast<int>(a[4].numero()) : tex.alto;
        if (dst_w <= 0 || dst_h <= 0 || tex.ancho <= 0 || tex.alto <= 0) return Valor::de_bool(false);

        for (int y = 0; y < dst_h; ++y) {
            int py = dst_y + y;
            if (py < 0 || py >= g_gfx.alto) continue;
            int ty = (y * tex.alto) / dst_h;
            for (int x = 0; x < dst_w; ++x) {
                int px = dst_x + x;
                if (px < 0 || px >= g_gfx.ancho) continue;
                int tx = (x * tex.ancho) / dst_w;
                poner_pixel_seguro(px, py, tex.pixeles[static_cast<std::size_t>(ty * tex.ancho + tx)]);
            }
        }
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "dibujar_sprite", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 7)
            throw ErrorEjecucion("motor2d.dibujar_sprite(imagen_id, sx, sy, sw, sh, dx, dy, dw?, dh?)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        int sx = static_cast<int>(a[1].numero());
        int sy = static_cast<int>(a[2].numero());
        int sw = static_cast<int>(a[3].numero());
        int sh = static_cast<int>(a[4].numero());
        int dx = static_cast<int>(a[5].numero());
        int dy = static_cast<int>(a[6].numero());
        int dw = a.size() > 7 ? static_cast<int>(a[7].numero()) : sw;
        int dh = a.size() > 8 ? static_cast<int>(a[8].numero()) : sh;

        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto it = g_gfx.texturas.find(id);
        if (it == g_gfx.texturas.end()) throw ErrorEjecucion("imagen no encontrada", u);
        const Textura& tex = it->second;
        if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0) return Valor::de_bool(false);

        for (int y = 0; y < dh; ++y) {
            int py = dy + y;
            if (py < 0 || py >= g_gfx.alto) continue;
            int ty = std::clamp(sy + (y * sh) / dh, 0, tex.alto - 1);
            for (int x = 0; x < dw; ++x) {
                int px = dx + x;
                if (px < 0 || px >= g_gfx.ancho) continue;
                int tx = std::clamp(sx + (x * sw) / dw, 0, tex.ancho - 1);
                poner_pixel_seguro(px, py, tex.pixeles[static_cast<std::size_t>(ty * tex.ancho + tx)]);
            }
        }
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "aplicar_textura", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("motor3d.aplicar_textura(objeto_id, imagen_id)", u);
        std::int64_t obj_id = static_cast<std::int64_t>(a[0].numero());
        std::int64_t tex_id = static_cast<std::int64_t>(a[1].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto it = g_gfx.objetos.find(obj_id);
        if (it == g_gfx.objetos.end()) throw ErrorEjecucion("objeto 3D no encontrado", u);
        if (tex_id != 0 && !g_gfx.texturas.count(tex_id)) throw ErrorEjecucion("textura no encontrada", u);
        it->second.textura_id = tex_id;
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "pixel", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("motor2d.pixel(x, y, color)", u);
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        poner_pixel_seguro(static_cast<int>(a[0].numero()), static_cast<int>(a[1].numero()),
                           static_cast<std::uint32_t>(a[2].numero()));
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "rectangulo", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 5) throw ErrorEjecucion("motor2d.rectangulo(x, y, ancho, alto, color)", u);
        int x0 = static_cast<int>(a[0].numero());
        int y0 = static_cast<int>(a[1].numero());
        int w = static_cast<int>(a[2].numero());
        int h = static_cast<int>(a[3].numero());
        std::uint32_t col = static_cast<std::uint32_t>(a[4].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        int x1 = std::min(g_gfx.ancho, x0 + w);
        int y1 = std::min(g_gfx.alto, y0 + h);
        for (int y = std::max(0, y0); y < y1; ++y) {
            for (int x = std::max(0, x0); x < x1; ++x) poner_pixel_seguro(x, y, col);
        }
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "circulo", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 4) throw ErrorEjecucion("motor2d.circulo(cx, cy, radio, color)", u);
        int cx = static_cast<int>(a[0].numero());
        int cy = static_cast<int>(a[1].numero());
        int r = static_cast<int>(a[2].numero());
        std::uint32_t col = static_cast<std::uint32_t>(a[3].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        int r2 = r * r;
        for (int dy = -r; dy <= r; ++dy) {
            int y = cy + dy;
            if (y < 0 || y >= g_gfx.alto) continue;
            for (int dx = -r; dx <= r; ++dx) {
                if (dx * dx + dy * dy <= r2) poner_pixel_seguro(cx + dx, y, col);
            }
        }
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "linea", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 5) throw ErrorEjecucion("motor2d.linea(x0, y0, x1, y1, color)", u);
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        dibujar_linea_2d(static_cast<int>(a[0].numero()), static_cast<int>(a[1].numero()),
                         static_cast<int>(a[2].numero()), static_cast<int>(a[3].numero()),
                         static_cast<std::uint32_t>(a[4].numero()));
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "texto", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("motor2d.texto(x, y, cadena, color?, escala?)", u);
        int x = static_cast<int>(a[0].numero());
        int y = static_cast<int>(a[1].numero());
        std::string txt = a[2].a_texto();
        std::uint32_t col = a.size() > 3 ? static_cast<std::uint32_t>(a[3].numero()) : 0xFFFFFFFFU;
        int esc = a.size() > 4 ? std::max(1, static_cast<int>(a[4].numero())) : 2;
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        dibujar_texto_2d(x, y, txt, col, esc);
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "colisiona", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 8) throw ErrorEjecucion("motor2d.colisiona(x1, y1, w1, h1, x2, y2, w2, h2)", u);
        double x1 = a[0].numero(), y1 = a[1].numero(), w1 = a[2].numero(), h1 = a[3].numero();
        double x2 = a[4].numero(), y2 = a[5].numero(), w2 = a[6].numero(), h2 = a[7].numero();
        return Valor::de_bool((x1 < x2 + w2) && (x1 + w1 > x2) && (y1 < y2 + h2) && (y1 + h1 > y2));
    });

    registrar_fn(*m->entorno, "luz", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 4) throw ErrorEjecucion("motor3d.luz(x, y, z, intensidad)", u);
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        g_gfx.luz_puntual_activa = true;
        g_gfx.luz_pos = {static_cast<float>(a[0].numero()), static_cast<float>(a[1].numero()), static_cast<float>(a[2].numero())};
        g_gfx.luz_intensidad = static_cast<float>(a[3].numero());
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "niebla", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("motor3d.niebla(activa, distancia_max, color)", u);
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        g_gfx.niebla_activa = a[0].verdad();
        g_gfx.niebla_dist = static_cast<float>(a[1].numero());
        g_gfx.niebla_color = static_cast<std::uint32_t>(a[2].numero());
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "crear_cubo", [](std::vector<Valor>& a, Ubicacion) {
        std::uint32_t c1 = a.size() > 0 ? static_cast<std::uint32_t>(a[0].numero()) : 0xFF54E5D0U;
        std::uint32_t c2 = a.size() > 1 ? static_cast<std::uint32_t>(a[1].numero()) : 0xFF70A7FFU;
        std::uint32_t c3 = a.size() > 2 ? static_cast<std::uint32_t>(a[2].numero()) : 0xFFA68BFFU;

        Vec3 v[8] = {
            {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
            {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}
        };
        Vec2 t00{0.0f, 1.0f}, t01{0.0f, 0.0f}, t11{1.0f, 0.0f}, t10{1.0f, 1.0f};

        Objeto3D obj;
        obj.malla = {
            {v[0], v[3], v[2], t00, t01, t11, c1}, {v[0], v[2], v[1], t00, t11, t10, c1},
            {v[5], v[6], v[7], t00, t01, t11, c1}, {v[5], v[7], v[4], t00, t11, t10, c1},
            {v[4], v[7], v[3], t00, t01, t11, c2}, {v[4], v[3], v[0], t00, t11, t10, c2},
            {v[1], v[2], v[6], t00, t01, t11, c2}, {v[1], v[6], v[5], t00, t11, t10, c2},
            {v[3], v[7], v[6], t00, t01, t11, c3}, {v[3], v[6], v[2], t00, t11, t10, c3},
            {v[4], v[0], v[1], t00, t01, t11, c3}, {v[4], v[1], v[5], t00, t11, t10, c3}
        };

        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::int64_t id = g_gfx.siguiente_obj++;
        g_gfx.objetos[id] = std::move(obj);
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "crear_piramide", [](std::vector<Valor>& a, Ubicacion) {
        std::uint32_t c1 = a.size() > 0 ? static_cast<std::uint32_t>(a[0].numero()) : 0xFFF4C66AU;
        std::uint32_t c2 = a.size() > 1 ? static_cast<std::uint32_t>(a[1].numero()) : 0xFFF08080U;

        Vec3 apex{0.0f, 0.6f, 0.0f};
        Vec3 b0{-0.5f, -0.4f, -0.5f}, b1{0.5f, -0.4f, -0.5f}, b2{0.5f, -0.4f, 0.5f}, b3{-0.5f, -0.4f, 0.5f};
        Vec2 t00{0.0f, 1.0f}, ttop{0.5f, 0.0f}, t10{1.0f, 1.0f};

        Objeto3D obj;
        obj.malla = {
            {b0, apex, b1, t00, ttop, t10, c1},
            {b1, apex, b2, t00, ttop, t10, c2},
            {b2, apex, b3, t00, ttop, t10, c1},
            {b3, apex, b0, t00, ttop, t10, c2},
            {b3, b0, b1, {0, 1}, {0, 0}, {1, 0}, c2},
            {b3, b1, b2, {0, 1}, {1, 0}, {1, 1}, c2}
        };

        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::int64_t id = g_gfx.siguiente_obj++;
        g_gfx.objetos[id] = std::move(obj);
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "crear_plano", [](std::vector<Valor>& a, Ubicacion) {
        std::uint32_t col = a.size() > 0 ? static_cast<std::uint32_t>(a[0].numero()) : 0xFF273246U;
        Vec3 p0{-2.0f, 0.0f, -2.0f}, p1{2.0f, 0.0f, -2.0f}, p2{2.0f, 0.0f, 2.0f}, p3{-2.0f, 0.0f, 2.0f};
        Objeto3D obj;
        obj.malla = {
            {p0, p3, p2, {0, 1}, {0, 0}, {1, 0}, col},
            {p0, p2, p1, {0, 1}, {1, 0}, {1, 1}, col}
        };
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::int64_t id = g_gfx.siguiente_obj++;
        g_gfx.objetos[id] = std::move(obj);
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "cargar_obj", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("motor3d.cargar_obj(ruta_obj, color?)", u);
        std::string ruta = a[0].a_texto();
        std::uint32_t col = a.size() > 1 ? static_cast<std::uint32_t>(a[1].numero()) : 0xFFD0D9E8U;

        std::ifstream f(ruta);
        if (!f) throw ErrorEjecucion("no se pudo abrir modelo .obj: " + ruta, u);

        std::vector<Vec3> verts;
        std::vector<Vec2> uvs;
        Objeto3D obj;
        std::string linea;

        while (std::getline(f, linea)) {
            if (linea.size() < 2 || linea[0] == '#') continue;
            std::istringstream ss(linea);
            std::string tipo;
            ss >> tipo;
            if (tipo == "v") {
                Vec3 v{};
                ss >> v.x >> v.y >> v.z;
                verts.push_back(v);
            } else if (tipo == "vt") {
                Vec2 vt{};
                ss >> vt.u >> vt.v;
                vt.v = 1.0f - vt.v;
                uvs.push_back(vt);
            } else if (tipo == "f") {
                std::vector<int> iv, ivt;
                std::string tok;
                while (ss >> tok) {
                    std::size_t s1 = tok.find('/');
                    int vi = std::stoi(tok.substr(0, s1));
                    iv.push_back(vi > 0 ? vi - 1 : static_cast<int>(verts.size()) + vi);
                    if (s1 != std::string::npos) {
                        std::size_t s2 = tok.find('/', s1 + 1);
                        std::string vt_str = tok.substr(s1 + 1, s2 == std::string::npos ? std::string::npos : s2 - s1 - 1);
                        if (!vt_str.empty()) {
                            int vti = std::stoi(vt_str);
                            ivt.push_back(vti > 0 ? vti - 1 : static_cast<int>(uvs.size()) + vti);
                        }
                    }
                }
                for (std::size_t k = 1; k + 1 < iv.size(); ++k) {
                    Triangulo t;
                    t.a = verts[static_cast<std::size_t>(iv[0])];
                    t.b = verts[static_cast<std::size_t>(iv[k])];
                    t.c = verts[static_cast<std::size_t>(iv[k + 1])];
                    if (ivt.size() >= iv.size()) {
                        t.uva = uvs[static_cast<std::size_t>(ivt[0])];
                        t.uvb = uvs[static_cast<std::size_t>(ivt[k])];
                        t.uvc = uvs[static_cast<std::size_t>(ivt[k + 1])];
                    }
                    t.color = col;
                    obj.malla.push_back(t);
                }
            }
        }

        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::int64_t id = g_gfx.siguiente_obj++;
        g_gfx.objetos[id] = std::move(obj);
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "transformar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 7) throw ErrorEjecucion("motor3d.transformar(id, x, y, z, rx, ry, rz)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto it = g_gfx.objetos.find(id);
        if (it == g_gfx.objetos.end()) throw ErrorEjecucion("objeto 3D no encontrado", u);
        it->second.pos = {static_cast<float>(a[1].numero()), static_cast<float>(a[2].numero()), static_cast<float>(a[3].numero())};
        it->second.rot = {static_cast<float>(a[4].numero()), static_cast<float>(a[5].numero()), static_cast<float>(a[6].numero())};
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "escalar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 4) throw ErrorEjecucion("motor3d.escalar(id, sx, sy, sz)", u);
        std::int64_t id = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto it = g_gfx.objetos.find(id);
        if (it == g_gfx.objetos.end()) throw ErrorEjecucion("objeto 3D no encontrado", u);
        it->second.escala = {static_cast<float>(a[1].numero()), static_cast<float>(a[2].numero()), static_cast<float>(a[3].numero())};
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "camara", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("motor3d.camara(x, y, z, rx?, ry?, rz?)", u);
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        g_gfx.cam_pos = {static_cast<float>(a[0].numero()), static_cast<float>(a[1].numero()), static_cast<float>(a[2].numero())};
        if (a.size() >= 6) {
            g_gfx.cam_rot = {static_cast<float>(a[3].numero()), static_cast<float>(a[4].numero()), static_cast<float>(a[5].numero())};
        }
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "lanzar_rayo", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 6) throw ErrorEjecucion("motor3d.lanzar_rayo(ox, oy, oz, dx, dy, dz)", u);
        Vec3 orig{static_cast<float>(a[0].numero()), static_cast<float>(a[1].numero()), static_cast<float>(a[2].numero())};
        Vec3 dir{static_cast<float>(a[3].numero()), static_cast<float>(a[4].numero()), static_cast<float>(a[5].numero())};
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (len > 0.0f) { dir.x /= len; dir.y /= len; dir.z /= len; }

        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        std::int64_t golpeado_id = 0;
        float mejor_t = std::numeric_limits<float>::infinity();

        for (const auto& kv : g_gfx.objetos) {
            const auto& obj = kv.second;
            if (!obj.visible) continue;
            for (const auto& tri : obj.malla) {
                Vec3 sa{tri.a.x * obj.escala.x, tri.a.y * obj.escala.y, tri.a.z * obj.escala.z};
                Vec3 sb{tri.b.x * obj.escala.x, tri.b.y * obj.escala.y, tri.b.z * obj.escala.z};
                Vec3 sc{tri.c.x * obj.escala.x, tri.c.y * obj.escala.y, tri.c.z * obj.escala.z};
                Vec3 v0 = rotar_vec3(sa, obj.rot);
                Vec3 v1 = rotar_vec3(sb, obj.rot);
                Vec3 v2 = rotar_vec3(sc, obj.rot);
                v0 = {v0.x + obj.pos.x, v0.y + obj.pos.y, v0.z + obj.pos.z};
                v1 = {v1.x + obj.pos.x, v1.y + obj.pos.y, v1.z + obj.pos.z};
                v2 = {v2.x + obj.pos.x, v2.y + obj.pos.y, v2.z + obj.pos.z};

                Vec3 e1{v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
                Vec3 e2{v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};
                Vec3 h{dir.y * e2.z - dir.z * e2.y, dir.z * e2.x - dir.x * e2.z, dir.x * e2.y - dir.y * e2.x};
                float det = e1.x * h.x + e1.y * h.y + e1.z * h.z;
                if (std::fabs(det) < 1e-6f) continue;
                float inv_det = 1.0f / det;
                Vec3 s{orig.x - v0.x, orig.y - v0.y, orig.z - v0.z};
                float bu = inv_det * (s.x * h.x + s.y * h.y + s.z * h.z);
                if (bu < 0.0f || bu > 1.0f) continue;
                Vec3 q{s.y * e1.z - s.z * e1.y, s.z * e1.x - s.x * e1.z, s.x * e1.y - s.y * e1.x};
                float bv = inv_det * (dir.x * q.x + dir.y * q.y + dir.z * q.z);
                if (bv < 0.0f || bu + bv > 1.0f) continue;
                float t = inv_det * (e2.x * q.x + e2.y * q.y + e2.z * q.z);
                if (t > 0.001f && t < mejor_t) {
                    mejor_t = t;
                    golpeado_id = kv.first;
                }
            }
        }
        return Valor::de_mapa({
            {"impacto", Valor::de_bool(golpeado_id != 0)},
            {"objeto_id", Valor::de_entero(golpeado_id)},
            {"distancia", Valor::de_decimal(golpeado_id != 0 ? mejor_t : -1.0)}
        });
    });

    registrar_fn(*m->entorno, "renderizar_3d", [](std::vector<Valor>&, Ubicacion) {
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        float rad = g_gfx.fov_grados * 3.14159265f / 180.0f;
        float fov = 1.0f / std::tan(rad * 0.5f);
        float aspect = static_cast<float>(g_gfx.ancho) / static_cast<float>(std::max(1, g_gfx.alto));

        auto proyectar = [&](Vec3 p) -> Vec3 {
            float z = p.z;
            float sx = ((p.x * fov / aspect) / z + 1.0f) * 0.5f * static_cast<float>(g_gfx.ancho);
            float sy = (1.0f - (p.y * fov) / z) * 0.5f * static_cast<float>(g_gfx.alto);
            return {sx, sy, z};
        };

        for (const auto& kv : g_gfx.objetos) {
            const auto& obj = kv.second;
            if (!obj.visible) continue;
            const Textura* tex = nullptr;
            if (obj.textura_id > 0) {
                auto it_tex = g_gfx.texturas.find(obj.textura_id);
                if (it_tex != g_gfx.texturas.end()) tex = &it_tex->second;
            }

            for (const auto& tri : obj.malla) {
                Vec3 sa{tri.a.x * obj.escala.x, tri.a.y * obj.escala.y, tri.a.z * obj.escala.z};
                Vec3 sb{tri.b.x * obj.escala.x, tri.b.y * obj.escala.y, tri.b.z * obj.escala.z};
                Vec3 sc{tri.c.x * obj.escala.x, tri.c.y * obj.escala.y, tri.c.z * obj.escala.z};

                Vec3 wa = rotar_vec3(sa, obj.rot);
                Vec3 wb = rotar_vec3(sb, obj.rot);
                Vec3 wc = rotar_vec3(sc, obj.rot);
                wa = {wa.x + obj.pos.x, wa.y + obj.pos.y, wa.z + obj.pos.z};
                wb = {wb.x + obj.pos.x, wb.y + obj.pos.y, wb.z + obj.pos.z};
                wc = {wc.x + obj.pos.x, wc.y + obj.pos.y, wc.z + obj.pos.z};

                Vec3 rel_a{wa.x - g_gfx.cam_pos.x, wa.y - g_gfx.cam_pos.y, wa.z - g_gfx.cam_pos.z};
                Vec3 rel_b{wb.x - g_gfx.cam_pos.x, wb.y - g_gfx.cam_pos.y, wb.z - g_gfx.cam_pos.z};
                Vec3 rel_c{wc.x - g_gfx.cam_pos.x, wc.y - g_gfx.cam_pos.y, wc.z - g_gfx.cam_pos.z};

                Vec3 ca = rotar_inverso_camara(rel_a, g_gfx.cam_rot);
                Vec3 cb = rotar_inverso_camara(rel_b, g_gfx.cam_rot);
                Vec3 cc = rotar_inverso_camara(rel_c, g_gfx.cam_rot);

                if (ca.z <= 0.1f || cb.z <= 0.1f || cc.z <= 0.1f) continue;

                Vec3 u1{cb.x - ca.x, cb.y - ca.y, cb.z - ca.z};
                Vec3 u2{cc.x - ca.x, cc.y - ca.y, cc.z - ca.z};
                Vec3 norm{
                    u1.y * u2.z - u1.z * u2.y,
                    u1.z * u2.x - u1.x * u2.z,
                    u1.x * u2.y - u1.y * u2.x
                };
                if (norm.x * ca.x + norm.y * ca.y + norm.z * ca.z >= 0.0f) continue;

                float len = std::sqrt(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
                float intensidad = len > 0.0f ? std::clamp(0.35f + 0.65f * std::max(0.0f, -norm.z / len), 0.25f, 1.0f) : 1.0f;

                if (g_gfx.luz_puntual_activa && len > 0.0f) {
                    Vec3 centro{(wa.x + wb.x + wc.x) / 3.0f, (wa.y + wb.y + wc.y) / 3.0f, (wa.z + wb.z + wc.z) / 3.0f};
                    Vec3 dl{g_gfx.luz_pos.x - centro.x, g_gfx.luz_pos.y - centro.y, g_gfx.luz_pos.z - centro.z};
                    float dist_l = std::sqrt(dl.x * dl.x + dl.y * dl.y + dl.z * dl.z);
                    if (dist_l > 0.01f) {
                        float atenuacion = g_gfx.luz_intensidad / (1.0f + 0.08f * dist_l * dist_l);
                        intensidad = std::clamp(intensidad * 0.5f + atenuacion, 0.2f, 1.4f);
                    }
                }

                rasterizar_triangulo_z(proyectar(ca), proyectar(cb), proyectar(cc),
                                       tri.uva, tri.uvb, tri.uvc, tri.color, intensidad, tex);
            }
        }
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "presentar", [](std::vector<Valor>&, Ubicacion) {
        std::lock_guard<std::mutex> lk(g_gfx_mutex);
        auto ahora = std::chrono::steady_clock::now();
        g_gfx.delta_seg = std::chrono::duration<double>(ahora - g_gfx.ultimo_frame).count();
        g_gfx.ultimo_frame = ahora;

#ifdef _WIN32
        if (g_gfx.abierta && g_gfx.hdc && !g_gfx.color.empty()) {
            StretchDIBits(g_gfx.hdc, 0, 0, g_gfx.ancho, g_gfx.alto, 0, 0, g_gfx.ancho, g_gfx.alto,
                          g_gfx.color.data(), &g_gfx.bmi, DIB_RGB_COLORS, SRCCOPY);
        }
#endif
        return Valor::de_bool(true);
    });

    return m;
}

std::shared_ptr<Modulo> crear_modulo_gui() {
    auto m = std::make_shared<Modulo>();
    m->nombre = "gui";
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

#ifdef _WIN32
    struct EstadoControles {
        std::mutex mtx;
        std::int64_t next_id = 1;
        std::map<std::int64_t, HWND> handles;
        std::int64_t ultimo_boton = 0;
        bool clase_ok = false;
    };
    static EstadoControles g_ui;

    auto wnd_proc = [](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
        if (msg == WM_COMMAND && HIWORD(wp) == BN_CLICKED) {
            g_ui.ultimo_boton = static_cast<std::int64_t>(LOWORD(wp));
        } else if (msg == WM_CLOSE) {
            DestroyWindow(hwnd);
            return 0;
        } else if (msg == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcA(hwnd, msg, wp, lp);
    };

    registrar_fn(*m->entorno, "ventana", [wnd_proc](std::vector<Valor>& a, Ubicacion u) {
        std::string titulo = a.empty() ? "ESP 2.0.0" : a[0].a_texto();
        int w = a.size() > 1 ? static_cast<int>(a[1].numero()) : 640;
        int h = a.size() > 2 ? static_cast<int>(a[2].numero()) : 420;
        HINSTANCE hInst = GetModuleHandleA(nullptr);
        if (!g_ui.clase_ok) {
            WNDCLASSA wc{};
            wc.lpfnWndProc = wnd_proc;
            wc.hInstance = hInst;
            wc.lpszClassName = "ESPVentanaControls2";
            wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
            wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
            RegisterClassA(&wc);
            g_ui.clase_ok = true;
        }
        HWND hwnd = CreateWindowExA(0, "ESPVentanaControls2", titulo.c_str(),
                                    WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                                    w, h, nullptr, nullptr, hInst, nullptr);
        if (!hwnd) throw ErrorEjecucion("no se pudo crear ventana GUI", u);
        std::lock_guard<std::mutex> lk(g_ui.mtx);
        std::int64_t id = g_ui.next_id++;
        g_ui.handles[id] = hwnd;
        return Valor::de_entero(id);
    });

    registrar_fn(*m->entorno, "boton", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("gui.boton(ventana, texto, x?, y?, w?, h?)", u);
        std::int64_t vid = static_cast<std::int64_t>(a[0].numero());
        std::string txt = a[1].a_texto();
        int x = a.size() > 2 ? static_cast<int>(a[2].numero()) : 20;
        int y = a.size() > 3 ? static_cast<int>(a[3].numero()) : 20;
        int w = a.size() > 4 ? static_cast<int>(a[4].numero()) : 140;
        int h = a.size() > 5 ? static_cast<int>(a[5].numero()) : 32;
        std::lock_guard<std::mutex> lk(g_ui.mtx);
        auto it = g_ui.handles.find(vid);
        if (it == g_ui.handles.end()) throw ErrorEjecucion("ventana no valida", u);
        std::int64_t cid = g_ui.next_id++;
        HWND btn = CreateWindowExA(0, "BUTTON", txt.c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                   x, y, w, h, it->second, reinterpret_cast<HMENU>(static_cast<INT_PTR>(cid)),
                                   GetModuleHandleA(nullptr), nullptr);
        g_ui.handles[cid] = btn;
        return Valor::de_entero(cid);
    });

    registrar_fn(*m->entorno, "etiqueta", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("gui.etiqueta(ventana, texto, x?, y?, w?, h?)", u);
        std::int64_t vid = static_cast<std::int64_t>(a[0].numero());
        std::string txt = a[1].a_texto();
        int x = a.size() > 2 ? static_cast<int>(a[2].numero()) : 20;
        int y = a.size() > 3 ? static_cast<int>(a[3].numero()) : 60;
        int w = a.size() > 4 ? static_cast<int>(a[4].numero()) : 300;
        int h = a.size() > 5 ? static_cast<int>(a[5].numero()) : 24;
        std::lock_guard<std::mutex> lk(g_ui.mtx);
        auto it = g_ui.handles.find(vid);
        if (it == g_ui.handles.end()) throw ErrorEjecucion("ventana no valida", u);
        std::int64_t cid = g_ui.next_id++;
        HWND lbl = CreateWindowExA(0, "STATIC", txt.c_str(), WS_VISIBLE | WS_CHILD,
                                   x, y, w, h, it->second, nullptr, GetModuleHandleA(nullptr), nullptr);
        g_ui.handles[cid] = lbl;
        return Valor::de_entero(cid);
    });

    registrar_fn(*m->entorno, "campo_texto", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("gui.campo_texto(ventana, texto, x?, y?, w?, h?)", u);
        std::int64_t vid = static_cast<std::int64_t>(a[0].numero());
        std::string txt = a[1].a_texto();
        int x = a.size() > 2 ? static_cast<int>(a[2].numero()) : 20;
        int y = a.size() > 3 ? static_cast<int>(a[3].numero()) : 95;
        int w = a.size() > 4 ? static_cast<int>(a[4].numero()) : 300;
        int h = a.size() > 5 ? static_cast<int>(a[5].numero()) : 26;
        std::lock_guard<std::mutex> lk(g_ui.mtx);
        auto it = g_ui.handles.find(vid);
        if (it == g_ui.handles.end()) throw ErrorEjecucion("ventana no valida", u);
        std::int64_t cid = g_ui.next_id++;
        HWND edt = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", txt.c_str(),
                                   WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
                                   x, y, w, h, it->second, nullptr, GetModuleHandleA(nullptr), nullptr);
        g_ui.handles[cid] = edt;
        return Valor::de_entero(cid);
    });

    registrar_fn(*m->entorno, "leer_texto", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("gui.leer_texto(control_id)", u);
        std::int64_t cid = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(g_ui.mtx);
        auto it = g_ui.handles.find(cid);
        if (it == g_ui.handles.end()) throw ErrorEjecucion("control no valido", u);
        int len = GetWindowTextLengthA(it->second);
        std::string buf(static_cast<std::size_t>(len + 1), '\0');
        GetWindowTextA(it->second, buf.data(), len + 1);
        buf.resize(static_cast<std::size_t>(len));
        return Valor::de_texto(std::move(buf));
    });

    registrar_fn(*m->entorno, "establecer_texto", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("gui.establecer_texto(control_id, texto)", u);
        std::int64_t cid = static_cast<std::int64_t>(a[0].numero());
        std::lock_guard<std::mutex> lk(g_ui.mtx);
        auto it = g_ui.handles.find(cid);
        if (it == g_ui.handles.end()) throw ErrorEjecucion("control no valido", u);
        SetWindowTextA(it->second, a[1].a_texto().c_str());
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "ejecutar", [](std::vector<Valor>&, Ubicacion) {
        g_ui.ultimo_boton = 0;
        MSG msg{};
        while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        return Valor::de_entero(g_ui.ultimo_boton);
    });
#else
    registrar_fn(*m->entorno, "ventana", [](std::vector<Valor>&, Ubicacion u) {
        throw ErrorEjecucion("GUI nativa disponible en Windows", u);
        return Valor::nulo();
    });
#endif
    return m;
}

std::shared_ptr<Modulo> crear_modulo_audio() {
    auto m = std::make_shared<Modulo>();
    m->nombre = "audio";
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "reproducir", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("audio.reproducir(ruta_wav, bucle?)", u);
        std::string ruta = a[0].a_texto();
        bool bucle = a.size() > 1 ? a[1].verdad() : false;
#ifdef _WIN32
        DWORD flags = SND_FILENAME | SND_ASYNC | SND_NODEFAULT | (bucle ? SND_LOOP : 0);
        return Valor::de_bool(PlaySoundA(ruta.c_str(), nullptr, flags) != FALSE);
#else
        (void)ruta;
        (void)bucle;
        return Valor::de_bool(false);
#endif
    });

    registrar_fn(*m->entorno, "detener", [](std::vector<Valor>&, Ubicacion) {
#ifdef _WIN32
        PlaySoundA(nullptr, nullptr, 0);
#endif
        return Valor::de_bool(true);
    });

    registrar_fn(*m->entorno, "tono", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("audio.tono(frecuencia_hz, duracion_ms)", u);
        int hz = std::clamp(static_cast<int>(a[0].numero()), 37, 32767);
        int ms = std::clamp(static_cast<int>(a[1].numero()), 1, 10000);
#ifdef _WIN32
        return Valor::de_bool(Beep(static_cast<DWORD>(hz), static_cast<DWORD>(ms)) != FALSE);
#else
        (void)hz;
        (void)ms;
        return Valor::de_bool(false);
#endif
    });

    return m;
}

}  // namespace esp