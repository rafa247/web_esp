#include "esp/runtime.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>
#include <shlobj.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "shell32.lib")
#else
#include <cstdlib>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace esp {
namespace {

std::string sha256_portable(const std::string& datos) {
    static constexpr std::array<std::uint32_t, 64> constantes = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };
    std::array<std::uint32_t, 8> estado = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                           0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    std::vector<std::uint8_t> mensaje(datos.begin(), datos.end());
    const std::uint64_t longitud_bits = static_cast<std::uint64_t>(mensaje.size()) * 8;
    mensaje.push_back(0x80);
    while (mensaje.size() % 64 != 56) mensaje.push_back(0);
    for (int i = 7; i >= 0; --i)
        mensaje.push_back(static_cast<std::uint8_t>(longitud_bits >> (i * 8)));

    const auto rotar = [](std::uint32_t valor, unsigned cantidad) {
        return (valor >> cantidad) | (valor << (32 - cantidad));
    };
    for (std::size_t bloque = 0; bloque < mensaje.size(); bloque += 64) {
        std::array<std::uint32_t, 64> palabras{};
        for (std::size_t i = 0; i < 16; ++i) {
            const std::size_t offset = bloque + i * 4;
            palabras[i] = (static_cast<std::uint32_t>(mensaje[offset]) << 24) |
                          (static_cast<std::uint32_t>(mensaje[offset + 1]) << 16) |
                          (static_cast<std::uint32_t>(mensaje[offset + 2]) << 8) |
                          static_cast<std::uint32_t>(mensaje[offset + 3]);
        }
        for (std::size_t i = 16; i < palabras.size(); ++i) {
            const std::uint32_t s0 = rotar(palabras[i - 15], 7) ^ rotar(palabras[i - 15], 18) ^
                                     (palabras[i - 15] >> 3);
            const std::uint32_t s1 = rotar(palabras[i - 2], 17) ^ rotar(palabras[i - 2], 19) ^
                                     (palabras[i - 2] >> 10);
            palabras[i] = palabras[i - 16] + s0 + palabras[i - 7] + s1;
        }

        auto a = estado[0];
        auto b = estado[1];
        auto c = estado[2];
        auto d = estado[3];
        auto e = estado[4];
        auto f = estado[5];
        auto g = estado[6];
        auto h = estado[7];
        for (std::size_t i = 0; i < palabras.size(); ++i) {
            const std::uint32_t s1 = rotar(e, 6) ^ rotar(e, 11) ^ rotar(e, 25);
            const std::uint32_t elegir = (e & f) ^ (~e & g);
            const std::uint32_t t1 = h + s1 + elegir + constantes[i] + palabras[i];
            const std::uint32_t s0 = rotar(a, 2) ^ rotar(a, 13) ^ rotar(a, 22);
            const std::uint32_t mayoria = (a & b) ^ (a & c) ^ (b & c);
            const std::uint32_t t2 = s0 + mayoria;
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }
        estado[0] += a;
        estado[1] += b;
        estado[2] += c;
        estado[3] += d;
        estado[4] += e;
        estado[5] += f;
        estado[6] += g;
        estado[7] += h;
    }

    std::ostringstream resultado;
    resultado << std::hex << std::setfill('0');
    for (const auto palabra : estado) resultado << std::setw(8) << palabra;
    return resultado.str();
}

}  // namespace

std::string leer_archivo(const std::string& ruta) {
    std::ifstream in(ruta, std::ios::binary);
    if (!in) throw ErrorEjecucion("no se pudo abrir: " + ruta);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void escribir_archivo(const std::string& ruta, const std::string& contenido) {
    std::ofstream out(ruta, std::ios::binary);
    if (!out) throw ErrorEjecucion("no se pudo escribir: " + ruta);
    out << contenido;
}

std::string directorio_de(const std::string& ruta) {
    auto p = ruta.find_last_of("/\\");
    if (p == std::string::npos) return ".";
    return ruta.substr(0, p);
}

std::string unir_ruta(const std::string& a, const std::string& b) {
#ifdef _WIN32
    char sep = '\\';
#else
    char sep = '/';
#endif
    if (a.empty()) return b;
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + sep + b;
}

std::string directorio_home_esp() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return unir_ruta(path, "ESP");
    }
    return ".esp";
#else
    const char* h = std::getenv("HOME");
    return unir_ruta(h ? h : ".", ".esp");
#endif
}

std::string sha256_hex(const std::string& datos) {
#ifdef _WIN32
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) {
        return sha256_portable(datos);
    }
    if (BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0) != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return sha256_portable(datos);
    }
    if (!datos.empty()) {
        BCryptHashData(hHash, reinterpret_cast<PUCHAR>(const_cast<char*>(datos.data())),
                       static_cast<ULONG>(datos.size()), 0);
    }
    BYTE buf[32];
    if (BCryptFinishHash(hHash, buf, sizeof(buf), 0) != 0) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return sha256_portable(datos);
    }
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.resize(64);
    for (int i = 0; i < 32; ++i) {
        out[i * 2] = hex[buf[i] >> 4];
        out[i * 2 + 1] = hex[buf[i] & 0xf];
    }
    return out;
#else
    return sha256_portable(datos);
#endif
}

std::string detectar_compilador() {
#ifdef _WIN32
    const char* candidatos[] = {"clang++.exe", "g++.exe", "cl.exe"};
    const char* cxx = std::getenv("CXX");
    if (cxx && *cxx) {
        std::string configurado(cxx);
        std::string ejecutable;
        if (configurado.front() == '"') {
            const auto cierre = configurado.find('"', 1);
            ejecutable = cierre == std::string::npos ? configurado.substr(1) : configurado.substr(1, cierre - 1);
        } else {
            const auto separador = configurado.find_first_of(" \t");
            ejecutable = configurado.substr(0, separador);
        }
        std::error_code error;
        if (std::filesystem::is_regular_file(ejecutable, error) && !error) return ejecutable;
        char encontrado[MAX_PATH];
        const DWORD longitud = SearchPathA(nullptr, ejecutable.c_str(), nullptr, MAX_PATH, encontrado, nullptr);
        if (longitud && longitud < MAX_PATH) return std::string(encontrado, longitud);
    }

    for (const char* variable : {"LLVM_HOME", "LLVM_ROOT", "MINGW_HOME", "MINGW_ROOT", "MSYS2_ROOT"}) {
        const char* raiz = std::getenv(variable);
        if (!raiz || !*raiz) continue;
        const auto bin = std::filesystem::path(raiz) / "bin";
        for (const char* candidato : candidatos) {
            const auto ruta = bin / candidato;
            std::error_code error;
            if (std::filesystem::is_regular_file(ruta, error) && !error) return ruta.string();
        }
    }

    for (auto c : candidatos) {
        char buf[MAX_PATH];
        DWORD n = SearchPathA(nullptr, c, nullptr, MAX_PATH, buf, nullptr);
        if (n) return std::string(buf);
    }

    const char* archivos_programa[] = {std::getenv("ProgramFiles"), std::getenv("ProgramFiles(x86)")};
    for (const char* raiz : archivos_programa) {
        if (!raiz || !*raiz) continue;
        const auto llvm = std::filesystem::path(raiz) / "LLVM" / "bin" / "clang++.exe";
        std::error_code error;
        if (std::filesystem::is_regular_file(llvm, error) && !error) return llvm.string();
    }

    const char* datos_locales = std::getenv("LOCALAPPDATA");
    if (datos_locales && *datos_locales) {
        const auto llvm = std::filesystem::path(datos_locales) / "Programs" / "LLVM";
        for (const auto& bin : {llvm / "bin", llvm / "LLVM" / "bin"}) {
            const auto clang = bin / "clang++.exe";
            std::error_code error;
            if (std::filesystem::is_regular_file(clang, error) && !error) return clang.string();
        }
    }

    const char* raiz_msys = std::getenv("MSYS2_ROOT");
    const std::filesystem::path msys = raiz_msys && *raiz_msys ? raiz_msys : "C:/msys64";
    for (const char* entorno : {"ucrt64", "mingw64"}) {
        const auto mingw = msys / entorno / "bin" / "g++.exe";
        std::error_code error;
        if (std::filesystem::is_regular_file(mingw, error) && !error) return mingw.string();
    }

    const char* raiz_vs = std::getenv("VCToolsInstallDir");
    if (raiz_vs && *raiz_vs) {
        const auto msvc = std::filesystem::path(raiz_vs) / "bin" / "Hostx64" / "x64" / "cl.exe";
        std::error_code error;
        if (std::filesystem::is_regular_file(msvc, error) && !error) return msvc.string();
    }
    throw ErrorEjecucion("no se encontro un compilador C++; configura CXX o instala LLVM/MSVC/MinGW");
#else
    const char* cxx = std::getenv("CXX");
    if (cxx && *cxx) {
        std::string ejecutable(cxx);
        if (ejecutable.front() == '"') {
            const auto cierre = ejecutable.find('"', 1);
            ejecutable = cierre == std::string::npos ? ejecutable.substr(1) : ejecutable.substr(1, cierre - 1);
        } else {
            const auto separador = ejecutable.find_first_of(" \t");
            if (separador != std::string::npos) ejecutable.resize(separador);
        }
        std::error_code error;
        if (std::filesystem::is_regular_file(ejecutable, error) && !error) return ejecutable;
    }
    const char* ruta_path = std::getenv("PATH");
    if (!ruta_path) throw ErrorEjecucion("no se encontro un compilador C++; configura CXX o PATH");
    const char* candidatos[] = {"clang++", "g++"};
    std::stringstream directorios(ruta_path);
    std::string directorio;
    while (std::getline(directorios, directorio, ':')) {
        if (directorio.empty()) directorio = ".";
        for (auto candidato : candidatos) {
            std::filesystem::path ruta = std::filesystem::path(directorio) / candidato;
            std::error_code error;
            if (std::filesystem::is_regular_file(ruta, error) && !error) return ruta.string();
        }
    }
    throw ErrorEjecucion("no se encontro un compilador C++; instala clang++/g++ o configura PATH");
#endif
}

std::string citar_argumento_proceso(const std::string& argumento) {
#ifdef _WIN32
    std::string resultado = "\"";
    std::size_t barras = 0;
    for (char c : argumento) {
        if (c == '\\') {
            ++barras;
        } else if (c == '"') {
            resultado.append(barras * 2 + 1, '\\');
            resultado += '"';
            barras = 0;
        } else {
            resultado.append(barras, '\\');
            resultado += c;
            barras = 0;
        }
    }
    resultado.append(barras * 2, '\\');
    resultado += '"';
    return resultado;
#else
    std::string resultado = "'";
    for (char c : argumento) {
        if (c == '\'')
            resultado += "'\\''";
        else
            resultado += c;
    }
    resultado += '\'';
    return resultado;
#endif
}

int ejecutar_proceso(const std::string& comando, const std::string& cwd, std::string* salida, std::string* error_out,
                     int timeout_ms) {
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    HANDLE out_r, out_w, err_r, err_w;
    CreatePipe(&out_r, &out_w, &sa, 0);
    CreatePipe(&err_r, &err_w, &sa, 0);
    SetHandleInformation(out_r, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(err_r, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = out_w;
    si.hStdError = err_w;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION pi{};
    std::string cmd = comando;
    std::vector<char> buf(cmd.begin(), cmd.end());
    buf.push_back(0);
    BOOL ok = CreateProcessA(nullptr, buf.data(), nullptr, nullptr, TRUE, 0, nullptr,
                             cwd.empty() ? nullptr : cwd.c_str(), &si, &pi);
    CloseHandle(out_w);
    CloseHandle(err_w);
    if (!ok) {
        CloseHandle(out_r);
        CloseHandle(err_r);
        throw ErrorEjecucion("no se pudo iniciar proceso: " + comando);
    }
    DWORD wait = timeout_ms > 0 ? static_cast<DWORD>(timeout_ms) : INFINITE;
    if (WaitForSingleObject(pi.hProcess, wait) == WAIT_TIMEOUT) {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(out_r);
        CloseHandle(err_r);
        throw ErrorEjecucion("tiempo de espera agotado");
    }
    auto leer = [](HANDLE h) {
        std::string s;
        char tmp[4096];
        DWORD n = 0;
        while (ReadFile(h, tmp, sizeof(tmp), &n, nullptr) && n) s.append(tmp, tmp + n);
        return s;
    };
    if (salida) *salida = leer(out_r);
    if (error_out) *error_out = leer(err_r);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(out_r);
    CloseHandle(err_r);
    return static_cast<int>(code);
#else
    (void)timeout_ms;
    std::string full = comando;
    if (!cwd.empty()) full = "cd " + citar_argumento_proceso(cwd) + " && " + comando;
    FILE* p = popen(full.c_str(), "r");
    if (!p) throw ErrorEjecucion("no se pudo iniciar proceso");
    std::string s;
    char tmp[4096];
    while (fgets(tmp, sizeof(tmp), p)) s += tmp;
    int st = pclose(p);
    if (salida) *salida = s;
    if (error_out) *error_out = "";
    return WEXITSTATUS(st);
#endif
}

}  // namespace esp