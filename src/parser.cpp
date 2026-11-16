#include "esp/parser.hpp"
#include <cstdlib>

namespace esp {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

const Token& Parser::actual() const {
    return tokens_[pos_];
}

const Token& Parser::anterior() const {
    return tokens_[pos_ > 0 ? pos_ - 1 : 0];
}

const Token& Parser::avanzar() {
    if (!fin()) ++pos_;
    return anterior();
}

bool Parser::fin() const {
    return actual().tipo == TipoToken::FinArchivo;
}

bool Parser::verificar(TipoToken tipo) const {
    return actual().tipo == tipo;
}

bool Parser::coincide(TipoToken tipo) {
    if (!verificar(tipo)) return false;
    avanzar();
    return true;
}

const Token& Parser::consumir(TipoToken tipo, const std::string& msg) {
    if (verificar(tipo)) return avanzar();
    throw ErrorSintaxis(msg + " (se encontro '" + actual().lexema + "')", actual().ubicacion);
}

const Token& Parser::consumir_identificador(const std::string& msg) {
    if (verificar(TipoToken::Identificador)) return avanzar();
    throw ErrorSintaxis(msg + " (se encontro '" + actual().lexema + "')", actual().ubicacion);
}

void Parser::saltar_saltos() {
    while (coincide(TipoToken::SaltoLinea) || coincide(TipoToken::PuntoYComa)) {
    }
}

void Parser::consumir_fin_linea() {
    if (fin() || verificar(TipoToken::KwFin) || verificar(TipoToken::KwSino) ||
        verificar(TipoToken::KwSiNoSi) || verificar(TipoToken::KwSiNoPuede)) {
        return;
    }
    if (coincide(TipoToken::SaltoLinea) || coincide(TipoToken::PuntoYComa)) {
        saltar_saltos();
        return;
    }
    throw ErrorSintaxis("se esperaba salto de linea o ';' al final de la instruccion", actual().ubicacion);
}

std::unique_ptr<NodoPrograma> Parser::analizar() {
    auto prog = std::make_unique<NodoPrograma>(actual().ubicacion);
    saltar_saltos();
    while (!fin()) {
        prog->sentencias.push_back(sentencia());
        saltar_saltos();
    }
    return prog;
}

std::vector<std::unique_ptr<Nodo>> Parser::bloque_hasta(const std::vector<TipoToken>& terminadores) {
    std::vector<std::unique_ptr<Nodo>> sentencias;
    saltar_saltos();
    while (!fin()) {
        for (TipoToken t : terminadores) {
            if (verificar(t)) return sentencias;
        }
        sentencias.push_back(sentencia());
        saltar_saltos();
    }
    throw ErrorSintaxis("bloque sin cerrar; falta 'fin'", actual().ubicacion);
}

std::unique_ptr<Nodo> Parser::sentencia() {
    Visibilidad vis = Visibilidad::Privado;
    bool tiene_vis = false;
    if (coincide(TipoToken::KwPublico)) {
        vis = Visibilidad::Publico;
        tiene_vis = true;
    } else if (coincide(TipoToken::KwPrivado)) {
        vis = Visibilidad::Privado;
        tiene_vis = true;
    }

    if (coincide(TipoToken::KwFuncion)) return declaracion_funcion(vis);
    if (coincide(TipoToken::KwClase)) return declaracion_clase(vis);
    if (coincide(TipoToken::KwVariable)) return declaracion_variable(false, vis);
    if (coincide(TipoToken::KwConstante)) return declaracion_variable(true, vis);

    if (tiene_vis) {
        throw ErrorSintaxis("los modificadores publico/privado solo aplican a funcion, clase, variable o constante",
                            anterior().ubicacion);
    }

    if (coincide(TipoToken::KwSi)) return sentencia_si();
    if (coincide(TipoToken::KwMientras)) return sentencia_mientras();
    if (coincide(TipoToken::KwPara)) return sentencia_para();
    if (coincide(TipoToken::KwDevolver)) return sentencia_devolver();
    if (coincide(TipoToken::KwConseguir)) return sentencia_conseguir();
    if (coincide(TipoToken::KwExportar)) return sentencia_exportar();
    if (coincide(TipoToken::KwIntenta)) return sentencia_intenta();
    if (coincide(TipoToken::KwRomper)) {
        Ubicacion u = anterior().ubicacion;
        consumir_fin_linea();
        return std::make_unique<NodoRomper>(std::move(u));
    }
    if (coincide(TipoToken::KwContinuar)) {
        Ubicacion u = anterior().ubicacion;
        consumir_fin_linea();
        return std::make_unique<NodoContinuar>(std::move(u));
    }

    // --- SINTAXIS SQL NATIVA EN ESPAÑOL ---
    if (coincide(TipoToken::KwBaseDatos)) {
        Ubicacion u = anterior().ubicacion;
        auto ruta = expresion();
        consumir_fin_linea();
        return std::make_unique<NodoBaseDatos>(std::move(ruta), std::move(u));
    }
    if (coincide(TipoToken::KwCrearTabla)) {
        Ubicacion u = anterior().ubicacion;
        std::string tabla = consumir_identificador("se esperaba nombre de tabla").lexema;
        consumir(TipoToken::ParenIzq, "se esperaba '(' despues del nombre de tabla");
        std::vector<std::string> cols;
        if (!verificar(TipoToken::ParenDer)) {
            do {
                cols.push_back(consumir_identificador("se esperaba nombre de columna").lexema);
            } while (coincide(TipoToken::Coma));
        }
        consumir(TipoToken::ParenDer, "se esperaba ')' al cerrar columnas");
        consumir_fin_linea();
        return std::make_unique<NodoCrearTabla>(std::move(tabla), std::move(cols), std::move(u));
    }
    if (coincide(TipoToken::KwInsertarEn)) {
        Ubicacion u = anterior().ubicacion;
        std::string tabla = consumir_identificador("se esperaba nombre de tabla").lexema;
        consumir(TipoToken::ParenIzq, "se esperaba '(' con columnas");
        std::vector<std::string> cols;
        do {
            cols.push_back(consumir_identificador("se esperaba nombre de columna").lexema);
        } while (coincide(TipoToken::Coma));
        consumir(TipoToken::ParenDer, "se esperaba ')'");
        consumir(TipoToken::KwValores, "se esperaba 'valores'");
        consumir(TipoToken::ParenIzq, "se esperaba '(' antes de los valores");
        std::vector<std::unique_ptr<Nodo>> vals;
        do {
            vals.push_back(expresion());
        } while (coincide(TipoToken::Coma));
        consumir(TipoToken::ParenDer, "se esperaba ')' despues de los valores");
        consumir_fin_linea();
        return std::make_unique<NodoInsertarEn>(std::move(tabla), std::move(cols), std::move(vals), std::move(u));
    }
    if (coincide(TipoToken::KwActualizar)) {
        Ubicacion u = anterior().ubicacion;
        auto upd = std::make_unique<NodoActualizar>(std::move(u));
        upd->tabla = consumir_identificador("se esperaba nombre de tabla").lexema;
        consumir(TipoToken::KwPoner, "se esperaba 'poner' en actualizar");
        do {
            std::string col = consumir_identificador("se esperaba columna").lexema;
            consumir(TipoToken::Igual, "se esperaba '='");
            upd->asignaciones.emplace_back(std::move(col), expresion());
        } while (coincide(TipoToken::Coma));
        if (coincide(TipoToken::KwDonde)) {
            upd->tiene_donde = true;
            upd->col_donde = consumir_identificador("se esperaba columna en 'donde'").lexema;
            if (coincide(TipoToken::IgualIgual) || coincide(TipoToken::Igual)) upd->op_donde = "=";
            else if (coincide(TipoToken::Diferente)) upd->op_donde = "!=";
            else if (coincide(TipoToken::Menor)) upd->op_donde = "<";
            else if (coincide(TipoToken::Mayor)) upd->op_donde = ">";
            else if (coincide(TipoToken::MenorIgual)) upd->op_donde = "<=";
            else if (coincide(TipoToken::MayorIgual)) upd->op_donde = ">=";
            else throw ErrorSintaxis("operador de comparacion no valido en 'donde'", actual().ubicacion);
            upd->val_donde = expresion();
        }
        consumir_fin_linea();
        return upd;
    }
    if (coincide(TipoToken::KwEliminarDe)) {
        Ubicacion u = anterior().ubicacion;
        auto del = std::make_unique<NodoEliminarDe>(std::move(u));
        del->tabla = consumir_identificador("se esperaba nombre de tabla").lexema;
        if (coincide(TipoToken::KwDonde)) {
            del->tiene_donde = true;
            del->col_donde = consumir_identificador("se esperaba columna en 'donde'").lexema;
            if (coincide(TipoToken::IgualIgual) || coincide(TipoToken::Igual)) del->op_donde = "=";
            else if (coincide(TipoToken::Diferente)) del->op_donde = "!=";
            else if (coincide(TipoToken::Menor)) del->op_donde = "<";
            else if (coincide(TipoToken::Mayor)) del->op_donde = ">";
            else if (coincide(TipoToken::MenorIgual)) del->op_donde = "<=";
            else if (coincide(TipoToken::MayorIgual)) del->op_donde = ">=";
            else throw ErrorSintaxis("operador de comparacion no valido en 'donde'", actual().ubicacion);
            del->val_donde = expresion();
        }
        consumir_fin_linea();
        return del;
    }

    // --- SINTAXIS WEB NATIVA EN ESPAÑOL ---
    if (coincide(TipoToken::KwPagina)) {
        Ubicacion u = anterior().ubicacion;
        auto titulo = expresion();
        consumir_fin_linea();
        auto pag = std::make_unique<NodoPagina>(std::move(titulo), std::move(u));
        pag->cuerpo = bloque_hasta({TipoToken::KwFin});
        consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'pagina'");
        consumir_fin_linea();
        return pag;
    }
    if (coincide(TipoToken::KwEstilo)) {
        Ubicacion u = anterior().ubicacion;
        std::string selector = verificar(TipoToken::Identificador) ? avanzar().lexema : "body";
        consumir_fin_linea();
        auto est = std::make_unique<NodoEstilo>(std::move(selector), std::move(u));
        saltar_saltos();
        while (!verificar(TipoToken::KwFin) && !fin()) {
            std::string prop = consumir_identificador("se esperaba propiedad de estilo (ej. fondo, color_texto)").lexema;
            if (!coincide(TipoToken::Igual)) consumir(TipoToken::DosPuntos, "se esperaba '=' o ':' en estilo");
            est->propiedades.emplace_back(std::move(prop), expresion());
            consumir_fin_linea();
        }
        consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'estilo'");
        consumir_fin_linea();
        return est;
    }
    if (coincide(TipoToken::KwScript)) {
        Ubicacion u = anterior().ubicacion;
        consumir_fin_linea();
        auto scr = std::make_unique<NodoScriptWeb>(std::move(u));
        scr->cuerpo = bloque_hasta({TipoToken::KwFin});
        consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'script'");
        consumir_fin_linea();
        return scr;
    }
    if (coincide(TipoToken::KwEncabezado) || coincide(TipoToken::KwSubtitulo) ||
        coincide(TipoToken::KwParrafo) || coincide(TipoToken::KwEntrada)) {
        Token kw = anterior();
        auto el = std::make_unique<NodoElementoWeb>(kw.lexema, kw.ubicacion);
        el->contenido = expresion();
        consumir_fin_linea();
        return el;
    }
    if (coincide(TipoToken::KwEnlace) || coincide(TipoToken::KwImagen)) {
        Token kw = anterior();
        auto el = std::make_unique<NodoElementoWeb>(kw.lexema, kw.ubicacion);
        el->contenido = expresion();
        if (coincide(TipoToken::Coma)) el->extra = expresion();
        consumir_fin_linea();
        return el;
    }
    if (coincide(TipoToken::KwBoton)) {
        Token kw = anterior();
        auto el = std::make_unique<NodoElementoWeb>("boton", kw.ubicacion);
        el->contenido = expresion();
        if (coincide(TipoToken::KwAlClic)) {
            consumir_fin_linea();
            el->al_clic = bloque_hasta({TipoToken::KwFin});
            consumir(TipoToken::KwFin, "se esperaba 'fin' despues de 'al_clic'");
        }
        consumir_fin_linea();
        return el;
    }
    if (coincide(TipoToken::KwContenedor)) {
        Token kw = anterior();
        auto el = std::make_unique<NodoElementoWeb>("contenedor", kw.ubicacion);
        if (!verificar(TipoToken::SaltoLinea) && !verificar(TipoToken::PuntoYComa)) {
            el->contenido = expresion();
        }
        consumir_fin_linea();
        el->hijos = bloque_hasta({TipoToken::KwFin});
        consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'contenedor'");
        consumir_fin_linea();
        return el;
    }

    Ubicacion u = actual().ubicacion;
    auto expr = expresion();
    consumir_fin_linea();
    return std::make_unique<NodoExpresionSentencia>(std::move(expr), std::move(u));
}

std::unique_ptr<Nodo> Parser::declaracion_variable(bool es_constante, Visibilidad vis) {
    Ubicacion u = anterior().ubicacion;
    const Token& nombre = consumir_identificador("se esperaba un nombre de variable");
    if (coincide(TipoToken::DosPuntos)) {
        (void)consumir_identificador("se esperaba tipo despues de ':'");
    }
    std::unique_ptr<Nodo> valor;
    if (coincide(TipoToken::Igual)) {
        valor = expresion();
    } else if (es_constante) {
        throw ErrorSintaxis("una constante debe inicializarse con '='", u);
    } else {
        valor = std::make_unique<NodoLiteralNulo>(u);
    }
    consumir_fin_linea();
    return std::make_unique<NodoDeclaracionVar>(nombre.lexema, std::move(valor), es_constante, vis, std::move(u));
}

std::unique_ptr<Nodo> Parser::declaracion_funcion(Visibilidad vis) {
    Ubicacion u = anterior().ubicacion;
    const Token& nombre = consumir_identificador("se esperaba nombre de funcion");
    consumir(TipoToken::ParenIzq, "se esperaba '(' despues del nombre de funcion");
    std::vector<Parametro> params;
    if (!verificar(TipoToken::ParenDer)) {
        do {
            Parametro p;
            p.nombre = consumir_identificador("se esperaba nombre de parametro").lexema;
            if (coincide(TipoToken::DosPuntos)) {
                p.tipo_anotado = consumir_identificador("se esperaba tipo del parametro").lexema;
            }
            if (coincide(TipoToken::Igual)) {
                p.defecto = expresion();
            }
            params.push_back(std::move(p));
        } while (coincide(TipoToken::Coma));
    }
    consumir(TipoToken::ParenDer, "se esperaba ')' despues de los parametros");
    std::string retorno;
    if (coincide(TipoToken::DosPuntos)) {
        retorno = consumir_identificador("se esperaba tipo de retorno").lexema;
    }
    consumir_fin_linea();
    auto cuerpo = bloque_hasta({TipoToken::KwFin});
    consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar la funcion");
    consumir_fin_linea();
    return std::make_unique<NodoFuncion>(nombre.lexema, std::move(params), std::move(retorno), std::move(cuerpo), vis,
                                         std::move(u));
}

std::unique_ptr<Nodo> Parser::declaracion_clase(Visibilidad vis) {
    Ubicacion u = anterior().ubicacion;
    const Token& nombre = consumir_identificador("se esperaba nombre de clase");
    std::string padre;
    if (coincide(TipoToken::KwHereda) || coincide(TipoToken::KwDe)) {
        padre = consumir_identificador("se esperaba nombre de clase padre").lexema;
    }
    consumir_fin_linea();
    auto clase = std::make_unique<NodoClase>(nombre.lexema, std::move(padre), vis, std::move(u));
    saltar_saltos();
    while (!fin() && !verificar(TipoToken::KwFin)) {
        Visibilidad mvis = Visibilidad::Publico;
        if (coincide(TipoToken::KwPublico)) mvis = Visibilidad::Publico;
        else if (coincide(TipoToken::KwPrivado)) mvis = Visibilidad::Privado;

        if (coincide(TipoToken::KwFuncion)) {
            auto fn = declaracion_funcion(mvis);
            clase->metodos.push_back(std::unique_ptr<NodoFuncion>(static_cast<NodoFuncion*>(fn.release())));
        } else if (coincide(TipoToken::KwVariable)) {
            auto v = declaracion_variable(false, mvis);
            clase->propiedades.push_back(
                std::unique_ptr<NodoDeclaracionVar>(static_cast<NodoDeclaracionVar*>(v.release())));
        } else if (coincide(TipoToken::KwConstante)) {
            auto v = declaracion_variable(true, mvis);
            clase->propiedades.push_back(
                std::unique_ptr<NodoDeclaracionVar>(static_cast<NodoDeclaracionVar*>(v.release())));
        } else {
            throw ErrorSintaxis("dentro de 'clase' solo se permiten propiedades y metodos", actual().ubicacion);
        }
        saltar_saltos();
    }
    consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar la clase");
    consumir_fin_linea();
    return clase;
}

std::unique_ptr<Nodo> Parser::sentencia_si() {
    Ubicacion u = anterior().ubicacion;
    auto nodo = std::make_unique<NodoSi>(std::move(u));
    RamaSi principal;
    principal.condicion = expresion();
    consumir_fin_linea();
    principal.cuerpo = bloque_hasta({TipoToken::KwSiNoSi, TipoToken::KwSino, TipoToken::KwFin});
    nodo->ramas.push_back(std::move(principal));

    while (coincide(TipoToken::KwSiNoSi)) {
        RamaSi rama;
        rama.condicion = expresion();
        consumir_fin_linea();
        rama.cuerpo = bloque_hasta({TipoToken::KwSiNoSi, TipoToken::KwSino, TipoToken::KwFin});
        nodo->ramas.push_back(std::move(rama));
    }

    if (coincide(TipoToken::KwSino)) {
        if (coincide(TipoToken::KwSi)) {
            RamaSi rama;
            rama.condicion = expresion();
            consumir_fin_linea();
            rama.cuerpo = bloque_hasta({TipoToken::KwSiNoSi, TipoToken::KwSino, TipoToken::KwFin});
            nodo->ramas.push_back(std::move(rama));
        } else {
            consumir_fin_linea();
            nodo->sino = bloque_hasta({TipoToken::KwFin});
        }
    }

    consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'si'");
    consumir_fin_linea();
    return nodo;
}

std::unique_ptr<Nodo> Parser::sentencia_mientras() {
    Ubicacion u = anterior().ubicacion;
    auto cond = expresion();
    consumir_fin_linea();
    auto cuerpo = bloque_hasta({TipoToken::KwFin});
    consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'mientras'");
    consumir_fin_linea();
    return std::make_unique<NodoMientras>(std::move(cond), std::move(cuerpo), std::move(u));
}

std::unique_ptr<Nodo> Parser::sentencia_para() {
    Ubicacion u = anterior().ubicacion;
    const Token& var = consumir_identificador("se esperaba variable de control en 'para'");
    if (coincide(TipoToken::Igual)) {
        auto inicio = expresion();
        consumir(TipoToken::KwHasta, "se esperaba 'hasta' en bucle 'para'");
        auto fin_expr = expresion();
        std::unique_ptr<Nodo> paso;
        if (coincide(TipoToken::KwPaso)) {
            paso = expresion();
        }
        consumir_fin_linea();
        auto cuerpo = bloque_hasta({TipoToken::KwFin});
        consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'para'");
        consumir_fin_linea();
        return std::make_unique<NodoParaRango>(var.lexema, std::move(inicio), std::move(fin_expr), std::move(paso),
                                               std::move(cuerpo), std::move(u));
    }
    consumir(TipoToken::KwEn, "se esperaba '=' o 'en' en bucle 'para'");
    auto iterable = expresion();
    consumir_fin_linea();
    auto cuerpo = bloque_hasta({TipoToken::KwFin});
    consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'para'");
    consumir_fin_linea();
    return std::make_unique<NodoParaEn>(var.lexema, std::move(iterable), std::move(cuerpo), std::move(u));
}

std::unique_ptr<Nodo> Parser::sentencia_devolver() {
    Ubicacion u = anterior().ubicacion;
    if (verificar(TipoToken::SaltoLinea) || verificar(TipoToken::PuntoYComa) || verificar(TipoToken::KwFin) || fin()) {
        consumir_fin_linea();
        return std::make_unique<NodoDevolver>(std::make_unique<NodoLiteralNulo>(u), std::move(u));
    }
    auto val = expresion();
    consumir_fin_linea();
    return std::make_unique<NodoDevolver>(std::move(val), std::move(u));
}

std::unique_ptr<Nodo> Parser::sentencia_conseguir() {
    Ubicacion u = anterior().ubicacion;
    std::string modulo;
    if (verificar(TipoToken::Texto)) {
        modulo = avanzar().lexema;
    } else {
        modulo = consumir_identificador("se esperaba nombre o ruta del modulo").lexema;
        while (coincide(TipoToken::Punto)) {
            modulo += "." + consumir_identificador("se esperaba submodulo despues de '.'").lexema;
        }
    }
    std::string alias;
    if (coincide(TipoToken::KwComo)) {
        alias = consumir_identificador("se esperaba alias despues de 'como'").lexema;
    }
    auto nodo = std::make_unique<NodoConseguir>(std::move(modulo), std::move(alias), std::move(u));
    while (verificar(TipoToken::Identificador)) {
        const std::string& clave = actual().lexema;
        if (clave == "version" || clave == "origen" || clave == "sha256") {
            avanzar();
            consumir(TipoToken::Igual, "se esperaba '=' despues de " + clave);
            const Token& val = consumir(TipoToken::Texto, "se esperaba texto despues de '='");
            if (clave == "version") nodo->version = val.lexema;
            else if (clave == "origen") nodo->origen = val.lexema;
            else nodo->sha256 = val.lexema;
        } else {
            break;
        }
    }
    consumir_fin_linea();
    return nodo;
}

std::unique_ptr<Nodo> Parser::sentencia_exportar() {
    Ubicacion u = anterior().ubicacion;
    const Token& nombre = consumir_identificador("se esperaba nombre de simbolo a exportar");
    consumir_fin_linea();
    return std::make_unique<NodoExportar>(nombre.lexema, std::move(u));
}

std::unique_ptr<Nodo> Parser::sentencia_intenta() {
    Ubicacion u = anterior().ubicacion;
    auto nodo = std::make_unique<NodoIntenta>(std::move(u));
    consumir_fin_linea();
    nodo->bloque_intenta = bloque_hasta({TipoToken::KwSiNoPuede});
    consumir(TipoToken::KwSiNoPuede, "se esperaba 'si_no_puede' en bloque 'intenta'");
    if (verificar(TipoToken::Identificador)) {
        nodo->var_error = avanzar().lexema;
    } else {
        nodo->var_error = "error";
    }
    consumir_fin_linea();
    nodo->bloque_catch = bloque_hasta({TipoToken::KwFin});
    consumir(TipoToken::KwFin, "se esperaba 'fin' para cerrar 'intenta'");
    consumir_fin_linea();
    return nodo;
}

std::unique_ptr<Nodo> Parser::expresion() {
    return asignacion();
}

std::unique_ptr<Nodo> Parser::asignacion() {
    auto expr = o_logico();
    if (coincide(TipoToken::Igual)) {
        Ubicacion u = anterior().ubicacion;
        auto val = asignacion();
        if (expr->tipo == TipoNodo::Identificador) {
            auto* id = static_cast<NodoIdentificador*>(expr.get());
            return std::make_unique<NodoAsignacion>(id->nombre, std::move(val), std::move(u));
        }
        if (expr->tipo == TipoNodo::Indice) {
            auto* idx = static_cast<NodoIndice*>(expr.get());
            return std::make_unique<NodoAsignacionIndice>(std::move(idx->objeto), std::move(idx->indice),
                                                          std::move(val), std::move(u));
        }
        if (expr->tipo == TipoNodo::Miembro) {
            auto* m = static_cast<NodoMiembro*>(expr.get());
            return std::make_unique<NodoAsignacionMiembro>(std::move(m->objeto), m->miembro, std::move(val),
                                                           std::move(u));
        }
        throw ErrorSintaxis("destino de asignacion no valido", u);
    }
    return expr;
}

std::unique_ptr<Nodo> Parser::o_logico() {
    auto izq = y_logico();
    while (coincide(TipoToken::KwO)) {
        Ubicacion u = anterior().ubicacion;
        auto der = y_logico();
        izq = std::make_unique<NodoBinario>("o", std::move(izq), std::move(der), std::move(u));
    }
    return izq;
}

std::unique_ptr<Nodo> Parser::y_logico() {
    auto izq = igualdad();
    while (coincide(TipoToken::KwY)) {
        Ubicacion u = anterior().ubicacion;
        auto der = igualdad();
        izq = std::make_unique<NodoBinario>("y", std::move(izq), std::move(der), std::move(u));
    }
    return izq;
}

std::unique_ptr<Nodo> Parser::igualdad() {
    auto izq = comparacion();
    while (coincide(TipoToken::IgualIgual) || coincide(TipoToken::Diferente)) {
        Token op = anterior();
        auto der = comparacion();
        izq = std::make_unique<NodoBinario>(op.lexema, std::move(izq), std::move(der), op.ubicacion);
    }
    return izq;
}

std::unique_ptr<Nodo> Parser::comparacion() {
    auto izq = suma();
    while (coincide(TipoToken::Menor) || coincide(TipoToken::MenorIgual) ||
           coincide(TipoToken::Mayor) || coincide(TipoToken::MayorIgual)) {
        Token op = anterior();
        auto der = suma();
        izq = std::make_unique<NodoBinario>(op.lexema, std::move(izq), std::move(der), op.ubicacion);
    }
    return izq;
}

std::unique_ptr<Nodo> Parser::suma() {
    auto izq = producto();
    while (coincide(TipoToken::Mas) || coincide(TipoToken::Menos)) {
        Token op = anterior();
        auto der = producto();
        izq = std::make_unique<NodoBinario>(op.lexema, std::move(izq), std::move(der), op.ubicacion);
    }
    return izq;
}

std::unique_ptr<Nodo> Parser::producto() {
    auto izq = unario();
    while (coincide(TipoToken::Estrella) || coincide(TipoToken::Barra) || coincide(TipoToken::Porcentaje)) {
        Token op = anterior();
        auto der = unario();
        izq = std::make_unique<NodoBinario>(op.lexema, std::move(izq), std::move(der), op.ubicacion);
    }
    return izq;
}

std::unique_ptr<Nodo> Parser::unario() {
    if (coincide(TipoToken::Menos)) {
        Ubicacion u = anterior().ubicacion;
        return std::make_unique<NodoUnario>("-", unario(), std::move(u));
    }
    if (coincide(TipoToken::KwNo)) {
        Ubicacion u = anterior().ubicacion;
        return std::make_unique<NodoUnario>("no", unario(), std::move(u));
    }
    return postfijo();
}

std::unique_ptr<Nodo> Parser::postfijo() {
    auto expr = primaria();
    while (true) {
        if (coincide(TipoToken::ParenIzq)) {
            Ubicacion u = anterior().ubicacion;
            std::vector<std::unique_ptr<Nodo>> args;
            if (!verificar(TipoToken::ParenDer)) {
                do {
                    args.push_back(expresion());
                } while (coincide(TipoToken::Coma));
            }
            consumir(TipoToken::ParenDer, "se esperaba ')' despues de los argumentos");
            expr = std::make_unique<NodoLlamada>(std::move(expr), std::move(args), std::move(u));
        } else if (coincide(TipoToken::CorcheteIzq)) {
            Ubicacion u = anterior().ubicacion;
            auto idx = expresion();
            consumir(TipoToken::CorcheteDer, "se esperaba ']' despues del indice");
            expr = std::make_unique<NodoIndice>(std::move(expr), std::move(idx), std::move(u));
        } else if (coincide(TipoToken::Punto)) {
            Ubicacion u = anterior().ubicacion;
            const Token& m = consumir_identificador("se esperaba nombre de miembro despues de '.'");
            expr = std::make_unique<NodoMiembro>(std::move(expr), m.lexema, std::move(u));
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Nodo> Parser::primaria() {
    if (coincide(TipoToken::KwSeleccionar)) {
        Ubicacion u = anterior().ubicacion;
        auto sel = std::make_unique<NodoSeleccionar>(std::move(u));
        if (coincide(TipoToken::Estrella)) {
            sel->columnas.push_back("*");
        } else {
            do {
                sel->columnas.push_back(consumir_identificador("se esperaba nombre de columna").lexema);
            } while (coincide(TipoToken::Coma));
        }
        consumir(TipoToken::KwDe, "se esperaba 'de' despues de las columnas en seleccionar");
        sel->tabla = consumir_identificador("se esperaba nombre de la tabla").lexema;
        if (coincide(TipoToken::KwDonde)) {
            sel->tiene_donde = true;
            sel->col_donde = consumir_identificador("se esperaba columna en condicion 'donde'").lexema;
            if (coincide(TipoToken::IgualIgual) || coincide(TipoToken::Igual)) sel->op_donde = "=";
            else if (coincide(TipoToken::Diferente)) sel->op_donde = "!=";
            else if (coincide(TipoToken::Menor)) sel->op_donde = "<";
            else if (coincide(TipoToken::Mayor)) sel->op_donde = ">";
            else if (coincide(TipoToken::MenorIgual)) sel->op_donde = "<=";
            else if (coincide(TipoToken::MayorIgual)) sel->op_donde = ">=";
            else throw ErrorSintaxis("operador de comparacion no valido en 'donde'", actual().ubicacion);
            sel->val_donde = expresion();
        }
        if (coincide(TipoToken::KwOrdenarPor)) {
            sel->ordenar_col = consumir_identificador("se esperaba columna en 'ordenar_por'").lexema;
            if (actual().lexema == "desc" || actual().lexema == "DESC") {
                avanzar();
                sel->descendente = true;
            } else if (actual().lexema == "asc" || actual().lexema == "ASC") {
                avanzar();
            }
        }
        if (coincide(TipoToken::KwLimite)) {
            sel->limite = static_cast<int>(std::stoll(consumir(TipoToken::Entero, "se esperaba numero en 'limite'").lexema));
        }
        return sel;
    }

    if (coincide(TipoToken::Entero)) {
        const Token& t = anterior();
        return std::make_unique<NodoLiteralEntero>(std::stoll(t.lexema), t.ubicacion);
    }
    if (coincide(TipoToken::Decimal)) {
        const Token& t = anterior();
        return std::make_unique<NodoLiteralDecimal>(std::stod(t.lexema), t.ubicacion);
    }
    if (coincide(TipoToken::Texto)) {
        const Token& t = anterior();
        return std::make_unique<NodoLiteralTexto>(t.lexema, t.ubicacion);
    }
    if (coincide(TipoToken::Verdadero)) {
        return std::make_unique<NodoLiteralBool>(true, anterior().ubicacion);
    }
    if (coincide(TipoToken::Falso)) {
        return std::make_unique<NodoLiteralBool>(false, anterior().ubicacion);
    }
    if (coincide(TipoToken::Nulo)) {
        return std::make_unique<NodoLiteralNulo>(anterior().ubicacion);
    }
    if (coincide(TipoToken::KwEste)) {
        return std::make_unique<NodoEste>(anterior().ubicacion);
    }
    if (coincide(TipoToken::KwSuper)) {
        return std::make_unique<NodoSuper>(anterior().ubicacion);
    }
    if (coincide(TipoToken::Identificador)) {
        const Token& t = anterior();
        return std::make_unique<NodoIdentificador>(t.lexema, t.ubicacion);
    }
    if (coincide(TipoToken::ParenIzq)) {
        auto e = expresion();
        consumir(TipoToken::ParenDer, "se esperaba ')' para cerrar agrupacion");
        return e;
    }
    if (coincide(TipoToken::CorcheteIzq)) {
        Ubicacion u = anterior().ubicacion;
        std::vector<std::unique_ptr<Nodo>> elems;
        saltar_saltos();
        if (!verificar(TipoToken::CorcheteDer)) {
            do {
                saltar_saltos();
                if (verificar(TipoToken::CorcheteDer)) break;
                elems.push_back(expresion());
                saltar_saltos();
            } while (coincide(TipoToken::Coma));
        }
        consumir(TipoToken::CorcheteDer, "se esperaba ']' al final de la lista");
        return std::make_unique<NodoLista>(std::move(elems), std::move(u));
    }
    if (coincide(TipoToken::LlaveIzq)) {
        Ubicacion u = anterior().ubicacion;
        std::vector<std::pair<std::unique_ptr<Nodo>, std::unique_ptr<Nodo>>> entradas;
        saltar_saltos();
        if (!verificar(TipoToken::LlaveDer)) {
            do {
                saltar_saltos();
                if (verificar(TipoToken::LlaveDer)) break;
                std::unique_ptr<Nodo> clave;
                if (verificar(TipoToken::Identificador)) {
                    const Token& id = avanzar();
                    clave = std::make_unique<NodoLiteralTexto>(id.lexema, id.ubicacion);
                } else {
                    clave = expresion();
                }
                consumir(TipoToken::DosPuntos, "se esperaba ':' despues de la clave del mapa");
                saltar_saltos();
                auto val = expresion();
                entradas.emplace_back(std::move(clave), std::move(val));
                saltar_saltos();
            } while (coincide(TipoToken::Coma));
        }
        consumir(TipoToken::LlaveDer, "se esperaba '}' al final del mapa");
        return std::make_unique<NodoMapa>(std::move(entradas), std::move(u));
    }

    throw ErrorSintaxis("expresion no valida cerca de '" + actual().lexema + "'", actual().ubicacion);
}

}  // namespace esp