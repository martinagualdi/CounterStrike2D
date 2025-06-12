#include "armaDeFuego.h"

class Ak47 : public ArmaDeFuego {
    private:
    int rafaga_restante = 3;
    std::chrono::steady_clock::time_point proximo_disparo_rafaga;
    bool rafaga_activa = false;
    float ultimo_angulo_rafaga = 0.0f;
    int id_jugador_rafaga = -1;


public:
    // Valores Hardcodeados hasta tener YAML
    Ak47(): ArmaDeFuego(
        "AK-47", 
        Configuracion::get<float>("precision_ak47"), 
        1000.0f, 
        Configuracion::get<int>("danio_min_ak47"), 
        Configuracion::get<int>("danio_max_ak47"), 
        true,
        Configuracion::get<int>("balas_ak47"),
        Configuracion::get<int>("balas_max_ak47"),
        200){}

    int accion(float distancia) override {
        if (municion_actual <= 0) return 0;
        if (distancia > alcance) return 0; 
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

    void iniciar_rafaga(float angulo, int id_jugador) {
        ultimo_angulo_rafaga = angulo;
        id_jugador_rafaga = id_jugador;
        rafaga_restante = 3;
        rafaga_activa = true;
        proximo_disparo_rafaga = std::chrono::steady_clock::now()- std::chrono::milliseconds(1);
    }

    void tick_rafaga() { 
        if (rafaga_restante > 0) {
            rafaga_restante--;
            municion_actual--; 
            if (rafaga_restante == 0) {
                rafaga_activa = false; // Termina la ráfaga
            }
        }
    }
    int get_id_jugador_rafaga() const { return id_jugador_rafaga; }
    float get_ultimo_angulo_rafaga() const { return ultimo_angulo_rafaga; }
    int get_rafaga_restante() const { return rafaga_restante; }
    bool hay_rafaga() const { return rafaga_activa; }
    std::chrono::steady_clock::time_point& get_proximo_disparo_rafaga() { return proximo_disparo_rafaga; }
};