#include "mapa.h"

#include <sstream>
#include <iostream>

Mapa::Mapa(std::string yamlPath) {
    YAML::Node data = YAML::LoadFile(yamlPath);
    this->ancho_mapa = 1920;/*data["ancho_max_mapa"].as<int>();*/
    this->alto_mapa = 1920;/*data["alto_max_mapa"].as<int>();*/
    for (const auto &nodo : data["elementos"]) {
        ElementoDeMapa elemento;
        elemento.x = nodo["x"].as<int>();
        elemento.y = nodo["y"].as<int>();
        elemento.ancho = nodo["ancho"].as<int>();
        elemento.alto = nodo["alto"].as<int>();
        std::string tipo_str = nodo["tipo"].as<std::string>();
        TipoElementoMapa tipo = {};
        if(tipo_str == "obstaculo"){
            tipo = OBSTACULO;
        } else if(tipo_str == "piso"){
            tipo = PISO;
        } else if(tipo_str == "arma"){
            tipo = ARMA;
        } else if (tipo_str == "inicio_ct") {
            tipo = INICIO_CT; 
        } else if (tipo_str == "inicio_tt"){
            tipo = INICIO_TT; 
        } else if (tipo_str == "zona_bombas") {
            tipo = ZONA_BOMBAS;
        }
        elemento.tipo = tipo;
        elementos.push_back(elemento);
    }
    std::stringstream dto;
    dto << data;
    this->inicio_mapa_dto = dto.str();
}