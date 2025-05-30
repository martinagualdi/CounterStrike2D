#ifndef ESTADO_JUEGO_H_
#define ESTADO_JUEGO_H_

#include <vector>
#include <cstddef>
#include "jugador.h"
#include "municion.h"

/*Structs para enviar la informacion necesaria para dibujar el juevo en "client"*/

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
};

struct InfoMunicion {
    int id_quien_disparo;
    float pos_x;
    float pos_y;
    float angulo_disparo;
};

struct Snapshot {
    std::vector<InfoJugador> info_jugadores;
    std::vector<InfoMunicion> balas_disparadas;

    Snapshot() : info_jugadores(), balas_disparadas() {}

    Snapshot(std::vector<Jugador *> &jugadores, std::vector<Municion> &balas) /*: balas_disparadas(balas_disparadas)*/{
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
    }

    InfoJugador* getJugadorPorId(int client_id) {
        for (auto& jugador : info_jugadores) {
            if (jugador.id == client_id)
                return &jugador;
        }
        return nullptr;
    }

    //Snapshot(std::vector<Jugador> &jugadores) : info_jugadores(jugadores) {}

    //Snapshot(std::vector<Jugador> &jugadores, std::vector<Municion> &balas_disparadas) : info_jugadores(jugadores), balas_disparadas(balas_disparadas) {}

    void actualizar_snapshot(const Snapshot &otro) {
        info_jugadores.clear();
        info_jugadores.insert(info_jugadores.end(), otro.info_jugadores.begin(),
                              otro.info_jugadores.end());
    }
};

#endif