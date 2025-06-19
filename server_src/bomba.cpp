#include "bomba.h"

int Bomba::accion(float distancia) {
    detonada = true;
    
    if (distancia >= alcance) return 0;
    float factor = 1.0f - (distancia / alcance);
    if (factor < 0.0f) factor = 0.0f;
    return static_cast<int>(max_danio * factor);
}

void Bomba::reiniciar() {
    activada = false;
    desactivada = false;
    detonada = false;
    tiempo_detonacion = std::chrono::steady_clock::time_point();
    tiempo_activacion = std::chrono::steady_clock::time_point();
}

void Bomba::activar(float x, float y) { 
    //Iniciar temporizador de detonacion
    x_plantada = x;
    y_plantada = y;
    activada = true; 
    tiempo_activacion = std::chrono::steady_clock::now();
    tiempo_detonacion = std::chrono::steady_clock::now() + std::chrono::seconds(tiempo_para_detonar);
}

bool Bomba::detonar(){
    if (getTiempoParaDetonar() <= 0){
        detonada = true;
    }
    return detonada;
}


int Bomba::getTiempoParaDetonar() {
    if (!activada) return tiempo_para_detonar;
    auto ahora = std::chrono::steady_clock::now();
    auto restante = std::chrono::duration_cast<std::chrono::seconds>(tiempo_detonacion - ahora).count();
    return restante > 0 ? static_cast<int>(restante) : 0;
}