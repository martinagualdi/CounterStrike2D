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
        elemento.y = alto_mapa - nodo["y"].as<int>();
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

bool Mapa::jugador_colision_contra_pared(float pos_x, float pos_y) {
    float max_pos_x_jugador = pos_x + 20; 
    float min_pos_x_jugador = pos_x - 20;
    float max_pos_y_jugador = pos_y + 20; 
    float min_pos_y_jugador = pos_y - 20;
    for (const ElementoDeMapa &elemento : elementos) {
        if (elemento.tipo == OBSTACULO) {
            float max_pos_x_elemento = elemento.x + elemento.ancho;
            float min_pos_x_elemento = elemento.x;
            float max_pos_y_elemento = elemento.y + elemento.alto;
            float min_pos_y_elemento = elemento.y;
            if (max_pos_x_jugador > min_pos_x_elemento &&
                min_pos_x_jugador < max_pos_x_elemento &&
                max_pos_y_jugador > min_pos_y_elemento &&
                min_pos_y_jugador < max_pos_y_elemento) {
                return true; 
            }
        }
    }
    return false;
}

bool Mapa::bala_colision_contra_pared(float pos_x, float pos_y) {
    for (const ElementoDeMapa &elemento : elementos) {
        if (elemento.tipo == OBSTACULO) {
            float max_pos_x_elemento = elemento.x + elemento.ancho;
            float min_pos_x_elemento = elemento.x;
            float max_pos_y_elemento = elemento.y + elemento.alto;
            float min_pos_y_elemento = elemento.y;
            if (pos_x >= min_pos_x_elemento && pos_x <= max_pos_x_elemento &&
                pos_y >= min_pos_y_elemento && pos_y <= max_pos_y_elemento) {
                std::cout << "Bala colisiona con pared" << std::endl;
                return true; 
            }
        }
    }
    return false;
}
