#ifndef GESTORAUDIO_H
#define GESTORAUDIO_H

#include <QObject>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMap>
#include <QString>
#include <QElapsedTimer>

class GestorAudio : public QObject {
    Q_OBJECT

public:
    explicit GestorAudio(QObject *parent = nullptr);
    ~GestorAudio() override;

    void inicializar();

    void reproducirMusica(const QString &archivo);
    void detenerMusica();

    void reproducirAmbiente(const QString &archivo);
    void detenerAmbiente();

    void reproducirCinematica(const QString &archivo);
    void detenerCinematica();

    void reproducirEfecto(const QString &nombre, int cooldownMs = 0);

    void establecerPausa(bool pausado);
    void alternarSilencio();
    bool estaSilenciado() const;

private:
    QString buscarCarpetaBase() const;
    void cargarEfecto(const QString &nombre, const QString &archivo, qreal volumen = 0.65);
    void aplicarVolumenes();

    QString carpetaBase;
    bool silenciado;
    bool enPausa;

    QString musicaActual;
    QString ambienteActual;

    QMediaPlayer *reproductorMusica;
    QMediaPlayer *reproductorAmbiente;
    QMediaPlayer *reproductorCinematica;
    QAudioOutput *salidaMusica;
    QAudioOutput *salidaAmbiente;
    QAudioOutput *salidaCinematica;

    QMap<QString, QSoundEffect*> efectos;
    QMap<QString, qreal> volumenesBaseEfectos;
    QMap<QString, qint64> ultimoDisparoEfecto;
    QElapsedTimer reloj;
};

#endif // GESTORAUDIO_H