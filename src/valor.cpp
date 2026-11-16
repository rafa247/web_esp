#include "esp/valor.hpp"
#include <cmath>
#include <sstream>

namespace esp {

bool Valor::verdad() const {
    switch (tipo) {
        case TipoValor::Nulo: return false;
        case TipoValor::Booleano: return booleano;
        case TipoValor::Entero: return entero != 0;
        case TipoValor::Decimal: return decimal != 0;
        case TipoValor::Texto: return !texto.empty();
        case TipoValor::Lista: return lista && !lista->empty();
        case TipoValor::Mapa: return mapa && !mapa->empty();
        default: return true;
    }
}

double Valor::numero() const {
    if (tipo == TipoValor::Entero) return static_cast<double>(entero);
    if (tipo == TipoValor::Decimal) return decimal;
    if (tipo == TipoValor::Booleano) return booleano ? 1 : 0;
    if (tipo == TipoValor::Texto) {
        try {
            return std::stod(texto);
        } catch (...) {
        }
    }
    throw ErrorEjecucion("se esperaba un numero, se obtuvo " + tipo_nombre());
}

std::string Valor::tipo_nombre() const {
    switch (tipo) {
        case TipoValor::Nulo: return "nulo";
        case TipoValor::Booleano: return "booleano";
        case TipoValor::Entero: return "entero";
        case TipoValor::Decimal: return "decimal";
        case TipoValor::Texto: return "texto";
        case TipoValor::Lista: return "lista";
        case TipoValor::Mapa: return "mapa";
        case TipoValor::Funcion: return "funcion";
        case TipoValor::Clase: return "clase";
        case TipoValor::Objeto: return "objeto";
        case TipoValor::Modulo: return "modulo";
    }
    return "?";
}

std::string Valor::a_texto() const {
    switch (tipo) {
        case TipoValor::Nulo: return "nulo";
        case TipoValor::Booleano: return booleano ? "verdadero" : "falso";
        case TipoValor::Entero: return std::to_string(entero);
        case TipoValor::Decimal: {
            std::ostringstream os;
            os << decimal;
            return os.str();
        }
        case TipoValor::Texto: return texto;
        case TipoValor::Lista: {
            std::string s = "[";
            if (lista) {
                for (std::size_t i = 0; i < lista->size(); ++i) {
                    if (i) s += ", ";
                    s += (*lista)[i].a_texto();
                }
            }
            s += "]";
            return s;
        }
        case TipoValor::Mapa: {
            std::string s = "{";
            bool primero = true;
            if (mapa) {
                for (auto& kv : *mapa) {
                    if (!primero) s += ", ";
                    primero = false;
                    s += kv.first + ": " + kv.second.a_texto();
                }
            }
            s += "}";
            return s;
        }
        case TipoValor::Funcion: return funcion ? "<funcion " + funcion->nombre + ">" : "<funcion>";
        case TipoValor::Clase: return clase ? "<clase " + clase->nombre + ">" : "<clase>";
        case TipoValor::Objeto: return objeto && objeto->clase ? "<objeto " + objeto->clase->nombre + ">" : "<objeto>";
        case TipoValor::Modulo: return modulo ? "<modulo " + modulo->nombre + ">" : "<modulo>";
    }
    return "";
}

bool Entorno::definir(const std::string& n, Valor v, Visibilidad vis, bool cte, bool redefinir) {
    auto it = simbolos.find(n);
    if (it != simbolos.end() && it->second.constante && !redefinir) return false;
    Miembro miembro{std::move(v), vis, cte, {}};
    if (vis == Visibilidad::Privado && es_clase) miembro.propietario_privado = nombre_clase;
    simbolos[n] = std::move(miembro);
    return true;
}

bool Entorno::asignar(const std::string& n, Valor v, std::string* error) {
    auto it = simbolos.find(n);
    if (it != simbolos.end()) {
        if (it->second.constante) {
            if (error) *error = "no se puede reasignar la constante '" + n + "'";
            return false;
        }
        it->second.valor = std::move(v);
        return true;
    }
    if (padre) return padre->asignar(n, std::move(v), error);
    if (error) *error = "variable no definida: " + n;
    return false;
}

bool Entorno::obtener(const std::string& n, Valor* salida, Visibilidad* vis) const {
    auto it = simbolos.find(n);
    if (it != simbolos.end()) {
        *salida = it->second.valor;
        if (vis) *vis = it->second.visibilidad;
        return true;
    }
    if (padre) return padre->obtener(n, salida, vis);
    return false;
}

static bool ambos_enteros(const Valor& a, const Valor& b) {
    return a.tipo == TipoValor::Entero && b.tipo == TipoValor::Entero;
}

Valor operar(const std::string& op, const Valor& a, const Valor& b, Ubicacion u) {
    if (op == "+") {
        if (a.tipo == TipoValor::Texto || b.tipo == TipoValor::Texto)
            return Valor::de_texto(a.a_texto() + b.a_texto());
        if (a.tipo == TipoValor::Lista && b.tipo == TipoValor::Lista) {
            std::vector<Valor> r = a.lista ? *a.lista : std::vector<Valor>{};
            if (b.lista) r.insert(r.end(), b.lista->begin(), b.lista->end());
            return Valor::de_lista(std::move(r));
        }
        if (ambos_enteros(a, b)) return Valor::de_entero(a.entero + b.entero);
        return Valor::de_decimal(a.numero() + b.numero());
    }
    if (op == "-") {
        if (ambos_enteros(a, b)) return Valor::de_entero(a.entero - b.entero);
        return Valor::de_decimal(a.numero() - b.numero());
    }
    if (op == "*") {
        if (ambos_enteros(a, b)) return Valor::de_entero(a.entero * b.entero);
        return Valor::de_decimal(a.numero() * b.numero());
    }
    if (op == "/") {
        double den = b.numero();
        if (den == 0) throw ErrorEjecucion("division entre cero", u);
        return Valor::de_decimal(a.numero() / den);
    }
    if (op == "%") {
        if (ambos_enteros(a, b)) {
            if (b.entero == 0) throw ErrorEjecucion("modulo entre cero", u);
            return Valor::de_entero(a.entero % b.entero);
        }
        throw ErrorEjecucion("modulo requiere enteros", u);
    }
    if (op == "**") {
        if (ambos_enteros(a, b) && b.entero >= 0) {
            std::int64_t r = 1;
            for (std::int64_t i = 0; i < b.entero; ++i) r *= a.entero;
            return Valor::de_entero(r);
        }
        return Valor::de_decimal(std::pow(a.numero(), b.numero()));
    }
    if (op == "==") {
        if (a.tipo != b.tipo) {
            if (a.es_numero() && b.es_numero()) return Valor::de_bool(a.numero() == b.numero());
            return Valor::de_bool(a.a_texto() == b.a_texto());
        }
        if (a.tipo == TipoValor::Texto) return Valor::de_bool(a.texto == b.texto);
        if (a.tipo == TipoValor::Entero) return Valor::de_bool(a.entero == b.entero);
        if (a.tipo == TipoValor::Booleano) return Valor::de_bool(a.booleano == b.booleano);
        if (a.tipo == TipoValor::Nulo) return Valor::de_bool(true);
        return Valor::de_bool(a.a_texto() == b.a_texto());
    }
    if (op == "!=") {
        auto eq = operar("==", a, b, u);
        return Valor::de_bool(!eq.booleano);
    }
    if (op == ">") return Valor::de_bool(a.numero() > b.numero());
    if (op == "<") return Valor::de_bool(a.numero() < b.numero());
    if (op == ">=") return Valor::de_bool(a.numero() >= b.numero());
    if (op == "<=") return Valor::de_bool(a.numero() <= b.numero());
    if (op == "y") return Valor::de_bool(a.verdad() && b.verdad());
    if (op == "o") return Valor::de_bool(a.verdad() || b.verdad());
    throw ErrorEjecucion("operador no soportado: " + op, u);
}

Valor unario(const std::string& op, const Valor& a, Ubicacion u) {
    if (op == "no") return Valor::de_bool(!a.verdad());
    if (op == "-") {
        if (a.tipo == TipoValor::Entero) return Valor::de_entero(-a.entero);
        return Valor::de_decimal(-a.numero());
    }
    throw ErrorEjecucion("operador unario no soportado", u);
}

}  // namespace esp