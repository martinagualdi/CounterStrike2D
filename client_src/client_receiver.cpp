#include "client_receiver.h"
#include "../common_src/socket.h"

ClientReceiver::ClientReceiver(ProtocoloCliente& protocolo, Queue<Snapshot>& cola) : protocolo(protocolo), cola_snapshots(cola){}

void ClientReceiver::run(){
    
    while(should_keep_running()){
        Snapshot snapshot;
        try{
            snapshot = protocolo.recibirSnapshot(); 
        } catch (const std::exception& e){
            break;
        }
        try{     
            cola_snapshots.try_push(snapshot);
        } catch(const ClosedQueue&){
            break;
        }
        mostrar_mensaje();
    }

    try{
        cola_snapshots.close();
    } catch(...){}
}

void ClientReceiver::mostrar_mensaje() const {
    Snapshot snapshot;
    cola_snapshots.try_pop(snapshot);
    std::cout << "Snapshot recibido: " << snapshot.jugadores.size() << " jugadores." << std::endl;
}

ClientReceiver::~ClientReceiver()
{
}
