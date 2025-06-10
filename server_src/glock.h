#include "armaDeFuego.h"

class Glock : public ArmaDeFuego {
    
public:
    // Valores Hardcodeados hasta tener YAML
    Glock() : ArmaDeFuego("Glock", 0.9f, 500.0f, 5, 30, false,50,150) {}
    int accion(float distancia) override {
        if (municion_actual <= 0) return 0;
        if (distancia > alcance) return 0; // Si la distancia es mayor que el alcance, no hace daño
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_real_distribution<> hit(0.0, 1.0);
        if (hit(gen) <= precision) {
            int danio = static_cast<int>(max_danio * (alcance/distancia) * 0.5f);
            return danio > max_danio ? max_danio : danio;
        }
        return min_danio;
    }

    bool puedeAccionar()  override {
        auto ahora = std::chrono::steady_clock::now();
        auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultima_accion);
        if (tiempo_transcurrido.count() >= cadencia_accion_ms && municion_actual > 0){
            municion_actual--;
            ultima_accion = std::chrono::steady_clock::now();
            return true;
        } 
        return false;
    }  
   
    
};