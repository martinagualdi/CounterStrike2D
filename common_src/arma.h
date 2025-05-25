#ifndef ARMA_H
#define ARMA_H

#include <string>
#include <random>

class Arma {
protected:
    const std::string nombre;
    const float precision; // 0.0 a 1.0
    const float alcance;   // en unidades del juego
    const int min_danio;
    const int max_danio;
    const bool es_automatica;

public:
    Arma(const std::string& nombre, float precision, float alcance, int min_danio, int max_danio, bool es_automatica):nombre(nombre), precision(precision), alcance(alcance),
          min_danio(min_danio), max_danio(max_danio), es_automatica(es_automatica) {}
; 

    virtual ~Arma() = default;

    virtual int accion(float distancia); // Devuelve el daño causado (0 si falla)
    virtual std::string getNombre() const { return nombre; }
    virtual bool esAutomatica() const { return es_automatica; }
};

#endif
