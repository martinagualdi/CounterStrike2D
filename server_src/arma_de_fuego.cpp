#include "arma_de_fuego.h"
#include "ak47.h"
#include "m3.h"
#include "awp.h"

#define NOMBRE_AK "ak47"
#define NOMBRE_AWP "awp"
#define NOMBRE_M3 "m3"

ArmaDeFuego* ArmaDeFuego::crearArma(const std::string& nombre) {
    if (nombre == NOMBRE_AK) {
        return new Ak47();
    } else if (nombre == NOMBRE_M3) {
        return new m3();
    } else if (nombre == NOMBRE_AWP) {
        return new Awp();
    } else {
        return nullptr;
    }
}