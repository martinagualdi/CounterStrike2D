#ifndef MENSAJEDTO_H
#define MENSAJEDTO_H
#include <string>
#include <iostream>

struct MensajeDTO {
    // cppcheck-suppress unusedStructMember
    std::string tipo_mensaje;

    // cppcheck-suppress unusedStructMember
    std::string informacion;
};
#endif
