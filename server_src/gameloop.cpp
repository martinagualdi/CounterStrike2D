#include "gameloop.h"
#include <cmath>

#define VELOCIDAD 0.3

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores, std::string yaml_partida)
    : queue_comandos(queue_comandos), queues_jugadores(queues_jugadores), jugadores(),
      activo(true), balas_disparadas(), ultimo_unido_ct(false), mapa(yaml_partida) {}

void GameLoop::agregar_jugador_a_partida(const int id) {
    Jugador *jugador = new Jugador(id);
    if (ultimo_unido_ct){ 
        jugador->establecer_equipo(TT);
        jugador->establecer_skin(PHEONIX); // Asignar skin por defecto a los Terroristas
    } else {
        jugador->establecer_equipo(CT);
        jugador->establecer_skin(SEAL_FORCE); // Asignar skin por defecto a los Contra Terroristas
    }
    ultimo_unido_ct = !ultimo_unido_ct; 
    jugadores.push_back(jugador);
}

bool GameLoop::jugador_colisiones_con_mapa(float nuevo_x, float nuevo_y) {
    return mapa.jugador_colision_contra_pared(nuevo_x, nuevo_y);
}

bool GameLoop::bala_golpea_jugador(const Municion &bala) {
    for (Jugador *jugador : jugadores) {
        if (jugador->getId() == bala.quien_disparo()) return false; // No se puede disparar a uno mismo
        float pos_x = bala.getPosX();
        float pos_y = bala.getPosY();
        float max_pos_x_jugador = jugador->getX() + 20; 
        float min_pos_x_jugador = jugador->getX() - 20;
        float max_pos_y_jugador = jugador->getY() + 20; 
        float min_pos_y_jugador = jugador->getY() - 20;
        if (pos_x >= min_pos_x_jugador && pos_x <= max_pos_x_jugador &&
            pos_y >= min_pos_y_jugador && pos_y <= max_pos_y_jugador) {
            jugador->recibir_danio(10);
            
            std::cout << "Jugador de ID: " << jugador->getId() << " ha sido impactado por la bala del jugador de ID: " 
                << bala.quien_disparo() << " || ";
            std::cout << "Vida restante del jugador: " << jugador->get_vida() << std::endl;
            return true;
        }
    }
    return false;
}

void GameLoop::ejecutar_movimiento(Jugador *jugador) {

    float velocidad_diagonal = VELOCIDAD / std::sqrt(2.0f);
    float nuevo_x = jugador->getX();
    float nuevo_y = jugador->getY();
    switch (jugador->getMovimiento()) {
        case ARRIBA:
            //jugador->setY(jugador->getY() + VELOCIDAD);
            nuevo_y += VELOCIDAD;
            break;
        case ABAJO: {
            // float y = jugador->getY() - VELOCIDAD;
            // if(y > 0)
            //     jugador->setY(y);
            nuevo_y -= VELOCIDAD;
            break;
        }
        case IZQUIERDA: {
            // float x = jugador->getX() - VELOCIDAD;
            // if(x > 0)
            //     jugador->setX(x);
            nuevo_x -= VELOCIDAD;
            break;
        }
        case DERECHA:
            //jugador->setX(jugador->getX() + VELOCIDAD);
            nuevo_x += VELOCIDAD;
            break;
        case DIAGONAL_SUP_IZQ:
            // jugador->setX(jugador->getX() - velocidad_diagonal);
            // jugador->setY(jugador->getY() + velocidad_diagonal);
            nuevo_x -= velocidad_diagonal;
            nuevo_y += velocidad_diagonal;
            break;
        case DIAGONAL_SUP_DER:
            // jugador->setX(jugador->getX() + velocidad_diagonal);
            // jugador->setY(jugador->getY() + velocidad_diagonal);
            nuevo_x += velocidad_diagonal;
            nuevo_y += velocidad_diagonal;
            break;
        case DIAGONAL_INF_IZQ:
            // jugador->setX(jugador->getX() - velocidad_diagonal);
            // jugador->setY(jugador->getY() - velocidad_diagonal);
            nuevo_x -= velocidad_diagonal;
            nuevo_y -= velocidad_diagonal;
            break;
        case DIAGONAL_INF_DER:
            // jugador->setX(jugador->getX() + velocidad_diagonal);
            // jugador->setY(jugador->getY() - velocidad_diagonal);
            nuevo_x += velocidad_diagonal;
            nuevo_y -= velocidad_diagonal;
            break;
        case DETENER:
            break;
    }
    if (jugador_colisiones_con_mapa(nuevo_x, nuevo_y)) {
        // Si hay colisión, no se actualizan las coordenadas
        return;
    }
    jugador->setX(nuevo_x);
    jugador->setY(nuevo_y);

    if ((!jugador->esta_moviendose() && jugador->getMovimiento() != DETENER)
        || (jugador->esta_moviendose() && jugador->getMovimiento() == DETENER)){
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
                if(bala_golpea_jugador(bala) || mapa.bala_colision_contra_pared(bala.getPosX(), bala.getPosY())) {
                    balas_disparadas.erase(balas_disparadas.begin() + i);
                } else {
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