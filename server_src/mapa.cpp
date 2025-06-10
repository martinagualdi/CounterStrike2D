#include "mapa.h"

#include <sstream>
#include <iostream>
#include <random>

Mapa::Mapa(std::string yamlPath) {
    YAML::Node data = YAML::LoadFile(yamlPath);
    this->ancho_mapa = data["ancho_max_mapa"].as<int>();
    this->alto_mapa =  data["alto_max_mapa"].as<int>();
    bool zona_bombas_definida = false;
    for (const auto &nodo : data["elementos"]) {
        ElementoDeMapa elemento;
        elemento.area = definir_inicio(
            nodo["x"].as<int>(),
            nodo["y"].as<int>(),
            nodo["ancho"].as<int>(),
            nodo["alto"].as<int>()
        );
        std::string tipo_str = nodo["tipo"].as<std::string>();
        TipoElementoMapa tipo = {};
        if(tipo_str == "obstaculo"){
            tipo = OBSTACULO;
        } else if(tipo_str == "piso"){
            tipo = PISO;
        } else if(tipo_str == "arma"){
            tipo = PISO;
        }
        elemento.tipo = tipo;
        elementos.push_back(elemento);
    }
    for (const auto &nodo : data["zonas"]) {
        std::string tipo_str = nodo["tipo"].as<std::string>();
        int x = nodo["x"].as<int>();
        int y = nodo["y"].as<int>();
        int ancho = nodo["ancho"].as<int>();
        int alto = nodo["alto"].as<int>();
        if (tipo_str == "inicio_ct") {
            inicio_ct = definir_inicio(x, y, ancho, alto);
        } else if (tipo_str == "inicio_tt"){
            inicio_tt = definir_inicio(x, y, ancho, alto);
        } else if (tipo_str == "zona_bombas") {
            if (!zona_bombas_definida) {
                zona_bombas.zona_bombas_a = definir_inicio(x, y, ancho, alto);
                zona_bombas_definida = true;
            }else {
                zona_bombas.zona_bombas_b = definir_inicio(x, y, ancho, alto);
            }
        }
    }
    std::stringstream dto;
    dto << data;
    this->inicio_mapa_dto = dto.str();
}

Area Mapa::definir_inicio(const int x, const int y, const int ancho, const int alto) {
    Area inicio;
    inicio.x = x;
    inicio.y = alto_mapa - y; // Invertir la coordenada Y
    inicio.ancho = ancho;
    inicio.alto = alto;
    return inicio;
}

bool Mapa::jugador_colision_contra_pared(float pos_x, float pos_y) {
    float max_pos_x_jugador = pos_x + 20; 
    float min_pos_x_jugador = pos_x - 20;
    float max_pos_y_jugador = pos_y + 20; 
    float min_pos_y_jugador = pos_y - 20;
    for (const ElementoDeMapa &elemento : elementos) {
        if (elemento.tipo == OBSTACULO) {
            float max_pos_x_elemento = elemento.area.x + elemento.area.ancho;
            float min_pos_x_elemento = elemento.area.x;
            float max_pos_y_elemento = elemento.area.y + elemento.area.alto;
            float min_pos_y_elemento = elemento.area.y;
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
            if (pos_x < 0 || pos_x > ancho_mapa || pos_y < 0 || pos_y > alto_mapa) {
                std::cout << "Bala fuera de los limites del mapa" << std::endl;
                return true; // La bala está fuera de los límites del mapa
            }
            float max_pos_x_elemento = elemento.area.x + elemento.area.ancho;
            float min_pos_x_elemento = elemento.area.x;
            float max_pos_y_elemento = elemento.area.y + elemento.area.alto;
            float min_pos_y_elemento = elemento.area.y;
            if (pos_x >= min_pos_x_elemento && pos_x <= max_pos_x_elemento &&
                pos_y >= min_pos_y_elemento && pos_y <= max_pos_y_elemento) {
                std::cout << "Bala colisiona con pared" << std::endl;
                return true; 
            }
        }
    }
    return false;
}

std::vector<float> Mapa::dar_posiciones_iniciales(bool es_tt) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    Area area_inicio;
    if (es_tt) 
        area_inicio = inicio_tt;
    else 
        area_inicio = inicio_ct;

    // std::cout << "Area inicio: x=" << area_inicio.x << " y=" << area_inicio.y
    //           << " ancho=" << area_inicio.ancho << " alto=" << area_inicio.alto << std::endl;


    std::uniform_real_distribution<float> dist_x(area_inicio.x, area_inicio.x + area_inicio.ancho);
    std::uniform_real_distribution<float> dist_y(area_inicio.y, area_inicio.y + area_inicio.alto);

    float x = dist_x(gen);
    float y = dist_y(gen);
    std::cout << "Posición inicial: (" << x << ", " << y << ")" << std::endl;
    std::vector<float> posicion_inicial{x, y};
    return posicion_inicial;
}