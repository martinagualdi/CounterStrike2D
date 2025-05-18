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
        //mostrar_mensaje(snapshot_anterior);
    }

    try{
        cola_snapshots.close();
    } catch(...){}
}

void ClientReceiver::mostrar_mensaje(Snapshot& snap_anterior) {

    /*Snapshot snapshot;
    cola_snapshots.try_pop(snapshot);
    if (snapshot.son_iguales(snap_anterior)) {
        return;
    }
    std::cout << "Snapshot recibido: " << snapshot.info_jugadores.size() << " jugadores." << std::endl;
    std::cout << "Jugadores en el snapshot:" << std::endl;

    for (const auto& jugador : snapshot.info_jugadores) {
        std::cout << "Jugador ID: " << jugador.getId() << ", Posicion: (" << jugador.getX() << ", " << jugador.getY() << ")" << std::endl;
    }
    snap_anterior.actualizar_snapshot(snapshot);*/
}

ClientReceiver::~ClientReceiver()
{
}
