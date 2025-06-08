#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "../common_src/socket.h"
#include "../common_src/snapshot.h"
#include "../common_src/comando_dto.h"

class ServerProtocol {
private:
    Socket& skt;

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

    void enviar_lista_mapas(const std::vector<std::string>& mapas);
    /*
    
    IDEA:

    Si este protocolo es solo para server finalmente, podemos crearle unicamente dos funciones publicas: enviar y recibir, ambas
    con el "all" ya que siempre sabemos el tamaño de los mensajes. Al hacerlo de esta manera podemos hacer que herede de una clase madre
    "Protocol" que ya las tenga implementadas y que solo se encargue de usar funciones abstactas que se van a implementar en cada
    una de las clases hijas "ServerProtocol" y "ClientProtocol". De esta manera no tenemos que repetir el mismo codigo en ambas clases.

    
    */

};

#endif