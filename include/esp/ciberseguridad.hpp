#pragma once
#include "valor.hpp"
#include <memory>
#include <string>

namespace esp {

std::shared_ptr<Modulo> crear_modulo_sockets(const std::string& nombre_alias = "redes.sockets");
std::shared_ptr<Modulo> crear_modulo_cripto(const std::string& nombre_alias = "cripto");
std::shared_ptr<Modulo> crear_modulo_forense();
std::shared_ptr<Modulo> crear_modulo_ast();

}  // namespace esp