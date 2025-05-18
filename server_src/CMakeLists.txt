target_sources(server
    PRIVATE
    # .cpp files
    server_main.cpp
    acceptor.cpp
    gameloop.cpp
    server_protocol.cpp
    clienthandler.cpp
    lista_queues.cpp
    receiver.cpp
    sender.cpp
    server.cpp
    processor.cpp
    partida.cpp
    PUBLIC
    # .h files
    acceptor.h
    gameloop.h
    server_protocol.h
    clienthandler.h
    lista_queues.h
    receiver.h
    sender.h
    server.h
    processor.h
    mensaje_dto.h
    partida.h
    )