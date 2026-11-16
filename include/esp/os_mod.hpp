#pragma once
#include "valor.hpp"
#include <memory>
#include <string>

namespace esp {

std::shared_ptr<Modulo> crear_modulo_os(const std::string& nombre_alias = "os");
std::shared_ptr<Modulo> crear_modulo_binario(const std::string& nombre_alias = "binario");

}  // namespace esp