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
    snapshot.balas_disparadas.size() > 0 ? buffer.push_back(0x01) : buffer.push_back(0x00); /* AGREGO UNA FORMA DE DEFINIR SI HAY QUE LEER BALAS*/
    uint16_t largo = htons(static_cast<uint16_t>(19 * snapshot.info_jugadores.size()));
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
        uint16_t vida = htons(static_cast<uint16_t>(j.get_vida()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&vida)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&vida)[1]);
        uint16_t dinero = htons(static_cast<uint16_t>(j.get_dinero()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&dinero)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&dinero)[1]);
        uint8_t arma_secundaria;
        if (j.get_nombre_arma_secundaria() == "Glock") {
            arma_secundaria = 0x01; // Glock
        } else if (j.get_nombre_arma_secundaria() == "AK-47") {
            arma_secundaria = 0x02; // AK-47                                    /*Logica de como creo va a ser mejor enviar el arma actual, usaria mejor id's para usar enums"*/
        } else if (j.get_nombre_arma_secundaria() == "Desert Eagle") {
            arma_secundaria = 0x03; // Desert Eagle
        } else {
            arma_secundaria = 0x00; // No tiene arma secundaria
        }
        buffer.push_back(arma_secundaria);
        uint8_t equipo = static_cast<uint8_t>(j.get_equipo());
        buffer.push_back(equipo); // Enviar el equipo del jugador
        uint8_t skin = static_cast<uint8_t>(j.get_skin_tipo());
        buffer.push_back(skin); // Enviar el skin del jugador
    }
    if (snapshot.balas_disparadas.size() > 0) {
        uint16_t largo_balas = htons(static_cast<uint16_t>(12 * snapshot.balas_disparadas.size()));
        buffer.push_back(reinterpret_cast<uint8_t*>(&largo_balas)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&largo_balas)[1]);
        for (const Municion& bala : snapshot.balas_disparadas) {
            uint16_t id_quien_disparo = htons(static_cast<uint16_t>(bala.quien_disparo()));
            buffer.push_back(reinterpret_cast<uint8_t*>(&id_quien_disparo)[0]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&id_quien_disparo)[1]);
            uint32_t pos_X = htonl(static_cast<uint32_t>(bala.getPosX() * 100));
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[0]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[1]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[2]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_X)[3]);
            uint32_t pos_Y = htonl(static_cast<uint32_t>(bala.getPosY() * 100));
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[0]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[1]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[2]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&pos_Y)[3]);
            uint16_t angulo_disparo = htons(static_cast<uint16_t>(bala.getAnguloDisparo() * 100));
            buffer.push_back(reinterpret_cast<uint8_t*>(&angulo_disparo)[0]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&angulo_disparo)[1]);
        }
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
            uint8_t m;
            skt.recvall(&m, 1);
            comando.movimiento = static_cast<enum Movimiento>(m);
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

std::vector<std::string> ServerProtocol::recibir_inicio_juego() {
    uint8_t codigo;
    std::cout << "Esperando comando..." << std::endl;
    skt.recvall(&codigo, sizeof(codigo));
    std::vector<std::string> comando;
    if (codigo == 0x0A) {
        /* Caso: Crear partida */
        comando.push_back("crear");
        std::cout << "Creando partida..." << std::endl;
    }else if (codigo == 0x0B) {
        /* Caso: Unirse a partida */
        comando.push_back("unirse");
        uint16_t id_partida;
        skt.recvall(&id_partida, sizeof(id_partida));
        comando.push_back(std::to_string(ntohs(id_partida)));
    } else {
        /* Caso: Listar partidas */
        comando.push_back("listar");
    }
    return comando;
}

void ServerProtocol::enviar_lista_partidas(const std::vector<std::string>& lista_partidas) {
    std::string mensaje = "Lista de partidas: \n";
    for (const std::string& info : lista_partidas) {
        mensaje += " - " + info + "\n";
    }
    enviar_mensaje(mensaje);
}

void ServerProtocol::enviar_mensaje(const std::string& mensaje) {
    std::vector<uint8_t> buffer;
    uint16_t largo = htons(static_cast<uint16_t>(mensaje.size()));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    buffer.insert(buffer.end(), mensaje.begin(), mensaje.end());
    skt.sendall(buffer.data(), buffer.size());
}