#ifndef INICIO_H
#define INICIO_H

#include <QWidget>
#include <QString>
#include <QMediaPlayer>
#include <QAudioOutput>

QT_BEGIN_NAMESPACE
namespace Ui { class Inicio; }
QT_END_NAMESPACE

class Inicio : public QWidget {
    Q_OBJECT

public:
    explicit Inicio(QWidget *parent = nullptr);
    ~Inicio();

signals:
    void datosIngresados(const QString& host, quint16 puerto, const QString& username);

private:
    Ui::Inicio *ui;
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
};

#endif // INICIO_H
