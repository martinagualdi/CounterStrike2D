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
: Arma("Bomba", 1.0f, 40.0f, 0, 100, false,1), activada(false), desactivada(false), detonada(false), 
tiempo_para_detonar(Configuracion::get<int>("tiempo_pare_que_explote_bomba")) {}
    int accion(float distancia) override {
        detonada = true;
        // Daño máximo a 0 metros, disminuye linealmente hasta 0 a 500 unidades
        const float distancia_maxima = 5000.0f;
        
        if (distancia >= distancia_maxima) return 0;
        float factor = 1.0f - (distancia / distancia_maxima);
        if (factor < 0.0f) factor = 0.0f;
        return static_cast<int>(max_danio * factor);
    }
    
    void desactivar() { 
        desactivada = true;
    }

    void reiniciar() {
        activada = false;
        desactivada = false;
        detonada = false;
        tiempo_detonacion = std::chrono::steady_clock::time_point();
        tiempo_activacion = std::chrono::steady_clock::time_point();
    }

    bool fueDesactivada() const { return desactivada; }
    void activar(float x, float y) { 
        //Iniciar temporizador de detonacion
        x_plantada = x;
        y_plantada = y;
        activada = true; 
        tiempo_activacion = std::chrono::steady_clock::now();
        tiempo_detonacion = std::chrono::steady_clock::now() + std::chrono::seconds(tiempo_para_detonar);
    }
    bool estaActivada() const { return activada; }
    int getBalas() override { return 1; } // La bomba no tiene balas, pero se usa para el HUD
    

    bool detonar(){
        if (getTiempoParaDetonar()<=0){
            detonada = true;
        }
        return detonada;
    }
    bool puedeAccionar() override {
        return false; // La bomba nunca puede ser disparada
    }

    bool estaPlantada() const { return plantada; }
    void setPlantada(bool plantada) { this->plantada = plantada; }
    int getTiempoParaDetonar() {
        if (!activada) return tiempo_para_detonar;
        auto ahora = std::chrono::steady_clock::now();
        auto restante = std::chrono::duration_cast<std::chrono::seconds>(tiempo_detonacion - ahora).count();
        return restante > 0 ? static_cast<int>(restante) : 0;
    }
    float getX(){
        return x_plantada;
    }
    float getY(){
        return y_plantada;
    }

    enum ArmaEnMano getCodigoArma() const override {
        return BOMBA_TT; // Asumiendo que la bomba es del equipo terrorista
    }
};

#endif