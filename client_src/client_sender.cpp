#include "client_sender.h"

ClientSender::ClientSender(ProtocoloCliente &protocolo, Queue<uint8_t> &cola) : protocolo(protocolo), cola_comandos(cola) {}

void ClientSender::run(){

    while(_keep_running){
        uint8_t comando;
        try{          
            cola_comandos.try_pop(comando);
        } catch(const ClosedQueue&){
            break;
        }
        try{
            protocolo.enviarComando(comando);
        } catch(std::exception& e){
            std::cerr << "Falla en el hilo ClientSender: " << e.what() << std::endl;
        }
    }

    try{
        cola_comandos.close();
    } catch(...){}
}

bool ClientSender::sigue_vivo(){
	return _keep_running;
}