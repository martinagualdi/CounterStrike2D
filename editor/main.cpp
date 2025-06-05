#include "editor_lobby.h"
#include "editor_main_window.h"
#include "editor_seleccion_mapa.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    EditorLobby* lobby = new EditorLobby;
    lobby->show();

    QObject::connect(lobby, &EditorLobby::crearMapaSolicitado, [=]() {
        auto* ventana = new MainWindow;
        ventana->gettopWidget()->preguntarTamanioMapa();
        ventana->show();
        lobby->close();
    });

    QObject::connect(lobby, &EditorLobby::editarMapaSolicitado, [=]() {
        auto* seleccion = new EditorSeleccionMapa;

        QObject::connect(seleccion, &EditorSeleccionMapa::mapaSeleccionado, [=](const QString& rutaYaml) {
            auto* ventana = new MainWindow;
            ventana->cargarDesdeYAML(rutaYaml);
            ventana->show();
            seleccion->close();
        });

        seleccion->show();
        seleccion->seleccionarArchivo();
        lobby->close();
    });

    return app.exec();
}
