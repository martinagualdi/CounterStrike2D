#ifndef ESTADO_JUEGO_H_
#define ESTADO_JUEGO_H_

#include <vector>
#include <cstddef>
#include <iostream>
#include "../server_src/jugador.h"
#include "../server_src/municion.h"


/*Structs para enviar la informacion necesaria para dibujar el juevo en "client"*/

enum EstadoBombaRonda {
    PLANTADA,
    DETONADA,
    DESACTIVADA,
    SIN_PLANTAR
};

struct ArmaEnSuelo {
    Arma *arma;
    float pos_x;
    float pos_y;

    ArmaEnSuelo(Arma *arma, float pos_x, float pos_y) : arma(arma), pos_x(pos_x), pos_y(pos_y) {}
    Arma* getArma() const { return arma; }
};

struct BombaEnSuelo {
    float pos_x;
    float pos_y;
    enum EstadoBombaRonda estado_bomba;
    int tiempo_para_detonar;
    bool acaba_de_detonar;
    bool acaba_de_ser_plantada;
    bool acaba_de_ser_desactivada;

    BombaEnSuelo(float pos_x, float pos_y, enum EstadoBombaRonda  estado, int tiempo_para_detonar, bool acaba_de_detonar, bool acaba_de_ser_plantada, bool acaba_de_ser_desactivada)
        : 
          pos_x(pos_x),
          pos_y(pos_y),
          estado_bomba(estado),
          tiempo_para_detonar(tiempo_para_detonar),
          acaba_de_detonar(acaba_de_detonar),
          acaba_de_ser_plantada(acaba_de_ser_plantada),
          acaba_de_ser_desactivada(acaba_de_ser_desactivada)
    {}
};


struct InfoBomba{
    enum EstadoBombaRonda  estado_bomba;
    float pos_x;
    float pos_y;
    int tiempo_para_detonar;
    bool acaba_de_detonar=false;
    bool acaba_de_ser_plantada=false;
    bool acaba_de_ser_desactivada=false;

}
;
struct InfoJugador {
    int id;
    float pos_x;
    float pos_y;
    float angulo;
    int vida;
    int dinero;
    enum Equipo equipo;
    enum SkinTipos skin_tipo;
    enum ArmaEnMano arma_en_mano;
    bool esta_vivo;
    bool esta_moviendose;
    bool esta_disparando;
    bool esta_plantando_bomba;
    bool esta_desactivando_bomba;
    bool puede_comprar_ya;
    bool acaba_de_comprar_arma;
    bool acaba_de_comprar_balas;
    int balas;
    int eliminaciones_esta_ronda;
    int eliminaciones_totales;
};

struct InfoMunicion {
    int id_quien_disparo;
    float pos_x;
    float pos_y;
    float angulo_disparo;
};

struct InfoArmaEnSuelo {
    enum ArmaEnMano tipo_arma;
    float pos_x;
    float pos_y;
    int municiones;
};

struct Snapshot {
    std::vector<InfoJugador> info_jugadores;
    std::vector<InfoMunicion> balas_disparadas;
    std::vector<InfoArmaEnSuelo> armas_sueltas;
    InfoBomba bomba_en_suelo;
    int tiempo_restante;
    enum Equipo equipo_ganador;

    Snapshot() : info_jugadores(), balas_disparadas() {}

    Snapshot(std::vector<Jugador *> &jugadores, std::vector<Municion> &balas, std::vector<ArmaEnSuelo> armas, BombaEnSuelo bomba_suelta,auto& t_restante, enum Equipo equipo_ganador) {
        for (const auto& jugador_ptr : jugadores) {
            InfoJugador info_jugador;
            info_jugador.id = jugador_ptr->getId();
            info_jugador.pos_x = jugador_ptr->getX();
            info_jugador.pos_y = jugador_ptr->getY();
            info_jugador.angulo = jugador_ptr->getAngulo();
            info_jugador.vida = jugador_ptr->get_vida();
            info_jugador.dinero = jugador_ptr->get_dinero();
            info_jugador.equipo = jugador_ptr->get_equipo();
            info_jugador.skin_tipo = jugador_ptr->get_skin_tipo();
            info_jugador.arma_en_mano = jugador_ptr->get_codigo_arma_en_mano();
            info_jugador.esta_vivo = jugador_ptr->esta_vivo();
            info_jugador.esta_moviendose = jugador_ptr->esta_moviendose();
            info_jugador.esta_disparando = jugador_ptr->esta_disparando();
            info_jugador.esta_plantando_bomba = jugador_ptr->esta_plantando_bomba();
            info_jugador.esta_desactivando_bomba = jugador_ptr->esta_desactivando_bomba();
            info_jugador.puede_comprar_ya = jugador_ptr->puede_comprar_ahora();
            info_jugador.acaba_de_comprar_arma = jugador_ptr->compro_arma_ahora();
            info_jugador.acaba_de_comprar_balas = jugador_ptr->compro_balas_ahora();
            info_jugador.balas = jugador_ptr->get_arma_actual()->getBalas();
            info_jugador.eliminaciones_esta_ronda = jugador_ptr->get_eliminaciones_esta_ronda();
            info_jugador.eliminaciones_totales = jugador_ptr->get_eliminaciones_totales();

            info_jugadores.push_back(info_jugador);
        }
        for (const auto& bala : balas) {
            InfoMunicion info_municion;
            info_municion.id_quien_disparo = bala.quien_disparo();
            info_municion.pos_x = bala.getPosX();
            info_municion.pos_y = bala.getPosY();
            info_municion.angulo_disparo = bala.getAnguloDisparo();

            balas_disparadas.push_back(info_municion);
        }
        for (const auto& arma : armas) {
            InfoArmaEnSuelo info_arma;
            info_arma.tipo_arma = arma.getArma()->getCodigoArma();
            info_arma.pos_x = arma.pos_x;
            info_arma.pos_y = arma.pos_y;
            info_arma.municiones = arma.getArma()->getBalas();

            armas_sueltas.push_back(info_arma);
        }
        bomba_en_suelo.pos_x = bomba_suelta.pos_x;
        bomba_en_suelo.pos_y = bomba_suelta.pos_y;
        bomba_en_suelo.estado_bomba = bomba_suelta.estado_bomba;
        bomba_en_suelo.tiempo_para_detonar = bomba_suelta.tiempo_para_detonar;
        bomba_en_suelo.acaba_de_detonar = bomba_suelta.acaba_de_detonar;
        bomba_en_suelo.acaba_de_ser_plantada = bomba_suelta.acaba_de_ser_plantada;
        bomba_en_suelo.acaba_de_ser_desactivada = bomba_suelta.acaba_de_ser_desactivada;
    
        this->tiempo_restante = t_restante;
        this->equipo_ganador = equipo_ganador;
    }


    InfoJugador* getJugadorPorId(int client_id) {
        for (auto& jugador : info_jugadores) {
            if (jugador.id == client_id)
                return &jugador;
        }
        return nullptr;
    }
};

#endif