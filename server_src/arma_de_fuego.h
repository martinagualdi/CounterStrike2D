#ifndef ARMA_DE_FUEGO_H
#define ARMA_DE_FUEGO_H

#include <string>
#include <random>
#include "arma.h"

class Ak47;
class m3;
class Awp;

class ArmaDeFuego: public Arma {
protected:
    int municion_actual ;
    const int municion_max;

public:
ArmaDeFuego(const std::string& nombre, float precision, float alcance, int min_danio, int max_danio, bool es_automatica, int municion_inicial, int municion_max, int cadencia)
    : Arma(nombre, precision, alcance, min_danio, max_danio, es_automatica, cadencia),
      municion_actual(municion_inicial), municion_max(municion_max){}

    static ArmaDeFuego* crearArma(const std::string& nombre);

    virtual ~ArmaDeFuego() = default;

    virtual bool puede_disparar() const { return municion_actual > 0; }
    virtual void recargar() { municion_actual = municion_max; }
    virtual void agregarMunicion(int municion) { municion_actual = municion_actual+municion; }
    virtual int getMunicion() const { return municion_actual; }
    virtual int getMunicionMax() const { return municion_max; }

    int getBalas() override {
        return municion_actual;
    }
    
};
#endif