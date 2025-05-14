#include "client.h"
#include <iostream>

int main(int argc, char* argv[]){

    try{
        if(argc != 4){
            std::cerr << "Error. Para iniciar: ./client <hostname> <servname> <username>\n";
            return 1;
        }

        const char* hostname = argv[1];
        const char* servname = argv[2];
        const char* username = argv[3]; // A modificar luego con SDL

        Client client(hostname, servname, username);

    } catch (const std::exception& e){
        std::cerr << "Algo salió mal: " << e.what() << std::endl;
    } catch (...){
        std::cerr << "Algo salió mal." << std::endl;
    }


}