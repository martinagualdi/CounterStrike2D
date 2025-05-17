#ifndef ESTADO_JUEGO_H_
#define ESTADO_JUEGO_H_

#include <vector>
#include "jugador.h" 

struct Snapshot
{
    std::vector<Jugador> info_jugadores;

    Snapshot() : info_jugadores() {}

    explicit Snapshot(std::vector<Jugador*>& jugadores) {
        info_jugadores.reserve(jugadores.size());
        for (auto* jugador_ptr : jugadores) {
            info_jugadores.push_back(*jugador_ptr);
        }
    }

};

#endif