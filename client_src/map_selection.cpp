#include "map_selection.h"
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>

MapSelectionDialog::MapSelectionDialog(const QVector<QPair<QString, QString>>& mapas, QWidget* parent)
    : QDialog(parent), selectedMap("") {
    setModal(true);
    setWindowTitle("Seleccionar Mapa");
    resize(600, 400);

    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: rgba(0, 0, 0, 200);");

    QLabel* titulo = new QLabel("CS2D  Seleccione el mapa", this);
    listaMapas = new QListWidget(this);
    previewLabel = new QLabel("Previsualización del mapa", this);
    QPushButton* okButton = new QPushButton("Aceptar", this);

    previewLabel->setMinimumSize(256, 256);
    previewLabel->setMaximumSize(256, 256);
    previewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Estética
    titulo->setAlignment(Qt::AlignLeft);
    titulo->setStyleSheet(R"(
        QLabel {
            color: yellow;
            font-weight: bold;
            font-size: 24px;
            padding: 10px;
        }
    )");

    listaMapas->setStyleSheet(R"(
        QListWidget {
            color: yellow;
            font-weight: bold;
            font-size: 20px;
            border: none;
        }
        QListWidget::item:hover {
            background-color: rgba(255, 255, 255, 40);
        }
        QListWidget::item:selected {
            background-color: rgba(255, 255, 255, 60);
        }
    )");

    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setStyleSheet(R"(
        QLabel {
            background-color: #222;
            color: white;
            padding: 0px;
            border: none;
            margin: 0px;
        }
    )");

    okButton->setFixedHeight(40);
    okButton->setStyleSheet(R"(
        QPushButton {
            color: yellow;
            font-weight: bold;
            font-size: 16px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: rgb(255, 255, 0);
            color: black;
        }
    )");

    // Layout
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(listaMapas);
    layout->addWidget(previewLabel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->insertWidget(0, titulo);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(okButton);

    setLayout(mainLayout);

    // Guardamos el mapeo
    for (const auto& par : mapas) {
        QListWidgetItem* item = new QListWidgetItem(par.first);
        listaMapas->addItem(item);
        this->mapaToMiniatura[par.first] = par.second;
    }

    // Conexiones
    connect(listaMapas, &QListWidget::itemEntered, this, &MapSelectionDialog::mostrarPreview);
    connect(listaMapas, &QListWidget::itemClicked, this, &MapSelectionDialog::mostrarPreview);
    connect(okButton, &QPushButton::clicked, this, &MapSelectionDialog::aceptarSeleccion);

    listaMapas->setMouseTracking(true);
}

void MapSelectionDialog::mostrarPreview(QListWidgetItem* item) {
    QString nombreMapa = item->text();
    QString rutaMiniatura = mapaToMiniatura.value(nombreMapa);

    QString basePath = QCoreApplication::applicationDirPath();
    QString fullPath = basePath + "/server_src/mapas_disponibles/" + rutaMiniatura;

    if (QFile::exists(fullPath)) {
        QPixmap miniatura(fullPath);
        previewLabel->clear();  // Limpiar cualquier texto o imagen anterior
        previewLabel->setPixmap(miniatura.scaled(
            previewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));
    } else {
        previewLabel->setText("No se encontró la miniatura para:\n" + nombreMapa);
    }
}

void MapSelectionDialog::aceptarSeleccion() {
    QListWidgetItem* item = listaMapas->currentItem();
    if (item) {
        selectedMap = item->text();
        accept();
    }
}

QString MapSelectionDialog::getSelectedMap() const {
    return selectedMap;
}
