#include "esp/codegen.hpp"
#include "esp/lexer.hpp"
#include "esp/parser.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

#ifdef ESP_TIENE_LLVM_API
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#endif

namespace esp {
namespace {

bool comando_existe(const std::string& cmd) {
#ifdef _WIN32
    std::string q = "where " + cmd + " >nul 2>nul";
#else
    std::string q = "command -v " + cmd + " >/dev/null 2>&1";
#endif
    return std::system(q.c_str()) == 0;
}

std::string escapar_cpp(const std::string& s) {
    std::string out = "\"";
    for (unsigned char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out.push_back(static_cast<char>(c));
    }
    out += "\"";
    return out;
}

std::string traducir_css_aot(const std::string& k) {
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

std::string ir_a_objeto_con_llvm_api(const std::string& ruta_ll, const std::string& ruta_obj,
                                     const std::string& nivel, std::string* detalle_error) {
#ifndef ESP_TIENE_LLVM_API
    (void)ruta_ll;
    (void)ruta_obj;
    (void)nivel;
    if (detalle_error) *detalle_error = "ESP fue compilado sin las bibliotecas C++ de LLVM.";
    return "";
#else
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    llvm::LLVMContext contexto;
    llvm::SMDiagnostic diag;
    auto modulo = llvm::parseIRFile(ruta_ll, diag, contexto);
    if (!modulo) {
        if (detalle_error) *detalle_error = "LLVM IR invalido: " + diag.getMessage().str();
        return "";
    }
    if (llvm::verifyModule(*modulo, &llvm::errs())) {
        if (detalle_error) *detalle_error = "La verificacion del modulo LLVM fallo.";
        return "";
    }

    const std::string triple = llvm::sys::getDefaultTargetTriple();
    modulo->setTargetTriple(triple);

    std::string err_target;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, err_target);
    if (!target) {
        if (detalle_error) *detalle_error = "No se encontro target LLVM para " + triple + ": " + err_target;
        return "";
    }

    llvm::TargetOptions opt;
    auto reloc = std::optional<llvm::Reloc::Model>(llvm::Reloc::PIC_);
    auto maquina = std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(triple, "generic", "", opt, reloc));
    if (!maquina) {
        if (detalle_error) *detalle_error = "No se pudo crear TargetMachine LLVM.";
        return "";
    }
    modulo->setDataLayout(maquina->createDataLayout());

    llvm::CodeGenOptLevel opt_level = llvm::CodeGenOptLevel::Default;
    if (nivel == "O0") opt_level = llvm::CodeGenOptLevel::None;
    else if (nivel == "O1") opt_level = llvm::CodeGenOptLevel::Less;
    else if (nivel == "O3") opt_level = llvm::CodeGenOptLevel::Aggressive;
    maquina->setOptLevel(opt_level);

    std::error_code ec;
    llvm::raw_fd_ostream dest(ruta_obj, ec, llvm::sys::fs::OF_None);
    if (ec) {
        if (detalle_error) *detalle_error = "No se pudo abrir archivo objeto: " + ec.message();
        return "";
    }

    llvm::legacy::PassManager pm;
    if (maquina->addPassesToEmitFile(pm, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        if (detalle_error) *detalle_error = "El backend LLVM no puede emitir archivos objeto.";
        return "";
    }
    pm.run(*modulo);
    dest.flush();
    return ruta_obj;
#endif
}

struct GeneradorCpp {
    std::ostringstream out;
    int indent = 0;
    std::set<std::string> variables_declaradas;

    void linea(const std::string& s = "") {
        for (int i = 0; i < indent; ++i) out << "    ";
        out << s << "\n";
    }

    std::string transpilar_js_aot(const std::vector<std::unique_ptr<Nodo>>& sents) {
        std::ostringstream js;
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
                        js << fn_nom << "(";
                        for (std::size_t i = 0; i < ll->argumentos.size(); ++i) {
                            if (i > 0) js << ", ";
                            if (ll->argumentos[i]->tipo == TipoNodo::LiteralTexto) {
                                js << escapar_cpp(static_cast<NodoLiteralTexto*>(ll->argumentos[i].get())->valor);
                            } else if (ll->argumentos[i]->tipo == TipoNodo::Identificador) {
                                js << static_cast<NodoIdentificador*>(ll->argumentos[i].get())->nombre;
                            } else {
                                js << "0";
                            }
                        }
                        js << "); ";
                    }
                }
            }
        }
        return js.str();
    }

    std::string expr(const Nodo* n) {
        if (!n) return "Val::nulo()";
        switch (n->tipo) {
            case TipoNodo::LiteralEntero:
                return "Val::entero(" + std::to_string(static_cast<const NodoLiteralEntero*>(n)->valor) + "LL)";
            case TipoNodo::LiteralDecimal:
                return "Val::decimal(" + std::to_string(static_cast<const NodoLiteralDecimal*>(n)->valor) + ")";
            case TipoNodo::LiteralTexto:
                return "Val::texto(" + escapar_cpp(static_cast<const NodoLiteralTexto*>(n)->valor) + ")";
            case TipoNodo::LiteralBool:
                return std::string("Val::booleano(") +
                       (static_cast<const NodoLiteralBool*>(n)->valor ? "true)" : "false)");
            case TipoNodo::LiteralNulo:
                return "Val::nulo()";
            case TipoNodo::Este:
                return "v_este";
            case TipoNodo::Super:
                return "v_super";
            case TipoNodo::Identificador: {
                const auto& nom = static_cast<const NodoIdentificador*>(n)->nombre;
                if (nom == "mostrar" || nom == "imprimir") return "esp_mostrar";
                if (nom == "longitud") return "esp_longitud";
                if (nom == "texto") return "esp_texto";
                if (nom == "entero") return "esp_entero";
                if (nom == "decimal") return "esp_decimal";
                if (nom == "agregar") return "esp_agregar";
                if (nom == "afirmar") return "esp_afirmar";
                if (nom == "guardar_web") return "esp_guardar_web";
                if (nom == "html_web") return "esp_html_web";
                if (nom == "servir_web") return "esp_servir_web";
                return "v_" + nom;
            }
            case TipoNodo::ExpresionSeleccionar: {
                const auto* sel = static_cast<const NodoSeleccionar*>(n);
                std::string cols = "{";
                for (std::size_t i = 0; i < sel->columnas.size(); ++i) {
                    if (i) cols += ", ";
                    cols += escapar_cpp(sel->columnas[i]);
                }
                cols += "}";
                std::string tiene_donde = sel->tiene_donde ? "true" : "false";
                std::string col_d = escapar_cpp(sel->col_donde);
                std::string op_d = escapar_cpp(sel->op_donde);
                std::string val_d = sel->tiene_donde ? expr(sel->val_donde.get()) : "Val::nulo()";
                std::string ord_c = escapar_cpp(sel->ordenar_col);
                std::string desc = sel->descendente ? "true" : "false";
                std::string lim = std::to_string(sel->limite);
                return "esp_sql_seleccionar(" + escapar_cpp(sel->tabla) + ", " + cols + ", " +
                       tiene_donde + ", " + col_d + ", " + op_d + ", " + val_d + ", " +
                       ord_c + ", " + desc + ", " + lim + ")";
            }
            case TipoNodo::Lista: {
                const auto* l = static_cast<const NodoLista*>(n);
                std::string s = "Val::lista_de({";
                for (std::size_t i = 0; i < l->elementos.size(); ++i) {
                    if (i) s += ", ";
                    s += expr(l->elementos[i].get());
                }
                s += "})";
                return s;
            }
            case TipoNodo::Mapa: {
                const auto* m = static_cast<const NodoMapa*>(n);
                std::string s = "Val::mapa_de({";
                for (std::size_t i = 0; i < m->entradas.size(); ++i) {
                    if (i) s += ", ";
                    s += "{" + expr(m->entradas[i].first.get()) + ".a_texto(), " +
                         expr(m->entradas[i].second.get()) + "}";
                }
                s += "})";
                return s;
            }
            case TipoNodo::Unario: {
                const auto* u = static_cast<const NodoUnario*>(n);
                if (u->operador == "-") return "esp_neg(" + expr(u->operando.get()) + ")";
                return "Val::booleano(!" + expr(u->operando.get()) + ".verdad())";
            }
            case TipoNodo::Binario: {
                const auto* b = static_cast<const NodoBinario*>(n);
                std::string izq = expr(b->izquierda.get());
                std::string der = expr(b->derecha.get());
                if (b->operador == "+") return "esp_sumar(" + izq + ", " + der + ")";
                if (b->operador == "-") return "esp_restar(" + izq + ", " + der + ")";
                if (b->operador == "*") return "esp_mul(" + izq + ", " + der + ")";
                if (b->operador == "/") return "esp_div(" + izq + ", " + der + ")";
                if (b->operador == "%") return "esp_mod(" + izq + ", " + der + ")";
                if (b->operador == "==") return "Val::booleano(" + izq + ".igual(" + der + "))";
                if (b->operador == "!=") return "Val::booleano(!" + izq + ".igual(" + der + "))";
                if (b->operador == "<") return "esp_menor(" + izq + ", " + der + ")";
                if (b->operador == "<=") return "esp_menor_ig(" + izq + ", " + der + ")";
                if (b->operador == ">") return "esp_mayor(" + izq + ", " + der + ")";
                if (b->operador == ">=") return "esp_mayor_ig(" + izq + ", " + der + ")";
                if (b->operador == "y") return "Val::booleano(" + izq + ".verdad() && " + der + ".verdad())";
                if (b->operador == "o") return "Val::booleano(" + izq + ".verdad() || " + der + ".verdad())";
                return "Val::nulo()";
            }
            case TipoNodo::Asignacion: {
                const auto* a = static_cast<const NodoAsignacion*>(n);
                return "(v_" + a->nombre + " = " + expr(a->valor.get()) + ")";
            }
            case TipoNodo::AsignacionIndice: {
                const auto* a = static_cast<const NodoAsignacionIndice*>(n);
                return "esp_asignar_indice(" + expr(a->objeto.get()) + ", " + expr(a->indice.get()) + ", " +
                       expr(a->valor.get()) + ")";
            }
            case TipoNodo::AsignacionMiembro: {
                const auto* a = static_cast<const NodoAsignacionMiembro*>(n);
                return "esp_asignar_miembro(" + expr(a->objeto.get()) + ", " + escapar_cpp(a->miembro) + ", " +
                       expr(a->valor.get()) + ")";
            }
            case TipoNodo::Indice: {
                const auto* idx = static_cast<const NodoIndice*>(n);
                return "esp_indice(" + expr(idx->objeto.get()) + ", " + expr(idx->indice.get()) + ")";
            }
            case TipoNodo::Miembro: {
                const auto* m = static_cast<const NodoMiembro*>(n);
                return "esp_miembro(" + expr(m->objeto.get()) + ", " + escapar_cpp(m->miembro) + ")";
            }
            case TipoNodo::Llamada: {
                const auto* ll = static_cast<const NodoLlamada*>(n);
                if (ll->callee->tipo == TipoNodo::Miembro) {
                    const auto* mem = static_cast<const NodoMiembro*>(ll->callee.get());
                    std::string args_s = "{";
                    for (std::size_t i = 0; i < ll->argumentos.size(); ++i) {
                        if (i) args_s += ", ";
                        args_s += expr(ll->argumentos[i].get());
                    }
                    args_s += "}";
                    return "esp_llamar_metodo(" + expr(mem->objeto.get()) + ", " + escapar_cpp(mem->miembro) + ", " + args_s + ")";
                }
                std::string s = expr(ll->callee.get()) + ".invocar({";
                for (std::size_t i = 0; i < ll->argumentos.size(); ++i) {
                    if (i) s += ", ";
                    s += expr(ll->argumentos[i].get());
                }
                s += "})";
                return s;
            }
            default:
                return "Val::nulo()";
        }
    }

    void sentencia(const Nodo* n) {
        if (!n) return;
        switch (n->tipo) {
            case TipoNodo::ExpresionSentencia:
                linea("(void)" + expr(static_cast<const NodoExpresionSentencia*>(n)->expresion.get()) + ";");
                break;

            case TipoNodo::DeclaracionVar: {
                const auto* d = static_cast<const NodoDeclaracionVar*>(n);
                if (variables_declaradas.insert(d->nombre).second) {
                    linea("Val v_" + d->nombre + " = " + expr(d->valor.get()) + ";");
                } else {
                    linea("v_" + d->nombre + " = " + expr(d->valor.get()) + ";");
                }
                break;
            }

            case TipoNodo::Conseguir: {
                const auto* c = static_cast<const NodoConseguir*>(n);
                std::string nom_local = c->alias.empty() ? c->modulo : c->alias;
                auto p = nom_local.find_last_of('.');
                if (c->alias.empty() && p != std::string::npos) nom_local = nom_local.substr(p + 1);
                if (variables_declaradas.insert(nom_local).second) {
                    linea("Val v_" + nom_local + " = esp_cargar_modulo_aot(" + escapar_cpp(c->modulo) + ");");
                } else {
                    linea("v_" + nom_local + " = esp_cargar_modulo_aot(" + escapar_cpp(c->modulo) + ");");
                }
                break;
            }

            case TipoNodo::Clase: {
                const auto* cls = static_cast<const NodoClase*>(n);
                linea("Val v_" + cls->nombre + " = Val::clase_de(" + escapar_cpp(cls->nombre) + ", " +
                      escapar_cpp(cls->padre) + ", [](Val& self) {");
                ++indent;
                if (!cls->padre.empty()) {
                    linea("if (g_clases_aot.count(" + escapar_cpp(cls->padre) + ")) g_clases_aot[" +
                          escapar_cpp(cls->padre) + "].init_props(self);");
                }
                for (const auto& prop : cls->propiedades) {
                    linea("(*self.mapa)[" + escapar_cpp(prop->nombre) + "] = " + expr(prop->valor.get()) + ";");
                }
                --indent;
                linea("});");
                for (const auto& met : cls->metodos) {
                    linea("g_clases_aot[" + escapar_cpp(cls->nombre) + "].metodos[" + escapar_cpp(met->nombre) +
                          "] = [&](Val v_este, std::vector<Val> args) -> Val {");
                    ++indent;
                    linea("Val v_super = v_este;");
                    for (std::size_t i = 0; i < met->parametros.size(); ++i) {
                        const auto& p = met->parametros[i];
                        std::string def = p.defecto ? expr(p.defecto.get()) : "Val::nulo()";
                        linea("Val v_" + p.nombre + " = args.size() > " + std::to_string(i) + " ? args[" +
                              std::to_string(i) + "] : " + def + ";");
                    }
                    for (const auto& st : met->cuerpo) sentencia(st.get());
                    linea("return Val::nulo();");
                    --indent;
                    linea("};");
                }
                break;
            }

            case TipoNodo::Si: {
                const auto* s = static_cast<const NodoSi*>(n);
                for (std::size_t i = 0; i < s->ramas.size(); ++i) {
                    linea(std::string(i == 0 ? "if (" : "} else if (") + expr(s->ramas[i].condicion.get()) +
                          ".verdad()) {");
                    ++indent;
                    for (const auto& st : s->ramas[i].cuerpo) sentencia(st.get());
                    --indent;
                }
                if (!s->sino.empty()) {
                    linea("} else {");
                    ++indent;
                    for (const auto& st : s->sino) sentencia(st.get());
                    --indent;
                }
                linea("}");
                break;
            }

            case TipoNodo::Mientras: {
                const auto* m = static_cast<const NodoMientras*>(n);
                linea("while (" + expr(m->condicion.get()) + ".verdad()) {");
                ++indent;
                for (const auto& st : m->cuerpo) sentencia(st.get());
                --indent;
                linea("}");
                break;
            }

            case TipoNodo::ParaRango: {
                const auto* p = static_cast<const NodoParaRango*>(n);
                linea("{");
                ++indent;
                linea("double ini = " + expr(p->inicio.get()) + ".num();");
                linea("double fin = " + expr(p->fin.get()) + ".num();");
                linea("double paso = " + (p->paso ? expr(p->paso.get()) + ".num()" : "1.0") + ";");
                linea("for (double idx = ini; (paso > 0 ? idx <= fin : idx >= fin); idx += paso) {");
                ++indent;
                linea("Val v_" + p->variable + " = Val::entero((long long)idx);");
                for (const auto& st : p->cuerpo) sentencia(st.get());
                --indent;
                linea("}");
                --indent;
                linea("}");
                break;
            }

            case TipoNodo::ParaEn: {
                const auto* p = static_cast<const NodoParaEn*>(n);
                linea("{");
                ++indent;
                linea("Val it = " + expr(p->iterable.get()) + ";");
                linea("if (it.lista) {");
                ++indent;
                linea("for (const auto& item : *it.lista) {");
                ++indent;
                linea("Val v_" + p->variable + " = item;");
                for (const auto& st : p->cuerpo) sentencia(st.get());
                --indent;
                linea("}");
                --indent;
                linea("}");
                --indent;
                linea("}");
                break;
            }

            case TipoNodo::Funcion: {
                const auto* f = static_cast<const NodoFuncion*>(n);
                linea("Val v_" + f->nombre + " = Val::funcion([&](std::vector<Val> args) -> Val {");
                ++indent;
                for (std::size_t i = 0; i < f->parametros.size(); ++i) {
                    const auto& p = f->parametros[i];
                    std::string def = p.defecto ? expr(p.defecto.get()) : "Val::nulo()";
                    linea("Val v_" + p.nombre + " = args.size() > " + std::to_string(i) + " ? args[" +
                          std::to_string(i) + "] : " + def + ";");
                }
                for (const auto& st : f->cuerpo) sentencia(st.get());
                linea("return Val::nulo();");
                --indent;
                linea("});");
                break;
            }

            case TipoNodo::Devolver:
                linea("return " + expr(static_cast<const NodoDevolver*>(n)->valor.get()) + ";");
                break;

            case TipoNodo::Romper:
                linea("break;");
                break;

            case TipoNodo::Continuar:
                linea("continue;");
                break;

            case TipoNodo::Intenta: {
                const auto* t = static_cast<const NodoIntenta*>(n);
                linea("try {");
                ++indent;
                for (const auto& st : t->bloque_intenta) sentencia(st.get());
                --indent;
                linea("} catch (const std::exception& err_nativo) {");
                ++indent;
                linea("Val v_" + t->var_error + " = Val::texto(err_nativo.what());");
                for (const auto& st : t->bloque_catch) sentencia(st.get());
                --indent;
                linea("}");
                break;
            }

            // --- SQL NATIVO EN AOT ---
            case TipoNodo::SentenciaBaseDatos: {
                const auto* bd = static_cast<const NodoBaseDatos*>(n);
                linea("esp_sql_abrir(" + expr(bd->ruta.get()) + ".a_texto());");
                break;
            }
            case TipoNodo::SentenciaCrearTabla: {
                const auto* ct = static_cast<const NodoCrearTabla*>(n);
                std::string cols_vec = "{";
                for (std::size_t i = 0; i < ct->columnas.size(); ++i) {
                    if (i) cols_vec += ", ";
                    cols_vec += escapar_cpp(ct->columnas[i]);
                }
                cols_vec += "}";
                linea("esp_sql_crear_tabla(" + escapar_cpp(ct->tabla) + ", " + cols_vec + ");");
                break;
            }
            case TipoNodo::SentenciaInsertarEn: {
                const auto* ins = static_cast<const NodoInsertarEn*>(n);
                std::string cols_vec = "{";
                for (std::size_t i = 0; i < ins->columnas.size(); ++i) {
                    if (i) cols_vec += ", ";
                    cols_vec += escapar_cpp(ins->columnas[i]);
                }
                cols_vec += "}";
                std::string vals_vec = "{";
                for (std::size_t i = 0; i < ins->valores.size(); ++i) {
                    if (i) vals_vec += ", ";
                    vals_vec += expr(ins->valores[i].get());
                }
                vals_vec += "}";
                linea("esp_sql_insertar(" + escapar_cpp(ins->tabla) + ", " + cols_vec + ", " + vals_vec + ");");
                break;
            }
            case TipoNodo::SentenciaActualizar: {
                const auto* upd = static_cast<const NodoActualizar*>(n);
                std::string asigs = "{";
                for (std::size_t i = 0; i < upd->asignaciones.size(); ++i) {
                    if (i) asigs += ", ";
                    asigs += "{" + escapar_cpp(upd->asignaciones[i].first) + ", " +
                             expr(upd->asignaciones[i].second.get()) + "}";
                }
                asigs += "}";
                std::string td = upd->tiene_donde ? "true" : "false";
                std::string vd = upd->tiene_donde ? expr(upd->val_donde.get()) : "Val::nulo()";
                linea("esp_sql_actualizar(" + escapar_cpp(upd->tabla) + ", " + asigs + ", " + td + ", " +
                      escapar_cpp(upd->col_donde) + ", " + escapar_cpp(upd->op_donde) + ", " + vd + ");");
                break;
            }
            case TipoNodo::SentenciaEliminarDe: {
                const auto* del = static_cast<const NodoEliminarDe*>(n);
                std::string td = del->tiene_donde ? "true" : "false";
                std::string vd = del->tiene_donde ? expr(del->val_donde.get()) : "Val::nulo()";
                linea("esp_sql_eliminar(" + escapar_cpp(del->tabla) + ", " + td + ", " +
                      escapar_cpp(del->col_donde) + ", " + escapar_cpp(del->op_donde) + ", " + vd + ");");
                break;
            }

            // --- WEB NATIVA EN AOT ---
            case TipoNodo::SentenciaPagina: {
                const auto* pag = static_cast<const NodoPagina*>(n);
                linea("g_web_titulo = " + expr(pag->titulo.get()) + ".a_texto();");
                linea("g_web_css.clear(); g_web_html.clear(); g_web_js.clear();");
                for (const auto& st : pag->cuerpo) sentencia(st.get());
                break;
            }
            case TipoNodo::SentenciaEstilo: {
                const auto* est = static_cast<const NodoEstilo*>(n);
                std::string sel = (est->selector == "body" || est->selector == "h1" || est->selector == "p")
                                      ? est->selector
                                      : ("." + est->selector);
                linea("g_web_css += " + escapar_cpp(sel + " {\n") + ";");
                for (const auto& kv : est->propiedades) {
                    linea("g_web_css += " + escapar_cpp("  " + traducir_css_aot(kv.first) + ": ") + " + " +
                          expr(kv.second.get()) + ".a_texto() + \";\\n\";");
                }
                linea("g_web_css += \"}\\n\";");
                break;
            }
            case TipoNodo::SentenciaScriptWeb: {
                const auto* scr = static_cast<const NodoScriptWeb*>(n);
                linea("g_web_js += " + escapar_cpp(transpilar_js_aot(scr->cuerpo)) + ";");
                break;
            }
            case TipoNodo::SentenciaElementoWeb: {
                const auto* el = static_cast<const NodoElementoWeb*>(n);
                if (el->etiqueta == "encabezado") {
                    linea("g_web_html += \"<h1>\" + " + expr(el->contenido.get()) + ".a_texto() + \"</h1>\\n\";");
                } else if (el->etiqueta == "subtitulo") {
                    linea("g_web_html += \"<h2>\" + " + expr(el->contenido.get()) + ".a_texto() + \"</h2>\\n\";");
                } else if (el->etiqueta == "parrafo") {
                    linea("g_web_html += \"<p>\" + " + expr(el->contenido.get()) + ".a_texto() + \"</p>\\n\";");
                } else if (el->etiqueta == "entrada") {
                    linea("{ std::string n = " + expr(el->contenido.get()) +
                          ".a_texto(); g_web_html += \"<input class='esp-entrada' name='\" + n + \"' placeholder='\" + n + \"'>\\n\"; }");
                } else if (el->etiqueta == "boton") {
                    std::string js_clic = transpilar_js_aot(el->al_clic);
                    linea("g_web_html += \"<button class='esp-boton' onclick=" + escapar_cpp(js_clic) + ">\" + " +
                          expr(el->contenido.get()) + ".a_texto() + \"</button>\\n\";");
                } else if (el->etiqueta == "contenedor") {
                    std::string cls = el->contenido ? (expr(el->contenido.get()) + ".a_texto()") : "\"esp-bloque\"";
                    linea("g_web_html += \"<div class='\" + " + cls + " + \"'>\\n\";");
                    for (const auto& h : el->hijos) sentencia(h.get());
                    linea("g_web_html += \"</div>\\n\";");
                }
                break;
            }

            default:
                break;
        }
    }

    std::string generar(const NodoPrograma* prog, bool es_biblioteca) {
        out << R"(#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Val {
    enum Tipo { NULO, BOOL, INT, DEC, TXT, LISTA, MAPA, FN, CLASE } tipo = NULO;
    bool b = false;
    long long i = 0;
    double d = 0.0;
    std::string s;
    std::shared_ptr<std::vector<Val>> lista;
    std::shared_ptr<std::map<std::string, Val>> mapa;
    std::function<Val(std::vector<Val>)> fn;

    static Val nulo() { return {}; }
    static Val booleano(bool v) { Val x; x.tipo = BOOL; x.b = v; return x; }
    static Val entero(long long v) { Val x; x.tipo = INT; x.i = v; return x; }
    static Val decimal(double v) { Val x; x.tipo = DEC; x.d = v; return x; }
    static Val texto(std::string v) { Val x; x.tipo = TXT; x.s = std::move(v); return x; }
    static Val lista_de(std::vector<Val> v) { Val x; x.tipo = LISTA; x.lista = std::make_shared<std::vector<Val>>(std::move(v)); return x; }
    static Val mapa_de(std::map<std::string, Val> v) { Val x; x.tipo = MAPA; x.mapa = std::make_shared<std::map<std::string, Val>>(std::move(v)); return x; }
    static Val funcion(std::function<Val(std::vector<Val>)> f) { Val x; x.tipo = FN; x.fn = std::move(f); return x; }
    static Val clase_de(const std::string& nom, const std::string& padre, std::function<void(Val&)> init_p);

    Val invocar(std::vector<Val> args) const {
        if (tipo == FN && fn) return fn(std::move(args));
        throw std::runtime_error("valor no invocable en AOT");
    }

    bool verdad() const {
        if (tipo == NULO) return false;
        if (tipo == BOOL) return b;
        if (tipo == INT) return i != 0;
        if (tipo == DEC) return d != 0.0;
        if (tipo == TXT) return !s.empty();
        if (tipo == LISTA) return lista && !lista->empty();
        if (tipo == MAPA) return mapa && !mapa->empty();
        return true;
    }

    double num() const {
        if (tipo == INT) return (double)i;
        if (tipo == DEC) return d;
        if (tipo == TXT) return std::stod(s);
        throw std::runtime_error("se esperaba numero");
    }

    bool igual(const Val& o) const {
        if (tipo != o.tipo) return false;
        if (tipo == NULO) return true;
        if (tipo == BOOL) return b == o.b;
        if (tipo == INT) return i == o.i;
        if (tipo == DEC) return d == o.d;
        if (tipo == TXT) return s == o.s;
        return false;
    }

    std::string a_texto() const {
        if (tipo == NULO) return "nulo";
        if (tipo == BOOL) return b ? "verdadero" : "falso";
        if (tipo == INT) return std::to_string(i);
        if (tipo == DEC) { std::ostringstream ss; ss << d; return ss.str(); }
        if (tipo == TXT) return s;
        if (tipo == LISTA && lista) {
            std::string r = "[";
            for (std::size_t k = 0; k < lista->size(); ++k) { if (k) r += ", "; r += (*lista)[k].a_texto(); }
            return r + "]";
        }
        return "{}";
    }
};

struct DefClaseAOT {
    std::string padre;
    std::function<void(Val&)> init_props;
    std::map<std::string, std::function<Val(Val, std::vector<Val>)>> metodos;
};
static std::map<std::string, DefClaseAOT> g_clases_aot;

Val Val::clase_de(const std::string& nom, const std::string& padre, std::function<void(Val&)> init_p) {
    g_clases_aot[nom].padre = padre;
    g_clases_aot[nom].init_props = init_p;
    return Val::funcion([nom](std::vector<Val> args) -> Val {
        Val inst = Val::mapa_de({{"__clase__", Val::texto(nom)}});
        if (g_clases_aot[nom].init_props) g_clases_aot[nom].init_props(inst);
        std::string cur = nom;
        while (!cur.empty()) {
            auto it = g_clases_aot[cur].metodos.find("iniciar");
            if (it != g_clases_aot[cur].metodos.end()) {
                it->second(inst, args);
                break;
            }
            cur = g_clases_aot[cur].padre;
        }
        return inst;
    });
}

static Val esp_llamar_metodo(Val obj, const std::string& met, std::vector<Val> args) {
    if (obj.tipo == Val::MAPA && obj.mapa) {
        auto it_cls = obj.mapa->find("__clase__");
        if (it_cls != obj.mapa->end()) {
            std::string cur = it_cls->second.a_texto();
            while (!cur.empty()) {
                auto it = g_clases_aot[cur].metodos.find(met);
                if (it != g_clases_aot[cur].metodos.end()) return it->second(obj, std::move(args));
                cur = g_clases_aot[cur].padre;
            }
        }
        auto it_fn = obj.mapa->find(met);
        if (it_fn != obj.mapa->end() && it_fn->second.tipo == Val::FN) return it_fn->second.invocar(std::move(args));
    }
    throw std::runtime_error("metodo no encontrado en objeto AOT: " + met);
}

static Val esp_cargar_modulo_aot(const std::string& mod) {
    if (mod == "matematicas") {
        return Val::mapa_de({
            {"PI", Val::decimal(3.14159265358979323846)},
            {"raiz", Val::funcion([](std::vector<Val> a) { return Val::decimal(std::sqrt(a[0].num())); })},
            {"abs", Val::funcion([](std::vector<Val> a) { return Val::decimal(std::fabs(a[0].num())); })}
        });
    }
    if (mod == "archivos") {
        return Val::mapa_de({
            {"leer", Val::funcion([](std::vector<Val> a) {
                std::ifstream f(a[0].a_texto());
                return Val::texto(std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()));
            })},
            {"escribir", Val::funcion([](std::vector<Val> a) {
                std::ofstream f(a[0].a_texto()); f << a[1].a_texto();
                return Val::booleano(true);
            })}
        });
    }
    return Val::mapa_de({});
}

struct TablaAOT {
    std::vector<std::string> cols;
    std::vector<std::map<std::string, Val>> filas;
};
static std::string g_db_ruta_aot = ":memoria:";
static std::map<std::string, TablaAOT> g_tablas_aot;

static bool esp_eval_cond_aot(const Val& a, const std::string& op, const Val& b) {
    if (op == "=") return a.a_texto() == b.a_texto();
    if (op == "!=") return a.a_texto() != b.a_texto();
    if (op == "<") return a.num() < b.num();
    if (op == ">") return a.num() > b.num();
    if (op == "<=") return a.num() <= b.num();
    if (op == ">=") return a.num() >= b.num();
    return false;
}

static void esp_sql_abrir(const std::string& ruta) { g_db_ruta_aot = ruta; }
static void esp_sql_crear_tabla(const std::string& tabla, const std::vector<std::string>& cols) {
    g_tablas_aot[tabla].cols = cols;
}
static void esp_sql_insertar(const std::string& tabla, const std::vector<std::string>& cols, const std::vector<Val>& vals) {
    std::map<std::string, Val> fila;
    for (std::size_t i = 0; i < cols.size() && i < vals.size(); ++i) fila[cols[i]] = vals[i];
    g_tablas_aot[tabla].filas.push_back(std::move(fila));
}
static Val esp_sql_seleccionar(const std::string& tabla, const std::vector<std::string>& cols,
                               bool tiene_d, const std::string& col_d, const std::string& op_d, const Val& val_d,
                               const std::string& ord_c, bool desc, int lim) {
    auto& t = g_tablas_aot[tabla];
    std::vector<std::map<std::string, Val>> sel;
    for (const auto& f : t.filas) {
        if (!tiene_d || (f.count(col_d) && esp_eval_cond_aot(f.at(col_d), op_d, val_d))) sel.push_back(f);
    }
    if (!ord_c.empty()) {
        std::sort(sel.begin(), sel.end(), [&](const auto& a, const auto& b) {
            std::string va = a.count(ord_c) ? a.at(ord_c).a_texto() : "";
            std::string vb = b.count(ord_c) ? b.at(ord_c).a_texto() : "";
            return desc ? (va > vb) : (va < vb);
        });
    }
    if (lim >= 0 && (std::size_t)lim < sel.size()) sel.resize((std::size_t)lim);
    std::vector<Val> out;
    for (const auto& f : sel) {
        if (cols.size() == 1 && cols[0] == "*") out.push_back(Val::mapa_de(f));
        else {
            std::map<std::string, Val> sub;
            for (const auto& c : cols) if (f.count(c)) sub[c] = f.at(c);
            out.push_back(Val::mapa_de(std::move(sub)));
        }
    }
    return Val::lista_de(std::move(out));
}
static void esp_sql_actualizar(const std::string& tabla, const std::vector<std::pair<std::string, Val>>& asigs,
                               bool tiene_d, const std::string& col_d, const std::string& op_d, const Val& val_d) {
    for (auto& f : g_tablas_aot[tabla].filas) {
        if (!tiene_d || (f.count(col_d) && esp_eval_cond_aot(f.at(col_d), op_d, val_d))) {
            for (const auto& kv : asigs) f[kv.first] = kv.second;
        }
    }
}
static void esp_sql_eliminar(const std::string& tabla, bool tiene_d, const std::string& col_d,
                             const std::string& op_d, const Val& val_d) {
    auto& filas = g_tablas_aot[tabla].filas;
    filas.erase(std::remove_if(filas.begin(), filas.end(), [&](const auto& f) {
        return !tiene_d || (f.count(col_d) && esp_eval_cond_aot(f.at(col_d), op_d, val_d));
    }), filas.end());
}

static std::string g_web_titulo = "ESP 2.0.0";
static std::string g_web_css;
static std::string g_web_html;
static std::string g_web_js;

static Val esp_html_web = Val::funcion([](std::vector<Val>) -> Val {
    std::ostringstream doc;
    doc << "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'><title>" << g_web_titulo
        << "</title><style>body{margin:0;padding:2rem;font-family:system-ui,sans-serif;background:#0b1020;color:#edf2ff;}"
        << ".esp-contenedor{max-width:960px;margin:0 auto;}.esp-boton{padding:.65rem 1.2rem;border-radius:10px;border:none;font-weight:700;background:#54e5d0;color:#061018;cursor:pointer;}"
        << ".esp-entrada{display:block;width:100%;padding:.65rem;margin:.5rem 0 1rem;border-radius:8px;border:1px solid #334;background:#090d1a;color:#fff;}"
        << g_web_css << "</style></head><body><main class='esp-contenedor'>" << g_web_html
        << "</main><script>" << g_web_js << "</script></body></html>\n";
    return Val::texto(doc.str());
});
static Val esp_guardar_web = Val::funcion([](std::vector<Val> a) -> Val {
    if (!a.empty()) { std::ofstream f(a[0].a_texto()); f << esp_html_web.invocar({}).a_texto(); }
    return Val::booleano(true);
});
static Val esp_servir_web = Val::funcion([](std::vector<Val>) -> Val { return Val::booleano(true); });

static Val esp_neg(const Val& a) { return a.tipo == Val::INT ? Val::entero(-a.i) : Val::decimal(-a.num()); }
static Val esp_sumar(const Val& a, const Val& b) {
    if (a.tipo == Val::TXT || b.tipo == Val::TXT) return Val::texto(a.a_texto() + b.a_texto());
    if (a.tipo == Val::INT && b.tipo == Val::INT) return Val::entero(a.i + b.i);
    return Val::decimal(a.num() + b.num());
}
static Val esp_restar(const Val& a, const Val& b) { return (a.tipo == Val::INT && b.tipo == Val::INT) ? Val::entero(a.i - b.i) : Val::decimal(a.num() - b.num()); }
static Val esp_mul(const Val& a, const Val& b) { return (a.tipo == Val::INT && b.tipo == Val::INT) ? Val::entero(a.i * b.i) : Val::decimal(a.num() * b.num()); }
static Val esp_div(const Val& a, const Val& b) {
    double y = b.num(); if (y == 0.0) throw std::runtime_error("division por cero");
    if (a.tipo == Val::INT && b.tipo == Val::INT && a.i % b.i == 0) return Val::entero(a.i / b.i);
    return Val::decimal(a.num() / y);
}
static Val esp_mod(const Val& a, const Val& b) { return Val::entero(a.i % b.i); }
static Val esp_menor(const Val& a, const Val& b) { return Val::booleano(a.num() < b.num()); }
static Val esp_menor_ig(const Val& a, const Val& b) { return Val::booleano(a.num() <= b.num()); }
static Val esp_mayor(const Val& a, const Val& b) { return Val::booleano(a.num() > b.num()); }
static Val esp_mayor_ig(const Val& a, const Val& b) { return Val::booleano(a.num() >= b.num()); }

static Val esp_mostrar = Val::funcion([](std::vector<Val> a) -> Val {
    for (std::size_t i = 0; i < a.size(); ++i) { if (i) std::cout << ' '; std::cout << a[i].a_texto(); }
    std::cout << '\n'; return Val::nulo();
});
static Val esp_longitud = Val::funcion([](std::vector<Val> a) -> Val {
    if (a[0].tipo == Val::TXT) return Val::entero((long long)a[0].s.size());
    if (a[0].tipo == Val::LISTA && a[0].lista) return Val::entero((long long)a[0].lista->size());
    if (a[0].tipo == Val::MAPA && a[0].mapa) return Val::entero((long long)a[0].mapa->size());
    return Val::entero(0);
});
static Val esp_texto = Val::funcion([](std::vector<Val> a) -> Val { return Val::texto(a.empty() ? "" : a[0].a_texto()); });
static Val esp_entero = Val::funcion([](std::vector<Val> a) -> Val { return Val::entero((long long)a[0].num()); });
static Val esp_decimal = Val::funcion([](std::vector<Val> a) -> Val { return Val::decimal(a[0].num()); });
static Val esp_agregar = Val::funcion([](std::vector<Val> a) -> Val { a[0].lista->push_back(a[1]); return Val::entero((long long)a[0].lista->size()); });
static Val esp_afirmar = Val::funcion([](std::vector<Val> a) -> Val {
    if (a.empty() || !a[0].verdad()) throw std::runtime_error(a.size() > 1 ? a[1].a_texto() : "afirmacion fallida");
    return Val::booleano(true);
});
static Val esp_indice(const Val& o, const Val& i) {
    if (o.tipo == Val::LISTA && o.lista) return (*o.lista)[(std::size_t)(i.i - 1)];
    if (o.tipo == Val::MAPA && o.mapa) return (*o.mapa)[i.a_texto()];
    throw std::runtime_error("indice no valido");
}
static Val esp_asignar_indice(Val o, const Val& i, Val v) { (*o.lista)[(std::size_t)(i.i - 1)] = v; return v; }
static Val esp_miembro(const Val& o, const std::string& m) {
    if (o.tipo == Val::MAPA && o.mapa) return (*o.mapa)[m];
    throw std::runtime_error("miembro no encontrado: " + m);
}
static Val esp_asignar_miembro(Val o, const std::string& m, Val v) { (*o.mapa)[m] = v; return v; }

)";
        if (es_biblioteca) {
            out << "#ifdef _WIN32\n#define ESP_EXPORT extern \"C\" __declspec(dllexport)\n"
                << "#else\n#define ESP_EXPORT extern \"C\" __attribute__((visibility(\"default\")))\n#endif\n"
                << "ESP_EXPORT int esp_biblioteca_principal() {\n";
        } else {
            out << "int main() {\n";
        }
        indent = 1;
        linea("try {");
        ++indent;
        for (const auto& s : prog->sentencias) sentencia(s.get());
        linea("return 0;");
        --indent;
        linea("} catch (const std::exception& e) {");
        ++indent;
        linea("std::cerr << \"[ErrorEjecucion] \" << e.what() << '\\n';");
        linea("return 1;");
        --indent;
        linea("}");
        out << "}\n";
        return out.str();
    }
};

}  // namespace

std::string detectar_compilador() {
    if (comando_existe("clang++")) return "clang++";
    if (comando_existe("g++")) return "g++";
    if (comando_existe("cl")) return "cl";
    return "";
}

ResultadoCompilacion compilar_fuente_esp(const std::string& fuente, const OpcionesCompilacion& opciones) {
    ResultadoCompilacion res;
    try {
        Lexer lexer(fuente, opciones.entrada);
        Parser parser(lexer.analizar());
        auto ast = parser.analizar();

        std::filesystem::path salida(opciones.salida);
        if (salida.empty()) {
            std::filesystem::path in(opciones.entrada);
            salida = in.parent_path() / in.stem();
            if (opciones.formato == "dll") salida += ".dll";
            else if (opciones.formato == "so") salida += ".so";
            else if (opciones.formato == "dylib") salida += ".dylib";
            else if (opciones.formato == "estatica") {
#ifdef _WIN32
                salida += ".lib";
#else
                salida += ".a";
#endif
            } else if (opciones.formato == "cpp") salida += ".cpp";
            else if (opciones.formato == "ll") salida += ".ll";
            else {
#ifdef _WIN32
                salida += ".exe";
#endif
            }
        }

        if (!salida.parent_path().empty()) {
            std::error_code ec;
            std::filesystem::create_directories(salida.parent_path(), ec);
        }

        const bool es_bib = opciones.formato == "dll" || opciones.formato == "so" ||
                            opciones.formato == "dylib" || opciones.formato == "estatica";
        GeneradorCpp gen;
        std::string cpp = gen.generar(ast.get(), es_bib);

        if (opciones.formato == "cpp") {
            escribir_archivo(salida.string(), cpp);
            res.ok = true;
            res.archivo_generado = salida.string();
            res.mensaje = "Codigo C++ intermedio generado en " + res.archivo_generado;
            return res;
        }

        const std::string clave_cache =
            sha256_hex(cpp + "|" + opciones.formato + "|" + opciones.nivel + "|" + VERSION);
        const std::filesystem::path dir_cache = std::filesystem::path(directorio_home_esp()) / "cache_compilacion";
        std::error_code ec_cache;
        std::filesystem::create_directories(dir_cache, ec_cache);

        std::string ext_cache = salida.extension().string();
        if (ext_cache.empty()) {
#ifdef _WIN32
            if (opciones.formato == "exe") ext_cache = ".exe";
#endif
        }
        const std::filesystem::path artefacto_cache = dir_cache / (clave_cache + ext_cache);
        if (std::filesystem::is_regular_file(artefacto_cache, ec_cache) && !ec_cache) {
            std::filesystem::copy_file(artefacto_cache, salida, std::filesystem::copy_options::overwrite_existing,
                                       ec_cache);
            if (!ec_cache) {
                res.ok = true;
                res.uso_cache = true;
                res.uso_llvm = true;
                res.archivo_generado = salida.string();
                res.mensaje = "Compilado desde cache nativa AOT en " + res.archivo_generado;
                return res;
            }
        }

        const std::string token_temp = clave_cache.substr(0, 16);
        const std::filesystem::path temp_cpp = dir_cache / ("esp_" + token_temp + ".cpp");
        const std::filesystem::path temp_ll = dir_cache / ("esp_" + token_temp + ".ll");
        const std::filesystem::path temp_obj = dir_cache / ("esp_" + token_temp + ".o");
        escribir_archivo(temp_cpp.string(), cpp);

        const std::string nivel = opciones.nivel.empty() ? "O2" : opciones.nivel;
        const bool tiene_clang = comando_existe("clang++");
        const bool tiene_llvm_llc = comando_existe("llc");

        if (opciones.formato == "ll") {
            if (!tiene_clang) {
                res.mensaje = "Se requiere clang++ para emitir LLVM IR (.ll).";
                return res;
            }
            std::string cmd = "clang++ -std=c++17 -" + nivel + " -S -emit-llvm " +
                              citar_argumento_proceso(temp_cpp.string()) + " -o " +
                              citar_argumento_proceso(salida.string());
            std::string stderr_clang;
            int rc = ejecutar_proceso(cmd, "", nullptr, &stderr_clang);
            std::error_code ec;
            std::filesystem::remove(temp_cpp, ec);
            if (rc != 0) {
                res.mensaje = "Fallo al generar LLVM IR: " + stderr_clang;
                return res;
            }
            res.ok = true;
            res.uso_llvm = true;
            res.archivo_generado = salida.string();
            res.mensaje = "LLVM IR generado en " + res.archivo_generado;
            return res;
        }

        if (tiene_clang) {
            std::string cmd_ir = "clang++ -std=c++17 -" + nivel + " -S -emit-llvm " +
                                 citar_argumento_proceso(temp_cpp.string()) + " -o " +
                                 citar_argumento_proceso(temp_ll.string());
            std::string stderr_ir;
            int rc_ir = ejecutar_proceso(cmd_ir, "", nullptr, &stderr_ir);
            if (rc_ir == 0) {
                std::string err_llvm_api;
                const bool objeto_api =
                    !ir_a_objeto_con_llvm_api(temp_ll.string(), temp_obj.string(), nivel, &err_llvm_api).empty();
                bool objeto_cli = false;
                if (!objeto_api && tiene_llvm_llc) {
                    std::string reloc = es_bib ? " -relocation-model=pic" : "";
                    std::string cmd_llc = "llc -" + nivel + reloc + " -filetype=obj " +
                                          citar_argumento_proceso(temp_ll.string()) + " -o " +
                                          citar_argumento_proceso(temp_obj.string());
                    objeto_cli = ejecutar_proceso(cmd_llc, "", nullptr, nullptr) == 0;
                }

                if (objeto_api || objeto_cli) {
                    int rc_link = 1;
                    std::string stderr_link;
                    if (opciones.formato == "estatica" && comando_existe("llvm-ar")) {
                        std::string cmd_ar = "llvm-ar rcs " + citar_argumento_proceso(salida.string()) + " " +
                                             citar_argumento_proceso(temp_obj.string());
                        rc_link = ejecutar_proceso(cmd_ar, "", nullptr, &stderr_link);
                    } else if (opciones.formato == "estatica" && comando_existe("ar")) {
                        std::string cmd_ar = "ar rcs " + citar_argumento_proceso(salida.string()) + " " +
                                             citar_argumento_proceso(temp_obj.string());
                        rc_link = ejecutar_proceso(cmd_ar, "", nullptr, &stderr_link);
                    } else if (opciones.formato != "estatica") {
                        std::string flag_bib = es_bib ? " -shared" : "";
                        std::string cmd_link = "clang++" + flag_bib + " " +
                                               citar_argumento_proceso(temp_obj.string()) + " -o " +
                                               citar_argumento_proceso(salida.string());
                        rc_link = ejecutar_proceso(cmd_link, "", nullptr, &stderr_link);
                    }
                    std::error_code ec;
                    std::filesystem::remove(temp_cpp, ec);
                    std::filesystem::remove(temp_ll, ec);
                    std::filesystem::remove(temp_obj, ec);
                    if (rc_link == 0) {
                        std::filesystem::copy_file(salida, artefacto_cache,
                                                   std::filesystem::copy_options::overwrite_existing, ec);
                        res.ok = true;
                        res.uso_llvm = true;
                        res.archivo_generado = salida.string();
                        res.mensaje = std::string("Compilado nativamente via LLVM (") +
                                      (objeto_api ? "API C++" : "IR/llc") + ") en " + res.archivo_generado;
                        return res;
                    }
                }
            }
        }

        std::string cc = detectar_compilador();
        if (cc.empty()) {
            std::error_code ec;
            std::filesystem::remove(temp_cpp, ec);
            res.mensaje = "No se encontro compilador nativo (clang++, g++ o cl).";
            return res;
        }

        std::string cmd;
        if (cc == "cl") {
            if (opciones.formato == "estatica") {
                cmd = "cl /nologo /std:c++17 /" + nivel + " /EHsc /c " +
                      citar_argumento_proceso(temp_cpp.string()) + " /Fo" +
                      citar_argumento_proceso(temp_obj.string()) + " && lib /nologo /OUT:" +
                      citar_argumento_proceso(salida.string()) + " " +
                      citar_argumento_proceso(temp_obj.string());
            } else {
                std::string dll = es_bib ? " /LD" : "";
                cmd = "cl /nologo /std:c++17 /" + nivel + " /EHsc" + dll + " " +
                      citar_argumento_proceso(temp_cpp.string()) + " /Fe:" +
                      citar_argumento_proceso(salida.string());
            }
        } else if (opciones.formato == "estatica") {
            cmd = cc + " -std=c++17 -" + nivel + " -c " + citar_argumento_proceso(temp_cpp.string()) + " -o " +
                  citar_argumento_proceso(temp_obj.string()) + " && ar rcs " +
                  citar_argumento_proceso(salida.string()) + " " + citar_argumento_proceso(temp_obj.string());
        } else {
            std::string flag_bib = es_bib ? " -shared -fPIC" : "";
            cmd = cc + " -std=c++17 -" + nivel + flag_bib + " " + citar_argumento_proceso(temp_cpp.string()) +
                  " -o " + citar_argumento_proceso(salida.string());
        }

        std::string stderr_cc;
        int rc = ejecutar_proceso(cmd, "", nullptr, &stderr_cc);
        std::error_code ec;
        std::filesystem::remove(temp_cpp, ec);
        std::filesystem::remove(temp_ll, ec);
        std::filesystem::remove(temp_obj, ec);

        if (rc != 0) {
            res.mensaje = "Fallo la compilacion nativa con " + cc + ": " + stderr_cc;
            return res;
        }

        std::filesystem::copy_file(salida, artefacto_cache, std::filesystem::copy_options::overwrite_existing, ec);
        res.ok = true;
        res.uso_llvm = (cc == "clang++");
        res.archivo_generado = salida.string();
        res.mensaje = "Compilado nativamente en " + res.archivo_generado + " usando " + cc;
        return res;
    } catch (const ErrorESP& e) {
        res.mensaje = formatear_error(e);
        return res;
    } catch (const std::exception& e) {
        res.mensaje = e.what();
        return res;
    }
}

}  // namespace esp