#pragma once
#include "ast.hpp"
#include "lexer.hpp"

namespace esp {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    NodoPtr analizar();

private:
    std::vector<Token> tokens_;
    std::size_t i_ = 0;

    const Token& actual() const;
    const Token& ver(std::size_t k = 0) const;
    bool es(TipoToken t) const;
    bool es_id(const std::string& p) const;
    bool es_fin() const;
    bool coincidir(TipoToken t);
    bool coincidir_id(const std::string& p);
    const Token& consumir(TipoToken t, const char* msg);
    void saltar_nl();
    NodoPtr programa();
    NodoPtr sentencia();
    NodoPtr declaracion();
    NodoPtr funcion(Visibilidad v);
    NodoPtr clase(Visibilidad v);
    NodoPtr condicional(bool cerrar_fin);
    bool inicio_alternativa() const;
    NodoPtr bloque_hasta_fin(bool aceptar_si_no = false);
    NodoPtr expresion();
    NodoPtr asignacion();
    NodoPtr o_logico();
    NodoPtr y_logico();
    NodoPtr igualdad();
    NodoPtr comparacion();
    NodoPtr termino();
    NodoPtr factor();
    NodoPtr potencia();
    NodoPtr unario();
    NodoPtr llamada();
    NodoPtr primario();
};

}  // namespace esp
