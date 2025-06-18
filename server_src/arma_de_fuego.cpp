#include "arma_de_fuego.h"
#include "ak47.h"
#include "m3.h"
#include "awp.h"

ArmaDeFuego* ArmaDeFuego::crearArma(const std::string& nombre) {
    if (nombre == "ak47") {
        return new Ak47();
    } else if (nombre == "m3") {
        return new m3();
    } else if (nombre == "awp") {
        return new Awp();
    } else {
        return nullptr;
    }
}