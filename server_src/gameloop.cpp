#include "gameloop.h"
#include <cmath>

#define VELOCIDAD 0.3

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores)
    : queue_comandos(queue_comandos), queues_jugadores(queues_jugadores), jugadores(),
      activo(true) {}

void GameLoop::agregar_jugador_a_partida(const int id) {
    Jugador *jugador = new Jugador(id);
    jugadores.push_back(jugador);
}

void GameLoop::ejecutar_movimiento(Jugador *jugador) {

    float velocidad_diagonal = VELOCIDAD / std::sqrt(2.0f);

    switch (jugador->getMovimiento()) {
        case ARRIBA:
            jugador->setY(jugador->getY() + VELOCIDAD);
            break;
        case ABAJO:
            float y = jugador->getY() - VELOCIDAD;
            if(y > 0)
                jugador->setY(y);
            break;
        case IZQUIERDA:
            float x = jugador->getX() - VELOCIDAD;
            if(x > 0)
                jugador->setX(x);
            break;
        case DERECHA:
            jugador->setX(jugador->getX() + VELOCIDAD);
            break;
        case DIAGONAL_SUP_IZQ:
            jugador->setX(jugador->getX() - velocidad_diagonal);
            jugador->setY(jugador->getY() + velocidad_diagonal);
            break;
        case DIAGONAL_SUP_DER:
            jugador->setX(jugador->getX() + velocidad_diagonal);
            jugador->setY(jugador->getY() + velocidad_diagonal);
            break;
        case DIAGONAL_INF_IZQ:
            jugador->setX(jugador->getX() - velocidad_diagonal);
            jugador->setY(jugador->getY() - velocidad_diagonal);
            break;
        case DIAGONAL_INF_DER:
            jugador->setX(jugador->getX() + velocidad_diagonal);
            jugador->setY(jugador->getY() - velocidad_diagonal);
            break;
        case DETENER:
            break;
    }
}

Jugador *GameLoop::findJugador(int id_jugador_buscado) {
    for (Jugador *j : jugadores) {
        if(j->getId() == id_jugador_buscado)
            return j;
    }

    return nullptr;
}

void GameLoop::run() {
    while (activo) {
        try {
            ComandoDTO comando;
            while (queue_comandos.try_pop(comando)) {
                Jugador* jugador = findJugador(comando.id_jugador);
                if (!jugador) continue;
                switch (comando.tipo) {
                    case MOVIMIENTO:
                        jugador->setMovimiento(comando.movimiento);
                        break;
                    case ROTACION:
                        jugador->setAngulo(comando.angulo);
                        break;
                    case DISPARO:
                        // logica disparo
                        std::cout << "Angulo recibido: " << comando.angulo << std::endl;
                        break;
                    default:
                        break;
                }
            }

            for (Jugador *jugador : jugadores) {
                if(jugador->estaMoviendo()){
                    ejecutar_movimiento(jugador);
                }
            }

            Snapshot snapshot(jugadores);
            queues_jugadores.broadcast(snapshot);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
        } catch (const ClosedQueue &) {
            break;
        }
    }
}