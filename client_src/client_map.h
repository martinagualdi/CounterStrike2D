#ifndef CLIENT_MAP_H
#define CLIENT_MAP_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL_image.h>
#include <yaml-cpp/yaml.h>

using namespace SDL2pp;

typedef enum {FONDO, OBSTACULO, PISO, ARMA} TipoElementoMapa;

struct Mapa{
    int alto_mapa_max;
    int ancho_mapa_max;
    std::vector<struct ElementoMapa> elementos;

    Mapa(int alto = 0, int ancho = 0)
        : alto_mapa_max(alto), ancho_mapa_max(ancho) {}
};

struct ElementoMapa{
    std::shared_ptr<Texture> texture;
    SDL_Rect dst;
    TipoElementoMapa tipo;
};

class ClientMap {
private:
    const std::string& map_str;
    Renderer& renderer;
    std::unordered_map<std::string, std::shared_ptr<Texture>> cache;
    struct Mapa mapa;
    std::shared_ptr<Texture> cargarTextura(const char* path);

public:
    explicit ClientMap(const std::string& map_str, Renderer& renderer);
    struct Mapa parsearMapa();
    ClientMap(const ClientMap&) = delete;
    ClientMap& operator=(const ClientMap&) = delete;
    ClientMap(ClientMap&&) = default;
    ClientMap& operator=(ClientMap&&) = default;
    ~ClientMap();
};

#endif