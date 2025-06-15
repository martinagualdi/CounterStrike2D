#include "client.h"
#include "inicio_window.h"
#include <QApplication>
#include <iostream>

char cwd[1024];
int main(int argc, char* argv[]){
    try{
        QApplication app(argc, argv);
        Inicio inicio;
        inicio.show();

        QObject::connect(&inicio, &Inicio::datosIngresados,
                     [&](const QString& host, quint16 port, const QString& username) {

            std::string host_str = host.toStdString();
            std::string port_str = std::to_string(port);
            std::string username_str = username.toStdString();

            // Obtener punteros const char* que pasan al constructor
            const char* host_cstr = host_str.c_str();
            const char* port_cstr = port_str.c_str();
            const char* username_cstr = username_str.c_str();
            inicio.close();

            Client client(host_cstr, port_cstr, username_cstr);
            client.iniciar();
        });

    return app.exec();

    } catch (const std::exception& e){
        std::cerr << "Algo salió mal: " << e.what() << std::endl;
    } catch (...){
        std::cerr << "Algo salió mal." << std::endl;
    }


}