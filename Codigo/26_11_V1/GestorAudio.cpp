#include "GestorAudio.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

GestorAudio::GestorAudio(QObject *parent)
    : QObject(parent),
    silenciado(false),
    enPausa(false),
    reproductorMusica(new QSoundEffect(this)),
    reproductorAmbiente(new QSoundEffect(this)),
    reproductorCinematica(new QSoundEffect(this)) {

    reloj.start();

    reproductorMusica->setLoopCount(QSoundEffect::Infinite);
    reproductorAmbiente->setLoopCount(QSoundEffect::Infinite);
    reproductorCinematica->setLoopCount(1);
}

GestorAudio::~GestorAudio() {
    detenerMusica();
    detenerAmbiente();
    detenerCinematica();
}

QString GestorAudio::buscarCarpetaBase() const {
    const QString appDir = QCoreApplication::applicationDirPath();
    QStringList rutasPosibles = {
        appDir + "/Audio_Kapax",
        appDir + "/audio",
        appDir + "/../Audio_Kapax",
        appDir + "/../audio",
        appDir + "/../../Audio_Kapax",
        appDir + "/../../audio",
        appDir + "/../../../Audio_Kapax",
        appDir + "/../../../audio",
        "Audio_Kapax",
        "audio"
    };

    for (const QString &ruta : rutasPosibles) {
        if (QDir(ruta + "/efectos").exists()) {
            return QFileInfo(ruta).absoluteFilePath();
        }
    }
    return QString();
}

void GestorAudio::inicializar() {
    carpetaBase = buscarCarpetaBase();
    if (carpetaBase.isEmpty()) {
        return;
    }

    // Precargar efectos cortos (~1.6 MB en total) para respuesta instantánea sin lag
    cargarEfecto("menu_seleccion",   "menu_seleccion.wav",   0.55);
    cargarEfecto("menu_confirmar",   "menu_confirmar.wav",   0.65);
    cargarEfecto("pausa",            "pausa.wav",            0.60);
    cargarEfecto("inicio_nivel",     "inicio_nivel.wav",     0.65);
    cargarEfecto("nivel_completado", "nivel_completado.wav", 0.75);
    cargarEfecto("victoria",         "victoria.wav",         0.75);
    cargarEfecto("game_over",        "game_over.wav",        0.75);
    cargarEfecto("tiempo_bajo",      "tiempo_bajo.wav",      0.55);

    cargarEfecto("salto",            "salto.wav",            0.55);
    cargarEfecto("doble_salto",      "doble_salto.wav",      0.65);
    cargarEfecto("aterrizaje",       "aterrizaje.wav",       0.40);
    cargarEfecto("paso_tierra",      "paso_tierra.wav",      0.28);
    cargarEfecto("nado_brazada",     "nado_brazada.wav",     0.32);
    cargarEfecto("entrada_agua",     "entrada_agua.wav",     0.65);

    cargarEfecto("recoger_gift",     "recoger_gift.wav",     0.65);
    cargarEfecto("fortachon",        "fortachon.wav",        0.70);
    cargarEfecto("saltarin",         "saltarin.wav",         0.70);
    cargarEfecto("pesado",           "pesado.wav",           0.65);
    cargarEfecto("fin_poder",        "fin_poder.wav",        0.55);
    cargarEfecto("aturdido",         "aturdido.wav",         0.65);

    cargarEfecto("perder_vida",      "perder_vida.wav",      0.75);
    cargarEfecto("lanzamiento",      "lanzamiento.wav",      0.45);
    cargarEfecto("impacto_proyectil","impacto_proyectil.wav",0.65);
    cargarEfecto("golpe_enemigo",    "golpe_enemigo.wav",    0.65);
    cargarEfecto("lodazal",          "lodazal.wav",          0.45);
    cargarEfecto("resbalon",         "resbalon.wav",         0.55);
    cargarEfecto("mordedura_vibora", "mordedura_vibora.wav", 0.70);
    cargarEfecto("remolino",         "remolino.wav",         0.50);
    cargarEfecto("tronco_choque",    "tronco_choque.wav",    0.70);
    cargarEfecto("choque_obstaculo", "choque_obstaculo.wav", 0.65);

    aplicarVolumenes();
}

void GestorAudio::cargarEfecto(const QString &nombre, const QString &archivo, qreal volumen) {
    QString ruta = carpetaBase + "/efectos/" + archivo;
    if (!QFile::exists(ruta)) {
        return;
    }

    QSoundEffect *fx = new QSoundEffect(this);
    fx->setSource(QUrl::fromLocalFile(ruta));
    fx->setLoopCount(1);
    fx->setVolume(volumen);
    efectos.insert(nombre, fx);
    volumenesBaseEfectos.insert(nombre, volumen);
}

void GestorAudio::reproducirMusica(const QString &archivo) {
    if (carpetaBase.isEmpty()) {
        return;
    }
    if (musicaActual == archivo && reproductorMusica->isPlaying()) {
        return;
    }

    QString ruta = carpetaBase + "/musica/" + archivo;
    if (!QFile::exists(ruta)) {
        return;
    }

    reproductorMusica->stop();
    musicaActual = archivo;
    reproductorMusica->setSource(QUrl::fromLocalFile(ruta));
    aplicarVolumenes();
    reproductorMusica->play();
}

void GestorAudio::detenerMusica() {
    reproductorMusica->stop();
    musicaActual.clear();
}

void GestorAudio::reproducirAmbiente(const QString &archivo) {
    if (carpetaBase.isEmpty()) {
        return;
    }
    if (ambienteActual == archivo && reproductorAmbiente->isPlaying()) {
        return;
    }

    QString ruta = carpetaBase + "/ambiente/" + archivo;
    if (!QFile::exists(ruta)) {
        return;
    }

    reproductorAmbiente->stop();
    ambienteActual = archivo;
    reproductorAmbiente->setSource(QUrl::fromLocalFile(ruta));
    aplicarVolumenes();
    reproductorAmbiente->play();
}

void GestorAudio::detenerAmbiente() {
    reproductorAmbiente->stop();
    ambienteActual.clear();
}

void GestorAudio::reproducirCinematica(const QString &archivo) {
    if (carpetaBase.isEmpty()) {
        return;
    }

    QString ruta = carpetaBase + "/cinematicas/" + archivo;
    if (!QFile::exists(ruta)) {
        return;
    }

    reproductorCinematica->stop();
    reproductorCinematica->setSource(QUrl::fromLocalFile(ruta));
    aplicarVolumenes();
    reproductorCinematica->play();
}

void GestorAudio::detenerCinematica() {
    reproductorCinematica->stop();
}

void GestorAudio::reproducirEfecto(const QString &nombre, int cooldownMs) {
    if (silenciado || !efectos.contains(nombre)) {
        return;
    }

    qint64 ahora = reloj.elapsed();
    if (cooldownMs > 0 && ultimoDisparoEfecto.contains(nombre)) {
        if (ahora - ultimoDisparoEfecto.value(nombre) < cooldownMs) {
            return;
        }
    }

    ultimoDisparoEfecto.insert(nombre, ahora);
    QSoundEffect *fx = efectos.value(nombre);
    if (fx != nullptr) {
        fx->play();
    }
}

void GestorAudio::establecerPausa(bool pausado) {
    enPausa = pausado;
    aplicarVolumenes();
}

void GestorAudio::alternarSilencio() {
    silenciado = !silenciado;
    aplicarVolumenes();
}

bool GestorAudio::estaSilenciado() const {
    return silenciado;
}

void GestorAudio::aplicarVolumenes() {
    if (silenciado) {
        reproductorMusica->setVolume(0.0);
        reproductorAmbiente->setVolume(0.0);
        reproductorCinematica->setVolume(0.0);
        for (QSoundEffect *fx : efectos) {
            if (fx != nullptr) fx->setVolume(0.0);
        }
        return;
    }

    qreal factorPausa = enPausa ? 0.30 : 1.0;
    reproductorMusica->setVolume(0.32 * factorPausa);
    reproductorAmbiente->setVolume(0.24 * factorPausa);
    reproductorCinematica->setVolume(0.75);

    for (auto it = efectos.begin(); it != efectos.end(); ++it) {
        if (it.value() != nullptr) {
            it.value()->setVolume(volumenesBaseEfectos.value(it.key(), 0.65));
        }
    }
}