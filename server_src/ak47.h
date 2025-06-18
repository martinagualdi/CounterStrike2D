#include "arma_de_fuego.h"

class Ak47 : public ArmaDeFuego {
    private:
    int rafaga_restante = 3;
    std::chrono::steady_clock::time_point proximo_disparo_rafaga;
    bool rafaga_activa = false;
    float ultimo_angulo_rafaga = 0.0f;
    int id_jugador_rafaga = -1;


public:
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

    int accion(float distancia) override;

    bool puedeAccionar() override;

    void iniciar_rafaga(float angulo, int id_jugador);

    void tick_rafaga();
    
    int get_id_jugador_rafaga() const { return id_jugador_rafaga; }
    float get_ultimo_angulo_rafaga() const { return ultimo_angulo_rafaga; }
    int get_rafaga_restante() const { return rafaga_restante; }
    bool hay_rafaga() const { return rafaga_activa; }
    std::chrono::steady_clock::time_point& get_proximo_disparo_rafaga() { return proximo_disparo_rafaga; }

    enum ArmaEnMano getCodigoArma() const override { return AK_47; }
};