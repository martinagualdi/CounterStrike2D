#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <string>
#include <vector>
#include <mutex>
#include "../common_src/queue.h"
#include "../common_src/thread.h"
#include "../common_src/socket.h"

#include <condition_variable>

class ProtocoloCliente {
public:

    Socket& socket;

    enum TipoComando {
        CAMINAR,
        DISPARAR,
        RECARGAR,
        SALTAR
    };

    struct Comando {
        TipoComando tipo;
        std::string datos; 
    };


    ProtocoloCliente(Socket& socket);
    
    
    void enviarComando(const Comando& comando);
    bool recibirMensaje();

    // Habria que ver si queremos esto aca o si es mas de un handler
    void iniciar();
    void detener();
    // Habria que ver si queremos esto aca o si es mas de un handler

    ProtocoloCliente(const ProtocoloCliente&) = delete;
    ProtocoloCliente& operator=(const ProtocoloCliente&) = delete;
    ProtocoloCliente(ProtocoloCliente&&) = delete;
    ProtocoloCliente& operator=(ProtocoloCliente&&) = delete;
    ~ProtocoloCliente();

private:


    bool recibirSnapshot();
    bool recibirMensajeDelJuego;
    bool enviarCaminar(const Comando& datos);
    bool enviarDisparar(const Comando& datos);
    bool enviarRecargar(const Comando& datos);
    bool enviarSaltar(const Comando& datos);


    // Creeria que esto es mas del receiver y sender 
    void hiloEmisor();
    void hiloReceptor();
    std::queue<Comando> colaComandos;
    std::mutex mutexComandos;
    std::mutex mutexMensajes;
    std::condition_variable condicionComandos;
    std::condition_variable condicionMensajes;
    bool ejecutando;
    // Creeria que esto es mas del receiver y sender 



    
};

#endif 