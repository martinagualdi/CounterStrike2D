#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <map>
#include <vector>
#include "../common_src/comando_dto.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"
#include "../common_src/socket.h"

class ProtocoloCliente {
private:
    Socket socket;
    void push_back_uint16(std::vector<uint8_t>& message, uint16_t value);
    void serializar_comando(ComandoDTO& comando, std::vector<uint8_t>& mensaje);
public:

    explicit ProtocoloCliente(const char* hostname, const char* servname);
    void enviarComando(ComandoDTO comando);
    
    Snapshot recibirSnapshot();
    int recibirID();
    void enviar_crear_partida(std::string username);
    void enviar_unirse_partida(int id_partida);
    void enviar_listar_partida();
    std::string recibir_lista_partidas();
    void enviar_mensaje(const std::string& mensaje);
    std::string recibir_mapa();
    std::vector<std::pair<std::string, std::string>> recibir_lista_mapas();

    ProtocoloCliente(const ProtocoloCliente&) = delete;
    ProtocoloCliente& operator=(const ProtocoloCliente&) = delete;
    ProtocoloCliente(ProtocoloCliente&&) = delete;
    ProtocoloCliente& operator=(ProtocoloCliente&&) = delete;
    ~ProtocoloCliente();    
};

#endif 