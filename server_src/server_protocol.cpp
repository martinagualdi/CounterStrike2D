#include "server_protocol.h"

#include <vector>
#include <iostream>
#include <arpa/inet.h>


ServerProtocol::ServerProtocol(Socket& skt) : skt(skt) {}

bool ServerProtocol::enviar_a_cliente(const Snapshot& snapshot) { 
    if (snapshot.info_jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint16_t largo = htons(static_cast<uint16_t>(snapshot.info_jugadores.size() * 6));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    for (const Jugador& j : snapshot.info_jugadores) {
        uint16_t id = htons(static_cast<uint16_t>(j.getId()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&id)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&id)[1]);
        uint16_t pos_X = htons(static_cast<uint16_t>(j.getX()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[1]);
        uint16_t pos_Y = htons(static_cast<uint16_t>(j.getY()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[1]);
    }
    skt.sendall(buffer.data(), buffer.size());
    return true;
}

bool ServerProtocol::recibir_de_cliente(ComandoDTO& comando) {
    uint16_t id;
    skt.recvall(&id, sizeof(id));
    comando.id_jugador = ntohs(id);
    uint16_t accion;
    skt.recvall(&accion, sizeof(accion));
    comando.movimiento = ntohs(accion);
    return true;
}

bool ServerProtocol::enviarID(int id_jugador) {
    uint16_t id = htons(static_cast<uint16_t>(id_jugador));
    skt.sendall(&id, sizeof(id));
    return true;
}