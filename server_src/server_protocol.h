#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "../common_src/socket.h"
#include "../common_src/snapshot.h"
#include "../common_src/comando_dto.h"

class ServerProtocol {
private:
    Socket& skt;

    void push_back_uint16_t(std::vector<uint8_t>& buffer, uint16_t value);
    void push_back_uint32_t(std::vector<uint8_t>& buffer, uint32_t value);

public:
    explicit ServerProtocol(Socket& skt);

    bool enviar_a_cliente(const Snapshot& snapshot);

    bool recibir_de_cliente(ComandoDTO& comando);

    bool enviarID(int id_jugador) ;

    std::vector<std::string> recibir_inicio_juego();

    void enviar_lista_partidas(const std::vector<std::string>& lista_partidas);

    void enviar_mensaje(const std::string& mensaje);

    std::string recibir_path_mapa(); 

    void enviar_mapa(const std::string& yaml_serializado);

    void enviar_lista_mapas(const std::vector<std::pair<std::string, std::string>>& mapas);

    void enviar_precios_mercado();

};

#endif