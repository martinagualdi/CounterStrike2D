#ifndef BOMBA_H
#define BOMBA_H

#include "arma.h"
#include <chrono>

class Bomba : public Arma {
    bool activada;
    bool desactivada; // Si se desactiva se acaba el juego
    bool detonada;
    int tiempo_para_detonar;
    bool plantada;
    // en segundos, configurable
    std::chrono::steady_clock::time_point tiempo_detonacion;
    std::chrono::steady_clock::time_point tiempo_activacion;
    float x_plantada;
    float y_plantada;

public:
    // Valores Hardcodeados hasta tener YAML
    Bomba()
: Arma("Bomba", 1.0f, 3000.0f, 0, 100, false,1), activada(false), desactivada(false), detonada(false), 
tiempo_para_detonar(Configuracion::get<int>("tiempo_pare_que_explote_bomba")) {}
    void desactivar() { desactivada = true; }
    bool fueDesactivada() const { return desactivada; }
    void activar(float x, float y);
    bool estaActivada() const { return activada; }
    int getBalas() override { return 1; } // La bomba no tiene balas, pero se usa para el HUD
    bool puedeAccionar() override { return false; } // La bomba nunca puede ser disparada
    bool estaPlantada() const { return plantada; }
    void setPlantada(bool plantada) { this->plantada = plantada; }
    float getX(){ return x_plantada; }
    float getY(){ return y_plantada; }
    enum ArmaEnMano getCodigoArma() const override { return BOMBA_TT; }

    int accion(float distancia) override;
    void reiniciar();
    bool detonar();
    int getTiempoParaDetonar();
};

#endif