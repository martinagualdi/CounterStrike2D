#ifndef BOMBA_H
#define BOMBA_H

#include "arma.h"

class Bomba : public Arma {
    bool activada;
    bool desactivada; // Si se desactiva se acaba el juego
    bool detonada;
    const int tiempo_para_detonar; // en segundos, configurable

public:
    // Valores Hardcodeados hasta tener YAML
    Bomba()
: Arma("Bomba", 1.0f, 40.0f, 0, 100, false,1), activada(false), desactivada(false), detonada(false), tiempo_para_detonar(40) {}
    int accion(float distancia) override {
        detonada = true;
        // Daño máximo a 0 metros, disminuye linealmente hasta 0 a 500 unidades
        const float distancia_maxima = 500.0f;
        
        if (distancia >= distancia_maxima) return 0;
        float factor = 1.0f - (distancia / distancia_maxima);
        if (factor < 0.0f) factor = 0.0f;
        return static_cast<int>(max_danio * factor);
    }
    
    void desactivar() { desactivada = true; }
    bool fueDesactivada() const { return desactivada; }
    void activar() { 
        //Iniciar temporizador de detonacion
        activada = true; }
    bool estaActivada() const { return activada; }
    int getBalas() override { return 1; } // La bomba no tiene balas, pero se usa para el HUD
    
    bool estaDetonada() const { return detonada; }
    bool puedeAccionar() override {
        return !detonada && !desactivada; // La bomba puede ser activada si no ha sido detonada ni desactivada
    }

    int getTiempoParaDetonar() const { return tiempo_para_detonar; }
};

#endif