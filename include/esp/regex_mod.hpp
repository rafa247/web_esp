#pragma once
#include "valor.hpp"
#include <memory>
#include <string>

namespace esp {
std::shared_ptr<Modulo> crear_modulo_re(const std::string& nombre_alias = "re");
}
