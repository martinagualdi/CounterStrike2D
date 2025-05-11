#include "client_receiver.h"
#include "../common_src/socket.h"

ClientReceiver::ClientReceiver(ProtocoloCliente& protocolo, Queue<Snapshot>& cola) : protocolo(protocolo), cola_snapshots(cola) {}

void ClientReceiver::run(){
    while(_keep_running){
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
    }

    try{
        cola_snapshots.close();
    } catch(...){}
}

bool ClientReceiver::sigue_vivo(){
	return _keep_running;
}
