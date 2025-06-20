#include "cuchillo.h"

int Cuchillo::accion(float distancia) {
        if (distancia <= alcance) {
            std::random_device rd; std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min_danio, max_danio);
            return dis(gen);
        }
        return 0;
    }

bool Cuchillo::puedeAccionar() {
    auto ahora = std::chrono::steady_clock::now();
    auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultima_accion);
    if (tiempo_transcurrido.count() >= cadencia_accion_ms ){
        ultima_accion = std::chrono::steady_clock::now();
        return true;
    }
    return false;
}
