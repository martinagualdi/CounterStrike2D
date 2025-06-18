#include "map_selection.h"
#include "../common_src/ruta_base.h"
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
    QPushButton* cancelButton = new QPushButton("Cancelar", this);  // Nuevo botón

    previewLabel->setMinimumSize(256, 256);
    previewLabel->setMaximumSize(256, 256);
    previewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

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
    cancelButton->setFixedHeight(40);
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
    cancelButton->setStyleSheet(R"(
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

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(listaMapas);
    layout->addWidget(previewLabel);

    cancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    okButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* botonesLayout = new QHBoxLayout;
    botonesLayout->setSpacing(0);
    botonesLayout->setContentsMargins(0, 10, 0, 0);
    botonesLayout->addWidget(cancelButton);
    botonesLayout->addWidget(okButton);


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->insertWidget(0, titulo);
    mainLayout->addLayout(layout);
    mainLayout->addLayout(botonesLayout);

    setLayout(mainLayout);

    for (const auto& par : mapas) {
        QListWidgetItem* item = new QListWidgetItem(par.first);
        listaMapas->addItem(item);
        mapaToMiniatura[par.first] = par.second;
    }

    connect(listaMapas, &QListWidget::itemEntered, this, &MapSelectionDialog::mostrarPreview);
    connect(listaMapas, &QListWidget::itemClicked, this, &MapSelectionDialog::mostrarPreview);
    connect(okButton, &QPushButton::clicked, this, &MapSelectionDialog::aceptarSeleccion);
    connect(cancelButton, &QPushButton::clicked, this, &MapSelectionDialog::reject);

    listaMapas->setMouseTracking(true);
}

void MapSelectionDialog::mostrarPreview(QListWidgetItem* item) {
    QString nombreMapa = item->text();
    QString rutaMiniatura = mapaToMiniatura.value(nombreMapa);

    QString fullPath = RUTA_SERVER("") + rutaMiniatura;

    if (QFile::exists(fullPath)) {
        QPixmap miniatura(fullPath);
        previewLabel->clear();
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
