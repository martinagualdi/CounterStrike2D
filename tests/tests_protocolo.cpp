#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../client_src/client_protocol.h"
#include "../server_src/server_protocol.h"
#include "../server_src/municion.h"
#include "../server_src/arma.h"
#include "../server_src/ak47.h"
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

TEST(ProtocoloTest, CrearPartida) {
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

TEST(ProtocoloTest, EnviaYRecibeSnapshot_UnJugadorUnaBalaYArmaDummySinBombaPlantada) {
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

TEST(ProtocoloTest, Snapshot_ConJugadorYUnaBala) {
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

TEST(ProtocoloTest, Snapshot_ConAWPyBombaPlantada) {
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

TEST(ProtocoloTest, SnapshotConBombaRecienDesactivada) {
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

