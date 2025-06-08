#include "map_selection.h"
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>

MapSelectionDialog::MapSelectionDialog(const QVector<QPair<QString, QString>>& mapas, QWidget* parent)
    : QDialog(parent), selectedMap("") {
    setModal(true);
    setWindowTitle("Seleccionar Mapa");
    resize(600, 400);

    setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    listaMapas = new QListWidget(this);
    previewLabel = new QLabel("Previsualización del mapa", this);
    QPushButton* okButton = new QPushButton("Aceptar", this);

    previewLabel->setFixedSize(300, 300); 

    // Estética
    listaMapas->setStyleSheet("QListWidget { background-color: #222; color: white; }");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setStyleSheet("QLabel { background-color: #444; color: white; padding: 10px; }");
    okButton->setStyleSheet("QPushButton { background-color: #2a82da; color: white; padding: 6px; }");

    // Layout
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(listaMapas);
    layout->addWidget(previewLabel, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(okButton);

    setLayout(mainLayout);

    // Guardamos el mapeo
    for (const auto& par : mapas) {
        QListWidgetItem* item = new QListWidgetItem(par.first);
        item->setToolTip("Preview disponible a la derecha");
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
        previewLabel->setPixmap(miniatura.scaled(previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
