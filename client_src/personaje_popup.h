#ifndef PERSONAJE_WINDOW_H
#define PERSONAJE_WINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalMapper>

class PersonajePopup : public QDialog {
    Q_OBJECT

public:
    explicit PersonajePopup(QWidget* parent = nullptr);

signals:
    void skinSeleccionado(int skin);

private slots:
    void paintEvent(QPaintEvent* event) override;
    void seleccionarSkin(int skinIndex);
    void mostrarSkins(const QString& basePath, const QStringList& skinFiles);

private:
    void mostrarOpcionesT();
    void mostrarOpcionesCT();

    QWidget* skinsWidget;
    QHBoxLayout* skinsLayout;

    QVBoxLayout* mainLayout;

    QSignalMapper* signalMapper;

    int equipoSeleccionado = -1;
};

#endif // PERSONAJE_WINDOW_H
