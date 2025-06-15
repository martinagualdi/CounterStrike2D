#include "client_map.h"

ClientMap::ClientMap(const std::string& map_str, Renderer& renderer) : 
    map_str(map_str),
    renderer(renderer),
    cache(), 
    mapa(0, 0)
    {}

struct Mapa ClientMap::parsearMapa() {
    YAML::Node data = YAML::Load(map_str);

    if (data["ancho_max_mapa"]) mapa.ancho_mapa_max = data["ancho_max_mapa"].as<int>();
    if (data["alto_max_mapa"]) mapa.alto_mapa_max = data["alto_max_mapa"].as<int>();

    std::string pathStr = data["fondo"].as<std::string>();
    const char* path_fondo = pathStr.c_str();
    std::shared_ptr<Texture> tex = cargarTextura(path_fondo);
    SDL_Rect rect {0, 0, mapa.ancho_mapa_max, mapa.alto_mapa_max};
    TipoElementoMapa tipo = FONDO;
    mapa.elementos.emplace_back(ElementoMapa{tex, rect, tipo});

    for (const auto& nodo : data["elementos"]) {
        if (!nodo["imagen"]) continue;

        std::string pathStr = nodo["imagen"].as<std::string>();
        std::shared_ptr<Texture> tex = cargarTextura(pathStr.c_str());
        if (!tex) continue;

        SDL_Rect rect;
        rect.x = nodo["x"].as<int>();
        rect.y = nodo["y"].as<int>() - mapa.alto_mapa_max;
        rect.w = nodo["ancho"].as<int>();
        rect.h = nodo["alto"].as<int>();

        std::string tipo_str = nodo["tipo"].as<std::string>();
        TipoElementoMapa tipo;
        if (tipo_str == "obstaculo") tipo = OBSTACULO;
        else if (tipo_str == "piso") tipo = PISO;
        else if (tipo_str == "arma") tipo = ARMA;
        else if (tipo_str == "bombsite") tipo = BOMBSITE;
        else continue;

        mapa.elementos.emplace_back(ElementoMapa{tex, rect, tipo});
    }

    std::cout << "Se cargaron " << mapa.elementos.size() << " elementos\n";


    return mapa;
}

std::shared_ptr<Texture> ClientMap::cargarTextura(const char* path) {
    std::cout << "Cargando textura: " << path << "\n";
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
