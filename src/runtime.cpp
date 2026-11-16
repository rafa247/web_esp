#include "esp/runtime.hpp"
#include "esp/codegen.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace esp {
namespace {

struct SenalDevolver {
    Valor valor;
};

struct SenalRomper {};
struct SenalContinuar {};

struct ContextoWebNativo {
    std::string titulo = "Aplicacion ESP 2.0.0";
    std::ostringstream css;
    std::ostringstream html;
    std::ostringstream js;
};

static thread_local ContextoWebNativo g_web_ctx;
static thread_local std::string g_db_nativa_activa = ":memoria:";

std::string escapar_html_nativo(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '&') out += "&amp;";
        else if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else if (c == '"') out += "&quot;";
        else out.push_back(c);
    }
    return out;
}

std::string escapar_sql_nativo(const Valor& v) {
    if (v.tipo == TipoValor::Nulo) return "NULL";
    if (v.tipo == TipoValor::Entero || v.tipo == TipoValor::Decimal || v.tipo == TipoValor::Booleano) {
        return v.a_texto();
    }
    std::string t = v.a_texto();
    std::string out = "'";
    for (char c : t) {
        if (c == '\'') out += "''";
        else out.push_back(c);
    }
    out += "'";
    return out;
}

std::string propiedad_css_esp(const std::string& k) {
    if (k == "fondo") return "background";
    if (k == "color_texto" || k == "color") return "color";
    if (k == "fuente") return "font-family";
    if (k == "tamano_fuente") return "font-size";
    if (k == "margen") return "margin";
    if (k == "relleno") return "padding";
    if (k == "borde") return "border";
    if (k == "radio_borde" || k == "redondeado") return "border-radius";
    if (k == "ancho") return "width";
    if (k == "alto") return "height";
    if (k == "alinear") return "text-align";
    if (k == "disposicion") return "display";
    return k;
}

std::string generar_html_final_web(const ContextoWebNativo& ctx) {
    std::ostringstream doc;
    doc << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
        << "  <meta charset=\"UTF-8\">\n"
        << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << "  <title>" << escapar_html_nativo(ctx.titulo) << "</title>\n"
        << "  <style>\n"
        << "    * { box-sizing: border-box; }\n"
        << "    body { margin: 0; padding: 2rem; font-family: system-ui, -apple-system, sans-serif; line-height: 1.6; background: #0b1020; color: #edf2ff; }\n"
        << "    .esp-contenedor { max-width: 980px; margin: 0 auto; }\n"
        << "    .tarjeta, .esp-bloque { border: 1px solid rgba(255,255,255,0.14); border-radius: 14px; padding: 1.4rem; margin: 1rem 0; background: rgba(255,255,255,0.04); }\n"
        << "    .esp-boton { padding: 0.65rem 1.2rem; border-radius: 10px; border: none; font-weight: 700; cursor: pointer; background: #54e5d0; color: #08121e; }\n"
        << "    .esp-entrada { display: block; width: 100%; padding: 0.65rem; margin: 0.5rem 0 1rem; border-radius: 8px; border: 1px solid rgba(255,255,255,0.2); background: #090d1a; color: #edf2ff; }\n"
        << ctx.css.str()
        << "  </style>\n</head>\n<body>\n  <main class=\"esp-contenedor\">\n"
        << ctx.html.str()
        << "  </main>\n";
    if (!ctx.js.str().empty()) {
        doc << "  <script>\n" << ctx.js.str() << "  </script>\n";
    }
    doc << "</body>\n</html>\n";
    return doc.str();
}

Valor clonar_valor_de_instancia(const Valor& v) {
    if (v.tipo == TipoValor::Lista && v.lista) {
        std::vector<Valor> copia;
        copia.reserve(v.lista->size());
        for (const auto& elem : *v.lista) copia.push_back(clonar_valor_de_instancia(elem));
        return Valor::de_lista(std::move(copia));
    }
    if (v.tipo == TipoValor::Mapa && v.mapa) {
        std::map<std::string, Valor> copia;
        for (const auto& kv : *v.mapa) copia.emplace(kv.first, clonar_valor_de_instancia(kv.second));
        return Valor::de_mapa(std::move(copia));
    }
    return v;
}

bool es_identificador_paquete_remoto(const std::string& nombre) {
    if (nombre.empty()) return false;
    for (unsigned char c : nombre) {
        if (!std::isalnum(c) && c != '_' && c != '-') return false;
    }
    return true;
}

std::string url_paquete_remoto(const std::string& nombre, const std::string& version, const std::string& origen) {
    if (origen.rfind("https://", 0) == 0 || origen.rfind("http://", 0) == 0) {
        if (!origen.empty() && origen.back() == '/') return origen + nombre + ".esp";
        return origen;
    }
    if (!es_identificador_paquete_remoto(nombre)) return "";
    const char* base_env = std::getenv("ESP_REGISTRO_URL");
    std::string base = (base_env && *base_env)
                           ? std::string(base_env)
                           : "https://raw.githubusercontent.com/esp-lang/registro/main/paquetes";
    while (!base.empty() && base.back() == '/') base.pop_back();
    if (!version.empty()) return base + "/" + nombre + "/" + version + "/" + nombre + ".esp";
    return base + "/" + nombre + "/latest/" + nombre + ".esp";
}

bool es_version_exacta(const std::string& version) {
    int partes = 1;
    bool digito = false;
    for (unsigned char c : version) {
        if (std::isdigit(c)) {
            digito = true;
        } else if (c == '.') {
            if (!digito || partes >= 3) return false;
            ++partes;
            digito = false;
        } else {
            return false;
        }
    }
    return digito && partes == 3;
}

std::size_t idx_lista(const Valor& lista, const Valor& indice, Ubicacion u) {
    if (indice.tipo != TipoValor::Entero) throw ErrorEjecucion("el indice de lista debe ser entero", u);
    std::int64_t i = indice.entero;
    std::size_t n = lista.lista ? lista.lista->size() : 0;
    if (i >= 1 && static_cast<std::size_t>(i) <= n) return static_cast<std::size_t>(i - 1);
    if (i == 0 && n > 0) return 0;
    throw ErrorEjecucion("indice de lista fuera de rango: " + std::to_string(i), u);
}

std::string nombre_clase_actual(const std::shared_ptr<Entorno>& env) {
    Valor este;
    if (env && env->obtener("este", &este) && este.tipo == TipoValor::Objeto && este.objeto) {
        return este.objeto->clase->nombre;
    }
    return "";
}

}  // namespace

Entorno::Entorno(std::shared_ptr<Entorno> padre) : padre_(std::move(padre)) {}

void Entorno::definir(const std::string& nombre, Valor valor, Visibilidad vis, bool es_constante, bool exportado) {
    simbolos_[nombre] = Simbolo{std::move(valor), vis, es_constante, exportado};
}

bool Entorno::asignar(const std::string& nombre, Valor valor, Ubicacion u) {
    auto it = simbolos_.find(nombre);
    if (it != simbolos_.end()) {
        if (it->second.es_constante) throw ErrorEjecucion("no se puede modificar la constante '" + nombre + "'", u);
        it->second.valor = std::move(valor);
        return true;
    }
    if (padre_) return padre_->asignar(nombre, std::move(valor), std::move(u));
    return false;
}

bool Entorno::obtener(const std::string& nombre, Valor* salida) const {
    auto it = simbolos_.find(nombre);
    if (it != simbolos_.end()) {
        if (salida) *salida = it->second.valor;
        return true;
    }
    if (padre_) return padre_->obtener(nombre, salida);
    return false;
}

Simbolo* Entorno::buscar_local(const std::string& nombre) {
    auto it = simbolos_.find(nombre);
    return it != simbolos_.end() ? &it->second : nullptr;
}

const Simbolo* Entorno::buscar_local(const std::string& nombre) const {
    auto it = simbolos_.find(nombre);
    return it != simbolos_.end() ? &it->second : nullptr;
}

Interprete::Interprete() {
    global_ = std::make_shared<Entorno>();
    actual_ = global_;
    registrar_nativas();
}

void Interprete::registrar_nativa(const std::string& nombre,
                                  std::function<Valor(std::vector<Valor>&, Ubicacion)> fn) {
    auto f = std::make_shared<Funcion>();
    f->nombre = nombre;
    f->es_nativa = true;
    f->nativa = std::move(fn);
    Valor v;
    v.tipo = TipoValor::Funcion;
    v.funcion = f;
    global_->definir(nombre, std::move(v), Visibilidad::Publico, true, true);
}

void Interprete::registrar_nativas() {
    registrar_nativa("mostrar", [](std::vector<Valor>& args, Ubicacion) {
        for (std::size_t i = 0; i < args.size(); ++i) {
            if (i) std::cout << ' ';
            std::cout << args[i].a_texto();
        }
        std::cout << '\n';
        return Valor::nulo();
    });

    registrar_nativa("imprimir", [](std::vector<Valor>& args, Ubicacion) {
        for (std::size_t i = 0; i < args.size(); ++i) {
            if (i) std::cout << ' ';
            std::cout << args[i].a_texto();
        }
        std::cout << '\n';
        return Valor::nulo();
    });

    registrar_nativa("leer_linea", [](std::vector<Valor>& args, Ubicacion) {
        if (!args.empty()) std::cout << args[0].a_texto() << std::flush;
        std::string linea;
        std::getline(std::cin, linea);
        return Valor::de_texto(std::move(linea));
    });

    registrar_nativa("longitud", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 1) throw ErrorEjecucion("longitud(x) espera 1 argumento", u);
        const Valor& v = args[0];
        if (v.tipo == TipoValor::Texto) return Valor::de_entero(static_cast<std::int64_t>(v.texto.size()));
        if (v.tipo == TipoValor::Lista) return Valor::de_entero(static_cast<std::int64_t>(v.lista ? v.lista->size() : 0));
        if (v.tipo == TipoValor::Mapa) return Valor::de_entero(static_cast<std::int64_t>(v.mapa ? v.mapa->size() : 0));
        throw ErrorEjecucion("tipo sin longitud: " + v.tipo_texto(), u);
    });

    registrar_nativa("tipo_de", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 1) throw ErrorEjecucion("tipo_de(x) espera 1 argumento", u);
        return Valor::de_texto(args[0].tipo_texto());
    });

    registrar_nativa("texto", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 1) throw ErrorEjecucion("texto(x) espera 1 argumento", u);
        return Valor::de_texto(args[0].a_texto());
    });

    registrar_nativa("entero", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 1) throw ErrorEjecucion("entero(x) espera 1 argumento", u);
        const Valor& v = args[0];
        if (v.tipo == TipoValor::Entero) return v;
        if (v.tipo == TipoValor::Decimal) return Valor::de_entero(static_cast<std::int64_t>(v.decimal));
        if (v.tipo == TipoValor::Booleano) return Valor::de_entero(v.booleano ? 1 : 0);
        if (v.tipo == TipoValor::Texto) {
            try {
                return Valor::de_entero(std::stoll(v.texto));
            } catch (...) {
                throw ErrorEjecucion("no se pudo convertir a entero: '" + v.texto + "'", u);
            }
        }
        throw ErrorEjecucion("no se puede convertir a entero", u);
    });

    registrar_nativa("decimal", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 1) throw ErrorEjecucion("decimal(x) espera 1 argumento", u);
        const Valor& v = args[0];
        if (v.tipo == TipoValor::Decimal) return v;
        if (v.tipo == TipoValor::Entero) return Valor::de_decimal(static_cast<double>(v.entero));
        if (v.tipo == TipoValor::Texto) {
            try {
                return Valor::de_decimal(std::stod(v.texto));
            } catch (...) {
                throw ErrorEjecucion("no se pudo convertir a decimal: '" + v.texto + "'", u);
            }
        }
        throw ErrorEjecucion("no se puede convertir a decimal", u);
    });

    registrar_nativa("agregar", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 2 || args[0].tipo != TipoValor::Lista || !args[0].lista)
            throw ErrorEjecucion("agregar(lista, valor) requiere una lista", u);
        args[0].lista->push_back(args[1]);
        return Valor::de_entero(static_cast<std::int64_t>(args[0].lista->size()));
    });

    registrar_nativa("claves", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.size() != 1 || args[0].tipo != TipoValor::Mapa || !args[0].mapa)
            throw ErrorEjecucion("claves(mapa) requiere un mapa", u);
        std::vector<Valor> res;
        for (const auto& kv : *args[0].mapa) res.push_back(Valor::de_texto(kv.first));
        return Valor::de_lista(std::move(res));
    });

    registrar_nativa("afirmar", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.empty() || !args[0].verdad()) {
            std::string msg = args.size() > 1 ? args[1].a_texto() : "afirmacion fallida";
            throw ErrorEjecucion(msg, u);
        }
        return Valor::de_bool(true);
    });

    registrar_nativa("guardar_web", [](std::vector<Valor>& args, Ubicacion u) {
        if (args.empty()) throw ErrorEjecucion("guardar_web(ruta_html)", u);
        std::string ruta = args[0].a_texto();
        escribir_archivo(ruta, generar_html_final_web(g_web_ctx));
        return Valor::de_bool(true);
    });

    registrar_nativa("html_web", [](std::vector<Valor>&, Ubicacion) {
        return Valor::de_texto(generar_html_final_web(g_web_ctx));
    });

    registrar_nativa("servir_web", [this](std::vector<Valor>& args, Ubicacion u) {
        int puerto = args.empty() ? 8080 : static_cast<int>(args[0].numero());
        int peticiones = args.size() > 1 ? static_cast<int>(args[1].numero()) : 5;
        auto mod_web = cargar_modulo_estandar("web");
        if (!mod_web) throw ErrorEjecucion("servidor web no disponible", u);
        Valor fn_srv;
        if (!mod_web->entorno->obtener("servidor", &fn_srv))
            throw ErrorEjecucion("funcion servidor no disponible", u);
        std::vector<Valor> call_args = {
            Valor::de_entero(puerto),
            Valor::de_mapa({{"/", Valor::de_texto(generar_html_final_web(g_web_ctx))}}),
            Valor::de_entero(peticiones)
        };
        return llamar(fn_srv, call_args, u);
    });
}

std::string Interprete::transpilar_js_esp(const std::vector<std::unique_ptr<Nodo>>& sents) {
    std::ostringstream out;
    for (const auto& s : sents) {
        if (!s) continue;
        if (s->tipo == TipoNodo::ExpresionSentencia) {
            auto* es = static_cast<NodoExpresionSentencia*>(s.get());
            if (es->expresion->tipo == TipoNodo::Llamada) {
                auto* ll = static_cast<NodoLlamada*>(es->expresion.get());
                if (ll->callee->tipo == TipoNodo::Identificador) {
                    std::string fn_nom = static_cast<NodoIdentificador*>(ll->callee.get())->nombre;
                    if (fn_nom == "alerta") fn_nom = "alert";
                    else if (fn_nom == "consola" || fn_nom == "mostrar") fn_nom = "console.log";
                    out << fn_nom << "(";
                    for (std::size_t i = 0; i < ll->argumentos.size(); ++i) {
                        if (i > 0) out << ", ";
                        Valor v = evaluar(ll->argumentos[i].get());
                        if (v.tipo == TipoValor::Texto) out << "\"" << v.texto << "\"";
                        else out << v.a_texto();
                    }
                    out << ");\n";
                }
            }
        } else if (s->tipo == TipoNodo::DeclaracionVar) {
            auto* dv = static_cast<NodoDeclaracionVar*>(s.get());
            Valor v = evaluar(dv->valor.get());
            out << (dv->es_constante ? "const " : "let ") << dv->nombre << " = ";
            if (v.tipo == TipoValor::Texto) out << "\"" << v.texto << "\";\n";
            else out << v.a_texto() << ";\n";
        } else if (s->tipo == TipoNodo::Funcion) {
            auto* fn_n = static_cast<NodoFuncion*>(s.get());
            out << "function " << fn_n->nombre << "(";
            for (std::size_t i = 0; i < fn_n->parametros.size(); ++i) {
                if (i > 0) out << ", ";
                out << fn_n->parametros[i].nombre;
            }
            out << ") {\n" << transpilar_js_esp(fn_n->cuerpo) << "}\n";
        }
    }
    return out.str();
}

Valor Interprete::ejecutar_fuente(const std::string& fuente, const std::string& archivo) {
    Lexer lexer(fuente, archivo);
    auto tokens = lexer.analizar();
    Parser parser(std::move(tokens));
    auto ast = parser.analizar();
    return ejecutar_programa(ast.get());
}

Valor Interprete::ejecutar_archivo(const std::string& ruta) {
    auto abs = std::filesystem::absolute(ruta);
    directorio_trabajo = abs.parent_path().string();
    return ejecutar_fuente(leer_archivo(abs.string()), abs.string());
}

Valor Interprete::evaluar_repl(const std::string& linea) {
    Lexer lexer(linea, "<repl>");
    auto tokens = lexer.analizar();
    Parser parser(std::move(tokens));
    auto ast = parser.analizar();
    Valor ultimo = Valor::nulo();
    for (const auto& s : ast->sentencias) {
        if (s->tipo == TipoNodo::ExpresionSentencia) {
            ultimo = evaluar(static_cast<NodoExpresionSentencia*>(s.get())->expresion.get());
        } else {
            ejecutar_sentencia(s.get());
            ultimo = Valor::nulo();
        }
    }
    return ultimo;
}

Valor Interprete::ejecutar_programa(const NodoPrograma* programa) {
    Valor ultimo = Valor::nulo();
    for (const auto& s : programa->sentencias) {
        if (s->tipo == TipoNodo::ExpresionSentencia) {
            ultimo = evaluar(static_cast<NodoExpresionSentencia*>(s.get())->expresion.get());
        } else {
            ejecutar_sentencia(s.get());
            ultimo = Valor::nulo();
        }
    }
    return ultimo;
}

void Interprete::ejecutar_bloque(const std::vector<std::unique_ptr<Nodo>>& sentencias,
                                 const std::shared_ptr<Entorno>& entorno) {
    auto anterior = actual_;
    actual_ = entorno;
    try {
        for (const auto& s : sentencias) ejecutar_sentencia(s.get());
    } catch (...) {
        actual_ = anterior;
        throw;
    }
    actual_ = anterior;
}

void Interprete::ejecutar_sentencia(const Nodo* nodo) {
    switch (nodo->tipo) {
        case TipoNodo::ExpresionSentencia:
            (void)evaluar(static_cast<const NodoExpresionSentencia*>(nodo)->expresion.get());
            return;

        case TipoNodo::DeclaracionVar: {
            const auto* n = static_cast<const NodoDeclaracionVar*>(nodo);
            Valor v = evaluar(n->valor.get());
            actual_->definir(n->nombre, std::move(v), n->visibilidad, n->es_constante, false);
            return;
        }

        case TipoNodo::Si: {
            const auto* n = static_cast<const NodoSi*>(nodo);
            for (const auto& rama : n->ramas) {
                if (evaluar(rama.condicion.get()).verdad()) {
                    ejecutar_bloque(rama.cuerpo, std::make_shared<Entorno>(actual_));
                    return;
                }
            }
            if (!n->sino.empty()) ejecutar_bloque(n->sino, std::make_shared<Entorno>(actual_));
            return;
        }

        case TipoNodo::Mientras: {
            const auto* n = static_cast<const NodoMientras*>(nodo);
            while (evaluar(n->condicion.get()).verdad()) {
                try {
                    ejecutar_bloque(n->cuerpo, std::make_shared<Entorno>(actual_));
                } catch (const SenalContinuar&) {
                    continue;
                } catch (const SenalRomper&) {
                    break;
                }
            }
            return;
        }

        case TipoNodo::ParaRango: {
            const auto* n = static_cast<const NodoParaRango*>(nodo);
            Valor ini = evaluar(n->inicio.get());
            Valor fin_v = evaluar(n->fin.get());
            Valor paso_v = n->paso ? evaluar(n->paso.get()) : Valor::de_entero(1);
            if (!ini.es_numero() || !fin_v.es_numero() || !paso_v.es_numero())
                throw ErrorEjecucion("el rango en 'para' requiere numeros", n->ubicacion);
            double paso = paso_v.numero();
            if (paso == 0.0) throw ErrorEjecucion("el paso en 'para' no puede ser cero", n->ubicacion);
            bool enteros = ini.tipo == TipoValor::Entero && fin_v.tipo == TipoValor::Entero &&
                           paso_v.tipo == TipoValor::Entero;
            double i = ini.numero();
            double limite = fin_v.numero();
            while ((paso > 0 && i <= limite) || (paso < 0 && i >= limite)) {
                auto ambito = std::make_shared<Entorno>(actual_);
                ambito->definir(n->variable,
                                enteros ? Valor::de_entero(static_cast<std::int64_t>(i)) : Valor::de_decimal(i),
                                Visibilidad::Privado, false, false);
                try {
                    ejecutar_bloque(n->cuerpo, ambito);
                } catch (const SenalContinuar&) {
                } catch (const SenalRomper&) {
                    break;
                }
                i += paso;
            }
            return;
        }

        case TipoNodo::ParaEn: {
            const auto* n = static_cast<const NodoParaEn*>(nodo);
            Valor it = evaluar(n->iterable.get());
            if (it.tipo == TipoValor::Lista && it.lista) {
                for (const auto& elem : *it.lista) {
                    auto ambito = std::make_shared<Entorno>(actual_);
                    ambito->definir(n->variable, elem, Visibilidad::Privado, false, false);
                    try {
                        ejecutar_bloque(n->cuerpo, ambito);
                    } catch (const SenalContinuar&) {
                    } catch (const SenalRomper&) {
                        break;
                    }
                }
                return;
            }
            if (it.tipo == TipoValor::Texto) {
                for (char c : it.texto) {
                    auto ambito = std::make_shared<Entorno>(actual_);
                    ambito->definir(n->variable, Valor::de_texto(std::string(1, c)), Visibilidad::Privado, false, false);
                    try {
                        ejecutar_bloque(n->cuerpo, ambito);
                    } catch (const SenalContinuar&) {
                    } catch (const SenalRomper&) {
                        break;
                    }
                }
                return;
            }
            throw ErrorEjecucion("el objeto no es iterable en 'para ... en'", n->ubicacion);
        }

        case TipoNodo::Funcion: {
            const auto* n = static_cast<const NodoFuncion*>(nodo);
            auto fn = std::make_shared<Funcion>();
            fn->nombre = n->nombre;
            fn->nodo = n;
            fn->clausura = actual_;
            Valor v;
            v.tipo = TipoValor::Funcion;
            v.funcion = fn;
            actual_->definir(n->nombre, std::move(v), n->visibilidad, false, false);
            return;
        }

        case TipoNodo::Devolver:
            throw SenalDevolver{evaluar(static_cast<const NodoDevolver*>(nodo)->valor.get())};

        case TipoNodo::Romper:
            throw SenalRomper{};

        case TipoNodo::Continuar:
            throw SenalContinuar{};

        case TipoNodo::Clase: {
            const auto* n = static_cast<const NodoClase*>(nodo);
            auto cls = std::make_shared<Clase>();
            cls->nombre = n->nombre;
            if (!n->padre.empty()) {
                Valor vp;
                if (!actual_->obtener(n->padre, &vp) || vp.tipo != TipoValor::Clase)
                    throw ErrorEjecucion("clase padre no encontrada: " + n->padre, n->ubicacion);
                cls->padre = vp.clase;
                cls->propiedades_defecto = cls->padre->propiedades_defecto;
                cls->visibilidad_propiedades = cls->padre->visibilidad_propiedades;
                cls->metodos = cls->padre->metodos;
                cls->visibilidad_metodos = cls->padre->visibilidad_metodos;
            }
            for (const auto& prop : n->propiedades) {
                cls->propiedades_defecto[prop->nombre] = evaluar(prop->valor.get());
                cls->visibilidad_propiedades[prop->nombre] = prop->visibilidad;
            }
            for (const auto& met : n->metodos) {
                auto fn = std::make_shared<Funcion>();
                fn->nombre = met->nombre;
                fn->nodo = met.get();
                fn->clausura = actual_;
                fn->clase_propietaria = n->nombre;
                cls->metodos[met->nombre] = fn;
                cls->visibilidad_metodos[met->nombre] = met->visibilidad;
            }
            Valor v;
            v.tipo = TipoValor::Clase;
            v.clase = cls;
            actual_->definir(n->nombre, std::move(v), n->visibilidad, false, false);
            return;
        }

        case TipoNodo::Conseguir: {
            const auto* n = static_cast<const NodoConseguir*>(nodo);
            auto mod = cargar_modulo(*n);
            std::string nombre_local = n->alias.empty() ? n->modulo : n->alias;
            auto punto = nombre_local.find_last_of('.');
            if (n->alias.empty() && punto != std::string::npos) nombre_local = nombre_local.substr(punto + 1);
            Valor v;
            v.tipo = TipoValor::Modulo;
            v.modulo = mod;
            actual_->definir(nombre_local, std::move(v), Visibilidad::Publico, true, false);
            return;
        }

        case TipoNodo::Exportar: {
            const auto* n = static_cast<const NodoExportar*>(nodo);
            Simbolo* s = actual_->buscar_local(n->nombre);
            if (!s) throw ErrorEjecucion("no se puede exportar un simbolo inexistente: " + n->nombre, n->ubicacion);
            s->exportado = true;
            s->visibilidad = Visibilidad::Publico;
            return;
        }

        case TipoNodo::Intenta: {
            const auto* n = static_cast<const NodoIntenta*>(nodo);
            try {
                ejecutar_bloque(n->bloque_intenta, std::make_shared<Entorno>(actual_));
            } catch (const ErrorESP& err) {
                auto ambito = std::make_shared<Entorno>(actual_);
                ambito->definir(n->var_error, Valor::de_texto(err.what()), Visibilidad::Privado, false, false);
                ejecutar_bloque(n->bloque_catch, ambito);
            } catch (const std::exception& err) {
                auto ambito = std::make_shared<Entorno>(actual_);
                ambito->definir(n->var_error, Valor::de_texto(err.what()), Visibilidad::Privado, false, false);
                ejecutar_bloque(n->bloque_catch, ambito);
            }
            return;
        }

        // --- EJECUCIÓN NATIVA SQL EN ESPAÑOL ---
        case TipoNodo::SentenciaBaseDatos: {
            const auto* n = static_cast<const NodoBaseDatos*>(nodo);
            g_db_nativa_activa = evaluar(n->ruta.get()).a_texto();
            auto mod_sql = cargar_modulo_estandar("datos.sql");
            if (!mod_sql) throw ErrorEjecucion("motor SQL no disponible", n->ubicacion);
            Valor fn_abrir;
            mod_sql->entorno->obtener("abrir", &fn_abrir);
            std::vector<Valor> args = {Valor::de_texto(g_db_nativa_activa)};
            (void)llamar(fn_abrir, args, n->ubicacion);
            return;
        }

        case TipoNodo::SentenciaCrearTabla: {
            const auto* n = static_cast<const NodoCrearTabla*>(nodo);
            auto mod_sql = cargar_modulo_estandar("datos.sql");
            Valor fn_abrir, fn_cons;
            mod_sql->entorno->obtener("abrir", &fn_abrir);
            mod_sql->entorno->obtener("consultar", &fn_cons);
            std::vector<Valor> a_db = {Valor::de_texto(g_db_nativa_activa)};
            (void)llamar(fn_abrir, a_db, n->ubicacion);

            std::ostringstream sql;
            sql << "CREATE TABLE IF NOT EXISTS " << n->tabla << "(";
            for (std::size_t i = 0; i < n->columnas.size(); ++i) {
                if (i > 0) sql << ", ";
                sql << n->columnas[i];
            }
            sql << ")";
            std::vector<Valor> a_q = {Valor::de_texto(g_db_nativa_activa), Valor::de_texto(sql.str())};
            (void)llamar(fn_cons, a_q, n->ubicacion);
            return;
        }

        case TipoNodo::SentenciaInsertarEn: {
            const auto* n = static_cast<const NodoInsertarEn*>(nodo);
            auto mod_sql = cargar_modulo_estandar("datos.sql");
            Valor fn_abrir, fn_cons;
            mod_sql->entorno->obtener("abrir", &fn_abrir);
            mod_sql->entorno->obtener("consultar", &fn_cons);
            std::vector<Valor> a_db = {Valor::de_texto(g_db_nativa_activa)};
            (void)llamar(fn_abrir, a_db, n->ubicacion);

            std::ostringstream sql;
            sql << "INSERT INTO " << n->tabla << "(";
            for (std::size_t i = 0; i < n->columnas.size(); ++i) {
                if (i > 0) sql << ", ";
                sql << n->columnas[i];
            }
            sql << ") VALUES (";
            for (std::size_t i = 0; i < n->valores.size(); ++i) {
                if (i > 0) sql << ", ";
                sql << escapar_sql_nativo(evaluar(n->valores[i].get()));
            }
            sql << ")";
            std::vector<Valor> a_q = {Valor::de_texto(g_db_nativa_activa), Valor::de_texto(sql.str())};
            (void)llamar(fn_cons, a_q, n->ubicacion);
            return;
        }

        case TipoNodo::SentenciaActualizar: {
            const auto* n = static_cast<const NodoActualizar*>(nodo);
            auto mod_sql = cargar_modulo_estandar("datos.sql");
            Valor fn_cons;
            mod_sql->entorno->obtener("consultar", &fn_cons);
            std::ostringstream sql;
            sql << "UPDATE " << n->tabla << " SET ";
            for (std::size_t i = 0; i < n->asignaciones.size(); ++i) {
                if (i > 0) sql << ", ";
                sql << n->asignaciones[i].first << " = " << escapar_sql_nativo(evaluar(n->asignaciones[i].second.get()));
            }
            if (n->tiene_donde) {
                sql << " WHERE " << n->col_donde << " " << n->op_donde << " "
                    << escapar_sql_nativo(evaluar(n->val_donde.get()));
            }
            std::vector<Valor> a_q = {Valor::de_texto(g_db_nativa_activa), Valor::de_texto(sql.str())};
            (void)llamar(fn_cons, a_q, n->ubicacion);
            return;
        }

        case TipoNodo::SentenciaEliminarDe: {
            const auto* n = static_cast<const NodoEliminarDe*>(nodo);
            auto mod_sql = cargar_modulo_estandar("datos.sql");
            Valor fn_cons;
            mod_sql->entorno->obtener("consultar", &fn_cons);
            std::ostringstream sql;
            sql << "DELETE FROM " << n->tabla;
            if (n->tiene_donde) {
                sql << " WHERE " << n->col_donde << " " << n->op_donde << " "
                    << escapar_sql_nativo(evaluar(n->val_donde.get()));
            }
            std::vector<Valor> a_q = {Valor::de_texto(g_db_nativa_activa), Valor::de_texto(sql.str())};
            (void)llamar(fn_cons, a_q, n->ubicacion);
            return;
        }

        // --- EJECUCIÓN NATIVA WEB EN ESPAÑOL ---
        case TipoNodo::SentenciaPagina: {
            const auto* n = static_cast<const NodoPagina*>(nodo);
            g_web_ctx.titulo = evaluar(n->titulo.get()).a_texto();
            g_web_ctx.css.str("");
            g_web_ctx.css.clear();
            g_web_ctx.html.str("");
            g_web_ctx.html.clear();
            g_web_ctx.js.str("");
            g_web_ctx.js.clear();
            ejecutar_bloque(n->cuerpo, std::make_shared<Entorno>(actual_));
            return;
        }

        case TipoNodo::SentenciaEstilo: {
            const auto* n = static_cast<const NodoEstilo*>(nodo);
            std::string sel = (n->selector == "body" || n->selector == "h1" || n->selector == "p" ||
                               n->selector == "button" || n->selector == "input")
                                  ? n->selector
                                  : ("." + n->selector);
            g_web_ctx.css << "    " << sel << " {\n";
            for (const auto& kv : n->propiedades) {
                g_web_ctx.css << "      " << propiedad_css_esp(kv.first) << ": "
                              << evaluar(kv.second.get()).a_texto() << ";\n";
            }
            g_web_ctx.css << "    }\n";
            return;
        }

        case TipoNodo::SentenciaScriptWeb: {
            const auto* n = static_cast<const NodoScriptWeb*>(nodo);
            g_web_ctx.js << transpilar_js_esp(n->cuerpo);
            return;
        }

        case TipoNodo::SentenciaElementoWeb: {
            const auto* n = static_cast<const NodoElementoWeb*>(nodo);
            if (n->etiqueta == "encabezado") {
                g_web_ctx.html << "    <h1>" << escapar_html_nativo(evaluar(n->contenido.get()).a_texto()) << "</h1>\n";
            } else if (n->etiqueta == "subtitulo") {
                g_web_ctx.html << "    <h2>" << escapar_html_nativo(evaluar(n->contenido.get()).a_texto()) << "</h2>\n";
            } else if (n->etiqueta == "parrafo") {
                g_web_ctx.html << "    <p>" << escapar_html_nativo(evaluar(n->contenido.get()).a_texto()) << "</p>\n";
            } else if (n->etiqueta == "entrada") {
                std::string nom = escapar_html_nativo(evaluar(n->contenido.get()).a_texto());
                g_web_ctx.html << "    <input class=\"esp-entrada\" name=\"" << nom << "\" placeholder=\"" << nom << "\">\n";
            } else if (n->etiqueta == "enlace") {
                std::string txt = escapar_html_nativo(evaluar(n->contenido.get()).a_texto());
                std::string url = n->extra ? escapar_html_nativo(evaluar(n->extra.get()).a_texto()) : "#";
                g_web_ctx.html << "    <a href=\"" << url << "\">" << txt << "</a>\n";
            } else if (n->etiqueta == "imagen") {
                std::string src = escapar_html_nativo(evaluar(n->contenido.get()).a_texto());
                std::string alt = n->extra ? escapar_html_nativo(evaluar(n->extra.get()).a_texto()) : "Imagen ESP";
                g_web_ctx.html << "    <img src=\"" << src << "\" alt=\"" << alt << "\" style=\"max-width:100%;border-radius:10px;\">\n";
            } else if (n->etiqueta == "boton") {
                std::string txt = escapar_html_nativo(evaluar(n->contenido.get()).a_texto());
                if (!n->al_clic.empty()) {
                    std::string js_inline = transpilar_js_esp(n->al_clic);
                    for (char& ch : js_inline) if (ch == '\n') ch = ' ';
                    g_web_ctx.html << "    <button class=\"esp-boton\" onclick='" << js_inline << "'>" << txt << "</button>\n";
                } else {
                    g_web_ctx.html << "    <button class=\"esp-boton\">" << txt << "</button>\n";
                }
            } else if (n->etiqueta == "contenedor") {
                std::string clase = n->contenido ? escapar_html_nativo(evaluar(n->contenido.get()).a_texto()) : "esp-bloque";
                g_web_ctx.html << "    <div class=\"" << clase << "\">\n";
                ejecutar_bloque(n->hijos, std::make_shared<Entorno>(actual_));
                g_web_ctx.html << "    </div>\n";
            }
            return;
        }

        default:
            throw ErrorEjecucion("sentencia no soportada", nodo->ubicacion);
    }
}

Valor Interprete::evaluar(const Nodo* nodo) {
    switch (nodo->tipo) {
        case TipoNodo::LiteralEntero:
            return Valor::de_entero(static_cast<const NodoLiteralEntero*>(nodo)->valor);
        case TipoNodo::LiteralDecimal:
            return Valor::de_decimal(static_cast<const NodoLiteralDecimal*>(nodo)->valor);
        case TipoNodo::LiteralTexto:
            return Valor::de_texto(static_cast<const NodoLiteralTexto*>(nodo)->valor);
        case TipoNodo::LiteralBool:
            return Valor::de_bool(static_cast<const NodoLiteralBool*>(nodo)->valor);
        case TipoNodo::LiteralNulo:
            return Valor::nulo();

        case TipoNodo::ExpresionSeleccionar: {
            const auto* n = static_cast<const NodoSeleccionar*>(nodo);
            auto mod_sql = cargar_modulo_estandar("datos.sql");
            Valor fn_abrir, fn_cons;
            mod_sql->entorno->obtener("abrir", &fn_abrir);
            mod_sql->entorno->obtener("consultar", &fn_cons);
            std::vector<Valor> a_db = {Valor::de_texto(g_db_nativa_activa)};
            (void)llamar(fn_abrir, a_db, n->ubicacion);

            std::ostringstream sql;
            sql << "SELECT ";
            for (std::size_t i = 0; i < n->columnas.size(); ++i) {
                if (i > 0) sql << ", ";
                sql << n->columnas[i];
            }
            sql << " FROM " << n->tabla;
            if (n->tiene_donde) {
                sql << " WHERE " << n->col_donde << " " << n->op_donde << " "
                    << escapar_sql_nativo(evaluar(n->val_donde.get()));
            }
            if (!n->ordenar_col.empty()) {
                sql << " ORDER BY " << n->ordenar_col << (n->descendente ? " DESC" : " ASC");
            }
            if (n->limite >= 0) {
                sql << " LIMIT " << n->limite;
            }
            std::vector<Valor> a_q = {Valor::de_texto(g_db_nativa_activa), Valor::de_texto(sql.str())};
            return llamar(fn_cons, a_q, n->ubicacion);
        }

        case TipoNodo::Identificador: {
            const auto* n = static_cast<const NodoIdentificador*>(nodo);
            Valor v;
            if (!actual_->obtener(n->nombre, &v))
                throw ErrorEjecucion("variable no definida: '" + n->nombre + "'", n->ubicacion);
            return v;
        }

        case TipoNodo::Este: {
            Valor v;
            if (!actual_->obtener("este", &v))
                throw ErrorEjecucion("'este' solo puede usarse dentro de un metodo", nodo->ubicacion);
            return v;
        }

        case TipoNodo::Super: {
            Valor v;
            if (!actual_->obtener("super", &v))
                throw ErrorEjecucion("'super' solo puede usarse dentro de una clase hija", nodo->ubicacion);
            return v;
        }

        case TipoNodo::Lista: {
            const auto* n = static_cast<const NodoLista*>(nodo);
            std::vector<Valor> items;
            items.reserve(n->elementos.size());
            for (const auto& e : n->elementos) items.push_back(evaluar(e.get()));
            return Valor::de_lista(std::move(items));
        }

        case TipoNodo::Mapa: {
            const auto* n = static_cast<const NodoMapa*>(nodo);
            std::map<std::string, Valor> m;
            for (const auto& kv : n->entradas) {
                Valor k = evaluar(kv.first.get());
                m[k.a_texto()] = evaluar(kv.second.get());
            }
            return Valor::de_mapa(std::move(m));
        }

        case TipoNodo::Unario: {
            const auto* n = static_cast<const NodoUnario*>(nodo);
            Valor v = evaluar(n->operando.get());
            if (n->operador == "no") return Valor::de_bool(!v.verdad());
            if (n->operador == "-") {
                if (v.tipo == TipoValor::Entero) return Valor::de_entero(-v.entero);
                if (v.tipo == TipoValor::Decimal) return Valor::de_decimal(-v.decimal);
                throw ErrorEjecucion("negacion aritmetica requiere numero", n->ubicacion);
            }
            throw ErrorEjecucion("operador unario desconocido", n->ubicacion);
        }

        case TipoNodo::Binario: {
            const auto* n = static_cast<const NodoBinario*>(nodo);
            if (n->operador == "y") {
                Valor izq = evaluar(n->izquierda.get());
                if (!izq.verdad()) return Valor::de_bool(false);
                return Valor::de_bool(evaluar(n->derecha.get()).verdad());
            }
            if (n->operador == "o") {
                Valor izq = evaluar(n->izquierda.get());
                if (izq.verdad()) return Valor::de_bool(true);
                return Valor::de_bool(evaluar(n->derecha.get()).verdad());
            }

            Valor izq = evaluar(n->izquierda.get());
            Valor der = evaluar(n->derecha.get());

            if (n->operador == "==") return Valor::de_bool(izq.igual_a(der));
            if (n->operador == "!=") return Valor::de_bool(!izq.igual_a(der));

            if (n->operador == "+" && (izq.tipo == TipoValor::Texto || der.tipo == TipoValor::Texto)) {
                return Valor::de_texto(izq.a_texto() + der.a_texto());
            }

            if (!izq.es_numero() || !der.es_numero()) {
                if (izq.tipo == TipoValor::Texto && der.tipo == TipoValor::Texto) {
                    if (n->operador == "<") return Valor::de_bool(izq.texto < der.texto);
                    if (n->operador == "<=") return Valor::de_bool(izq.texto <= der.texto);
                    if (n->operador == ">") return Valor::de_bool(izq.texto > der.texto);
                    if (n->operador == ">=") return Valor::de_bool(izq.texto >= der.texto);
                }
                throw ErrorEjecucion("operacion '" + n->operador + "' no valida entre " + izq.tipo_texto() + " y " +
                                         der.tipo_texto(),
                                     n->ubicacion);
            }

            bool ambos_enteros = izq.tipo == TipoValor::Entero && der.tipo == TipoValor::Entero;
            double a = izq.numero();
            double b = der.numero();

            if (n->operador == "+") {
                return ambos_enteros ? Valor::de_entero(izq.entero + der.entero) : Valor::de_decimal(a + b);
            }
            if (n->operador == "-") {
                return ambos_enteros ? Valor::de_entero(izq.entero - der.entero) : Valor::de_decimal(a - b);
            }
            if (n->operador == "*") {
                return ambos_enteros ? Valor::de_entero(izq.entero * der.entero) : Valor::de_decimal(a * b);
            }
            if (n->operador == "/") {
                if (b == 0.0) throw ErrorEjecucion("division por cero", n->ubicacion);
                if (ambos_enteros && izq.entero % der.entero == 0) return Valor::de_entero(izq.entero / der.entero);
                return Valor::de_decimal(a / b);
            }
            if (n->operador == "%") {
                if (!ambos_enteros || der.entero == 0)
                    throw ErrorEjecucion("modulo requiere enteros distintos de cero", n->ubicacion);
                return Valor::de_entero(izq.entero % der.entero);
            }
            if (n->operador == "<") return Valor::de_bool(a < b);
            if (n->operador == "<=") return Valor::de_bool(a <= b);
            if (n->operador == ">") return Valor::de_bool(a > b);
            if (n->operador == ">=") return Valor::de_bool(a >= b);
            throw ErrorEjecucion("operador desconocido: " + n->operador, n->ubicacion);
        }

        case TipoNodo::Asignacion: {
            const auto* n = static_cast<const NodoAsignacion*>(nodo);
            Valor v = evaluar(n->valor.get());
            if (!actual_->asignar(n->nombre, v, n->ubicacion))
                throw ErrorEjecucion("no se puede asignar a variable no declarada: '" + n->nombre + "'", n->ubicacion);
            return v;
        }

        case TipoNodo::AsignacionIndice: {
            const auto* n = static_cast<const NodoAsignacionIndice*>(nodo);
            Valor obj = evaluar(n->objeto.get());
            Valor idx = evaluar(n->indice.get());
            Valor val = evaluar(n->valor.get());
            if (obj.tipo == TipoValor::Lista && obj.lista) {
                std::size_t pos = idx_lista(obj, idx, n->ubicacion);
                (*obj.lista)[pos] = val;
                return val;
            }
            if (obj.tipo == TipoValor::Mapa && obj.mapa) {
                (*obj.mapa)[idx.a_texto()] = val;
                return val;
            }
            throw ErrorEjecucion("asignacion por indice no soportada para " + obj.tipo_texto(), n->ubicacion);
        }

        case TipoNodo::AsignacionMiembro: {
            const auto* n = static_cast<const NodoAsignacionMiembro*>(nodo);
            Valor obj = evaluar(n->objeto.get());
            Valor val = evaluar(n->valor.get());
            asignar_miembro(obj, n->miembro, val, n->ubicacion);
            return val;
        }

        case TipoNodo::Indice: {
            const auto* n = static_cast<const NodoIndice*>(nodo);
            Valor obj = evaluar(n->objeto.get());
            Valor idx = evaluar(n->indice.get());
            if (obj.tipo == TipoValor::Lista && obj.lista) {
                return (*obj.lista)[idx_lista(obj, idx, n->ubicacion)];
            }
            if (obj.tipo == TipoValor::Mapa && obj.mapa) {
                auto it = obj.mapa->find(idx.a_texto());
                if (it == obj.mapa->end()) throw ErrorEjecucion("clave no encontrada: " + idx.a_texto(), n->ubicacion);
                return it->second;
            }
            if (obj.tipo == TipoValor::Texto) {
                if (idx.tipo != TipoValor::Entero) throw ErrorEjecucion("indice de texto debe ser entero", n->ubicacion);
                std::int64_t i = idx.entero;
                if (i < 1 || static_cast<std::size_t>(i) > obj.texto.size())
                    throw ErrorEjecucion("indice de texto fuera de rango", n->ubicacion);
                return Valor::de_texto(std::string(1, obj.texto[static_cast<std::size_t>(i - 1)]));
            }
            throw ErrorEjecucion("indexacion no soportada en " + obj.tipo_texto(), n->ubicacion);
        }

        case TipoNodo::Miembro: {
            const auto* n = static_cast<const NodoMiembro*>(nodo);
            Valor obj = evaluar(n->objeto.get());
            return acceder_miembro(obj, n->miembro, n->ubicacion);
        }

        case TipoNodo::Llamada: {
            const auto* n = static_cast<const NodoLlamada*>(nodo);
            Valor callee = evaluar(n->callee.get());
            std::vector<Valor> args;
            args.reserve(n->argumentos.size());
            for (const auto& a : n->argumentos) args.push_back(evaluar(a.get()));
            return llamar(callee, args, n->ubicacion);
        }

        default:
            throw ErrorEjecucion("expresion no evaluable", nodo->ubicacion);
    }
}

bool Interprete::acceso_privado_permitido(const std::string& clase_propietaria) const {
    if (clase_propietaria.empty()) return true;
    std::string actual_cls = nombre_clase_actual(actual_);
    if (actual_cls == clase_propietaria) return true;
    Valor este;
    if (actual_ && actual_->obtener("este", &este) && este.tipo == TipoValor::Objeto && este.objeto) {
        auto cls = este.objeto->clase;
        while (cls) {
            if (cls->nombre == clase_propietaria) return true;
            cls = cls->padre;
        }
    }
    return false;
}

Valor Interprete::acceder_miembro(const Valor& objetivo, const std::string& miembro, Ubicacion u) {
    if (objetivo.tipo == TipoValor::Modulo && objetivo.modulo) {
        const Simbolo* s = objetivo.modulo->entorno->buscar_local(miembro);
        if (!s) throw ErrorEjecucion("el modulo '" + objetivo.modulo->nombre + "' no tiene '" + miembro + "'", u);
        if (s->visibilidad == Visibilidad::Privado || !s->exportado)
            throw ErrorEjecucion("acceso denegado: '" + miembro + "' es privado en el modulo", u);
        return s->valor;
    }

    if (objetivo.tipo == TipoValor::Mapa && objetivo.mapa) {
        auto it = objetivo.mapa->find(miembro);
        if (it == objetivo.mapa->end()) throw ErrorEjecucion("clave no encontrada en mapa: '" + miembro + "'", u);
        return it->second;
    }

    if (objetivo.tipo == TipoValor::Objeto && objetivo.objeto) {
        auto obj = objetivo.objeto;
        auto it_vis_prop = obj->clase->visibilidad_propiedades.find(miembro);
        if (it_vis_prop != obj->clase->visibilidad_propiedades.end() &&
            it_vis_prop->second == Visibilidad::Privado && !acceso_privado_permitido(obj->clase->nombre)) {
            throw ErrorEjecucion("acceso privado denegado a propiedad '" + miembro + "'", u);
        }
        auto it_prop = obj->propiedades.find(miembro);
        if (it_prop != obj->propiedades.end()) return it_prop->second;

        auto it_met = obj->clase->metodos.find(miembro);
        if (it_met != obj->clase->metodos.end()) {
            auto it_vis_met = obj->clase->visibilidad_metodos.find(miembro);
            if (it_vis_met != obj->clase->visibilidad_metodos.end() &&
                it_vis_met->second == Visibilidad::Privado &&
                !acceso_privado_permitido(it_met->second->clase_propietaria)) {
                throw ErrorEjecucion("acceso privado denegado a metodo '" + miembro + "'", u);
            }
            auto enlazada = std::make_shared<Funcion>(*it_met->second);
            enlazada->instancia = obj;
            Valor v;
            v.tipo = TipoValor::Funcion;
            v.funcion = enlazada;
            return v;
        }
        throw ErrorEjecucion("miembro '" + miembro + "' no existe en " + obj->clase->nombre, u);
    }

    throw ErrorEjecucion("no se puede acceder a '" + miembro + "' en " + objetivo.tipo_texto(), u);
}

void Interprete::asignar_miembro(const Valor& objetivo, const std::string& miembro, Valor valor, Ubicacion u) {
    if (objetivo.tipo == TipoValor::Mapa && objetivo.mapa) {
        (*objetivo.mapa)[miembro] = std::move(valor);
        return;
    }
    if (objetivo.tipo == TipoValor::Objeto && objetivo.objeto) {
        auto obj = objetivo.objeto;
        auto it_vis = obj->clase->visibilidad_propiedades.find(miembro);
        if (it_vis != obj->clase->visibilidad_propiedades.end() &&
            it_vis->second == Visibilidad::Privado && !acceso_privado_permitido(obj->clase->nombre)) {
            throw ErrorEjecucion("no se puede modificar la propiedad privada '" + miembro + "'", u);
        }
        obj->propiedades[miembro] = std::move(valor);
        return;
    }
    throw ErrorEjecucion("asignacion de miembro no soportada en " + objetivo.tipo_texto(), u);
}

Valor Interprete::llamar(const Valor& callee, std::vector<Valor>& args, Ubicacion u) {
    if (callee.tipo == TipoValor::Clase && callee.clase) {
        auto instancia = std::make_shared<Objeto>();
        instancia->clase = callee.clase;
        for (const auto& kv : callee.clase->propiedades_defecto) {
            instancia->propiedades.emplace(kv.first, clonar_valor_de_instancia(kv.second));
        }
        Valor obj_val;
        obj_val.tipo = TipoValor::Objeto;
        obj_val.objeto = instancia;

        auto it_init = callee.clase->metodos.find("iniciar");
        if (it_init != callee.clase->metodos.end()) {
            auto init = std::make_shared<Funcion>(*it_init->second);
            init->instancia = instancia;
            Valor fn_val;
            fn_val.tipo = TipoValor::Funcion;
            fn_val.funcion = init;
            (void)llamar(fn_val, args, u);
        } else if (!args.empty()) {
            throw ErrorEjecucion("la clase '" + callee.clase->nombre + "' no define iniciar(...) para argumentos", u);
        }
        return obj_val;
    }

    if (callee.tipo != TipoValor::Funcion || !callee.funcion)
        throw ErrorEjecucion("el valor de tipo '" + callee.tipo_texto() + "' no es invocable", u);

    const auto& fn = callee.funcion;
    if (fn->es_nativa) return fn->nativa(args, u);

    const auto* nodo = fn->nodo;
    auto ambito = std::make_shared<Entorno>(fn->clausura);
    if (fn->instancia) {
        Valor este;
        este.tipo = TipoValor::Objeto;
        este.objeto = fn->instancia;
        ambito->definir("este", std::move(este), Visibilidad::Publico, true, false);
        if (fn->instancia->clase->padre) {
            auto padre_obj = std::make_shared<Objeto>(*fn->instancia);
            padre_obj->clase = fn->instancia->clase->padre;
            Valor sup;
            sup.tipo = TipoValor::Objeto;
            sup.objeto = padre_obj;
            ambito->definir("super", std::move(sup), Visibilidad::Publico, true, false);
        }
    }

    if (args.size() > nodo->parametros.size())
        throw ErrorEjecucion("demasiados argumentos para '" + fn->nombre + "'", u);

    for (std::size_t i = 0; i < nodo->parametros.size(); ++i) {
        const auto& param = nodo->parametros[i];
        Valor val;
        if (i < args.size()) {
            val = args[i];
        } else if (param.defecto) {
            val = evaluar(param.defecto.get());
        } else {
            throw ErrorEjecucion("falta argumento obligatorio '" + param.nombre + "' en '" + fn->nombre + "'", u);
        }
        ambito->definir(param.nombre, std::move(val), Visibilidad::Privado, false, false);
    }

    try {
        ejecutar_bloque(nodo->cuerpo, ambito);
    } catch (const SenalDevolver& ret) {
        return ret.valor;
    }
    return Valor::nulo();
}

std::shared_ptr<Modulo> Interprete::cargar_modulo(const NodoConseguir& req) {
    const std::string clave_cache =
        req.modulo + "|" + req.version + "|" + req.origen + "|" + req.sha256 + "|" + directorio_trabajo;
    auto it = modulos_cargados_.find(clave_cache);
    if (it != modulos_cargados_.end()) return it->second;

    if (modulos_cargando_.count(clave_cache))
        throw ErrorEjecucion("dependencia circular detectada al cargar '" + req.modulo + "'", req.ubicacion);

    if (req.origen.empty() && req.version.empty() && req.sha256.empty()) {
        if (auto estandar = cargar_modulo_estandar(req.modulo)) {
            modulos_cargados_[clave_cache] = estandar;
            return estandar;
        }
    }

    if (!req.version.empty() && !es_version_exacta(req.version)) {
        throw ErrorEjecucion("la version de '" + req.modulo + "' debe ser exacta (X.Y.Z)", req.ubicacion);
    }

    std::filesystem::path ruta;
    bool desde_red = false;
    if (!req.origen.empty() && (req.origen.rfind("https://", 0) == 0 || req.origen.rfind("http://", 0) == 0)) {
        desde_red = true;
    } else if (!req.origen.empty()) {
        ruta = std::filesystem::path(req.origen);
        if (ruta.is_relative()) ruta = std::filesystem::path(directorio_trabajo) / ruta;
    } else {
        std::filesystem::path local = std::filesystem::path(directorio_trabajo) / (req.modulo + EXTENSION);
        std::filesystem::path paquete =
            std::filesystem::path(directorio_home_esp()) / "paquetes" / req.modulo / (req.modulo + EXTENSION);
        std::error_code ec;
        if (std::filesystem::exists(local, ec) && !ec) {
            ruta = local;
        } else if (std::filesystem::exists(paquete, ec) && !ec) {
            ruta = paquete;
        } else {
            desde_red = true;
        }
    }

    std::string codigo;
    std::string origen_real;
    if (desde_red) {
        if (req.sha256.empty()) {
            throw ErrorEjecucion("los modulos remotos requieren sha256='...' por seguridad: " + req.modulo,
                                 req.ubicacion);
        }
        const std::string url = url_paquete_remoto(req.modulo, req.version, req.origen);
        if (url.empty()) throw ErrorEjecucion("nombre de modulo remoto no valido: " + req.modulo, req.ubicacion);
        std::string stderr_curl;
        int rc = ejecutar_proceso("curl --silent --show-error --fail --location --max-time 30 " +
                                      citar_argumento_proceso(url),
                                  "", &codigo, &stderr_curl, 35000);
        if (rc != 0) {
            throw ErrorEjecucion("no se pudo descargar el modulo '" + req.modulo + "' desde " + url +
                                     (stderr_curl.empty() ? "" : ": " + stderr_curl),
                                 req.ubicacion);
        }
        origen_real = url;
    } else {
        std::error_code ec;
        if (!std::filesystem::exists(ruta, ec) || ec)
            throw ErrorEjecucion("modulo no encontrado: " + req.modulo, req.ubicacion);
        codigo = leer_archivo(ruta.string());
        origen_real = ruta.string();
    }

    if (!req.sha256.empty()) {
        std::string actual_hash = sha256_hex(codigo);
        std::string esperado = req.sha256;
        for (char& c : esperado) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (actual_hash != esperado)
            throw ErrorEjecucion("SHA-256 invalido para modulo '" + req.modulo + "'", req.ubicacion);
    }

    modulos_cargando_.insert(clave_cache);
    auto mod = std::make_shared<Modulo>();
    mod->nombre = req.modulo;
    mod->version = req.version.empty() ? "1.0.0" : req.version;
    mod->entorno = std::make_shared<Entorno>(global_);

    auto env_anterior = actual_;
    auto dir_anterior = directorio_trabajo;
    actual_ = mod->entorno;
    if (!desde_red) directorio_trabajo = ruta.parent_path().string();
    try {
        Lexer lexer(codigo, origen_real);
        Parser parser(lexer.analizar());
        auto ast = parser.analizar();
        (void)ejecutar_programa(ast.get());
    } catch (...) {
        actual_ = env_anterior;
        directorio_trabajo = dir_anterior;
        modulos_cargando_.erase(clave_cache);
        throw;
    }
    actual_ = env_anterior;
    directorio_trabajo = dir_anterior;
    modulos_cargando_.erase(clave_cache);

    modulos_cargados_[clave_cache] = mod;
    return mod;
}

}  // namespace esp