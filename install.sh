set -e 

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

echo "[3/4] Instalando archivos..."

sudo rm -rf /var/$TP_NAME/assets

sudo mkdir -p /usr/bin
sudo mkdir -p /var/$TP_NAME/assets
sudo mkdir -p /etc/$TP_NAME
sudo mkdir -p /usr/share/applications
sudo mkdir -p /usr/share/pixmaps
sudo mkdir -p /var/$TP_NAME/editor/mapas
sudo mkdir -p /var/$TP_NAME/server/mapas_disponibles

sudo rm -rf /var/$TP_NAME/editor/mapas/*
sudo rm -rf /var/$TP_NAME/server/mapas_disponibles/*

sudo chmod 777 /var/$TP_NAME/editor/mapas
sudo chmod 777 /var/$TP_NAME/server/mapas_disponibles

sudo cp ./client /usr/bin/$TP_NAME-client
sudo cp ./server /usr/bin/$TP_NAME-server
sudo cp ./taller_editor /usr/bin/$TP_NAME-editor
sudo cp -r editor/mapas /var/$TP_NAME/editor/
sudo cp -r server_src/mapas_disponibles /var/$TP_NAME/server/


sudo cp configuracion.yaml /etc/$TP_NAME/
sudo cp client_config.yaml /etc/$TP_NAME/
if [ -d config ]; then
  sudo cp -r config/* /etc/$TP_NAME/
fi

sudo cp -r gfx /var/$TP_NAME/assets/
sudo cp -r sfx /var/$TP_NAME/assets/ 2>/dev/null || true

sudo cp gfx/logos/editor.png /usr/share/pixmaps/$TP_NAME-editor.png
sudo cp gfx/logos/cliente.png /usr/share/pixmaps/$TP_NAME-client.png
sudo cp gfx/logos/server.png  /usr/share/pixmaps/$TP_NAME-server.png

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
Terminal=false
Categories=Game;
EOF

# Servidor
cat <<EOF | sudo tee /usr/share/applications/$TP_NAME-server.desktop > /dev/null
[Desktop Entry]
Type=Application
Name=CounterStrike2D Servidor
Exec=/usr/bin/$TP_NAME-server /etc/$TP_NAME/configuracion.yaml
Icon=$TP_NAME-server
Terminal=true
Categories=Game;
EOF

echo "[3.2] Copiando accesos al escritorio del usuario actual..."
DESKTOP_DIR="$(eval echo ~$SUDO_USER)/Desktop"

mkdir -p "$DESKTOP_DIR"

cp /usr/share/applications/$TP_NAME-*.desktop "$DESKTOP_DIR/"
chmod +x "$DESKTOP_DIR/$TP_NAME-"*.desktop

echo "[3.3] Si ves 'Untrusted Desktop File', hacé click derecho en los íconos del escritorio y seleccioná 'Permitir lanzamiento'."

echo "[4/4] Instalación completada con éxito ✅"
