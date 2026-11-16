#pragma once
#include "ast.hpp"
#include <string>

namespace esp {

struct OpcionesCompilacion {
    std::string entrada;
    std::string salida;
    std::string nivel = "O2";
    std::string formato = "exe";  // exe, dll, so, dylib, estatica, ll, cpp
    bool usar_llvm = false;
    bool cache = true;
};

struct ResultadoCompilacion {
    bool ok = false;
    std::string mensaje;
    std::string archivo_generado;
    // Textual LLVM IR, populated when the requested output format is "ll".
    std::string ir_llvm;
    std::string fuente_cpp;
    bool uso_llvm = false;
    bool uso_cache = false;
};

ResultadoCompilacion compilar_programa(const Nodo* ast, const std::string& fuente, const OpcionesCompilacion& opt);
ResultadoCompilacion compilar_fuente_esp(const std::string& fuente, const OpcionesCompilacion& opt);
// Directly lowers scalar-compatible AST nodes; the .ll compilation backend lowers supported ASTs through C++.
std::string generar_llvm_ir(const Nodo* ast, const std::string& nombre_modulo);
std::string generar_cpp(const Nodo* ast, const std::string& nombre_modulo, bool como_biblioteca = false);
// Stable local-cache key only; it is not a cryptographic integrity check.
std::string hash_compilacion(const std::string& fuente, const OpcionesCompilacion& opt);

}  // namespace esp
