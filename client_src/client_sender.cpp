#include "client_sender.h"

ClientSender::ClientSender(ProtocoloCliente &protocolo, Queue<ComandoDTO> &cola) : protocolo(protocolo), cola_comandos(cola) {}

void ClientSender::run(){

    while(should_keep_running()){
        ComandoDTO comando;
        try{
            comando = cola_comandos.pop();
        } catch(const ClosedQueue&){
            break;
        }
        try{
            protocolo.enviarComando(comando);
        } catch(std::exception& e){
            std::cerr << "Falla en el hilo ClientSender: " << e.what() << std::endl;
        }
    }
}

ClientSender::~ClientSender()
{
}
