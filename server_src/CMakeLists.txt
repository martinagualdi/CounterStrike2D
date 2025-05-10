target_sources(taller_server
    PRIVATE
    # .cpp files
    server_main.cpp
    acceptor.cpp
    gameloop.cpp
    server_protocol.cpp
    client_handler.cpp
    lista_queues.cpp
    receiver.cpp
    sender.cpp
    server.cpp
    PUBLIC
    # .h files
    acceptor.h
    gameloop.h
    server_protocol.h
    client_handler.h
    lista_queues.h
    receiver.h
    sender.h
    server.h
    )