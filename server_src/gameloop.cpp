#include "gameloop.h"
#include <cmath>

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores)
    : queue_comandos(queue_comandos), queues_jugadores(queues_jugadores), jugadores(),
      activo(true) {}

void GameLoop::agregar_jugador_a_partida(const int id) {
    Jugador *jugador = new Jugador(id);
    jugadores.push_back(jugador);
}

float GameLoop::calcularAngulo(int x_personaje, int y_personaje, int x_mouse, int y_mouse) {
    int dx = x_mouse - x_personaje;
    int dy = y_mouse - y_personaje;
    float angulo_radianes = std::atan2(dy, dx);
    float angulo_grados = angulo_radianes * (180.0 / M_PI);
    return angulo_grados;
}

void GameLoop::ejecutar_movimiento(Jugador *jugador, enum Movimiento movimiento) {
    switch (movimiento) {
    case ARRIBA:
        jugador->setY(jugador->getY() + 5);
        break;
    case ABAJO:
        jugador->setY(jugador->getY() - 5);
        break;
    case IZQUIERDA:
        jugador->setX(jugador->getX() - 5);
        break;
    case DERECHA:
        jugador->setX(jugador->getX() + 5);
        break;
    }
}

void GameLoop::ejecutar_rotacion(Jugador *jugador, int mouseX, int mouseY) {
    float angulo = calcularAngulo(jugador->getX(), jugador->getY(), mouseX, mouseY);
    jugador->setAngulo(angulo);
}

void GameLoop::run() {
    while (activo) {
        try {
            ComandoDTO comando;
            if (queue_comandos.try_pop(comando)) {
                for (Jugador *jugador : jugadores) {
                    if (jugador->comparar_id(comando.id_jugador)) {
                        switch (comando.tipo) {
                        case MOVIMIENTO:
                            ejecutar_movimiento(jugador, comando.movimiento);
                            break;
                        case ROTACION:
                            ejecutar_rotacion(jugador, comando.mouseX, comando.mouseY);
                            break;
                        case DISPARO:
                            //...
                            break;
                        default:
                            break;
                        }
                    }
                }

                Snapshot snapshot(jugadores);

                queues_jugadores.broadcast(snapshot);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        } catch (const ClosedQueue &) {
            break;
        }
    }
}