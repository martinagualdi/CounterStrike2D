#include "client_receiver.h"
#include "../common_src/socket.h"

ClientReceiver::ClientReceiver(ProtocoloCliente& protocolo, Queue<Snapshot>& cola) : protocolo(protocolo), cola_snapshots(cola){}
void ClientReceiver::run(){
    while(should_keep_running()){
        Snapshot snapshot;
        try{
            if(should_keep_running())
            snapshot = protocolo.recibirSnapshot();
            if(snapshot.termino_partida)
                this->stop();
            
        } catch (const std::exception& e){
            break;
        }
        try{   
            cola_snapshots.try_push(snapshot);
        } catch(const ClosedQueue&){
            break;
        }
    }
}

ClientReceiver::~ClientReceiver()
{
}
