#include "gameloop.h"
#include <cmath>

#define VELOCIDAD 0.3

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores, std::string yaml_partida)
    : queue_comandos(queue_comandos), queues_jugadores(queues_jugadores), jugadores(),
      activo(true), balas_disparadas(), ultimo_unido_ct(false), mapa(yaml_partida), ronda_actual(1), equipo_ct(), equipo_tt(), rondas_ganadas_ct(0),
      rondas_ganadas_tt(0), bomba_plantada(false) {}

void GameLoop::agregar_jugador_a_partida(const int id) {
    Jugador *jugador = new Jugador(id);
    if (ultimo_unido_ct){ 
        jugador->establecer_equipo(TT);
        jugador->establecer_skin(SKIN1); // Asignar skin por defecto a los Terroristas
        equipo_tt.push_back(jugador);
    } else {
        jugador->establecer_equipo(CT);
        jugador->establecer_skin(SKIN1); // Asignar skin por defecto a los Contra Terroristas
        equipo_ct.push_back(jugador);
    }
    std::vector<float> posicion_inicial = mapa.dar_posiciones_iniciales(ultimo_unido_ct);
    jugador->definir_spawn(posicion_inicial[0], posicion_inicial[1]); // Posición inicial por defecto
    ultimo_unido_ct = !ultimo_unido_ct; 
    jugadores.push_back(jugador);
}

bool GameLoop::jugador_colisiones_con_mapa(float nuevo_x, float nuevo_y) {
    return mapa.jugador_colision_contra_pared(nuevo_x, nuevo_y);
}

bool GameLoop::bala_golpea_jugador(const Municion &bala) {
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
            nuevo_y += VELOCIDAD;
            if (mapa.limite_alto() < nuevo_y) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case ABAJO:
            nuevo_y -= VELOCIDAD;
            if (nuevo_y < 0) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case IZQUIERDA:
            nuevo_x -= VELOCIDAD;
            if (nuevo_x < 0) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case DERECHA:
            nuevo_x += VELOCIDAD;
            if (mapa.limite_ancho() < nuevo_x) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case DIAGONAL_SUP_IZQ:
            nuevo_x -= velocidad_diagonal;
            nuevo_y += velocidad_diagonal;
            if (nuevo_x < 0 || mapa.limite_alto() < nuevo_y) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case DIAGONAL_SUP_DER:
            nuevo_x += velocidad_diagonal;
            nuevo_y += velocidad_diagonal;
            if (mapa.limite_ancho() < nuevo_x || mapa.limite_alto() < nuevo_y) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case DIAGONAL_INF_IZQ:
            nuevo_x -= velocidad_diagonal;
            nuevo_y -= velocidad_diagonal;
            if (nuevo_x < 0 || nuevo_y < 0) 
                return; // Evitar que el jugador se mueva fuera del mapa
            break;
        case DIAGONAL_INF_DER:
            nuevo_x += velocidad_diagonal;
            nuevo_y -= velocidad_diagonal;
            if (mapa.limite_ancho() < nuevo_x || nuevo_y < 0) 
                return; // Evitar que el jugador se mueva fuera del mapa
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
}

Jugador *GameLoop::findJugador(int id_jugador_buscado) {
    for (Jugador *j : jugadores) {
        if(j->getId() == id_jugador_buscado)
            return j;
    }

    return nullptr;
}

enum Equipo GameLoop::se_termino_ronda() {
    // Verificar si todos los jugadores de un equipo han muerto
    bool ct_vivos = false;
    bool tt_vivos = false;
    if (equipo_ct.empty() || equipo_tt.empty()) {
        return NONE; // Si no hay jugadores en uno de los equipos, la ronda no ha terminado
    }
    for (Jugador *jugador : jugadores) {
        if (jugador->esta_vivo()) {
            if (jugador->get_equipo() == CT) {
                ct_vivos = true;
            } else if (jugador->get_equipo() == TT) {
                tt_vivos = true;
            }
        }
        if (ct_vivos && tt_vivos) {
            break; // Si ambos equipos tienen jugadores vivos, no se ha terminado la ronda
        }
    }

    if (!ct_vivos || !tt_vivos) {
        std::cout << "Ronda terminada. Equipo ganador: " 
                  << (ct_vivos ? "CT" : "TT") << std::endl;
        if (ct_vivos) { 
            rondas_ganadas_ct++;
            return CT;
        } else {
            rondas_ganadas_tt++;
            return TT;
        }
    }
    /*
    HACE FALTA IMPLEMENTAR LA LOGICA DE LA BOMBA
    CON EL TIEMPO DE LA MISMA
    HACE FALTA IMPLEMENTAR LA LOGICA DE FINALIZAR PARTIDA POR TIEMPO
    */
   return NONE;
}

bool GameLoop::jugar_ronda() {
    bool en_juego = true;
    std::cout << "Iniciando ronda " << ronda_actual << std::endl;
    auto t_inicio = std::chrono::steady_clock::now();
    while (activo && en_juego) {
        try {
            for(Jugador *j: jugadores) {
                if (!j->esta_vivo() || !j->esta_disparando()) continue; 
                j->dejar_de_disparar(); // Dejar de disparar para evitar múltiples disparos en un mismo frame
            }
            ComandoDTO comando;
            while (queue_comandos.try_pop(comando)) {
                Jugador* jugador = findJugador(comando.id_jugador);
                if (!jugador || !jugador->esta_vivo()) continue;
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
                    case COMPRAR:
                        if (comando.compra == BALAS_PRIMARIA || comando.compra == BALAS_SECUNDARIA) {
                            if (!jugador->comprarBalas(comando.compra)) {
                                std::cout << "Jugador de ID: " << jugador->getId() << " no tiene dinero suficiente para comprar balas." << std::endl;
                            }
                        } else {
                            if (!jugador->comprarArma(comando.compra)) {
                                std::cout << "Jugador de ID: " << jugador->getId() << " no tiene dinero suficiente para comprar el arma." << std::endl;
                            }
                        }
                        break;
                    case SELECCIONAR_SKIN:
                        jugador->set_skin_tipo(comando.skin);
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
            enum Equipo eq_ganador = se_termino_ronda();
            if (eq_ganador != NONE) {
                // Reiniciar la ronda
                for (Jugador *jugador : jugadores) {
                    jugador->reiniciar();
                }
                balas_disparadas.clear();
                ronda_actual++;
                en_juego = false; // Terminar el bucle de juego
            }
            auto t_actual = std::chrono::steady_clock::now();
            auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_actual - t_inicio).count();
            Snapshot snapshot(jugadores, balas_disparadas, t_transcurrido, eq_ganador);
            queues_jugadores.broadcast(snapshot);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
        } catch (const ClosedQueue &) {
            return false;
        }
    }
    return true;
}

void GameLoop::run() {
    while (equipo_ct.empty() && equipo_tt.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Esperar hasta que haya jugadores
    }
    while (activo) {
        jugar_ronda();
    }
}