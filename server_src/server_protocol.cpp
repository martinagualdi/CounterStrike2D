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
    uint16_t largo = htons(static_cast<uint16_t>(10));
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
        uint32_t anguloParaEnviar = htons(j.getAngulo() * 100);
        buffer.push_back(reinterpret_cast<uint8_t*>(&anguloParaEnviar)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&anguloParaEnviar)[1]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&anguloParaEnviar)[2]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&anguloParaEnviar)[3]);
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
        case PREFIJO_ROTACION:
            comando.tipo = ROTACION;
            uint16_t posicionesMouse[2];
            skt.recvall(&posicionesMouse, sizeof(uint16_t) * 2);
            comando.mouseX = ntohs(posicionesMouse[0]);
            comando.mouseY = ntohs(posicionesMouse[1]);
            break;
        case PREFIJO_DISPARAR:
            //....
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