#include "client_sender.h"

ClientSender::ClientSender(ProtocoloCliente &protocolo, Queue<uint8_t> &cola) : protocolo(protocolo), cola_comandos(cola) {}

void ClientSender::run(){

    while(should_keep_running()){
        enviarMensaje();
        uint8_t comando = 0;
        try{          
            cola_comandos.try_pop(comando);
        } catch(const ClosedQueue&){
            break;
        }
        try{
            protocolo.enviarComando(cliente_id,comando);
        } catch(std::exception& e){
            std::cerr << "Falla en el hilo ClientSender: " << e.what() << std::endl;
        }
    }

    try{
        cola_comandos.close();
    } catch(...){}
}

void ClientSender::set_cliente_id(int id){
    this->cliente_id = id;
}

void ClientSender::enviarMensaje(){
    std::string comando_str;
    std::cout << "Ingrese comando: ";
    if (!std::getline(std::cin, comando_str)) {
        return ;
    }
    if (comando_str.size() == 1) {
        char c = std::toupper(comando_str[0]);
        int dir = 0;
        switch (c) {
            case 'W': dir = 1; break;
            case 'A': dir = 2; break;
            case 'S': dir = 3; break;
            case 'D': dir = 4; break;
        }
        cola_comandos.try_push(dir);
    }
}

ClientSender::~ClientSender()
{
}
