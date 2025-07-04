#ifndef RUTA_BASE_H
#define RUTA_BASE_H

#ifdef INSTALADO
    #define RUTA_BASE_IMAGENES "/var/CounterStrike2D/assets/gfx/"
    #define RUTA_BASE_SONIDOS "/var/CounterStrike2D/assets/sfx/"
    #define RUTA_SERVER_BASE "/var/CounterStrike2D/server/mapas_disponibles/"
    #define RUTA_BASE_EDITOR "/var/CounterStrike2D/editor/mapas/"
    #define RUTA_BASE_CLIENT_CONFIG "/etc/CounterStrike2D/"
#else
    #define RUTA_BASE_IMAGENES "gfx/"
    #define RUTA_BASE_SONIDOS "sfx/"
    #define RUTA_SERVER_BASE "server_src/mapas_disponibles/"
    #define RUTA_BASE_EDITOR "editor/mapas/"
    #define RUTA_BASE_CLIENT_CONFIG ""
#endif

#define RUTA_IMAGENES(x) (std::string(RUTA_BASE_IMAGENES) + std::string(x))
#define RUTA_SONIDO(x) (std::string(RUTA_BASE_SONIDOS) + std::string(x))
#define RUTA_SERVER(x) (QString(RUTA_SERVER_BASE) + QString(x))
#define RUTA_CONFIG_CLIENT(x) (std::string(RUTA_BASE_CLIENT_CONFIG) + std::string(x))

#endif // RUTA_BASE_H
