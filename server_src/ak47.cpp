#include "ak47.h"

int Ak47::accion(float distancia) {
    if (municion_actual <= 0) return 0;
    if (distancia > alcance) return 0; 
    if (distancia*2.5>=alcance) precision = 1;
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_real_distribution<> hit(0.0, 1.0);
    if (hit(gen) <= precision) {
        float factor = 1.0f - (distancia / alcance);
        int danio = static_cast<int>(max_danio * factor );
        return danio > max_danio ? max_danio : danio;
    }
    return min_danio;
}

bool Ak47::puedeAccionar() {
    auto ahora = std::chrono::steady_clock::now();
    auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultima_accion);
    if (tiempo_transcurrido.count() >= cadencia_accion_ms && municion_actual > 0){
        municion_actual--;
        ultima_accion = std::chrono::steady_clock::now();
        return true;
    }
    return false;
}

void Ak47::iniciar_rafaga(float angulo, int id_jugador) {
    ultimo_angulo_rafaga = angulo;
    id_jugador_rafaga = id_jugador;
    rafaga_restante = 3;
    rafaga_activa = true;
    proximo_disparo_rafaga = std::chrono::steady_clock::now()- std::chrono::milliseconds(1);
}

void Ak47::tick_rafaga() { 
    if (rafaga_restante > 0) {
        rafaga_restante--;
        municion_actual--; 
        if (rafaga_restante == 0) {
            rafaga_activa = false; // Termina la ráfaga
        }
    }
}