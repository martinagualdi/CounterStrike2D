#include "server_protocol.h"

#include <vector>
#include <iostream>
#include <arpa/inet.h>


ServerProtocol::ServerProtocol(Socket& skt) : skt(skt) {}

bool ServerProtocol::enviar_a_cliente(const Snapshot& snapshot) { 
    /*
    if (mensaje.jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint8_t tipo_mensaje = 0x00;
    if (mensaje.tipo_mensaje == "movimiento"){
        tipo_mensaje = 0x01;                    /* EJEMPLO DE LO QUE TENDRIA QUE SUCEDER PARA CUANDO DEFINAMOS QUE CODIGO USA CADA TIPO DE MENSAJE */
    /*
    }
    buffer.push_back(tipo_mensaje);
    uint16_t largo = htons(static_cast<uint16_t>(mensaje.jugadores.size()));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    buffer.insert(buffer.end(), mensaje.jugadores.begin(), mensaje.jugadores.end());
    

    skt.sendall(buffer.data(), buffer.size());
    return true;
    */
    if (snapshot.jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint16_t largo = htons(static_cast<uint16_t>(snapshot.jugadores.size() * 2))
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0])
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    for (const Jugador& j : snapshot.jugadores) {
        uint16_t pos_X = htons(static_cast<uint16_t>(j.getX()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[1]);
        uint16_t pos_Y = htons(static_cast<uint16_t>(j.getY()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[1]);
        std::string salto = "\n"
        buffer.insert(buffer.end(), salto.begin(), salto.end());
    }
    skt.sendall(buffer.data(), buffer.size());
    return true;

}

//NO TIENE QUE RECIBIR SNAPSHOT, TIENE QUE RECIBIR UN MOVIMIENTO
bool ServerProtocol::recibir_de_cliente(ComandoDTO& comando) {
    //std::uint8_t tipo_mensaje;
    /*FUNCION PARA PASAR EL TIPO DE MENSAJE A STRING*/
    /*
    std::uint16_t largo;
    skt.recvall(&tipo_mensaje, sizeof(tipo_mensaje));
    skt.recvall(&largo, sizeof(largo));
    largo = ntohs(largo);
    std::vector<uint8_t> buffer(largo);
    skt.recvall(buffer.data(), largo);
    std::cout << std::dec << std::endl;
    std::string info(buffer.begin(), buffer.end());
    mensaje.tipo_mensaje = std::to_string(tipo_mensaje);
    mensaje.informacion = info;
    return true;
    */
    
}