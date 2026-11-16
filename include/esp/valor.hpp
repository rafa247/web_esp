#pragma once
#include "ast.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace esp {

struct Funcion;
struct Clase;
struct Objeto;
struct Modulo;

enum class TipoValor { Nulo, Booleano, Entero, Decimal, Texto, Lista, Mapa, Funcion, Clase, Objeto, Modulo };

struct Valor {
    TipoValor tipo = TipoValor::Nulo;
    bool booleano = false;
    std::int64_t entero = 0;
    double decimal = 0;
    std::string texto;
    std::shared_ptr<std::vector<Valor>> lista;
    std::shared_ptr<std::map<std::string, Valor>> mapa;
    std::shared_ptr<Funcion> funcion;
    std::shared_ptr<Clase> clase;
    std::shared_ptr<Objeto> objeto;
    std::shared_ptr<Modulo> modulo;

    static Valor nulo() { return {}; }
    static Valor de_bool(bool v) {
        Valor x;
        x.tipo = TipoValor::Booleano;
        x.booleano = v;
        return x;
    }
    static Valor de_entero(std::int64_t v) {
        Valor x;
        x.tipo = TipoValor::Entero;
        x.entero = v;
        return x;
    }
    static Valor de_decimal(double v) {
        Valor x;
        x.tipo = TipoValor::Decimal;
        x.decimal = v;
        return x;
    }
    static Valor de_texto(std::string v) {
        Valor x;
        x.tipo = TipoValor::Texto;
        x.texto = std::move(v);
        return x;
    }
    static Valor de_lista(std::vector<Valor> v) {
        Valor x;
        x.tipo = TipoValor::Lista;
        x.lista = std::make_shared<std::vector<Valor>>(std::move(v));
        return x;
    }
    static Valor de_mapa(std::map<std::string, Valor> v) {
        Valor x;
        x.tipo = TipoValor::Mapa;
        x.mapa = std::make_shared<std::map<std::string, Valor>>(std::move(v));
        return x;
    }

    bool es_nulo() const { return tipo == TipoValor::Nulo; }
    bool verdad() const;
    double numero() const;
    bool es_numero() const { return tipo == TipoValor::Entero || tipo == TipoValor::Decimal; }
    std::string a_texto() const;
    std::string tipo_nombre() const;
};

struct Miembro {
    Valor valor;
    Visibilidad visibilidad = Visibilidad::Publico;
    bool constante = false;
    std::string propietario_privado;
};

struct Entorno {
    std::shared_ptr<Entorno> padre;
    std::map<std::string, Miembro> simbolos;
    bool es_clase = false;
    std::string nombre_clase;
    Entorno* entorno_clase_actual = nullptr;

    bool definir(const std::string& n, Valor v, Visibilidad vis, bool cte, bool redefinir);
    bool asignar(const std::string& n, Valor v, std::string* error);
    bool obtener(const std::string& n, Valor* salida, Visibilidad* vis = nullptr) const;
};

struct Funcion {
    std::string nombre;
    std::vector<std::string> parametros;
    const Nodo* cuerpo = nullptr;
    std::shared_ptr<Entorno> cierre;
    Visibilidad visibilidad = Visibilidad::Publico;
    std::function<Valor(std::vector<Valor>&, Ubicacion)> nativa;
    bool es_nativa = false;
};

struct Clase {
    std::string nombre;
    std::shared_ptr<Clase> base;
    std::map<std::string, Miembro> miembros;
    std::shared_ptr<Entorno> cierre;
};

struct Objeto {
    std::shared_ptr<Clase> clase;
    std::map<std::string, Miembro> campos;
};

struct Modulo {
    std::string nombre;
    std::string version;
    std::shared_ptr<Entorno> entorno;
    std::shared_ptr<Nodo> ast;
};

Valor operar(const std::string& op, const Valor& a, const Valor& b, Ubicacion u);
Valor unario(const std::string& op, const Valor& a, Ubicacion u);

}  // namespace esp
