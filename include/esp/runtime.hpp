#pragma once
#include "parser.hpp"
#include "valor.hpp"
#include <set>

namespace esp {

struct ControlFlujo {
    enum class Tipo { Ninguno, Devolver, Romper, Continuar } tipo = Tipo::Ninguno;
    Valor valor;
};

class Interprete {
public:
    Interprete();
    Valor ejecutar_archivo(const std::string& ruta);
    Valor ejecutar_fuente(const std::string& fuente, const std::string& nombre);
    Valor evaluar_repl(const std::string& linea);
    std::shared_ptr<Entorno> global() { return global_; }
    const Nodo* ultimo_ast() const { return ast_.get(); }
    std::string raiz_instalacion;
    std::string directorio_trabajo;
    bool permitir_instalar = true;

private:
    std::shared_ptr<Entorno> global_;
    NodoPtr ast_;
    std::vector<std::shared_ptr<Nodo>> ast_repl_;
    std::vector<std::shared_ptr<Entorno>> pila_clase_;
    std::map<std::string, std::shared_ptr<Modulo>> cache_modulos_;
    std::set<std::string> modulos_cargando_;
    ControlFlujo flujo_;

    void registrar_builtins();
    Valor ejecutar(const Nodo* n, std::shared_ptr<Entorno> env);
    Valor evaluar(const Nodo* n, std::shared_ptr<Entorno> env);
    Valor llamar(const Valor& callee, std::vector<Valor> args, Ubicacion u, std::shared_ptr<Entorno> env);
    Valor acceder_miembro(const Valor& obj, const std::string& nombre, Ubicacion u, bool escribir);
    bool acceso_privado_permitido(const std::string& propietario) const;
    Valor conseguir(const std::string& nombre, const std::string& extra, Ubicacion u);
    std::shared_ptr<Modulo> cargar_modulo_estandar(const std::string& nombre);
    std::shared_ptr<Modulo> cargar_modulo_archivo(const std::string& nombre, const std::string& version);
    Valor nativo_imprimir(std::vector<Valor>& a);
};

std::string leer_archivo(const std::string& ruta);
void escribir_archivo(const std::string& ruta, const std::string& contenido);
std::string directorio_de(const std::string& ruta);
std::string unir_ruta(const std::string& a, const std::string& b);
std::string directorio_home_esp();
std::string sha256_hex(const std::string& datos);
std::string detectar_compilador();
std::string citar_argumento_proceso(const std::string& argumento);
int ejecutar_proceso(const std::string& comando, const std::string& cwd, std::string* salida, std::string* error,
                     int timeout_ms = 0);

}  // namespace esp
