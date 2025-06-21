#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../client_src/client_protocol.h"
#include "../server_src/server_protocol.h"
#include "../server_src/municion.h"
#include "../server_src/arma.h"
#include "../server_src/awp.h"
#include "../server_src/m3.h"
#include "../server_src/ak47.h"
#include "../server_src/glock.h"
#include "../server_src/cuchillo.h"
#include "../server_src/jugador.h"
#include "../server_src/configuracion.h"
#include "../common_src/socket.h"
#include "../common_src/snapshot.h"
#include "../common_src/enums_jugador.h"


constexpr const char* kHost = "127.0.0.1";
constexpr const char* kPort = "8085";
constexpr int kDelay = 100;

class DummyArma : public ArmaDeFuego {
public:
    DummyArma() : ArmaDeFuego("dummy", 1.0f, 100.0f, 10, 20, true, 100, 10, 5) {}

    int accion(float) override { return 0; }
    bool puedeAccionar() override { return true; }
    int getBalas() override { return 5; }
    enum ArmaEnMano getCodigoArma() const override { return ArmaEnMano::AK_47; }
};

TEST(ProtocoloTest, EnviaYRecibeDisparo) {
    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);
        ComandoDTO comando;
        proto.recibir_de_cliente(comando);
        ASSERT_EQ(comando.tipo, DISPARO);
        ASSERT_NEAR(comando.angulo, 45.5f, 0.01f);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        ComandoDTO cmd;
        cmd.tipo = DISPARO;
        cmd.angulo = 45.5f;
        cliente.enviarComando(cmd);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, EnviaYRecibeRotacion) {
    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);
        ComandoDTO comando;
        proto.recibir_de_cliente(comando);
        ASSERT_EQ(comando.tipo, ROTACION);
        ASSERT_NEAR(comando.angulo, 90.0f, 0.01f);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        ComandoDTO cmd;
        cmd.tipo = ROTACION;
        cmd.angulo = 90.0f;
        cliente.enviarComando(cmd);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, EnviaYRecibeMensajeTexto) {
    std::string mensaje = "Hola desde el cliente";
    std::thread server_thread([&]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);
        std::string recibido = proto.recibir_path_mapa();
        ASSERT_EQ(recibido, mensaje);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([&]() {
        ProtocoloCliente cliente(kHost, kPort);
        cliente.enviar_mensaje(mensaje);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, EnviaYRecibeID) {
    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);
        proto.enviarID(42);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        int id = cliente.recibirID();
        ASSERT_EQ(id, 42);
    });

    client_thread.join();
    server_thread.join();
}

TEST(PartidaTest, CrearPartida) {
    std::string username = "jugador_test";
    std::thread server_thread([&]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);
        auto comando = proto.recibir_inicio_juego();
        ASSERT_EQ(comando[0], "crear");
        ASSERT_EQ(comando[1], username);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([&]() {
        ProtocoloCliente cliente(kHost, kPort);
        cliente.enviar_crear_partida(username);
    });

    client_thread.join();
    server_thread.join();
}

TEST(PartidaTest, ClienteListaPartidasCorrectamente) {
    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        auto comando = proto.recibir_inicio_juego();
        ASSERT_EQ(comando[0], "listar");

        std::vector<std::string> partidas = {
            "Inferno", "Dust2"
        };
        proto.enviar_lista_partidas(partidas);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        cliente.enviar_listar_partida();
        std::string lista = cliente.recibir_lista_partidas();

        // Se espera que sea un string con los mapas separados por \n
        EXPECT_NE(lista.find("Inferno"), std::string::npos);
        EXPECT_NE(lista.find("Dust2"), std::string::npos);
    });

    client_thread.join();
    server_thread.join();
}

TEST(PartidaTest, ClienteSeUneAPartidaExitosa) {
    std::string username = "jugador_test";

    std::thread server_thread([&]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        auto comando = proto.recibir_inicio_juego();
        ASSERT_EQ(comando[0], "unirse");
        ASSERT_EQ(comando[1], "3");  // ID de partida
        ASSERT_EQ(comando[2], username);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([&]() {
        ProtocoloCliente cliente(kHost, kPort);
        cliente.enviar_unirse_partida(3, username);  // Usa el método ya implementado
    });

    client_thread.join();
    server_thread.join();
}

TEST(PartidaTest, jugadorSeleccionaSkinCorrectamente) {
    std::thread servidor([]() {
        Socket servidor_socket(kPort);
        Socket conexion = servidor_socket.accept();
        ServerProtocol proto(conexion);

        ComandoDTO comando;
        proto.recibir_de_cliente(comando);

        ASSERT_EQ(comando.tipo, SELECCIONAR_SKIN);
        EXPECT_EQ(comando.skin, SkinTipos::SKIN2);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread cliente([]() {
        ProtocoloCliente cliente(kHost, kPort);
        ComandoDTO comando;
        comando.tipo = SELECCIONAR_SKIN;
        comando.skin = SkinTipos::SKIN2;
        cliente.enviarComando(comando);
    });

    cliente.join();
    servidor.join();
}

TEST(ProtocoloTest, EnviaYRecibeMapa) {
    std::string mapa = "editor/mapas/mapa2.yaml";
    std::thread server_thread([&]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);
        proto.enviar_mapa(mapa);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([&]() {
        ProtocoloCliente cliente(kHost, kPort);
        std::string recibido = cliente.recibir_mapa();
        ASSERT_EQ(recibido, mapa);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, SnapshotUnJugadorUnaBalaYArmaDummySinBombaPlantada) {
    Configuracion::cargar_path("configuracion.yaml");
    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "TestPlayer";
        Jugador jugador(1, nombre);
        jugador.setX(10.0f);
        jugador.setY(20.0f);
        jugador.setAngulo(45.0f);

        std::vector<Jugador*> jugadores = { &jugador };

        Municion bala(1, 15.0f, 25.0f, 60.0f);
        std::vector<Municion> balas = { bala };

        ArmaEnSuelo arma_suelta(new DummyArma(), 30.0f, 40.0f);
        std::vector<ArmaEnSuelo> armas = { arma_suelta };

        BombaEnSuelo bomba(50.0f, 60.0f, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 120;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 1, 2, 3, 5, Equipo::CT, false);

        proto.enviar_a_cliente(snap);

        delete arma_suelta.arma; // evitar memory leak
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        ASSERT_EQ(recibido.info_jugadores[0].pos_x, 10.0f);
        ASSERT_EQ(recibido.info_jugadores[0].pos_y, 20.0f);
        ASSERT_EQ(recibido.info_jugadores[0].angulo, 45.0f);

        ASSERT_EQ(recibido.balas_disparadas.size(), 1);
        ASSERT_EQ(recibido.balas_disparadas[0].pos_x, 15.0f);

        ASSERT_EQ(recibido.armas_sueltas.size(), 1);
        ASSERT_EQ(recibido.armas_sueltas[0].pos_x, 30.0f);

        ASSERT_EQ(recibido.rondas_info.rondas_ganadas_ct, 1);
        ASSERT_EQ(recibido.rondas_info.rondas_ganadas_tt, 2);
        ASSERT_EQ(recibido.rondas_info.ronda_actual, 3);
        ASSERT_EQ(recibido.rondas_info.total_rondas, 5);
        ASSERT_EQ(recibido.tiempo_restante, 120);
        ASSERT_EQ(recibido.equipo_ganador, Equipo::CT);
        ASSERT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, SnapshotConJugadorYUnaBala) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "TestPlayer";
        Jugador jugador(1, nombre);
        jugador.setX(100.0f);
        jugador.setY(200.0f);
        jugador.setAngulo(90.0f);

        std::vector<Jugador*> jugadores = { &jugador };

        Municion bala(1, 110.0f, 210.0f, 90.0f);
        std::vector<Municion> balas = { bala };

        std::vector<ArmaEnSuelo> armas;  // Sin armas en el suelo
        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 90;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 1, 10, Equipo::CT, false);

        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        EXPECT_FLOAT_EQ(recibido.info_jugadores[0].pos_x, 100.0f);
        EXPECT_FLOAT_EQ(recibido.info_jugadores[0].pos_y, 200.0f);
        EXPECT_FLOAT_EQ(recibido.info_jugadores[0].angulo, 90.0f);

        ASSERT_EQ(recibido.balas_disparadas.size(), 1);
        EXPECT_FLOAT_EQ(recibido.balas_disparadas[0].pos_x, 110.0f);
        EXPECT_FLOAT_EQ(recibido.balas_disparadas[0].pos_y, 210.0f);

        ASSERT_TRUE(recibido.armas_sueltas.empty());

        EXPECT_EQ(recibido.rondas_info.ronda_actual, 1);
        EXPECT_EQ(recibido.tiempo_restante, 90);
        EXPECT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::SIN_PLANTAR);
        EXPECT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, SnapshotConAWPyBombaPlantada) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "AWPUser";
        Jugador jugador(2, nombre);
        jugador.setX(300.0f);
        jugador.setY(150.0f);
        jugador.setAngulo(270.0f);

        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas;  // Sin balas

        ArmaEnSuelo arma_suelta(new Awp(), 350.0f, 160.0f);
        std::vector<ArmaEnSuelo> armas = { arma_suelta };

        BombaEnSuelo bomba(400.0f, 200.0f, EstadoBombaRonda::PLANTADA, 5.0f, true, false, false);

        int tiempo = 45;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 3, 2, 6, 10, Equipo::TT, false);

        proto.enviar_a_cliente(snap);

        delete arma_suelta.arma;  // evitar memory leak
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        EXPECT_FLOAT_EQ(recibido.info_jugadores[0].pos_x, 300.0f);
        EXPECT_FLOAT_EQ(recibido.info_jugadores[0].pos_y, 150.0f);
        EXPECT_FLOAT_EQ(recibido.info_jugadores[0].angulo, 270.0f);

        ASSERT_TRUE(recibido.balas_disparadas.empty());

        ASSERT_EQ(recibido.armas_sueltas.size(), 1);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_x, 350.0f);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_y, 160.0f);

        EXPECT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::PLANTADA);
        EXPECT_FLOAT_EQ(recibido.bomba_en_suelo.pos_x, 400.0f);
        EXPECT_FLOAT_EQ(recibido.bomba_en_suelo.pos_y, 200.0f);
        EXPECT_FLOAT_EQ(recibido.bomba_en_suelo.tiempo_para_detonar, 5.0f);

        EXPECT_EQ(recibido.tiempo_restante, 45);
        EXPECT_EQ(recibido.rondas_info.rondas_ganadas_ct, 3);
        EXPECT_EQ(recibido.rondas_info.rondas_ganadas_tt, 2);
        EXPECT_EQ(recibido.rondas_info.ronda_actual, 6);
        EXPECT_EQ(recibido.rondas_info.total_rondas, 10);
        EXPECT_EQ(recibido.equipo_ganador, Equipo::TT);
        EXPECT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ProtocoloTest, SnapshotConDosJugadoresYBombaDesactivada) {
    Configuracion::cargar_path("configuracion.yaml");
    
    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        // Jugador CT
        std::string nombre_ct = "JugadorCT";
        Jugador jugador_ct(1, nombre_ct);
        jugador_ct.setX(10.0f);
        jugador_ct.setY(20.0f);
        jugador_ct.setAngulo(90.0f);
        jugador_ct.establecer_equipo(Equipo::CT);

        // Jugador TT
        std::string nombre_tt = "JugadorTT";
        Jugador jugador_tt(2, nombre_tt);
        jugador_tt.setX(30.0f);
        jugador_tt.setY(40.0f);
        jugador_tt.setAngulo(180.0f);
        jugador_tt.establecer_equipo(Equipo::TT);

        std::vector<Jugador*> jugadores = { &jugador_ct, &jugador_tt };

        // Balas disparadas
        Municion bala_ct(1, 15.0f, 25.0f, 60.0f);
        Municion bala_tt(2, 35.0f, 45.0f, 120.0f);
        std::vector<Municion> balas = { bala_ct, bala_tt };

        // Armas en suelo
        ArmaEnSuelo awp(new Awp(), 50.0f, 60.0f);
        ArmaEnSuelo ak47(new Ak47(), 70.0f, 80.0f);
        std::vector<ArmaEnSuelo> armas = { awp, ak47 };

        // Bomba desactivada
        BombaEnSuelo bomba(100.0f, 100.0f, EstadoBombaRonda::DESACTIVADA, 0, false, false, true);

        // Resto del snapshot
        int tiempo = 150;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 3, 4, 2, 5, Equipo::CT, false);
        proto.enviar_a_cliente(snap);

        delete awp.arma;
        delete ak47.arma;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 2);
        ASSERT_EQ(recibido.balas_disparadas.size(), 2);
        ASSERT_EQ(recibido.armas_sueltas.size(), 2);

        ASSERT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::DESACTIVADA);
        ASSERT_TRUE(recibido.bomba_en_suelo.acaba_de_ser_desactivada);

        ASSERT_EQ(recibido.rondas_info.rondas_ganadas_ct, 3);
        ASSERT_EQ(recibido.rondas_info.rondas_ganadas_tt, 4);
        ASSERT_EQ(recibido.rondas_info.ronda_actual, 2);
        ASSERT_EQ(recibido.rondas_info.total_rondas, 5);
        ASSERT_EQ(recibido.tiempo_restante, 150);
        ASSERT_EQ(recibido.equipo_ganador, Equipo::CT);
        ASSERT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ArmasTest, SnapshotConBombaRecienDesactivada) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "CTHero";
        Jugador jugador(5, nombre);
        jugador.setX(200.0f);
        jugador.setY(300.0f);
        jugador.setAngulo(0.0f);
        jugador.establecer_equipo(Equipo::CT);

        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas;

        ArmaEnSuelo arma(new Awp(), 210.0f, 310.0f);
        std::vector<ArmaEnSuelo> armas = { arma };

        // Bomba recientemente desactivada
        BombaEnSuelo bomba(400.0f, 400.0f, EstadoBombaRonda::DESACTIVADA, 0, false, false, true);

        int tiempo = 10;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 4, 3, 7, 10, Equipo::CT, false);
        proto.enviar_a_cliente(snap);

        delete arma.arma;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        EXPECT_EQ(recibido.info_jugadores[0].id, 5);
        EXPECT_EQ(recibido.info_jugadores[0].equipo, Equipo::CT);

        ASSERT_EQ(recibido.armas_sueltas.size(), 1);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_x, 210.0f);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_y, 310.0f);

        ASSERT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::DESACTIVADA);
        ASSERT_TRUE(recibido.bomba_en_suelo.acaba_de_ser_desactivada);
        ASSERT_FALSE(recibido.bomba_en_suelo.acaba_de_detonar);

        ASSERT_EQ(recibido.tiempo_restante, 10);
        ASSERT_EQ(recibido.rondas_info.rondas_ganadas_ct, 4);
        ASSERT_EQ(recibido.rondas_info.rondas_ganadas_tt, 3);
        ASSERT_EQ(recibido.rondas_info.ronda_actual, 7);
        ASSERT_EQ(recibido.rondas_info.total_rondas, 10);
        ASSERT_EQ(recibido.equipo_ganador, Equipo::CT);
        ASSERT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ArmasTest, SnapshotConBombaPlantadaActiva) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre_tt = "Plantador";
        Jugador jugador_tt(7, nombre_tt);
        jugador_tt.setX(150.0f);
        jugador_tt.setY(250.0f);
        jugador_tt.setAngulo(180.0f);
        jugador_tt.establecer_equipo(Equipo::TT);

        std::vector<Jugador*> jugadores = { &jugador_tt };

        Municion bala(7, 160.0f, 260.0f, 90.0f);
        std::vector<Municion> balas = { bala };

        ArmaEnSuelo arma(new Glock(), 170.0f, 270.0f);
        std::vector<ArmaEnSuelo> armas = { arma };

        // Bomba plantada y activa
        BombaEnSuelo bomba(300.0f, 350.0f, EstadoBombaRonda::PLANTADA, 20, false, false, false);

        int tiempo = 40;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 1, 1, 2, 10, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);

        delete arma.arma;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        EXPECT_EQ(recibido.info_jugadores[0].id, 7);
        EXPECT_EQ(recibido.info_jugadores[0].equipo, Equipo::TT);

        ASSERT_EQ(recibido.balas_disparadas.size(), 1);
        EXPECT_FLOAT_EQ(recibido.balas_disparadas[0].pos_x, 160.0f);
        EXPECT_FLOAT_EQ(recibido.balas_disparadas[0].pos_y, 260.0f);

        ASSERT_EQ(recibido.armas_sueltas.size(), 1);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_x, 170.0f);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_y, 270.0f);

        ASSERT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::PLANTADA);
        EXPECT_EQ(recibido.bomba_en_suelo.tiempo_para_detonar, 20);
        EXPECT_FALSE(recibido.bomba_en_suelo.acaba_de_detonar);
        EXPECT_FALSE(recibido.bomba_en_suelo.acaba_de_ser_desactivada);

        EXPECT_EQ(recibido.tiempo_restante, 40);
        EXPECT_EQ(recibido.rondas_info.rondas_ganadas_ct, 1);
        EXPECT_EQ(recibido.rondas_info.rondas_ganadas_tt, 1);
        EXPECT_EQ(recibido.rondas_info.ronda_actual, 2);
        EXPECT_EQ(recibido.rondas_info.total_rondas, 10);
        EXPECT_EQ(recibido.equipo_ganador, Equipo::NONE);
        EXPECT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(TiempoTest, SnapshotConTiempoCompleto) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "JugadorTiempo";
        Jugador jugador(1, nombre);
        jugador.setX(50.0f);
        jugador.setY(60.0f);
        jugador.setAngulo(135.0f);
        jugador.establecer_equipo(Equipo::TT);

        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas; // sin disparos aún
        std::vector<ArmaEnSuelo> armas; // sin armas sueltas

        BombaEnSuelo bomba(0.0f, 0.0f, EstadoBombaRonda::SIN_PLANTAR, 0,false, false, false);

        int tiempo_ronda_completo = Configuracion::get<int>("tiempo_por_ronda");
        Snapshot snap(jugadores, balas, armas,bomba,tiempo_ronda_completo, 0, 0, 1, 12, Equipo::NONE, false);

        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        // Verifica jugador
        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        EXPECT_EQ(recibido.info_jugadores[0].id, 1);
        EXPECT_EQ(recibido.info_jugadores[0].equipo, Equipo::TT);

        // Tiempo completo
        int tiempo_ronda_completo = Configuracion::get<int>("tiempo_por_ronda");
        EXPECT_EQ(recibido.tiempo_restante, tiempo_ronda_completo);

        // Validaciones adicionales
        EXPECT_EQ(recibido.rondas_info.ronda_actual, 1);
        EXPECT_EQ(recibido.rondas_info.total_rondas, 12);
        EXPECT_EQ(recibido.balas_disparadas.size(), 0);
        EXPECT_EQ(recibido.armas_sueltas.size(), 0);
        EXPECT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::SIN_PLANTAR);
    });

    client_thread.join();
    server_thread.join();
}

TEST(TiempoTest, SnapshotConTiempoCeroPartidaTerminada) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "JugadorTiempo";
        Jugador jugador(2, nombre);
        jugador.establecer_equipo(Equipo::CT);
        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0.0f, 0.0f, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo_restante = 0;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo_restante, 0, 0, 5, 12, Equipo::TT, true);

        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        EXPECT_EQ(recibido.tiempo_restante, 0);
        EXPECT_TRUE(recibido.termino_partida);
        EXPECT_EQ(recibido.rondas_info.ronda_actual, 5);
        EXPECT_EQ(recibido.rondas_info.total_rondas, 12);
        EXPECT_EQ(recibido.equipo_ganador, Equipo::TT);
    });

    client_thread.join();
    server_thread.join();
}

TEST(TiempoTest, SnapshotConTiempoParcial) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        int tiempo_parcial = Configuracion::get<int>("tiempo_por_ronda") / 2;

        std::string nombre = "JugadorParcial";
        Jugador jugador(3, nombre);
        jugador.establecer_equipo(Equipo::CT);
        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0.0f, 0.0f, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        Snapshot snap(jugadores, balas, armas, bomba, tiempo_parcial, 0, 0, 3, 12, Equipo::NONE, false);

        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        EXPECT_GT(recibido.tiempo_restante, 0);
        EXPECT_LT(recibido.tiempo_restante, Configuracion::get<int>("tiempo_por_ronda"));
        EXPECT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(TiempoTest, SnapshotConBombaDesactivandoseYTiempoBajo) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        int tiempo_bajo = 3;

        std::string nombre = "JugadorFinal";
        Jugador jugador(4, nombre);
        jugador.establecer_equipo(Equipo::CT);
        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;

        BombaEnSuelo bomba(150.0f, 120.0f, EstadoBombaRonda::PLANTADA, Configuracion::get<int>("tiempo_desactivacion_bomba"), false, false, false);

        Snapshot snap(jugadores, balas, armas, bomba, tiempo_bajo, 0, 0, 8, 12, Equipo::NONE, false);

        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        EXPECT_EQ(recibido.tiempo_restante, 3);
        EXPECT_EQ(recibido.bomba_en_suelo.estado_bomba, EstadoBombaRonda::PLANTADA);
        EXPECT_FALSE(recibido.bomba_en_suelo.acaba_de_detonar);
        EXPECT_FALSE(recibido.termino_partida);
    });

    client_thread.join();
    server_thread.join();
}

TEST(VidaTest, JugadorVidaCompleta) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "JugadorCompleto";
        Jugador jugador(1, nombre);

        std::vector<Jugador*> jugadores = { &jugador };
        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        const auto& jugador = recibido.info_jugadores[0];
        EXPECT_EQ(jugador.vida, Configuracion::get<int>("vida_inicial"));
        EXPECT_TRUE(jugador.esta_vivo);
    });

    client_thread.join();
    server_thread.join();
}

TEST(VidaTest, JugadorRecibeDanioRestaVida) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        int vida_maxima = Configuracion::get<int>("vida_inicial");

        std::string nombre = "JugadorHerido";
        Jugador jugador(2, nombre);
        jugador.recibir_danio(vida_maxima / 2);  // Baja vida pero sigue vivo

        std::vector<Jugador*> jugadores = { &jugador };
        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        const auto& jugador = recibido.info_jugadores[0];
        EXPECT_LT(jugador.vida, Configuracion::get<int>("vida_inicial"));
        EXPECT_GT(jugador.vida, 0);
        EXPECT_TRUE(jugador.esta_vivo);
    });

    client_thread.join();
    server_thread.join();
}

TEST(VidaTest, JugadorRecibeDanioMuere) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        int vida_maxima = Configuracion::get<int>("vida_inicial");

        std::string nombre = "JugadorMuerto";
        Jugador jugador(3, nombre);
        jugador.recibir_danio(vida_maxima);  // Muerte asegurada

        std::vector<Jugador*> jugadores = { &jugador };
        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        const auto& jugador = recibido.info_jugadores[0];
        EXPECT_EQ(jugador.vida, 0);
        EXPECT_FALSE(jugador.esta_vivo);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ArmasTest, JugadorCambiaArmaEnMano) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "JugadorCambio";
        Jugador jugador(5, nombre);

        jugador.comprarArma(Compra::C_AK47);
        jugador.cambiar_arma_en_mano();
        jugador.cambiar_arma_en_mano();  
        jugador.cambiar_arma_en_mano();

        std::vector<Jugador*> jugadores = { &jugador };
        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        const auto& jugador = recibido.info_jugadores[0];
        EXPECT_EQ(jugador.arma_en_mano, ArmaEnMano::AK_47);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ArmasTest, JugadorSueltaArmaPrincipal) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "JugadorDroper";
        Jugador jugador(6, nombre);
        jugador.setX(100.0f);
        jugador.setY(200.0f);

        jugador.comprarArma(Compra::C_M3);
        ArmaDeFuego* arma_suelta = jugador.soltar_arma_pricipal();

        std::vector<Jugador*> jugadores = { &jugador };

        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        if (arma_suelta) {
            armas.emplace_back(arma_suelta, jugador.getX(), jugador.getY());
        }

        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);
        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.armas_sueltas.size(), 1);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_x, 100.0f);
        EXPECT_FLOAT_EQ(recibido.armas_sueltas[0].pos_y, 200.0f);
        EXPECT_EQ(recibido.armas_sueltas[0].tipo_arma, ArmaEnMano::M3);
    });

    client_thread.join();
    server_thread.join();
}

TEST(ArmasTest, JugadorLevantaArmaDelSuelo) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "Recolector";
        Jugador jugador(7, nombre);
        jugador.setX(120.0f);
        jugador.setY(140.0f);

        // Simular arma en el suelo
        ArmaEnSuelo arma_suelta(new Awp(), 120.0f, 140.0f);

        // El jugador la recoge
        jugador.levantar_arma(arma_suelta.arma);

        std::vector<Jugador*> jugadores = { &jugador };
        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;  // Ya fue recogida, así que vacía

        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);
        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        const auto& j = recibido.info_jugadores[0];
        EXPECT_EQ(j.arma_en_mano, ArmaEnMano::AWP);  // AWP debe estar equipada
    });

    client_thread.join();
    server_thread.join();
}

TEST(ArmasTest, JugadorCambiaArmaSinPrincipal) {
    Configuracion::cargar_path("configuracion.yaml");

    std::thread server_thread([]() {
        Socket server_socket(kPort);
        Socket client_conn = server_socket.accept();
        ServerProtocol proto(client_conn);

        std::string nombre = "SinPrincipal";
        Jugador jugador(8, nombre);

        // Inicial: Glock (secundaria)
        jugador.cambiar_arma_en_mano();  // → cuchillo
        jugador.cambiar_arma_en_mano();  // → Glock (secundaria)
        jugador.cambiar_arma_en_mano();  // → cuchillo (solo alterna entre estas dos)

        std::vector<Jugador*> jugadores = { &jugador };
        std::vector<Municion> balas;
        std::vector<ArmaEnSuelo> armas;
        BombaEnSuelo bomba(0, 0, EstadoBombaRonda::SIN_PLANTAR, 0, false, false, false);

        int tiempo = 60;
        Snapshot snap(jugadores, balas, armas, bomba, tiempo, 0, 0, 0, 0, Equipo::NONE, false);
        proto.enviar_a_cliente(snap);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(kDelay));

    std::thread client_thread([]() {
        ProtocoloCliente cliente(kHost, kPort);
        Snapshot recibido = cliente.recibirSnapshot();

        ASSERT_EQ(recibido.info_jugadores.size(), 1);
        const auto& j = recibido.info_jugadores[0];

        // Debería haber terminado en cuchillo después de alternar
        EXPECT_EQ(j.arma_en_mano, ArmaEnMano::CUCHILLO);
    });

    client_thread.join();
    server_thread.join();
}

TEST(Dinerotests, JugadorDineroInicial) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Jugador";
    Jugador jugador(1, nombre);
    EXPECT_EQ(jugador.get_dinero(), Configuracion::get<int>("dinero_inicial"));
}

TEST(Dinerotests, JugadorCompraArmaRestaDinero) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Jugador";
    Jugador jugador(1, nombre);
    int dinero_inicial = jugador.get_dinero();
    int precio_ak = Configuracion::get<int>("precio_ak47");

    bool compro = jugador.comprarArma(Compra::C_AK47);
    EXPECT_TRUE(compro);
    EXPECT_EQ(jugador.get_dinero(), dinero_inicial - precio_ak);
}

TEST(Dinerotests, JugadorDinero0NoPuedeComprar) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Jugador";
    Jugador jugador(1, nombre);

    jugador.comprarArma(Compra::C_AK47);
    while (jugador.get_dinero() >= 5)
        jugador.comprarBalas(BALAS_PRIMARIA);

    int dinero = jugador.get_dinero();
    EXPECT_EQ(dinero, 0);
    EXPECT_FALSE(jugador.comprarArma(Compra::C_AK47));
}

TEST(Dinerotests, JugadorCompraBalasSecundariasRestaDinero) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Jugador";
    Jugador jugador(1, nombre);
    int dinero_inicial = jugador.get_dinero();
    int precio = Configuracion::get<int>("precio_municion_secundaria");

    bool compro = jugador.comprarBalas(BALAS_SECUNDARIA);

    EXPECT_TRUE(compro);
    EXPECT_EQ(jugador.get_dinero(), dinero_inicial - precio);
}

TEST(Dinerotests, JugadorCompraBalasPrimariasSinArmaNoCompra) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Jugador";
    Jugador jugador(1, nombre);

    bool compro = jugador.comprarBalas(BALAS_PRIMARIA);
    EXPECT_FALSE(compro);
}

TEST(Dinerotests, JugadorCompraBalasPrimariasConArmaRestaDinero) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Jugador";
    Jugador jugador(1, nombre);

    jugador.comprarArma(C_AK47);

    int dinero_antes = jugador.get_dinero();
    int precio = Configuracion::get<int>("precio_municion_principal");

    bool compro = jugador.comprarBalas(BALAS_PRIMARIA);
    EXPECT_TRUE(compro);
    EXPECT_EQ(jugador.get_dinero(), dinero_antes - precio);
}

TEST(DineroTests, JugadorPuedeComprarDuranteTiempoDeCompra) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Comprador";
    Jugador jugador(1, nombre);

    jugador.en_posicion_de_compra(true);

    EXPECT_TRUE(jugador.puede_comprar_ahora());
    EXPECT_TRUE(jugador.comprarArma(C_M3));
}

TEST(DineroTests, JugadorNoPuedeComprarFueraDeTiempoDeCompra) {
    Configuracion::cargar_path("configuracion.yaml");
    std::string nombre = "Comprador";
    Jugador jugador(1, nombre);

    jugador.en_posicion_de_compra(false);

    EXPECT_FALSE(jugador.puede_comprar_ahora());
}

TEST(ArmasTest, AwpHaceDanioSiAciertaDisparo) {
    Configuracion::cargar_path("configuracion.yaml");
    Awp awp;
    float distancia = 1000.0f;
    while (!awp.puedeAccionar()) {}
    int danio = awp.accion(distancia);
    EXPECT_GE(danio, 0);
    EXPECT_LE(danio, Configuracion::get<int>("danio_max_awp"));
}

TEST(ArmasTest, M3HaceDanioSiAciertaDisparo) {
    Configuracion::cargar_path("configuracion.yaml");
    m3 arma;
    float distancia = 500.0f;
    while (!arma.puedeAccionar()) {}
    int danio = arma.accion(distancia);
    EXPECT_GE(danio, 0);
    EXPECT_LE(danio, Configuracion::get<int>("danio_max_m3"));
}

TEST(ArmasTest, Ak47HaceDanioSiAciertaDisparo) {
    Configuracion::cargar_path("configuracion.yaml");
    Ak47 ak47;
    float distancia = 1000.0f;
    while (!ak47.puedeAccionar()) {}
    int danio = ak47.accion(distancia);
    EXPECT_GE(danio, 0);
    EXPECT_LE(danio, Configuracion::get<int>("danio_max_ak47"));
}

TEST(ArmasTest, GlockHaceDanioSiAciertaDisparo) {
    Configuracion::cargar_path("configuracion.yaml");
    Glock glock;
    float distancia = 300.0f;
    while (!glock.puedeAccionar()) {}
    int danio = glock.accion(distancia);
    EXPECT_GE(danio, 0);
    EXPECT_LE(danio, Configuracion::get<int>("danio_max_glock"));
}

TEST(ArmasTest, CuchilloHaceDanioSiAciertaDisparo) {
    Configuracion::cargar_path("configuracion.yaml");
    Cuchillo cuchillo;
    float distancia = 10.0f;
    while (!cuchillo.puedeAccionar()) {}
    int danio = cuchillo.accion(distancia);
    EXPECT_GE(danio, Configuracion::get<int>("danio_min_cuchillo"));
    EXPECT_LE(danio, Configuracion::get<int>("danio_max_cuchillo"));
}

