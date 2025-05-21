#ifndef ESTADO_JUEGO_H_
#define ESTADO_JUEGO_H_

#include <vector>
#include <cstddef>
#include "jugador.h"

struct Snapshot {
    std::vector<Jugador> info_jugadores;

    Snapshot() : info_jugadores() {}

    explicit Snapshot(std::vector<Jugador *> &jugadores) {
        info_jugadores.reserve(jugadores.size());
        for (auto *jugador_ptr : jugadores) {
            info_jugadores.push_back(*jugador_ptr);
        }
    }

    Snapshot(std::vector<Jugador> &jugadores) : info_jugadores(jugadores) {}

    bool son_iguales(const Snapshot &otro) const {
        if (info_jugadores.size() != otro.info_jugadores.size()) {
            return false;
        }
        for (size_t i = 0; i < info_jugadores.size(); ++i) {
            if (info_jugadores[i].getId() != otro.info_jugadores[i].getId() ||
                info_jugadores[i].getX() != otro.info_jugadores[i].getX() ||
                info_jugadores[i].getY() != otro.info_jugadores[i].getY() ||
                info_jugadores[i].getAngulo() != otro.info_jugadores[i].getAngulo()) {
                return false;
            }
        }
        return true;
    }

    void actualizar_snapshot(const Snapshot &otro) {
        info_jugadores.clear();
        info_jugadores.insert(info_jugadores.end(), otro.info_jugadores.begin(),
                              otro.info_jugadores.end());
    }
};

#endif