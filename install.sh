#!/bin/bash

set -e  # Corta si algo falla

REAL_USER=$(logname)
USER_HOME=$(eval echo ~$REAL_USER)

TP_NAME="CounterStrike2D"

echo "[1/4] Instalando dependencias del sistema..."
sudo apt-get update
sudo apt-get install -y \
  cmake g++ qt6-base-dev qt6-multimedia-dev \
  libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev \
  libwavpack1 libwavpack-dev libfreetype-dev \
  libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev \
  libyaml-cpp-dev

echo "[2/4] Compilando proyecto y corriendo tests..."
#make build-and-test

echo "[3/4] Instalando archivos..."

# Limpiar assets viejos
sudo rm -rf /var/$TP_NAME/assets

# Crear directorios
sudo mkdir -p /usr/bin
sudo mkdir -p /var/$TP_NAME/assets
sudo mkdir -p /etc/$TP_NAME
sudo mkdir -p /usr/share/applications
sudo mkdir -p /usr/share/pixmaps
sudo mkdir -p /var/$TP_NAME/editor/mapas
# Instalar mapas disponibles para el servidor
sudo mkdir -p /var/$TP_NAME/server/mapas_disponibles


# Copiar binarios
sudo cp ./client /usr/bin/$TP_NAME-client
sudo cp ./server /usr/bin/$TP_NAME-server
sudo cp ./taller_editor /usr/bin/$TP_NAME-editor
sudo cp -r editor/mapas /var/$TP_NAME/editor/
sudo cp -r server_src/mapas_disponibles/* /var/$TP_NAME/server/mapas_disponibles/

# Copiar configuración principal
sudo cp configuracion.yaml /etc/$TP_NAME/

# Copiar otros archivos de config si existen
if [ -d config ]; then
  sudo cp -r config/* /etc/$TP_NAME/
fi

# Copiar assets
sudo cp -r gfx /var/$TP_NAME/assets/
sudo cp -r sfx /var/$TP_NAME/assets/ 2>/dev/null || true

# Copiar íconos desde gfx/logos/
sudo cp gfx/logos/editor.png /usr/share/pixmaps/$TP_NAME-editor.png
sudo cp gfx/logos/cliente.png /usr/share/pixmaps/$TP_NAME-client.png
sudo cp gfx/logos/server.png  /usr/share/pixmaps/$TP_NAME-server.png

# Crear accesos directos
echo "[3.1] Creando accesos de escritorio..."

# Editor
cat <<EOF | sudo tee /usr/share/applications/$TP_NAME-editor.desktop > /dev/null
[Desktop Entry]
Type=Application
Name=CounterStrike2D Editor
Exec=/usr/bin/$TP_NAME-editor
Icon=$TP_NAME-editor
Terminal=false
Categories=Game;
EOF

# Cliente
cat <<EOF | sudo tee /usr/share/applications/$TP_NAME-client.desktop > /dev/null
[Desktop Entry]
Type=Application
Name=CounterStrike2D Cliente
Exec=/usr/bin/$TP_NAME-client 
Icon=$TP_NAME-client
Path=/home/parallels/Desktop/CounterStrike2D
Terminal=true
Categories=Game;
EOF

# Servidor
cat <<EOF | sudo tee /usr/share/applications/$TP_NAME-server.desktop > /dev/null
[Desktop Entry]
Type=Application
Name=CounterStrike2D Servidor
Exec=/usr/bin/$TP_NAME-server /etc/$TP_NAME/configuracion.yaml
Icon=$TP_NAME-server
Path=/home/parallels/Desktop/CounterStrike2D
Terminal=true
Categories=Game;
EOF

echo "[3.2] Copiando accesos al escritorio del usuario actual..."
DESKTOP_DIR="$USER_HOME/Desktop"

# Crear escritorio si no existe
mkdir -p "$DESKTOP_DIR"

# Copiar y permitir ejecución
cp /usr/share/applications/$TP_NAME-*.desktop "$DESKTOP_DIR/"
chmod +x "$DESKTOP_DIR/$TP_NAME-"*.desktop

echo "[3.3] Si ves 'Untrusted Desktop File', hacé click derecho en los íconos del escritorio y seleccioná 'Permitir lanzamiento'."

echo "[4/4] Instalación completada con éxito ✅"
