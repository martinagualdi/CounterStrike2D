#ifndef RECEIVER_H
#define RECEIVER_H

#include <atomic>
#include <utility>
#include <filesystem>
#include <syslog.h>

#include "server_protocol.h"
#include "monitor_partidas.h"
#include "sender.h"
#include "../common_src/snapshot.h"
#include "../common_src/socket.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"

#define EXCEPCION_CONOCIDA "Se produjo una excepcion conocida: "
#define EXCEPCION_INESPERADA "Se produjo una excepcion inesperada: "
#define EXCEPCION_DESCONOCIDA "Se produjo una exepcion desconocida"



class Receiver : public Thread {
  private:
    ServerProtocol& protocol;
    MonitorPartidas& monitor_partidas;
    std::atomic<bool>& alive;
    int player_id;
    int game_id;
    Queue<ComandoDTO>* queue_comandos = nullptr;
    Queue<Snapshot>& queue_enviadora;
    Sender sender;

    void comunicacion_de_partida();

  public:
    explicit Receiver(ServerProtocol &protocolo, MonitorPartidas& monitor_partidas, std::atomic<bool> &is_alive, int player_id, Queue<Snapshot>& queue_enviadora);

    virtual void run() override;

    void terminar_ejecucion() {
        this->alive = false;
    }

    std::vector<std::pair<std::string, std::string>> listar_mapas_disponibles();

    void comunicacion_del_lobby();

    virtual ~Receiver();
};

#endif
