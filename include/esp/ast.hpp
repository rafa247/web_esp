#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp= Visibilidad::Publico;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espe;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp parametros;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp bases;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espquierdo;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esprecho;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espndicion;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esperpo;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espternativo;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espicial;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espso;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espptr<Nodo>> elementos;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espd::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> pares;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espptr<Nodo>> sentencias;
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espptr<Nodo>;#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esppoNodo t, Ubicacion u) {
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace espe<Nodo>();
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp(u);
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp
#pragma once
#include "comun.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esp {

enum class TipoNodo {
    Programa,
    LiteralEntero,
    LiteralDecimal,
    LiteralTexto,
    LiteralBool,
    LiteralNulo,
    Identificador,
    Este,
    Super,
    Lista,
    Mapa,
    Unario,
    Binario,
    Llamada,
    Indice,
    Miembro,
    Asignacion,
    AsignacionIndice,
    AsignacionMiembro,
    ExpresionSentencia,
    DeclaracionVar,
    Si,
    Mientras,
    ParaRango,
    ParaEn,
    Funcion,
    Devolver,
    Romper,
    Continuar,
    Clase,
    Conseguir,
    Exportar,
    Intenta,

    // Nodos Nativos Web y SQL en Español
    SentenciaBaseDatos,
    SentenciaCrearTabla,
    SentenciaInsertarEn,
    ExpresionSeleccionar,
    SentenciaActualizar,
    SentenciaEliminarDe,
    SentenciaPagina,
    SentenciaEstilo,
    SentenciaScriptWeb,
    SentenciaElementoWeb
};

struct Nodo {
    TipoNodo tipo;
    Ubicacion ubicacion;
    Nodo(TipoNodo t, Ubicacion u) : tipo(t), ubicacion(std::move(u)) {}
    virtual ~Nodo() = default;
};

struct NodoPrograma : Nodo {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    explicit NodoPrograma(Ubicacion u = {}) : Nodo(TipoNodo::Programa, std::move(u)) {}
};

struct NodoLiteralEntero : Nodo {
    std::int64_t valor;
    NodoLiteralEntero(std::int64_t v, Ubicacion u) : Nodo(TipoNodo::LiteralEntero, std::move(u)), valor(v) {}
};

struct NodoLiteralDecimal : Nodo {
    double valor;
    NodoLiteralDecimal(double v, Ubicacion u) : Nodo(TipoNodo::LiteralDecimal, std::move(u)), valor(v) {}
};

struct NodoLiteralTexto : Nodo {
    std::string valor;
    NodoLiteralTexto(std::string v, Ubicacion u) : Nodo(TipoNodo::LiteralTexto, std::move(u)), valor(std::move(v)) {}
};

struct NodoLiteralBool : Nodo {
    bool valor;
    NodoLiteralBool(bool v, Ubicacion u) : Nodo(TipoNodo::LiteralBool, std::move(u)), valor(v) {}
};

struct NodoLiteralNulo : Nodo {
    explicit NodoLiteralNulo(Ubicacion u) : Nodo(TipoNodo::LiteralNulo, std::move(u)) {}
};

struct NodoIdentificador : Nodo {
    std::string nombre;
    NodoIdentificador(std::string n, Ubicacion u) : Nodo(TipoNodo::Identificador, std::move(u)), nombre(std::move(n)) {}
};

struct NodoEste : Nodo {
    explicit NodoEste(Ubicacion u) : Nodo(TipoNodo::Este, std::move(u)) {}
};

struct NodoSuper : Nodo {
    explicit NodoSuper(Ubicacion u) : Nodo(TipoNodo::Super, std::move(u)) {}
};

struct NodoLista : Nodo {
    std::vector<std::unique_ptr<Nodo>> elementos;
    NodoLista(std::vector<std::unique_ptr<Nodo>> e, Ubicacion u)
        : Nodo(TipoNodo::Lista, std::move(u)), elementos(std::move(e)) {}
};

struct NodoMapa : Nodo {
    std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
    NodoMapa(std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> e, Ubicacion u)
        : Nodo(TipoNodo::Mapa, std::move(u)), entradas(std::move(e)) {}
};

struct NodoUnario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> operando;
    NodoUnario(std::string op, std::unique_ptr<Nodo> x, Ubicacion u)
        : Nodo(TipoNodo::Unario, std::move(u)), operador(std::move(op)), operando(std::move(x)) {}
};

struct NodoBinario : Nodo {
    std::string operador;
    std::unique_ptr<Nodo> izquierda;
    std::unique_ptr<Nodo> derecha;
    NodoBinario(std::string op, std::unique_ptr<Nodo> izq, std::unique_ptr<Nodo> der, Ubicacion u)
        : Nodo(TipoNodo::Binario, std::move(u)),
          operador(std::move(op)),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

struct NodoLlamada : Nodo {
    std::unique_ptr<Nodo> callee;
    std::vector<std::unique_ptr<Nodo>> argumentos;
    NodoLlamada(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> args, Ubicacion u)
        : Nodo(TipoNodo::Llamada, std::move(u)), callee(std::move(c)), argumentos(std::move(args)) {}
};

struct NodoIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    NodoIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, Ubicacion u)
        : Nodo(TipoNodo::Indice, std::move(u)), objeto(std::move(o)), indice(std::move(i)) {}
};

struct NodoMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    NodoMiembro(std::unique_ptr<Nodo> o, std::string m, Ubicacion u)
        : Nodo(TipoNodo::Miembro, std::move(u)), objeto(std::move(o)), miembro(std::move(m)) {}
};

struct NodoAsignacion : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    NodoAsignacion(std::string n, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::Asignacion, std::move(u)), nombre(std::move(n)), valor(std::move(v)) {}
};

struct NodoAsignacionIndice : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::unique_ptr<Nodo> indice;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionIndice(std::unique_ptr<Nodo> o, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionIndice, std::move(u)),
          objeto(std::move(o)),
          indice(std::move(i)),
          valor(std::move(v)) {}
};

struct NodoAsignacionMiembro : Nodo {
    std::unique_ptr<Nodo> objeto;
    std::string miembro;
    std::unique_ptr<Nodo> valor;
    NodoAsignacionMiembro(std::unique_ptr<Nodo> o, std::string m, std::unique_ptr<Nodo> v, Ubicacion u)
        : Nodo(TipoNodo::AsignacionMiembro, std::move(u)),
          objeto(std::move(o)),
          miembro(std::move(m)),
          valor(std::move(v)) {}
};

struct NodoExpresionSentencia : Nodo {
    std::unique_ptr<Nodo> expresion;
    NodoExpresionSentencia(std::unique_ptr<Nodo> e, Ubicacion u)
        : Nodo(TipoNodo::ExpresionSentencia, std::move(u)), expresion(std::move(e)) {}
};

struct NodoDeclaracionVar : Nodo {
    std::string nombre;
    std::unique_ptr<Nodo> valor;
    bool es_constante = false;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoDeclaracionVar(std::string n, std::unique_ptr<Nodo> v, bool c, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::DeclaracionVar, std::move(u)),
          nombre(std::move(n)),
          valor(std::move(v)),
          es_constante(c),
          visibilidad(vis) {}
};

struct RamaSi {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
};

struct NodoSi : Nodo {
    std::vector<RamaSi> ramas;
    std::vector<std::unique_ptr<Nodo>> sino;
    explicit NodoSi(Ubicacion u) : Nodo(TipoNodo::Si, std::move(u)) {}
};

struct NodoMientras : Nodo {
    std::unique_ptr<Nodo> condicion;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoMientras(std::unique_ptr<Nodo> c, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::Mientras, std::move(u)), condicion(std::move(c)), cuerpo(std::move(b)) {}
};

struct NodoParaRango : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> inicio;
    std::unique_ptr<Nodo> fin;
    std::unique_ptr<Nodo> paso;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaRango(std::string v, std::unique_ptr<Nodo> i, std::unique_ptr<Nodo> f, std::unique_ptr<Nodo> p,
                  std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaRango, std::move(u)),
          variable(std::move(v)),
          inicio(std::move(i)),
          fin(std::move(f)),
          paso(std::move(p)),
          cuerpo(std::move(b)) {}
};

struct NodoParaEn : Nodo {
    std::string variable;
    std::unique_ptr<Nodo> iterable;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoParaEn(std::string v, std::unique_ptr<Nodo> it, std::vector<std::unique_ptr<Nodo>> b, Ubicacion u)
        : Nodo(TipoNodo::ParaEn, std::move(u)),
          variable(std::move(v)),
          iterable(std::move(it)),
          cuerpo(std::move(b)) {}
};

struct Parametro {
    std::string nombre;
    std::string tipo_anotado;
    std::unique_ptr<Nodo> defecto;
};

struct NodoFuncion : Nodo {
    std::string nombre;
    std::vector<Parametro> parametros;
    std::string tipo_retorno;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoFuncion(std::string n, std::vector<Parametro> p, std::string ret, std::vector<std::unique_ptr<Nodo>> b,
                Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Funcion, std::move(u)),
          nombre(std::move(n)),
          parametros(std::move(p)),
          tipo_retorno(std::move(ret)),
          cuerpo(std::move(b)),
          visibilidad(vis) {}
};

struct NodoDevolver : Nodo {
    std::unique_ptr<Nodo> valor;
    NodoDevolver(std::unique_ptr<Nodo> v, Ubicacion u) : Nodo(TipoNodo::Devolver, std::move(u)), valor(std::move(v)) {}
};

struct NodoRomper : Nodo {
    explicit NodoRomper(Ubicacion u) : Nodo(TipoNodo::Romper, std::move(u)) {}
};

struct NodoContinuar : Nodo {
    explicit NodoContinuar(Ubicacion u) : Nodo(TipoNodo::Continuar, std::move(u)) {}
};

struct NodoClase : Nodo {
    std::string nombre;
    std::string padre;
    std::vector<std::unique_ptr<NodoDeclaracionVar>> propiedades;
    std::vector<std::unique_ptr<NodoFuncion>> metodos;
    Visibilidad visibilidad = Visibilidad::Privado;
    NodoClase(std::string n, std::string p, Visibilidad vis, Ubicacion u)
        : Nodo(TipoNodo::Clase, std::move(u)), nombre(std::move(n)), padre(std::move(p)), visibilidad(vis) {}
};

struct NodoConseguir : Nodo {
    std::string modulo;
    std::string alias;
    std::string version;
    std::string origen;
    std::string sha256;
    NodoConseguir(std::string m, std::string a, Ubicacion u)
        : Nodo(TipoNodo::Conseguir, std::move(u)), modulo(std::move(m)), alias(std::move(a)) {}
};

struct NodoExportar : Nodo {
    std::string nombre;
    NodoExportar(std::string n, Ubicacion u) : Nodo(TipoNodo::Exportar, std::move(u)), nombre(std::move(n)) {}
};

struct NodoIntenta : Nodo {
    std::vector<std::unique_ptr<Nodo>> bloque_intenta;
    std::string var_error;
    std::vector<std::unique_ptr<Nodo>> bloque_catch;
    explicit NodoIntenta(Ubicacion u) : Nodo(TipoNodo::Intenta, std::move(u)) {}
};

// --- NODOS NATIVOS SQL Y WEB EN ESPAÑOL ---

struct NodoBaseDatos : Nodo {
    std::unique_ptr<Nodo> ruta;
    NodoBaseDatos(std::unique_ptr<Nodo> r, Ubicacion u)
        : Nodo(TipoNodo::SentenciaBaseDatos, std::move(u)), ruta(std::move(r)) {}
};

struct NodoCrearTabla : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    NodoCrearTabla(std::string t, std::vector<std::string> cols, Ubicacion u)
        : Nodo(TipoNodo::SentenciaCrearTabla, std::move(u)), tabla(std::move(t)), columnas(std::move(cols)) {}
};

struct NodoInsertarEn : Nodo {
    std::string tabla;
    std::vector<std::string> columnas;
    std::vector<std::unique_ptr<Nodo>> valores;
    NodoInsertarEn(std::string t, std::vector<std::string> cols, std::vector<std::unique_ptr<Nodo>> vals, Ubicacion u)
        : Nodo(TipoNodo::SentenciaInsertarEn, std::move(u)),
          tabla(std::move(t)),
          columnas(std::move(cols)),
          valores(std::move(vals)) {}
};

struct NodoSeleccionar : Nodo {
    std::vector<std::string> columnas;
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    std::string ordenar_col;
    bool descendente = false;
    int limite = -1;
    explicit NodoSeleccionar(Ubicacion u) : Nodo(TipoNodo::ExpresionSeleccionar, std::move(u)) {}
};

struct NodoActualizar : Nodo {
    std::string tabla;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> asignaciones;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoActualizar(Ubicacion u) : Nodo(TipoNodo::SentenciaActualizar, std::move(u)) {}
};

struct NodoEliminarDe : Nodo {
    std::string tabla;
    bool tiene_donde = false;
    std::string col_donde;
    std::string op_donde;
    std::unique_ptr<Nodo> val_donde;
    explicit NodoEliminarDe(Ubicacion u) : Nodo(TipoNodo::SentenciaEliminarDe, std::move(u)) {}
};

struct NodoEstilo : Nodo {
    std::string selector;
    std::vector<std::pair<std::string, std::unique_ptr<Nodo>>> propiedades;
    NodoEstilo(std::string sel, Ubicacion u)
        : Nodo(TipoNodo::SentenciaEstilo, std::move(u)), selector(std::move(sel)) {}
};

struct NodoScriptWeb : Nodo {
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    explicit NodoScriptWeb(Ubicacion u) : Nodo(TipoNodo::SentenciaScriptWeb, std::move(u)) {}
};

struct NodoElementoWeb : Nodo {
    std::string etiqueta;
    std::unique_ptr<Nodo> contenido;
    std::unique_ptr<Nodo> extra;
    std::vector<std::unique_ptr<Nodo>> al_clic;
    std::vector<std::unique_ptr<Nodo>> hijos;
    NodoElementoWeb(std::string et, Ubicacion u)
        : Nodo(TipoNodo::SentenciaElementoWeb, std::move(u)), etiqueta(std::move(et)) {}
};

struct NodoPagina : Nodo {
    std::unique_ptr<Nodo> titulo;
    std::vector<std::unique_ptr<Nodo>> cuerpo;
    NodoPagina(std::unique_ptr<Nodo> t, Ubicacion u)
        : Nodo(TipoNodo::SentenciaPagina, std::move(u)), titulo(std::move(t)) {}
};

}  // namespace esp