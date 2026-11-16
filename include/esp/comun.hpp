#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace esp {

constexpr const char* VERSION = "2.0.0";
constexpr const char* EXTENSION = ".esp";

struct Ubicacion {
    std::string archivo = "<entrada>";
    int linea = 1;
    int columna = 1;
};

enum class TipoToken {
    FinArchivo,
    SaltoLinea,
    Identificador,
    Entero,
    Decimal,
    Texto,
    Verdadero,
    Falso,
    Nulo,

    // Palabras clave del lenguaje
    KwFuncion,
    KwDevolver,
    KwVariable,
    KwConstante,
    KwSi,
    KwSino,
    KwSiNoSi,
    KwMientras,
    KwPara,
    KwHasta,
    KwPaso,
    KwEn,
    KwFin,
    KwClase,
    KwHereda,
    KwEste,
    KwSuper,
    KwPublico,
    KwPrivado,
    KwConseguir,
    KwComo,
    KwExportar,
    KwIntenta,
    KwSiNoPuede,
    KwRomper,
    KwContinuar,
    KwDatos,
    KwLista,
    KwY,
    KwO,
    KwNo,

    // Sintaxis Web Nativa en ESP
    KwPagina,
    KwEstilo,
    KwScript,
    KwEncabezado,
    KwSubtitulo,
    KwParrafo,
    KwBoton,
    KwEntrada,
    KwEnlace,
    KwImagen,
    KwContenedor,
    KwAlClic,

    // Sintaxis SQL Nativa en ESP
    KwBaseDatos,
    KwCrearTabla,
    KwInsertarEn,
    KwValores,
    KwSeleccionar,
    KwDe,
    KwDonde,
    KwOrdenarPor,
    KwLimite,
    KwActualizar,
    KwPoner,
    KwEliminarDe,

    // Operadores y delimitadores
    Mas,
    Menos,
    Estrella,
    Barra,
    Porcentaje,
    Igual,
    IgualIgual,
    Diferente,
    Menor,
    MenorIgual,
    Mayor,
    MayorIgual,
    ParenIzq,
    ParenDer,
    CorcheteIzq,
    CorcheteDer,
    LlaveIzq,
    LlaveDer,
    Coma,
    Punto,
    DosPuntos,
    PuntoYComa
};

struct Token {
    TipoToken tipo = TipoToken::FinArchivo;
    std::string lexema;
    Ubicacion ubicacion;
};

enum class Visibilidad {
    Publico,
    Privado
};

struct ErrorESP : std::runtime_error {
    std::string categoria;
    Ubicacion ubicacion;
    ErrorESP(std::string cat, const std::string& msg, Ubicacion u)
        : std::runtime_error(msg), categoria(std::move(cat)), ubicacion(std::move(u)) {}
};

struct ErrorLexico : ErrorESP {
    ErrorLexico(const std::string& msg, Ubicacion u) : ErrorESP("ErrorLexico", msg, std::move(u)) {}
};

struct ErrorSintaxis : ErrorESP {
    ErrorSintaxis(const std::string& msg, Ubicacion u) : ErrorESP("ErrorSintaxis", msg, std::move(u)) {}
};

struct ErrorEjecucion : ErrorESP {
    ErrorEjecucion(const std::string& msg, Ubicacion u = {}) : ErrorESP("ErrorEjecucion", msg, std::move(u)) {}
};

std::string formatear_error(const ErrorESP& err);
std::string leer_archivo(const std::string& ruta);
void escribir_archivo(const std::string& ruta, const std::string& contenido);
std::string sha256_hex(const std::string& datos);
std::string directorio_home_esp();
std::string citar_argumento_proceso(const std::string& arg);
int ejecutar_proceso(const std::string& comando, const std::string& entrada_stdin = "",
                     std::string* salida_stdout = nullptr, std::string* salida_stderr = nullptr,
                     int timeout_ms = 0);

}  // namespace esp

