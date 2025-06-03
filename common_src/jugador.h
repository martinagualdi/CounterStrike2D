#ifndef JUGADOR_H
#define JUGADOR_H
#include <memory>
#include "comando_dto.h"
#include "cuchillo.h"
#include "arma.h"
#include "glock.h"
#include "ak47.h"
#include "armaDeFuego.h"
#include "enums_jugador.h"


class Jugador {
  private:
    int id;
    float x;
    float y;
    float angulo;
    int vida;
    int dinero;
    enum Equipo equipo_actual;
    enum SkinTipos skin_tipo;
    bool vivo = true;
    bool moviendose = false;
    enum Movimiento movimiento_actual = DETENER;
    std::unique_ptr<ArmaDeFuego> arma_principal;
    std::unique_ptr<ArmaDeFuego> arma_secundaria;
    std::unique_ptr<Cuchillo> cuchillo;
    Arma* arma_en_mano;

  public:  
    explicit Jugador(int id) : id(id), x(10), y(10), angulo(0), vida(100), dinero(500), equipo_actual(), skin_tipo(), vivo(true), arma_principal(new AK47()),arma_secundaria(new Glock()), cuchillo(new Cuchillo()), arma_en_mano(arma_secundaria.get()) {}


    /*Jugador(int id, float x, float y, float angulo, enum Equipo equipo, enum SkinTipos skin, int vida, int dinero, uint8_t arma_secundaria_id)
        : id(id), x(x), y(y), angulo(angulo), vida(vida), dinero(dinero), equipo_actual(equipo), skin_tipo(skin), vivo(true), 
        arma_secundaria(arma_secundaria_id == 0x01 ? new Glock() : nullptr) {};*/

    int getId() const {
        return id;
    }

    float getX() const {
        return x;
    }
    float getY() const {
        return y;
    }

    void setX(float posX) {
        this->x = posX;
    }
    void setY(float posY) {
        this->y = posY;
    }

    void setAngulo(float angulo){
        this->angulo = angulo;
    }

    float getAngulo() const {
        return angulo;
    }
    
    enum Movimiento getMovimiento() const {
        return movimiento_actual;
    }
    
    void setMovimiento(enum Movimiento m){
        this->movimiento_actual = m;
    }

    int get_vida() const {
        return vida;
    }

    int get_dinero() const {
        return dinero;
    }

    bool disparar() {
        /* HARCODEADO PAEA PROBAR DISPARO */
        if (arma_en_mano->accion(0.0f) > 0)
            return true;
        return false;
    }

    void recibir_danio(int danio) { 
        vida -= danio; 
        if (vida <= 0) {
            vida = 0;
            vivo = false;
        }
    }

    std::string get_nombre_arma_en_mano() const {
        return arma_en_mano->getNombre();
    }

    enum Equipo get_equipo() const {
        return equipo_actual;
    }

    void establecer_equipo(enum Equipo equipo) {
        this->equipo_actual = equipo;
    }

    void establecer_skin(enum SkinTipos skin) {
        this->skin_tipo = skin;
    }

    enum SkinTipos get_skin_tipo() const {
        return skin_tipo;
    }

    void set_skin_tipo(enum SkinTipos skin) {
        this->skin_tipo = skin;
    }

    bool esta_vivo() const {
        return vivo;
    }

    bool esta_moviendose() const { return moviendose == true; }

    void cambiar_estado_moviendose() { moviendose = !moviendose; }

    void cambiar_arma_en_mano() {
        if (arma_en_mano == arma_principal.get()) {
            arma_en_mano = arma_secundaria.get();
        } else if (arma_en_mano == arma_secundaria.get()) {
            arma_en_mano = cuchillo.get();
        } else {
            arma_en_mano = arma_principal.get();
        }
    }

    enum ArmaEnMano get_codigo_arma_en_mano() const {
        std::string nombre_arma = arma_en_mano->getNombre();
        if (nombre_arma == "Cuchillo") {
            return CUCHILLO;
        } else if (nombre_arma == "Glock") {
            return GLOCK;
        } else if (nombre_arma == "AK-47") {
            return AK_47;
        } else if (nombre_arma == "M3") {
            return M3;
        } else if (nombre_arma == "AWP") {
            return AWP;
        } else {
            return BOMBA_TT;
        }
    }
    
    ~Jugador() {
       
    }

    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif