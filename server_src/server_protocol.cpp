#include "server_protocol.h"
#include "../common_src/prefijos_protocolo.h"
#include <vector>
#include <iostream>
#include <arpa/inet.h>

ServerProtocol::ServerProtocol(Socket& skt) : skt(skt) {}

bool ServerProtocol::enviar_a_cliente(const Snapshot& snapshot) { 
    if (snapshot.info_jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint16_t largo = htons(static_cast<uint16_t>(12 * snapshot.info_jugadores.size()));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    for (const Jugador& j : snapshot.info_jugadores) {
        uint16_t id = htons(static_cast<uint16_t>(j.getId()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&id)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&id)[1]);
        uint32_t pos_X = htonl(static_cast<uint32_t>(j.getX() * 100));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[1]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[2]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[3]);
        uint32_t pos_Y = htonl(static_cast<uint32_t>(j.getY() * 100));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[1]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[2]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[3]);
        uint16_t angulo = htons(static_cast<uint16_t>(j.getAngulo() * 100));
        buffer.push_back(reinterpret_cast<uint8_t*>(&angulo)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&angulo)[1]);
    }
    skt.sendall(buffer.data(), buffer.size());
    return true;
}

bool ServerProtocol::recibir_de_cliente(ComandoDTO& comando) {

    uint8_t prefijo;
    skt.recvall(&prefijo, sizeof(prefijo));
    
    switch(prefijo){
        case PREFIJO_MOVIMIENTO:
            comando.tipo = MOVIMIENTO;
            enum Movimiento m;
            skt.recvall(&m, 1);
            comando.movimiento = m;
            break;
        case PREFIJO_DISPARAR:
            comando.tipo = DISPARO;
            uint16_t angulo_disparo;
            skt.recvall(&angulo_disparo, 2);
            comando.angulo = static_cast<float>(ntohs(angulo_disparo))/100;
            break;
        case PREFIJO_ROTACION:
            comando.tipo = ROTACION;
            uint16_t angulo;
            skt.recvall(&angulo, 2);
            comando.angulo = static_cast<float>(ntohs(angulo))/100;
            break;
        default:
            break;
    }

    return true;
}

bool ServerProtocol::enviarID(int id_jugador) {
    uint16_t id = htons(static_cast<uint16_t>(id_jugador));
    skt.sendall(&id, sizeof(id));
    return true;
}