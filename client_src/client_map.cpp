#include "client_map.h"

ClientMap::ClientMap(const std::string& map_str, Renderer& renderer) : 
    map_str(map_str),
    renderer(renderer),
    cache(), 
    elementos()
    {}


std::vector<ElementoMapa> ClientMap::parsearMapa() {
    YAML::Node data = YAML::Load(map_str);

    // Cargar fondo
    std::string pathStr = data["fondo"].as<std::string>();
    const char* path_fondo = pathStr.c_str();
    std::shared_ptr<Texture> tex = cargarTextura(path_fondo);
    SDL_Rect rect {0, 0, 1920, 1920}; // O bien usá ancho_max_mapa/alto_max_mapa si querés escalar
    TipoElementoMapa tipo = FONDO;
    elementos.emplace_back(ElementoMapa{tex, rect, tipo});

    // Guardar dimensiones del mapa
    if (data["ancho_max_mapa"]) ancho_max_mapa = data["ancho_max_mapa"].as<int>();
    if (data["alto_max_mapa"]) alto_max_mapa = data["alto_max_mapa"].as<int>();

    // Cargar elementos
    for (const auto& nodo : data["elementos"]) {
        if (!nodo["imagen"]) continue;

        std::string pathStr = nodo["imagen"].as<std::string>();
        std::shared_ptr<Texture> tex = cargarTextura(pathStr.c_str());
        if (!tex) continue;

        SDL_Rect rect;
        rect.x = nodo["x"].as<int>();
        rect.y = nodo["y"].as<int>();
        rect.w = nodo["ancho"].as<int>();
        rect.h = nodo["alto"].as<int>();

        std::string tipo_str = nodo["tipo"].as<std::string>();
        TipoElementoMapa tipo;
        if (tipo_str == "obstaculo") tipo = OBSTACULO;
        else if (tipo_str == "piso") tipo = PISO;
        else if (tipo_str == "arma") tipo = ARMA;
        else continue;

        elementos.emplace_back(ElementoMapa{tex, rect, tipo});
    }

    // (Opcional) Cargar zonas
    /*if (data["zonas"]) {
        for (const auto& zona : data["zonas"]) {
            std::string tipo_zona = zona["tipo"].as<std::string>();
            int x = zona["x"].as<int>();
            int y = zona["y"].as<int>();
            int ancho = zona["ancho"].as<int>();
            int alto = zona["alto"].as<int>();
            std::string id = zona["id"].as<std::string>();

            // Creá tu clase o struct `ZonaRectItem` según lo necesites
            zonas.emplace_back(ZonaRectItem{tipo_zona, x, y, ancho, alto, id});
        }
    }*/

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