#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../client_src/client_protocol.h"
#include "../server_src/server_protocol.h"
#include "../server_src/municion.h"
#include "../server_src/arma.h"
#include "../server_src/jugador.h"
#include "../common_src/socket.h"
#include "../common_src/snapshot.h"

constexpr const char* kHost = "127.0.0.1";
constexpr const char* kPort = "8085";
constexpr int kDelay = 100;

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

