#ifndef MAP_SELECTION_H
#define MAP_SELECTION_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class MapSelectionDialog : public QDialog {
    Q_OBJECT

public:
    MapSelectionDialog(const QVector<QPair<QString, QString>>& mapas, QWidget* parent = nullptr);
    QString getSelectedMap() const;

private:
    QListWidget* listaMapas;
    QLabel* previewLabel;
    QString selectedMap;
    QMap<QString, QString> mapaToMiniatura; 

private slots:
    void mostrarPreview(QListWidgetItem* item);
    void aceptarSeleccion();
};


#endif
