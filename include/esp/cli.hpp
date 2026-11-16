#pragma once
#include <string>
#include <vector>

namespace esp {

int ejecutar_cli(int argc, char** argv);
void mostrar_ayuda();
int instalar_en_sistema(const std::string& destino, bool asociar);
int desinstalar_del_sistema();
int actualizar_path(const std::string& binario);
bool asociar_extension(const std::string& exe);
int diagnostico();

}  // namespace esp
