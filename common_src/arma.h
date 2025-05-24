#ifndef ARMA_H
#define ARMA_H

#include <string>
#include <random>

class Arma {
protected:
    std::string nombre;
    int municion_max;
    int municion_actual;
    float precision; // 0.0 a 1.0
    float alcance;   // en unidades del juego
    int min_danio;
    int max_danio;
    bool es_automatica;

public:
    Arma() = default; 

    virtual ~Arma() = default;

    virtual int disparar(float distancia) = 0; // Devuelve el daño causado (0 si falla)
    virtual bool puede_disparar() const { return municion_actual > 0; }
    virtual void recargar() { municion_actual = municion_max; }
    virtual std::string getNombre() const { return nombre; }
    virtual int getMunicion() const { return municion_actual; }
    virtual int getMunicionMax() const { return municion_max; }
    virtual bool esAutomatica() const { return es_automatica; }
};

#endif
