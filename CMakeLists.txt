cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS ON)

project(taller_tp
    VERSION 1.0
    HOMEPAGE_URL "https://github.com/martinagualdi/CounterStrike2D/"
    LANGUAGES CXX
    )

option(INSTALADO "Compilar con rutas instaladas" OFF)

if(INSTALADO)
    message(STATUS "Compilando con INSTALADO=ON")
    add_compile_definitions(INSTALADO)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia MultimediaWidgets)
set(CMAKE_AUTOMOC ON)

option(TALLER_TESTS "Enable / disable tests." ON)
option(TALLER_CLIENT "Enable / disable client program." ON)
option(TALLER_SERVER "Enable / disable server program." ON)
option(TALLER_EDITOR "Enable / disable editor program." ON)
option(TALLER_MAKE_WARNINGS_AS_ERRORS "Enable / disable warnings as errors." ON)

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Define target: lib common between client, server and editor programs
# HEY TODO XXX: change the name to something that you like
add_library(taller_common STATIC)

# add the source files that make the lib
# HEY!! TODO XXX: add more subfolders here
add_subdirectory(common_src/)


# set compiler warnings to the targets
include(cmake/CompilerWarnings.cmake)
set_project_warnings(taller_common ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

# Ignorar sugerencias de override como error (por liberror)
target_compile_options(taller_common PRIVATE -Wno-error=suggest-override)


# Say where the headers will be found required by library target
# Make this PUBLIC so any dependant linking lib will inherit this
# location (so our dependants will not have to explicitly request this)
target_include_directories(taller_common PUBLIC .)

# HEY!! TODO XXX: you need to install some runtime and dev libraries *before*
# compiling the client/editor code.
#
# In xubuntu 24.04 these are:
#
# sudo apt-get install libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack
#

# Install libSDL2 and libSDL2pp (for client or editor)
# HEY!! TODO XXX: add more dependendies (either for client, server or editor)
if(TALLER_CLIENT OR TALLER_EDITOR)
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-2.8.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-2.22.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    set(SDL2PP_WITH_TTF ON CACHE BOOL "" FORCE)
    set(SDL2PP_WITH_MIXER ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        libSDL2pp
        OVERRIDE_FIND_PACKAGE
        URL https://github.com/libSDL2pp/libSDL2pp/archive/cc198c9a5657048bee67ece82de620b2d5661084.zip
    )

    FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer SDL2_ttf libSDL2pp)

    # YAML-CPP
    include(FetchContent)
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master  # o usa yaml-cpp-0.8.0 si existe
    )
    FetchContent_MakeAvailable(yaml-cpp)
    

endif()

# Program section
# ---------------
if(TALLER_CLIENT)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(client)

    # Make them depend on taller_common lib
    add_dependencies(client taller_common SDL2pp::SDL2pp)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(client_src)

    set_project_warnings(client ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(client PRIVATE ${libSDL2pp_SOURCE_DIR})

    # Link the taller_common lib target
    target_link_libraries(client taller_common SDL2pp::SDL2pp SDL2_image SDL2_ttf SDL2_mixer)

    target_link_libraries(client Qt6::Widgets)
    target_link_libraries(client Qt6::Multimedia)


endif()

if(TALLER_SERVER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(server)

    # Make them depend on taller_common lib
    add_dependencies(server taller_common)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(server_src)

    set_project_warnings(server ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)
    target_compile_options(server PRIVATE -Wno-error=suggest-override)

    # Link the taller_common lib target
    target_link_libraries(server taller_common)
    target_link_libraries(taller_common yaml-cpp)
endif()

if(TALLER_EDITOR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(taller_editor)

    # Make them depend on taller_common lib
    #add_dependencies(taller_editor taller_common SDL2pp::SDL2pp)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(editor)

    set_project_warnings(taller_editor ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(taller_editor PRIVATE ${libSDL2pp_SOURCE_DIR})

    # Link the taller_common lib target
    target_link_libraries(taller_editor taller_common SDL2pp::SDL2pp)

    target_link_libraries(taller_editor Qt6::Widgets)
    target_link_libraries(taller_editor Qt6::Multimedia)
    target_link_libraries(taller_editor yaml-cpp)

    # Habilitar MOC, UIC, etc. (útil para Qt)
    set_target_properties(taller_editor PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
    )
endif()


# Testing section
# ---------------

if(TALLER_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    add_executable(taller_tests
        tests/tests_protocolo.cpp
        client_src/client_protocol.cpp
        server_src/server_protocol.cpp
        server_src/jugador.cpp
        server_src/ak47.cpp
        server_src/glock.cpp
        server_src/m3.cpp
        server_src/awp.cpp
        server_src/bomba.cpp
        server_src/cuchillo.cpp
        common_src/configuracion.cpp
        server_src/municion.cpp
    )


    add_dependencies(taller_tests taller_common)

    target_include_directories(taller_tests PUBLIC .)

    add_subdirectory(tests/)

    set_project_warnings(taller_tests ${TALLER_MAKE_WARNINGS_AS_ERRORS} TRUE)

    target_include_directories(taller_tests
        PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
    )

    target_link_libraries(taller_tests
        taller_common
        GTest::gtest_main
        pthread
    )
endif()

