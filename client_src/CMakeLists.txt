target_sources(client
    PRIVATE
    client_event_handler.cpp
    client_main.cpp
    client_protocol.cpp
    client_receiver.cpp
    client_sender.cpp
    client_sonido.cpp
    client_parseador.cpp
    client.cpp
    client_main.cpp
    client_map.cpp
    client_dibujador.cpp
    lobby_window.cpp
    inicio_window.cpp
    map_selection.cpp
    PUBLIC
    client_event_handler.h
    client_map.h
    client_protocol.h
    client_receiver.h
    client_sender.h
    client_sonido.h
    client.h
    client_parseador.h
    client_dibujador.h
    lobby_window.h
    inicio_window.h
    mensaje_popup.h
    map_selection.h
    )