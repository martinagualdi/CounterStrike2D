#include "gameloop.h"
#include <cmath>

#define VELOCIDAD 0.3

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores, std::string yaml_partida):
    queue_comandos(queue_comandos), 
    queues_jugadores(queues_jugadores), 
    jugadores(),
    cant_max_jugadores(Configuracion::get<int>("cantidad_max_jugadores")),
    cant_min_ct(Configuracion::get<int>("cantidad_min_ct")),
    cant_max_ct(Configuracion::get<int>("cantidad_max_ct")),
    cant_min_tt(Configuracion::get<int>("cantidad_min_tt")),
    cant_max_tt(Configuracion::get<int>("cantidad_max_tt")),
    activo(true), 
    balas_disparadas(), 
    ultimo_unido_ct(false), 
    mapa(yaml_partida),
    tiempo_max_ronda(Configuracion::get<int>("tiempo_por_ronda")),
    tiempo_max_comprar(Configuracion::get<int>("tiempo_max_para_comprar")),
    algun_jugador_puede_comprar(false),
    ronda_actual(0), 
    cant_rondas(Configuracion::get<int>("ronda_por_partida")),
    rondas_por_equipo(Configuracion::get<int>("rondas_por_bando")),
    equipo_ct(), 
    equipo_tt(), 
    rondas_ganadas_ct(0),
    rondas_ganadas_tt(0), 
    bomba_plantada(false),
    armas_en_suelo() {}

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

void GameLoop::volver_jugadores_a_spawn() {
    for (Jugador *jugador : jugadores) {
        jugador->reiniciar();
    }
}

void GameLoop::cargar_dinero_por_eliminaciones() {
    for (Jugador *jugador : jugadores) {
        jugador->finalizar_ronda();
    }
}

bool GameLoop::jugador_colisiones_con_mapa(float nuevo_x, float nuevo_y) {
    return mapa.jugador_colision_contra_pared(nuevo_x, nuevo_y);
}

bool GameLoop::bala_golpea_jugador(const Municion &bala, bool esperando) {
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
            int id_tirador = bala.quien_disparo();
            Jugador *jugador_tirador = findJugador(id_tirador);    
            float dx = jugador->getX() - jugador_tirador->getX();
            float dy = jugador->getY() - jugador_tirador->getY();
            float distancia = std::sqrt(dx * dx + dy * dy);
            if (!esperando) {
                jugador->recibir_danio(jugador_tirador->get_arma_actual()->accion(distancia));
                if (!jugador->esta_vivo())
                    jugador_tirador->sumar_eliminacion(); // Si el jugador muere, sumar eliminación al tirador
            }
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

void GameLoop::chequear_estados_jugadores(){
    for(Jugador *j: jugadores) {
        j->reiniciar_compras();
        if (j->esta_disparando())
            j->dejar_de_disparar(); // Dejar de disparar para evitar múltiples disparos en un mismo frame
    }
}

void GameLoop::chequear_si_pueden_comprar(auto t_inicio) {
    auto t_ahora = std::chrono::steady_clock::now();
    auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_ahora - t_inicio).count();
    if (t_transcurrido <= tiempo_max_comprar) {
        size_t cantidad = 0;
        for (Jugador *jugador : jugadores) {
            if (mapa.en_posicion_de_compra(jugador->getX(), jugador->getY(), jugador->get_equipo())) {
                jugador->en_posicion_de_compra(true);
                cantidad++;
                if (!algun_jugador_puede_comprar)
                    algun_jugador_puede_comprar = true; // Al menos un jugador puede comprar
            } else {
                jugador->en_posicion_de_compra(false);
            }
        }
        if (cantidad == 0) 
            algun_jugador_puede_comprar = false; // Si no hay jugadores en posición de compra, ninguno puede comprar
    } else {
        if (algun_jugador_puede_comprar) {
            for (Jugador *jugador : jugadores) {
                jugador->en_posicion_de_compra(false); // Desactivar la compra para todos los jugadores
            }
            algun_jugador_puede_comprar = false; // Ningún jugador puede comprar
        }
    }
}

void GameLoop::ejecucion_comandos_recibidos() {
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
                if (jugador->puede_disparar()){
                    if (jugador->get_codigo_arma_en_mano() == M3){
                        jugador->disparar();
                        for (int i = 0; i < 3; i++) {
                            Municion bala_disparada(comando.id_jugador, jugador->getX(), jugador->getY(), comando.angulo + (i - 1) * 5);
                            balas_disparadas.push_back(bala_disparada);
                        }
                        break;
                    }
                    if (jugador->get_codigo_arma_en_mano() == AK_47) {
                        Ak47 *ak47 = dynamic_cast<Ak47 *>(jugador->get_arma_actual());
                        // Solo inicia la ráfaga si no hay una en curso y puede disparar
                        if (!ak47->hay_rafaga()) {
                            ak47->agregarMunicion(1); // Disparar disminuye la municion sin hacer acción, entonces debo agregar una bala
                            ak47->iniciar_rafaga(comando.angulo, comando.id_jugador);
                        }
                        break;
                    }
                    jugador->disparar();
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
                if (!jugador->puede_comprar_ahora()) {
                    std::cout << "Jugador de ID: " << jugador->getId() << " no puede comprar en este momento." << std::endl;
                    break;; // Si el jugador no puede comprar, no se procesa el comando
                }
                if (comando.compra == BALAS_PRIMARIA || comando.compra == BALAS_SECUNDARIA) {
                    if (!jugador->comprarBalas(comando.compra)) {
                        std::cout << "Jugador de ID: " << jugador->getId() << " no tiene dinero suficiente para comprar balas o no tiene arma principal." << std::endl;
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
            case DESCONECTAR:
                // manejar desconexion de un cliente
                break;
            case ACCION_SOBRE_BOMBA:
                if(comando.estado_bomba == ACCIONANDO){
                    // Posibles casos a manjear:
                    // * Un TT comenzo a plantar la bomba.
                    // * Un CT comenzo a desactivar la bomba.
                } else if(comando.estado_bomba == DETENIDO){
                    // Posibles casos a manejar:
                    // * Dejar de accionar si no llego a concretar el plantado/desactivado.
                    // * Nada si se pudo realizar el plantado/accionado.
                }
                break;
            case DROPEAR: {
                ArmaDeFuego *arma_suelta = jugador->soltar_arma_pricipal();
                if (arma_suelta)
                    armas_en_suelo.push_back(ArmaEnSuelo(arma_suelta, jugador->getX(), jugador->getY())); 
                break;
            }
            case LEVANTAR: {
                float max_pos_x_jugador = jugador->getX() + 20; 
                float min_pos_x_jugador = jugador->getX() - 20;
                float max_pos_y_jugador = jugador->getY() + 20; 
                float min_pos_y_jugador = jugador->getY() - 20;
                size_t i = 0;
                while (i < armas_en_suelo.size()) {
                    ArmaEnSuelo &arma = armas_en_suelo[i];
                    if (arma.pos_x >= min_pos_x_jugador && arma.pos_x <= max_pos_x_jugador &&
                        arma.pos_y >= min_pos_y_jugador && arma.pos_y <= max_pos_y_jugador) {
                        ArmaDeFuego *arma_suelta = jugador->levantar_arma(arma.getArma());
                        armas_en_suelo.erase(armas_en_suelo.begin() + i); 
                        if (arma_suelta)
                            armas_en_suelo.push_back(ArmaEnSuelo(arma_suelta, jugador->getX(), jugador->getY()));
                        break; // Salir del bucle una vez que se levanta un arma
                    }
                    i++;
                }
                break;
            }
            default:
                break;
        }
    }
}

void GameLoop::disparar_rafagas_restantes() {
    for (Jugador *jugador : jugadores) {
        if (jugador->get_codigo_arma_en_mano() == AK_47) {
            Ak47 *ak47 = dynamic_cast<Ak47 *>(jugador->get_arma_actual());
            if (ak47->hay_rafaga()) {
                auto ahora = std::chrono::steady_clock::now();
                auto& proximo = ak47->get_proximo_disparo_rafaga();
                if (ahora >= proximo) {
                    jugador->disparar();
                    Municion bala_disparada(
                        ak47->get_id_jugador_rafaga(),
                        jugador->getX(),
                        jugador->getY(),
                        ak47->get_ultimo_angulo_rafaga()
                    );
                    balas_disparadas.push_back(bala_disparada);

                    ak47->tick_rafaga();
                    proximo = ahora + std::chrono::milliseconds(80); // 80ms entre balas
                }
            }
        }
        ejecutar_movimiento(jugador);
    }
}

void GameLoop::chequear_colisiones(bool esperando) {
    size_t i = 0;
    while ( i < balas_disparadas.size()) {
        Municion &bala = balas_disparadas[i];
        if(bala_golpea_jugador(bala, esperando) || mapa.bala_colision_contra_pared(bala.getPosX(), bala.getPosY())) {
            balas_disparadas.erase(balas_disparadas.begin() + i);
        } else {
            bala.setPosX(bala.getPosX() + std::cos(bala.getAnguloDisparo() * M_PI / 180.0f) * 8);
            bala.setPosY(bala.getPosY() + std::sin(bala.getAnguloDisparo() * M_PI / 180.0f) * 8);
        }
        i++;
    }
}

void GameLoop::chequear_si_equipo_gano(enum Equipo& eq_ganador, bool& en_juego) {
    if (eq_ganador != NONE) {
        // Reiniciar la ronda
        volver_jugadores_a_spawn();
        cargar_dinero_por_eliminaciones();
        balas_disparadas.clear();
        ronda_actual++;
        en_juego = false; // Terminar el bucle de juego
    }
}

void GameLoop::chequear_si_completaron_equipos(enum Equipo& eq_ganador, bool& en_juego) {
    if (!esperando_jugadores()) {
        en_juego = false;
        ronda_actual++;
        volver_jugadores_a_spawn();
    }
    for (Jugador *jugador : jugadores) {
        if (!jugador->esta_vivo())
            jugador->reiniciar();
    }
    if (eq_ganador != NONE)
        eq_ganador = NONE;
}

bool GameLoop::jugar_ronda(bool esperando) {
    bool en_juego = true;
    std::cout << "Iniciando ronda " << ronda_actual << std::endl;
    auto t_inicio = std::chrono::steady_clock::now();
    while (activo && en_juego) {
        try {
            chequear_estados_jugadores();
            if (!esperando)
                chequear_si_pueden_comprar(t_inicio);
            ejecucion_comandos_recibidos();
            disparar_rafagas_restantes();
            chequear_colisiones(esperando);
            enum Equipo eq_ganador = se_termino_ronda();
            if (esperando)
                chequear_si_completaron_equipos(eq_ganador, en_juego);
            else
                chequear_si_equipo_gano(eq_ganador, en_juego);
            
            auto t_actual = std::chrono::steady_clock::now();
            auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_actual - t_inicio).count();
            int t_restante = tiempo_max_ronda - t_transcurrido;
            Snapshot snapshot(jugadores, balas_disparadas, armas_en_suelo, (esperando) ? tiempo_max_ronda : t_restante, rondas_ganadas_ct, rondas_ganadas_tt, ronda_actual, cant_rondas, eq_ganador);
            queues_jugadores.broadcast(snapshot);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } catch (const ClosedQueue &) {
            return false;
        }
    }
    return true;
}

bool GameLoop::esperando_jugadores() {
    return (equipo_ct.size() < static_cast<size_t>(cant_min_ct) ||
            equipo_tt.size() < static_cast<size_t>(cant_min_tt));
}

void GameLoop::run() {
    jugar_ronda(true);
    while (activo) {
        jugar_ronda(false);
    }
}