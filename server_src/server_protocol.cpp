#include "server_protocol.h"

#include <vector>
#include <iostream>
#include <arpa/inet.h>


ServerProtocol::ServerProtocol(Socket& skt) : skt(skt) {}

bool ServerProtocol::enviar_a_cliente(const Snapshot& snapshot) { 
    if (snapshot.jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint16_t largo = htons(static_cast<uint16_t>(snapshot.jugadores.size() * 2));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    for (const Jugador& j : snapshot.jugadores) {
        uint16_t pos_X = htons(static_cast<uint16_t>(j.getX()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[1]);
        uint16_t pos_Y = htons(static_cast<uint16_t>(j.getY()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[1]);
        std::string salto = "\n";
        buffer.insert(buffer.end(), salto.begin(), salto.end());
    }
    skt.sendall(buffer.data(), buffer.size());
    return true;

}

bool ServerProtocol::recibir_de_cliente(ComandoDTO& comando) {
    uint16_t id;
    skt.recvall(&id, sizeof(id));
    comando.id_jugador = ntohs(id);
    uint16_t comando;
    skt.recvall(&comando, sizeof(comando));
    comando.movimiento = std::string(reinterpret_cast<char*>(&comando), sizeof(comando));
    return true;
}