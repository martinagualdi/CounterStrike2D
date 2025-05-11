#include "client.h"

Client::Client(const char *hostname, const char *servname, const char* username) : protocolo(hostname, servname), 
username(username), hilo_enviador(protocolo, cola_enviador), hilo_recibidor(protocolo, cola_recibidor) {}

void Client::iniciar() {

    hilo_enviador.start();
    hilo_recibidor.start();

    while(true){
        // Aca iria lo de 
    }
}

Client::~Client(){
    hilo_enviador.stop();
    hilo_recibidor.stop();
    hilo_enviador.join();
    hilo_recibidor.join();
}
