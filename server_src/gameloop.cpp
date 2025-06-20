#include "gameloop.h"
#include <cmath>

#define VELOCIDAD 0.3

GameLoop::GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores, bool activa, std::string yaml_partida):
    queue_comandos(queue_comandos), 
    queues_jugadores(queues_jugadores), 
    jugadores(),
    cant_min_ct(Configuracion::get<int>("cantidad_min_ct")),
    cant_min_tt(Configuracion::get<int>("cantidad_min_tt")),
    activo(activa), 
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
    armas_en_suelo(),
    info_bomba(0.0f, 0.0f, SIN_PLANTAR, Configuracion::get<int>("tiempo_pare_que_explote_bomba"), false, false, false) {}


void GameLoop::agregar_jugador_a_partida(const int id, std::string& nombre) {
    Jugador *jugador = new Jugador(id, nombre);

    bool puede_ct = equipo_ct.size() < static_cast<size_t>(cant_min_ct);
    bool puede_tt = equipo_tt.size() < static_cast<size_t>(cant_min_tt);

    if (puede_ct && puede_tt) {
        // Si puedo unirme a ambos equipos
        if (ultimo_unido_ct) {
            jugador->establecer_equipo(TT);
            equipo_tt.push_back(jugador);
        } else {
            jugador->establecer_equipo(CT);
            equipo_ct.push_back(jugador);
        }
        ultimo_unido_ct = !ultimo_unido_ct;
    } else if (puede_ct) {
        jugador->establecer_equipo(CT);
        equipo_ct.push_back(jugador);
        ultimo_unido_ct = true;
    } else if (puede_tt) {
        jugador->establecer_equipo(TT);
        equipo_tt.push_back(jugador);
        ultimo_unido_ct = false;
    }

    jugador->establecer_skin(SKIN1); // Asignar skin por defecto
    std::vector<float> posicion_inicial = mapa.dar_posiciones_iniciales(jugador->get_equipo());
    jugador->definir_spawn(posicion_inicial[0], posicion_inicial[1]);
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
                if (!jugador->esta_vivo()) {
                    jugador_tirador->sumar_eliminacion(); // Si el jugador muere, sumar eliminación al tirador
                    ArmaDeFuego* arma_que_suelta = jugador->soltar_arma_pricipal();
                    if (arma_que_suelta) {
                        ArmaEnSuelo arma(arma_que_suelta, jugador->getX(), jugador->getY()); // Deja su arma principal en el suelo
                        armas_en_suelo.push_back(arma);
                    }
                }
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
        if (ct_vivos && !bomba_plantada) { 
            rondas_ganadas_ct++;
            return CT;
        } if (tt_vivos){
            rondas_ganadas_tt++;
            return TT;
        }
    }
    if(bomba) {
        if (bomba->fueDesactivada()){
            rondas_ganadas_ct++;
            return CT;
        }
        if (bomba->detonar()){
            info_bomba= BombaEnSuelo(bomba->getX(),bomba->getY(),DETONADA,0,true,false,false);
            explosion();
            rondas_ganadas_tt++;
            return TT;
        }
    }
    /*
    HACE FALTA IMPLEMENTAR LA LOGICA DE FINALIZAR PARTIDA POR TIEMPO
    */
   return NONE;
}

void GameLoop::explosion(){
    if (!bomba) return;
    for (Jugador* jugador : jugadores) {
        if (jugador->get_equipo() == CT && jugador->esta_vivo()) {
            float dx = jugador->getX() - bomba->getX();
            float dy = jugador->getY() - bomba->getY();
            float distancia = std::sqrt(dx * dx + dy * dy);
            jugador->recibir_danio(bomba->accion(distancia));
        }
    }
}

void GameLoop::chequear_estados_jugadores(){
    for(Jugador *j: jugadores) {
        j->reiniciar_compras();
        if (j->esta_disparando())
            j->dejar_de_disparar(); // Dejar de disparar para evitar múltiples disparos en un mismo frame
        if (j->get_equipo() == TT && j->tiene_la_bomba()) 
            j->set_puede_plantar(mapa.verificar_zona_bombas(j->getX(), j->getY()));
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
                            ak47->agregarMunicion(1); 
                            ak47->iniciar_rafaga(comando.angulo, comando.id_jugador);
                        }
                        break;
                    }
                    jugador->disparar();
                    if (jugador->get_codigo_arma_en_mano() == CUCHILLO) {
                        // El cuchillo no dispara, pero se puede usar para atacar
                        Municion bala_disparada(comando.id_jugador, jugador->getX(), jugador->getY(), comando.angulo);
                        bala_golpea_jugador(bala_disparada, false); // Verifica si golpea a un jugador
                        break;
                    }
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
                if (comando.estado_bomba == ACCIONANDO) {
                    if (!bomba_plantada && jugador->get_equipo() == TT &&
                        mapa.verificar_zona_bombas(jugador->getX(), jugador->getY())) {
                        jugador->empezar_a_plantar();
                        jugador_plantando = jugador;
                        tiempo_inicio_plantado = std::chrono::steady_clock::now();
                        std::cout << "Jugador " << jugador->getId() << " comenzó a plantar la bomba." << std::endl;
                    }else if(bomba_plantada && jugador->get_equipo() == CT &&
                        mapa.verificar_zona_bombas(jugador->getX(), jugador->getY())) {
                        //Desactivar bomba
                        jugador->empezar_a_desactivar();
                        jugador_desactivando = jugador;
                        tiempo_inicio_desactivado = std::chrono::steady_clock::now();
                        std::cout << "Jugador " << jugador->getId() << " comenzó a desactivar la bomba." << std::endl;
                        
                    }
                }
                else if(comando.estado_bomba == DETENIDO){
                    if (jugador == jugador_plantando) {
                        jugador_plantando = nullptr;
                        jugador->cancelar_plantado_bomba();
                        std::cout << "Jugador " << jugador->getId() << " interrumpió el plantado de la bomba." << std::endl;
                    }else if (jugador==jugador_desactivando){
                        jugador_desactivando = nullptr;
                        jugador->cancelar_desactivado_bomba();
                        bomba->reiniciar();
                        std::cout << "Jugador " << jugador->getId() << " interrumpió el desactivado de la bomba." << std::endl;
                    }
                    
                }
                break;
            case DROPEAR: {
                if (jugador->get_codigo_arma_en_mano() == BOMBA_TT) {
                        Bomba* arma_suelta = jugador->soltar_bomba();
                        if (arma_suelta) {
                            armas_en_suelo.push_back(ArmaEnSuelo(arma_suelta, jugador->getX(), jugador->getY()));
                        }
                         
                    break;
                }
                
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
                        if (arma.getArma()->getCodigoArma() == BOMBA_TT && jugador->get_equipo() == TT && !bomba_plantada){
                            Bomba* bomba_en_suelo = jugador->levantar_bomba(arma.getArma());
                            if(bomba_en_suelo ) {
                                jugador->asignar_bomba();
                                armas_en_suelo.erase(armas_en_suelo.begin() + i); 
                                armas_en_suelo.push_back(ArmaEnSuelo(bomba_en_suelo, jugador->getX(), jugador->getY()));
                                break;
                            }
                        
                        }else{
                            ArmaDeFuego* arma_suelta = jugador->levantar_arma(arma.getArma());
                            armas_en_suelo.erase(armas_en_suelo.begin() + i); 
                            if (arma_suelta){
                            armas_en_suelo.push_back(ArmaEnSuelo(arma_suelta, jugador->getX(), jugador->getY()));}
                            break;
                        }

                         // Salir del bucle una vez que se levanta un arma
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
        balas_disparadas.clear();
        ronda_actual++;
        en_juego = false; // Terminar el bucle de juego
    }
}

void GameLoop::reiniciar_estado_bomba(){
    if(bomba){
        bomba->reiniciar();
        info_bomba= BombaEnSuelo(0.0f, 0.0f, SIN_PLANTAR, Configuracion::get<int>("tiempo_pare_que_explote_bomba"), false, false, false);
        jugador_desactivando = nullptr;
        jugador_plantando = nullptr;
        bomba = nullptr;
        bomba_plantada = false;
        tiempo_inicio_desactivado = std::chrono::steady_clock::time_point();
        tiempo_inicio_plantado = std::chrono::steady_clock::time_point();
    }
}

void GameLoop::chequear_bomba_plantada() {
    if (jugador_plantando) {
        auto t_actual = std::chrono::steady_clock::now();
        auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_actual - tiempo_inicio_plantado).count();
        if (t_transcurrido >= Configuracion::get<int>("tiempo_plantado_bomba")) {
            bomba_plantada = true;
            jugador_plantando->plantar_bomba(jugador_plantando->getX(), jugador_plantando->getY());
            int id_jugador = jugador_plantando->getId();
            bomba= jugador_plantando->soltar_bomba(); 
            if (bomba) {
                info_bomba = BombaEnSuelo (jugador_plantando->getX(), jugador_plantando->getY(), PLANTADA, bomba->getTiempoParaDetonar(),
            false, true, false);
            }
            jugador_plantando = nullptr;
            std::cout << "Bomba plantada por el jugador " << id_jugador << std::endl;
            
        }
    }
}

void GameLoop::chequear_bomba_desactivada(){
    if (jugador_desactivando) {
        auto t_actual = std::chrono::steady_clock::now();
        auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_actual - tiempo_inicio_desactivado).count();
        if (t_transcurrido >= Configuracion::get<int>("tiempo_desactivacion_bomba")) {
            bomba_plantada = false;
            info_bomba = BombaEnSuelo( bomba->getX(), bomba->getY(), DESACTIVADA, 0, false, false, true);
            jugador_desactivando->desactivar_bomba();
            bomba->desactivar();
            bomba->setPlantada(false);
            int id_jugador = jugador_desactivando->getId();
            jugador_desactivando= nullptr;
            std::cout << "Bomba desactivada por el jugador " << id_jugador<< std::endl;
            
        }
    }else{
       if (bomba_plantada){
            info_bomba = BombaEnSuelo(bomba->getX(), bomba->getY(), PLANTADA, bomba->getTiempoParaDetonar(), false, false, false);
       }else{
        info_bomba = BombaEnSuelo(0.0f,0.0f,SIN_PLANTAR,Configuracion::get<int>("tiempo_desactivacion_bomba"),false,false,false);
       }
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

void GameLoop::realizar_cambio_equipo_si_es_necesario() {
    if (rondas_por_equipo + 1 == ronda_actual) {
        int aux;
        aux = cant_min_ct;
        cant_min_ct = cant_min_tt;
        cant_min_tt = aux;
        aux = rondas_ganadas_ct;
        rondas_ganadas_ct = rondas_ganadas_tt;
        rondas_ganadas_tt = aux;
        for (Jugador *jugador : jugadores) {
            if (jugador->get_equipo() == CT) {
                jugador->establecer_equipo(TT);
                equipo_tt.push_back(jugador);
                equipo_ct.erase(std::remove(equipo_ct.begin(), equipo_ct.end(), jugador), equipo_ct.end());
            } else if (jugador->get_equipo() == TT) {
                jugador->establecer_equipo(CT);
                equipo_ct.push_back(jugador);
                equipo_tt.erase(std::remove(equipo_tt.begin(), equipo_tt.end(), jugador), equipo_tt.end());
            }
            std::vector<float> posicion_inicial = mapa.dar_posiciones_iniciales(jugador->get_equipo());
            jugador->definir_spawn(posicion_inicial[0], posicion_inicial[1]);
        }
    }
}

bool GameLoop::chequear_si_termino_partida() {
    return ronda_actual > cant_rondas;
}

void GameLoop::esperar_entre_rondas(int segundos, int t_restante, enum Equipo eq_ganador){
    auto t_inicio = std::chrono::steady_clock::now();
    while (true) {
        auto t_actual = std::chrono::steady_clock::now();
        auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_actual - t_inicio).count();
        if (t_transcurrido >= segundos) break;

        // Seguí enviando snapshots para que el cliente pueda actualizar sonidos/animaciones
        Snapshot snapshot(
            jugadores, balas_disparadas, armas_en_suelo, info_bomba,
            t_restante, rondas_ganadas_ct, rondas_ganadas_tt, ronda_actual, cant_rondas, eq_ganador,
            chequear_si_termino_partida()
        );
        
        queues_jugadores.broadcast(snapshot);
        if(info_bomba.acaba_de_detonar){
            info_bomba= BombaEnSuelo(0.0f, 0.0f, DETONADA, 0, false, false, false);
        }    

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }
    armas_en_suelo.clear();
    reiniciar_estado_bomba();
    realizar_cambio_equipo_si_es_necesario();
    volver_jugadores_a_spawn();
    cargar_dinero_por_eliminaciones();
}

bool GameLoop::esperando_jugadores() {
    return (equipo_ct.size() < static_cast<size_t>(cant_min_ct) ||
            equipo_tt.size() < static_cast<size_t>(cant_min_tt));
}

void GameLoop::asignar_bomba_si_es_necesario(bool esperando) {
    if (!esperando) {
        // Buscar jugadores TT vivos y sin bomba
        std::vector<Jugador*> tts_sin_bomba;
        for (Jugador* jugador : equipo_tt) {
            if (jugador->esta_vivo() && !jugador->posee_bomba()) {
                tts_sin_bomba.push_back(jugador);
            }
        }
        if (!tts_sin_bomba.empty()) {
            // Asignar la bomba a un TT aleatorio
            size_t idx = static_cast<size_t>(std::rand()) % tts_sin_bomba.size();
            tts_sin_bomba[idx]->asignar_bomba();
        }
    }
}

void GameLoop::colocar_armas_del_mapa() {
    std::vector<ArmaDefaultDelMapa> armas_default = mapa.get_armas_default();
    for (const auto& arma_default : armas_default) {
        ArmaDeFuego* arma = ArmaDeFuego::crearArma(arma_default.nombre);
        armas_en_suelo.push_back(ArmaEnSuelo(arma, arma_default.x, arma_default.y));
    }
}

bool GameLoop::jugar_ronda(bool esperando) {
    bool en_juego = true;
    std::cout << "Iniciando ronda " << ronda_actual << std::endl;
    auto t_inicio = std::chrono::steady_clock::now();
    asignar_bomba_si_es_necesario(esperando);
    colocar_armas_del_mapa();
    enum Equipo eq_ganador = NONE;
    while (activo && en_juego) {
        try {
            chequear_estados_jugadores();
            if (!esperando)
                chequear_si_pueden_comprar(t_inicio);
            ejecucion_comandos_recibidos();
            disparar_rafagas_restantes();
            chequear_colisiones(esperando);
            chequear_bomba_desactivada();
            chequear_bomba_plantada();            
            eq_ganador = se_termino_ronda();
            if (esperando) {
                chequear_si_completaron_equipos(eq_ganador, en_juego);
            } else {
                chequear_si_equipo_gano(eq_ganador, en_juego);
                if (!en_juego) break;
            }
            auto t_actual = std::chrono::steady_clock::now();
            auto t_transcurrido = std::chrono::duration_cast<std::chrono::seconds>(t_actual - t_inicio).count();
            int t_restante = tiempo_max_ronda - t_transcurrido;
            t_restante = (!esperando) ? t_restante : tiempo_max_ronda;
            Snapshot snapshot(jugadores, balas_disparadas, armas_en_suelo, info_bomba, t_restante, rondas_ganadas_ct, rondas_ganadas_tt, ronda_actual, cant_rondas, eq_ganador, false);
            queues_jugadores.broadcast(snapshot);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } catch (const ClosedQueue &) {
            return false;
        }
    }
    constexpr int tiempo_entre_rondas = 4;
    int t_restante = 0; // o el valor que quieras mostrar en el cliente
     
    esperar_entre_rondas(tiempo_entre_rondas, t_restante, eq_ganador);

    return true;
}

void GameLoop::run() {
    std::cout << "Iniciando el GameLoop..." << std::endl;
    jugar_ronda(true);
    while (activo) {
        jugar_ronda(false);
        if (chequear_si_termino_partida()) 
            activo = false;
    }
    std::cout << "[Gameloop] Se termino la partida" << std::endl;
    queue_comandos.close(); 
}

void GameLoop::eliminar_jugador_de_partida(int id_jugador) {
    Jugador *jugador = findJugador(id_jugador);
    if (jugador) {
        if (jugador->get_equipo() == CT) {
            equipo_ct.erase(std::remove(equipo_ct.begin(), equipo_ct.end(), jugador), equipo_ct.end());
        } else if (jugador->get_equipo() == TT) {
            equipo_tt.erase(std::remove(equipo_tt.begin(), equipo_tt.end(), jugador), equipo_tt.end());
        }
        jugadores.erase(std::remove(jugadores.begin(), jugadores.end(), jugador), jugadores.end());
        delete jugador; // Liberar memoria
        std::cout << "[Gameloop] Se elimino jugador de id: " << id_jugador << std::endl;
    }
}

GameLoop::~GameLoop() {
    std::cout << "[Gameloop] Destruyendo GameLoop..." << std::endl;
    for (Jugador *jugador : jugadores) {
        delete jugador; // Liberar memoria de los jugadores
    }
    for (ArmaEnSuelo &arma : armas_en_suelo) {
        delete arma.getArma(); // Liberar memoria de las armas en el suelo
    }
    if (bomba) {
        delete bomba; // Liberar memoria de la bomba si existe
    }
    std::cout << "[Gameloop] Se destruyo el GameLoop" << std::endl;
}