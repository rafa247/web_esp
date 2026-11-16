#include "esp/cli.hpp"
#include "esp/comun.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

namespace {

std::filesystem::path obtener_ruta_ejecutable_actual() {
#ifdef _WIN32
    char buf[MAX_PATH]{};
    DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        return std::filesystem::path(std::string(buf, len));
    }
#elif defined(__APPLE__)
    char buf[4096]{};
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) {
        std::error_code ec;
        return std::filesystem::canonical(buf, ec);
    }
#else
    std::error_code ec;
    auto p = std::filesystem::read_symlink("/proc/self/exe", ec);
    if (!ec) return p;
#endif
    return std::filesystem::current_path() / "esp";
}

bool auto_registrar_en_path(const std::filesystem::path& dir_binario) {
    std::string dir_str = dir_binario.string();
#ifdef _WIN32
    HKEY hKey = nullptr;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        return false;
    }
    char path_actual[32768]{};
    DWORD tipo = REG_EXPAND_SZ;
    DWORD tam = sizeof(path_actual);
    std::string nuevo_path;
    if (RegQueryValueExA(hKey, "Path", nullptr, &tipo, reinterpret_cast<LPBYTE>(path_actual), &tam) == ERROR_SUCCESS) {
        nuevo_path = path_actual;
        if (nuevo_path.find(dir_str) != std::string::npos) {
            RegCloseKey(hKey);
            return true;  // Ya estaba registrado
        }
        if (!nuevo_path.empty() && nuevo_path.back() != ';') nuevo_path += ';';
    }
    nuevo_path += dir_str;
    LONG res = RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ,
                              reinterpret_cast<const BYTE*>(nuevo_path.c_str()),
                              static_cast<DWORD>(nuevo_path.size() + 1));
    RegCloseKey(hKey);
    SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, reinterpret_cast<LPARAM>("Environment"),
                        SMTO_ABORTIFHUNG, 2000, nullptr);
    return res == ERROR_SUCCESS;
#else
    const char* home = std::getenv("HOME");
    if (!home) return false;
    std::string export_line = "\n# ESP 2.0.0 PATH automatico\nexport PATH=\"$PATH:" + dir_str + "\"\n";
    for (const char* rc : {".bashrc", ".zshrc", ".profile"}) {
        std::filesystem::path rc_path = std::filesystem::path(home) / rc;
        std::error_code ec;
        if (std::filesystem::exists(rc_path, ec)) {
            std::string contenido = esp::leer_archivo(rc_path.string());
            if (contenido.find(dir_str) == std::string::npos) {
                esp::escribir_archivo(rc_path.string(), contenido + export_line);
            }
        }
    }
    return true;
#endif
}

}  // namespace

int main(int argc, char** argv) {
    std::filesystem::path ruta_exe = obtener_ruta_ejecutable_actual();
    std::filesystem::path dir_exe = ruta_exe.parent_path();

    // Auto-registro silencioso en el PATH del usuario si aun no esta presente
    auto_registrar_en_path(dir_exe);

    std::vector<std::string> args;
    args.reserve(argc > 1 ? static_cast<std::size_t>(argc - 1) : 0);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        // Si es el primer argumento y parece un archivo .esp que no existe en el directorio actual,
        // buscarlo automaticamente junto al ejecutable o en la raiz del proyecto
        if (i == 1 && arg.size() > 4 && arg.substr(arg.size() - 4) == ".esp") {
            std::error_code ec;
            if (!std::filesystem::exists(arg, ec)) {
                std::filesystem::path junto_exe = dir_exe / arg;
                std::filesystem::path raiz_proy = dir_exe.parent_path().parent_path() / arg;
                if (std::filesystem::exists(junto_exe, ec)) {
                    arg = junto_exe.string();
                } else if (std::filesystem::exists(raiz_proy, ec)) {
                    arg = raiz_proy.string();
                }
            }
        }
        args.push_back(std::move(arg));
    }

    if (!args.empty() && args[0] == "ruta") {
        std::cout << "Ejecutable ESP: " << ruta_exe.string() << "\n"
                  << "Directorio bin: " << dir_exe.string() << "\n";
        return 0;
    }

    return esp::ejecutar_cli(args);
}
