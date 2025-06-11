#ifndef JUGADOR_H
#define JUGADOR_H
#include <memory>
#include "cuchillo.h"
#include "arma.h"
#include "glock.h"
#include "ak47.h"
#include "m3.h"
#include "awp.h"
#include "armaDeFuego.h"
#include "../common_src/enums_jugador.h"
#include "../common_src/comando_dto.h"


class Jugador {
  private:
    int id;
    float x;
    float y;
    float spawn_x;
    float spawn_y;
    float angulo;
    int vida;
    int dinero;
    enum Equipo equipo_actual;
    enum SkinTipos skin_tipo;
    bool vivo;
    bool moviendose;
    bool disparando;
    bool plantando_bomba;
    enum Movimiento movimiento_actual = DETENER;
    std::unique_ptr<ArmaDeFuego> arma_principal;
    std::unique_ptr<ArmaDeFuego> arma_secundaria;
    std::unique_ptr<Cuchillo> cuchillo;
    Arma* arma_en_mano;

  public:  

    explicit Jugador(int id) : id(id), x(1000), y(1000), angulo(0), vida(100), dinero(500), equipo_actual(), 
        skin_tipo(), vivo(true), moviendose(false), disparando(false), plantando_bomba(false), arma_principal(nullptr), 
        arma_secundaria(new Glock()), cuchillo(new Cuchillo()), arma_en_mano(arma_secundaria.get()) {}

    // Getters, Setters y estados del jugador
    int getId() const { return id; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setX(float posX) { this->x = posX; }
    void setY(float posY) { this->y = posY; }
    void setAngulo(float angulo){ this->angulo = angulo; }
    float getAngulo() const { return angulo; }
    enum Movimiento getMovimiento() const { return movimiento_actual; }
    void setMovimiento(enum Movimiento m){ this->movimiento_actual = m; }
    int get_vida() const { return vida;}
    int get_dinero() const { return dinero; }
    enum Equipo get_equipo() const { return equipo_actual; }
    void establecer_equipo(enum Equipo equipo) { this->equipo_actual = equipo;  }
    void establecer_skin(enum SkinTipos skin) { this->skin_tipo = skin; }
    enum SkinTipos get_skin_tipo() const { return skin_tipo; }
    void set_skin_tipo(enum SkinTipos skin) { this->skin_tipo = skin; }
    bool esta_vivo() const { return vivo; }
    bool esta_moviendose() const { return moviendose == true; }
    bool esta_disparando() const {  return disparando; }
    void dejar_de_disparar() { disparando = false; }
    bool esta_plantando_bomba() const { return plantando_bomba; }
    bool puede_disparar() const { return arma_en_mano->puedeAccionar(); }

    // Logicas
    void disparar();

    void recibir_danio(int danio);

    std::string get_nombre_arma_en_mano();

    void cambiar_estado_moviendose();

    void cambiar_arma_en_mano();

    bool comprarArma(enum Compra arma);

    bool comprarBalas(enum Compra tipo_bala);

    enum ArmaEnMano get_codigo_arma_en_mano();

    Arma* get_arma_actual() const;

    void definir_spawn(float x, float y);

    void reiniciar();
    
    ~Jugador() {}
    
    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif