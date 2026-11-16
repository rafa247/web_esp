#pragma once
#include "valor.hpp"
#include <memory>
#include <string>

namespace esp {

std::shared_ptr<Modulo> crear_modulo_graficos(const std::string& nombre_alias);
std::shared_ptr<Modulo> crear_modulo_gui();
std::shared_ptr<Modulo> crear_modulo_audio();

}  // namespace esp