#include "gameloop.h"
#include <cmath>

#define VELOCIDAD 0.3

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores)
    : queue_comandos(queue_comandos), queues_jugadores(queues_jugadores), jugadores(),
      activo(true), balas_disparadas() {}

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
        case ABAJO: {
            float y = jugador->getY() - VELOCIDAD;
            if(y > 0)
                jugador->setY(y);
            break;
        }
        case IZQUIERDA: {
            float x = jugador->getX() - VELOCIDAD;
            if(x > 0)
                jugador->setX(x);
            break;
        }
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

    if ((!jugador->esta_moviendose() && jugador->getMovimiento() != DETENER)
        || (jugador->esta_moviendose() && jugador->getMovimiento() == DETENER)) {
        jugador->cambiar_estado_moviendose(); 
    }
    /*if (jugador->esta_moviendose())
        std::cout << "ESTA MOVIENDOSE " << std::endl;
    else
        std::cout << "NO ESTA MOVIENDOSE " << std::endl;*/
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
                        std::cout << "Angulo recibido: " << comando.angulo << std::endl;
                        if (jugador->disparar() ){
                            Municion bala_disparada(comando.id_jugador, jugador->getX(), jugador->getY(), comando.angulo);
                            balas_disparadas.push_back(bala_disparada);
                        }
                        break;
                    case CAMBIAR_ARMA:
                        jugador->cambiar_arma_en_mano();
                        std::cout << "Jugador de ID: " << jugador->getId() << " ha cambiado su arma a: " 
                                  << jugador->get_nombre_arma_en_mano() << std::endl;
                        break;
                    default:
                        break;
                }
            }

            for (Jugador *jugador : jugadores) {
                ejecutar_movimiento(jugador);
            }
            size_t i = 0;
            while ( i < balas_disparadas.size()) {
                Municion &bala = balas_disparadas[i];
                bool bala_impacto = false;
                for (Jugador *jugador : jugadores) {
                    if (jugador->getId() == bala.quien_disparo()) continue; // No se puede disparar a uno mismo
                    float pos_x = bala.getPosX();
                    float pos_y = bala.getPosY();
                    float max_pos_x_jugador = jugador->getX() + 20; 
                    float min_pos_x_jugador = jugador->getX() - 20;
                    float max_pos_y_jugador = jugador->getY() + 20; 
                    float min_pos_y_jugador = jugador->getY() - 20;
                    if (pos_x >= min_pos_x_jugador && pos_x <= max_pos_x_jugador &&
                        pos_y >= min_pos_y_jugador && pos_y <= max_pos_y_jugador) {
                        jugador->recibir_danio(10);
                        balas_disparadas.erase(balas_disparadas.begin() + i);
                        std::cout << "Jugador de ID: " << jugador->getId() << " ha sido impactado por la bala del jugador de ID: " 
                                  << bala.quien_disparo() << " || ";
                        std::cout << "Vida restante del jugador: " << jugador->get_vida() << std::endl;
                        bala_impacto = true;
                        break; 
                    }
                }
                if (!bala_impacto) {
                    bala.setPosX(bala.getPosX() + std::cos(bala.getAnguloDisparo() * M_PI / 180.0f) * 8);
                    bala.setPosY(bala.getPosY() + std::sin(bala.getAnguloDisparo() * M_PI / 180.0f) * 8);
                }
                i++;
            }
            Snapshot snapshot(jugadores, balas_disparadas);
            queues_jugadores.broadcast(snapshot);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
        } catch (const ClosedQueue &) {
            break;
        }
    }
}