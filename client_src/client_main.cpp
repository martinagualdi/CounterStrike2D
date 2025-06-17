#include "client.h"
#include "inicio_window.h"
#include "mensaje_popup.h"
#include <QApplication>
#include <iostream>

int main(int argc, char* argv[]){

    QApplication app(argc, argv);
    Inicio inicio;
    inicio.show();

    QObject::connect(&inicio, &Inicio::datosIngresados,
        [&](const QString& host, quint16 port, const QString& username) {
            std::string host_str = host.toStdString();
            std::string port_str = std::to_string(port);
            std::string username_str = username.toStdString();

            const char* host_cstr = host_str.c_str();
            const char* port_cstr = port_str.c_str();
            const char* username_cstr = username_str.c_str();

            try {
                Client client(host_cstr, port_cstr, username_cstr);
                inicio.close();
                client.iniciar();
            } catch (const std::exception& e) {
                MensajePopup popup("Error de conexión", e.what(), &inicio);
                popup.exec();
            } catch (...) {
                MensajePopup popup("Error desconocido", "No se pudo conectar al servidor.", &inicio);
                popup.exec();
            }
        });

    return app.exec();
}
