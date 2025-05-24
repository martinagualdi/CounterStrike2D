#include "arma.h"  

class Cuchillo : public Arma {
public:
    Cuchillo() : Arma() {}
    int disparar(float distancia) override {
        if (distancia <= alcance) {
            std::random_device rd; std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min_danio, max_danio);
            return dis(gen);
        }
        return 0;
    }
    bool puede_disparar() const override { return true; }
protected:
    std::string nombre= "Cuchillo";
    int municion_actual = 0;
    int municion_max    = 0;
    float alcance = 1.0f; // Distancia máxima para usar el cuchillo
    int min_danio = 10;
    int max_danio = 20;
    bool es_automatica = false;
};