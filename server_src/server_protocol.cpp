#include "server_protocol.h"
#include "../common_src/prefijos_protocolo.h"
#include <vector>
#include <iostream>
#include <arpa/inet.h>

void ServerProtocol::push_back_uint16_t(std::vector<uint8_t> &buffer, uint16_t value) {
    value = htons(value);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[1]);
}

void ServerProtocol::push_back_uint32_t(std::vector<uint8_t> &buffer, uint32_t value) {
    value = htonl(value);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[1]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[2]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[3]);
}

ServerProtocol::ServerProtocol(Socket &skt) : skt(skt) {}

bool ServerProtocol::enviar_a_cliente(const Snapshot& snapshot) {     
    if (snapshot.info_jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint16_t largo_jugadores = htons(static_cast<uint16_t>(snapshot.info_jugadores.size() * 19));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_jugadores)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_jugadores)[1]);
    for (const InfoJugador& j : snapshot.info_jugadores) {
        uint8_t id = static_cast<uint8_t>(j.id);
        buffer.push_back(id);
        push_back_uint32_t(buffer, static_cast<uint32_t>(j.pos_x * 100));
        push_back_uint32_t(buffer, static_cast<uint32_t>(j.pos_y * 100));
        push_back_uint16_t(buffer, static_cast<uint16_t>(j.angulo * 100));
        buffer.push_back(static_cast<uint8_t>(j.vida));
        push_back_uint16_t(buffer, static_cast<uint16_t>(j.dinero));
        buffer.push_back(static_cast<uint8_t>(j.arma_en_mano));
        buffer.push_back(static_cast<uint8_t>(j.equipo)); // Enviar el equipo del jugador
        buffer.push_back(static_cast<uint8_t>(j.skin_tipo)); // Enviar el skin del jugador
        uint8_t esta_vivo = j.esta_vivo ? 0x01 : 0x00;
        buffer.push_back(esta_vivo); // Enviar si el jugador está vivo
        uint8_t esta_moviendose = j.esta_moviendose ? 0x01 : 0x00;
        buffer.push_back(esta_moviendose); // Enviar si el jugador está moviéndose
    }
    uint16_t largo_balas = htons(static_cast<uint16_t>(snapshot.balas_disparadas.size() * 11));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_balas)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_balas)[1]);
    for (const InfoMunicion& bala : snapshot.balas_disparadas) {
        buffer.push_back(static_cast<uint8_t>(bala.id_quien_disparo));
        push_back_uint32_t(buffer, static_cast<uint32_t>(bala.pos_x * 100));
        push_back_uint32_t(buffer, static_cast<uint32_t>(bala.pos_y * 100));
        push_back_uint16_t(buffer, static_cast<uint16_t>(bala.angulo_disparo * 100));
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
            uint8_t mov;
            skt.recvall(&mov, sizeof(mov));
            comando.movimiento = static_cast<enum Movimiento>(mov);
            break;
        case PREFIJO_DISPARAR:
            comando.tipo = DISPARO;
            uint16_t angulo_disparo;
            skt.recvall(&angulo_disparo, sizeof(angulo_disparo));
            comando.angulo = static_cast<float>(ntohs(angulo_disparo))/100;
            break;
        case PREFIJO_ROTACION:
            comando.tipo = ROTACION;
            uint16_t angulo;
            skt.recvall(&angulo, sizeof(angulo));
            comando.angulo = static_cast<float>(ntohs(angulo))/100;
            break;
        case PREFIJO_CAMBIO_ARMA:
            comando.tipo = CAMBIAR_ARMA;
            break;
        case PREFIJO_COMPRAR:
            comando.tipo = COMPRAR;
            uint8_t compra;
            skt.recvall(&compra, sizeof(compra));
            comando.compra = static_cast<enum Compra>(compra);
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
    //std::cout << "Esperando comando..." << std::endl;
    skt.recvall(&codigo, sizeof(codigo));
    std::vector<std::string> comando;
    if (codigo == PREFIJO_CREAR_PARTIDA) {
        /* Caso: Crear partida */
        comando.push_back("crear");
        //std::cout << "Recibiendo nombre de usuario..." << std::endl;
        uint16_t largo;
        skt.recvall(&largo, sizeof(largo));
        largo = ntohs(largo);
        //std::cout << "Largo del nombre de usuario: " << largo << std::endl;
        std::vector<uint8_t> buffer(largo);
        skt.recvall(buffer.data(), largo);
        std::string username(buffer.begin(), buffer.end());
        comando.push_back(username);
        //std::cout << "Creando partida..." << std::endl;
    }else if (codigo == PREFIJO_UNIRSE_PARTIDA) {
        /* Caso: Unirse a partida */
        comando.push_back("unirse");
        uint16_t id_partida;
        skt.recvall(&id_partida, sizeof(id_partida));
        comando.push_back(std::to_string(ntohs(id_partida)));
    } else if(codigo == PREFIJO_LISTAR) {
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

std::string ServerProtocol::recibir_path_mapa() {
    uint16_t largo;
    skt.recvall(&largo, sizeof(largo));
    largo = ntohs(largo);
    std::vector<uint8_t> buffer(largo);
    skt.recvall(buffer.data(), largo);
    std::string path_mapa(buffer.begin(), buffer.end());
    return path_mapa;
}