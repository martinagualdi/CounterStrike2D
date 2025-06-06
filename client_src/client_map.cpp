#include "client_map.h"

ClientMap::ClientMap(const std::string& map_str, Renderer& renderer) : 
    map_str(map_str),
    renderer(renderer),
    cache(), 
    elementos()
    {}


std::vector<ElementoMapa> ClientMap::parsearMapa() {

    YAML::Node data = YAML::Load(map_str);

    if (!data.IsMap()) {
        throw std::runtime_error("Error: el archivo YAML no tiene formato de mapa.");
    }

    std::string pathStr = data["fondo"].as<std::string>();
    const char* path_fondo = pathStr.c_str();
    if(path_fondo){
        std::shared_ptr<Texture> tex = cargarTextura(path_fondo);
        SDL_Rect rect {0, 0, 1920, 1920};
        TipoElementoMapa tipo = FONDO;
        ElementoMapa fondo = {tex, rect, tipo};
        elementos.emplace_back(fondo);
    }

    for (const auto &nodo : data["elementos"]) {
        if (!nodo["imagen"]) continue;
        std::string pathStr = nodo["imagen"].as<std::string>();
        const char* path = pathStr.c_str();
        if(!path)
            continue;
        std::shared_ptr<Texture> tex = cargarTextura(path);
        if(!tex)
            continue;

        SDL_Rect rect;
        rect.x = nodo["x"].as<int>();
        rect.y = nodo["y"].as<int>();
        rect.w = nodo["ancho"].as<int>();
        rect.h = nodo["alto"].as<int>();

        std::string tipo_str = nodo["tipo"].as<std::string>();
        TipoElementoMapa tipo = {};
        if(tipo_str == "obstaculo"){
            tipo = OBSTACULO;
        } else if(tipo_str == "piso"){
            tipo = PISO;
        } else if(tipo_str == "arma"){
            tipo = ARMA;
        }

        ElementoMapa elemento = {tex, rect, tipo};
        elementos.emplace_back(elemento);
    }

    return elementos;
}

std::shared_ptr<Texture> ClientMap::cargarTextura(const char* path) {
    auto it = cache.find(path);
    if (it != cache.end())
        return it->second;

    try {
        auto surface = IMG_Load(path);
        if (!surface) {
            std::cerr << "No se pudo cargar la imagen: " << path << " - " << IMG_GetError() << std::endl;
            return nullptr;
        }

        auto texture = std::make_shared<Texture>(renderer, Surface(surface));
        cache[path] = texture;
        return texture;

    } catch (const std::exception& e) {
        std::cerr << "Excepción cargando textura: " << e.what() << std::endl;
        return nullptr;
    }
}

ClientMap::~ClientMap() {}