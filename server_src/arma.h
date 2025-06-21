#ifndef ARMA_H
#define ARMA_H

#include <string>
#include <random>
#include <chrono>

#include "configuracion.h" 
#include "../common_src/enums_jugador.h"

class Arma {
protected:
    const std::string nombre;
    float precision; // 0.0 a 1.0
    const float alcance;   
    const int min_danio;
    const int max_danio;
    const bool es_automatica;
    const int cadencia_accion_ms;

    std::chrono::steady_clock::time_point ultima_accion;


public:
    Arma(const std::string& nombre, float precision, float alcance, int min_danio, int max_danio, bool es_automatica,int cadencia):nombre(nombre), precision(precision), alcance(alcance),
          min_danio(min_danio), max_danio(max_danio), es_automatica(es_automatica), cadencia_accion_ms(cadencia), 
          ultima_accion(std::chrono::steady_clock::now() - std::chrono::milliseconds(cadencia)){}
; 

    virtual int accion(float distancia)=0;
    virtual bool puedeAccionar()=0; // Verifica si se puede disparar (ej. si hay munición)
    virtual int getBalas()  =0; 
    virtual std::string getNombre() const { return nombre; }
    virtual bool esAutomatica() const { return es_automatica; }
    virtual enum ArmaEnMano getCodigoArma() const = 0; 

    virtual ~Arma() = default; 
};

#endif
