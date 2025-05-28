#ifndef ESTADO_JUEGO_H_
#define ESTADO_JUEGO_H_

#include <vector>
#include <cstddef>
#include "jugador.h"
#include "municion.h"

struct Snapshot {
    std::vector<Jugador> info_jugadores;
    std::vector<Municion> balas_disparadas;

    Snapshot() : info_jugadores() {}

    explicit Snapshot(std::vector<Jugador *> &jugadores) {
        info_jugadores.reserve(jugadores.size());
        for (auto *jugador_ptr : jugadores) {
            info_jugadores.push_back(*jugador_ptr);
        }
    }

    Jugador* getJugadorPorId(int client_id) {
        for (auto& jugador : info_jugadores) {
            if (jugador.getId() == client_id)
                return &jugador;
        }
        return nullptr;
    }

    Snapshot(std::vector<Jugador> &jugadores) : info_jugadores(jugadores) {}

    Snapshot(std::vector<Jugador> &jugadores, std::vector<Municion> &balas_disparadas) : info_jugadores(jugadores), balas_disparadas(balas_disparadas) {}

    void actualizar_snapshot(const Snapshot &otro) {
        info_jugadores.clear();
        info_jugadores.insert(info_jugadores.end(), otro.info_jugadores.begin(),
                              otro.info_jugadores.end());
    }
};

#endif