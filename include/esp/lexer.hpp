#pragma once
#include "comun.hpp"
#include <vector>

namespace esp {

enum class TipoToken {
    FinArchivo,
    Identificador,
    Entero,
    Decimal,
    Texto,
    Verdadero,
    Falso,
    Nulo,
    Mas,
    Menos,
    Por,
    Entre,
    Modulo,
    Potencia,
    Igual,
    IgualIgual,
    Distinto,
    Mayor,
    Menor,
    MayorIgual,
    MenorIgual,
    ParIzq,
    ParDer,
    CorIzq,
    CorDer,
    LlaveIzq,
    LlaveDer,
    Coma,
    Punto,
    DosPuntos,
    NuevaLinea,
};

struct Token {
    TipoToken tipo = TipoToken::FinArchivo;
    std::string lexema;
    Ubicacion ubicacion;
};

class Lexer {
public:
    Lexer(std::string fuente, std::string archivo);
    std::vector<Token> analizar();

private:
    std::string fuente_;
    std::string archivo_;
    std::size_t i_ = 0;
    int linea_ = 1;
    int columna_ = 1;

    char actual() const;
    char siguiente() const;
    char avanzar();
    void saltar_espacio_y_comentarios();
    Token texto();
    Token numero();
    Token identificador_o_palabra();
    Ubicacion ubi() const;
};

}  // namespace esp
