#include "glock.h"

int Glock::accion(float distancia) {
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

bool Glock::puedeAccionar() {
    auto ahora = std::chrono::steady_clock::now();
    auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultima_accion);
    if (tiempo_transcurrido.count() >= cadencia_accion_ms && municion_actual > 0){
        municion_actual--;
        ultima_accion = std::chrono::steady_clock::now();
        return true;
    } 
    return false;
}  