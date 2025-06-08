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
    std::vector<ElementoMapa> elementos;
    int ancho_max_mapa;
    int alto_max_mapa;
    //std::vector<ZonaRectItem> zonas;
    std::shared_ptr<Texture> cargarTextura(const char* path);

public:
    explicit ClientMap(const std::string& map_str, Renderer& renderer);
    std::vector<ElementoMapa> parsearMapa();
    ClientMap(const ClientMap&) = delete;
    ClientMap& operator=(const ClientMap&) = delete;
    ClientMap(ClientMap&&) = default;
    ClientMap& operator=(ClientMap&&) = default;
    ~ClientMap();
};

#endif