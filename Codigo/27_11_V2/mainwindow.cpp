#include "mainwindow.h"
#include "NivelRibera.h"
#include "NivelRio.h"
#include "Lodazal.h"
#include "Resbaladero.h"
#include "Vibora.h"
#include "TroncoFlotante.h"
#include "Remolino.h"
#include "Cazador.h"
#include "GiftFortachon.h"
#include "GiftSaltarin.h"

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QString>
#include <QImage>
#include <QTransform>
#include <QQueue>
#include <QPoint>
#include <QPainterPath>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QtMath>
#include <algorithm>
#include <cmath>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    pilaCentral(new QStackedWidget(this)),
    escena(new QGraphicsScene(this)),
    vista(new QGraphicsView(escena, this)),
    paginaVideo(new QWidget(this)),
    visorVideo(new QLabel(paginaVideo)),
    pelicula(new QMovie(this)),
    bufferVideo(new QBuffer(this)),
    etiquetaSaltar(new QLabel("Presiona [ESPACIO], [ENTER] o [ESC] para saltar la cinematica", paginaVideo)),
    timer(new QTimer(this)),
    juego(new Juego(2)),
    audio(new GestorAudio(this)),
    estadoPantalla(EstadoPantalla::MenuPrincipal),
    confirmandoSalidaMenu(false),
    mirandoDerecha(true),
    tiempoAnimacion(0.0f),
    spritesCargados(false),
    spritesNadoCargados(false),
    cinematicaActual(TipoCinematica::Ninguna),
    ultimoFrameVideo(-1),
    rioViolentoActivo(false) {

    setWindowTitle("La Travesía de Kapax - Menú Principal");
    setFixedSize(960, 600);
    setFocusPolicy(Qt::StrongFocus);

    // Página 0: Escena del juego
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    vista->setFocusPolicy(Qt::NoFocus);

    // Página 1: Visor de cinemáticas GIF
    paginaVideo->setStyleSheet("background-color: #0D1B2A;");
    paginaVideo->setFocusPolicy(Qt::NoFocus);

    visorVideo->setAlignment(Qt::AlignCenter);
    visorVideo->setStyleSheet("background-color: #0D1B2A;");
    visorVideo->setFocusPolicy(Qt::NoFocus);

    etiquetaSaltar->setAlignment(Qt::AlignCenter);
    etiquetaSaltar->setFixedHeight(36);
    etiquetaSaltar->setStyleSheet(
        "color: #F4C465; background-color: #09131E; font-family: Arial; "
        "font-size: 12px; font-weight: bold; border-top: 2px solid #F4C465;");

    QVBoxLayout *layoutVideo = new QVBoxLayout(paginaVideo);
    layoutVideo->setContentsMargins(0, 0, 0, 0);
    layoutVideo->setSpacing(0);
    layoutVideo->addWidget(visorVideo, 1);
    layoutVideo->addWidget(etiquetaSaltar, 0);

    pelicula->setCacheMode(QMovie::CacheNone);
    visorVideo->setMovie(pelicula);
    connect(pelicula, &QMovie::finished, this, &MainWindow::finalizarCinematica);
    connect(pelicula, &QMovie::frameChanged, this, &MainWindow::alCambiarFrameVideo);

    pilaCentral->addWidget(vista);
    pilaCentral->addWidget(paginaVideo);
    pilaCentral->setCurrentWidget(vista);
    setCentralWidget(pilaCentral);

    try {
        cargarSprites();
    } catch (const std::runtime_error &error) {
        spritesCargados = false;
        qWarning() << error.what() << "- se usaran figuras de respaldo.";
    }

    try {
        audio->inicializar();
    } catch (const std::runtime_error &error) {
        qWarning() << error.what() << "- el juego continuara sin audio.";
    }
    audio->reproducirMusica("menu.wav");

    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarFrame);
    timer->start(16);
}

MainWindow::~MainWindow() {
    timer->stop();
    pelicula->stop();
    delete juego;
}

void MainWindow::activarAudioNivelActual() {
    audio->establecerPausa(false);
    rioViolentoActivo = false;

    if (juego->getNivelActual() == 2) {
        audio->reproducirMusica("nivel2_rio.wav");
        audio->reproducirAmbiente("rio.wav");
        audio->reproducirEfecto("entrada_agua");
        audio->reproducirEfecto("inicio_nivel");
    } else {
        audio->reproducirMusica("nivel1_selva.wav");
        audio->reproducirAmbiente("selva.wav");
        audio->reproducirEfecto("inicio_nivel");
    }
}

void MainWindow::volverAlMenuPrincipal() {
    confirmandoSalidaMenu = false;
    estadoPantalla = EstadoPantalla::MenuPrincipal;
    teclasPresionadas.clear();
    audio->establecerPausa(false);
    audio->detenerAmbiente();
    audio->detenerCinematica();
    audio->reproducirEfecto("menu_seleccion");
    audio->reproducirMusica("menu.wav");
}

QString MainWindow::buscarRutaVideo(const QString &nombreArchivo) const {
    const QString rutaRecurso = ":/videos/" + nombreArchivo;
    if (QFile::exists(rutaRecurso)) {
        return rutaRecurso;
    }

    const QString appDir = QCoreApplication::applicationDirPath();
    QStringList rutasPosibles = {
        appDir + "/videos/" + nombreArchivo,
        appDir + "/" + nombreArchivo,
        appDir + "/../videos/" + nombreArchivo,
        appDir + "/../../videos/" + nombreArchivo,
        appDir + "/../../../videos/" + nombreArchivo,
        "videos/" + nombreArchivo,
        nombreArchivo
    };

    for (const QString &ruta : rutasPosibles) {
        if (QFile::exists(ruta)) {
            return QFileInfo(ruta).absoluteFilePath();
        }
    }
    return QString();
}

void MainWindow::reproducirCinematica(TipoCinematica tipo) {
    QString nombreVideo;
    QString nombreAudio;

    if (tipo == TipoCinematica::Intro) {
        nombreVideo = "01_intro.gif";
        nombreAudio = "intro.wav";
    } else if (tipo == TipoCinematica::Transicion) {
        nombreVideo = "02_transicion.gif";
        nombreAudio = "transicion.wav";
    } else if (tipo == TipoCinematica::Victoria) {
        nombreVideo = "03_victoria.gif";
        nombreAudio = "victoria.wav";
    } else if (tipo == TipoCinematica::GameOver) {
        nombreVideo = "04_game_over.gif";
        nombreAudio = "game_over.wav";
    }

    QString ruta = buscarRutaVideo(nombreVideo);
    if (ruta.isEmpty()) {
        cinematicaActual = TipoCinematica::Ninguna;
        pilaCentral->setCurrentWidget(vista);
        if (tipo == TipoCinematica::Intro || tipo == TipoCinematica::Transicion) {
            activarAudioNivelActual();
        } else if (tipo == TipoCinematica::Victoria) {
            audio->detenerMusica();
            audio->detenerAmbiente();
            audio->reproducirEfecto("victoria");
        } else if (tipo == TipoCinematica::GameOver) {
            audio->detenerMusica();
            audio->detenerAmbiente();
            audio->reproducirEfecto("game_over");
        }
        return;
    }

    pelicula->stop();
    if (ruta.startsWith(":/")) {
        QFile archivoGif(ruta);
        if (!archivoGif.open(QIODevice::ReadOnly)) {
            cinematicaActual = TipoCinematica::Ninguna;
            pilaCentral->setCurrentWidget(vista);
            return;
        }
        bufferVideo->close();
        bufferVideo->setData(archivoGif.readAll());
        bufferVideo->open(QIODevice::ReadOnly);
        pelicula->setDevice(bufferVideo);
    } else {
        bufferVideo->close();
        bufferVideo->setData(QByteArray());
        pelicula->setDevice(nullptr);
        pelicula->setFileName(ruta);
    }
    if (!pelicula->isValid()) {
        cinematicaActual = TipoCinematica::Ninguna;
        pilaCentral->setCurrentWidget(vista);
        if (tipo == TipoCinematica::Intro || tipo == TipoCinematica::Transicion) {
            activarAudioNivelActual();
        }
        return;
    }

    audio->detenerMusica();
    audio->detenerAmbiente();
    audio->reproducirCinematica(nombreAudio);

    teclasPresionadas.clear();
    ultimoFrameVideo = -1;
    cinematicaActual = tipo;
    pelicula->setScaledSize(QSize(960, 540));
    pilaCentral->setCurrentWidget(paginaVideo);
    pelicula->start();
    setFocus();
}

void MainWindow::alCambiarFrameVideo(int frameActual) {
    if (cinematicaActual == TipoCinematica::Ninguna) {
        return;
    }

    if (ultimoFrameVideo > 0 && frameActual <= ultimoFrameVideo) {
        finalizarCinematica();
        return;
    }
    ultimoFrameVideo = frameActual;
}

void MainWindow::finalizarCinematica() {
    if (cinematicaActual == TipoCinematica::Ninguna) {
        return;
    }

    TipoCinematica terminada = cinematicaActual;

    pelicula->stop();
    pelicula->setFileName(QString());
    audio->detenerCinematica();
    ultimoFrameVideo = -1;
    cinematicaActual = TipoCinematica::Ninguna;
    teclasPresionadas.clear();
    pilaCentral->setCurrentWidget(vista);
    setFocus();

    if (terminada == TipoCinematica::Intro || terminada == TipoCinematica::Transicion) {
        activarAudioNivelActual();
    } else if (terminada == TipoCinematica::Victoria) {
        audio->reproducirEfecto("victoria");
    } else if (terminada == TipoCinematica::GameOver) {
        audio->reproducirEfecto("game_over");
    }

    renderizarEscena();
}

QPixmap MainWindow::recortarSinBordeBlanco(const QPixmap &hoja, double rx, double ry, double rw, double rh) {
    int x = static_cast<int>(hoja.width() * rx);
    int y = static_cast<int>(hoja.height() * ry);
    int w = static_cast<int>(hoja.width() * rw);
    int h = static_cast<int>(hoja.height() * rh);
    return hoja.copy(x, y, w, h);
}

QPixmap MainWindow::limpiarFondoCelda(const QPixmap &celda, int tamObjetivo) {
    QImage img = celda.toImage().convertToFormat(QImage::Format_ARGB32);
    int w = img.width();
    int h = img.height();
    if (w <= 0 || h <= 0) {
        return QPixmap();
    }

    auto esFondoBlanco = [&](int px, int py) {
        QRgb c = img.pixel(px, py);
        int r = qRed(c);
        int g = qGreen(c);
        int b = qBlue(c);
        return (r > 225 && g > 225 && b > 225 && std::abs(r - g) < 18 && std::abs(g - b) < 18);
    };

    QVector<bool> visitado(w * h, false);
    QQueue<QPoint> cola;

    auto encolarSiFondo = [&](int px, int py) {
        int idx = py * w + px;
        if (!visitado[idx] && esFondoBlanco(px, py)) {
            visitado[idx] = true;
            img.setPixel(px, py, qRgba(0, 0, 0, 0));
            cola.enqueue(QPoint(px, py));
        }
    };

    for (int px = 0; px < w; ++px) {
        encolarSiFondo(px, 0);
        encolarSiFondo(px, h - 1);
    }
    for (int py = 0; py < h; ++py) {
        encolarSiFondo(0, py);
        encolarSiFondo(w - 1, py);
    }

    const int dx4[4] = {1, -1, 0, 0};
    const int dy4[4] = {0, 0, 1, -1};
    while (!cola.isEmpty()) {
        QPoint pt = cola.dequeue();
        for (int k = 0; k < 4; ++k) {
            int nx = pt.x() + dx4[k];
            int ny = pt.y() + dy4[k];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                encolarSiFondo(nx, ny);
            }
        }
    }

    int minX = w, maxX = -1, minY = h, maxY = -1;
    for (int py = 0; py < h; ++py) {
        for (int px = 0; px < w; ++px) {
            if (qAlpha(img.pixel(px, py)) > 20) {
                if (px < minX) minX = px;
                if (px > maxX) maxX = px;
                if (py < minY) minY = py;
                if (py > maxY) maxY = py;
            }
        }
    }

    if (maxX < minX || maxY < minY) {
        return QPixmap();
    }

    QImage recorte = img.copy(minX, minY, maxX - minX + 1, maxY - minY + 1);
    QPixmap pix = QPixmap::fromImage(recorte);
    return pix.scaled(tamObjetivo, tamObjetivo, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QVector<QPixmap> MainWindow::extraerFilaSprites(const QPixmap &hoja, double rx, double ry, double rw, double rh, int altoObjetivo) {
    QVector<QPixmap> resultado;

    QImage img = recortarSinBordeBlanco(hoja, rx, ry, rw, rh).toImage().convertToFormat(QImage::Format_ARGB32);
    int w = img.width();
    int h = img.height();
    if (w <= 0 || h <= 0) {
        return resultado;
    }

    auto esFondoClaro = [&](int px, int py) {
        QRgb c = img.pixel(px, py);
        int r = qRed(c);
        int g = qGreen(c);
        int b = qBlue(c);
        return (r > 210 && g > 210 && b > 210 && std::abs(r - g) < 20 && std::abs(g - b) < 20);
    };

    QVector<bool> visitado(w * h, false);
    QQueue<QPoint> cola;

    auto encolarSiFondo = [&](int px, int py) {
        int idx = py * w + px;
        if (!visitado[idx] && esFondoClaro(px, py)) {
            visitado[idx] = true;
            img.setPixel(px, py, qRgba(0, 0, 0, 0));
            cola.enqueue(QPoint(px, py));
        }
    };

    for (int px = 0; px < w; ++px) {
        encolarSiFondo(px, 0);
        encolarSiFondo(px, h - 1);
    }
    for (int py = 0; py < h; ++py) {
        encolarSiFondo(0, py);
        encolarSiFondo(w - 1, py);
    }

    const int dx4[4] = {1, -1, 0, 0};
    const int dy4[4] = {0, 0, 1, -1};
    while (!cola.isEmpty()) {
        QPoint pt = cola.dequeue();
        for (int k = 0; k < 4; ++k) {
            int nx = pt.x() + dx4[k];
            int ny = pt.y() + dy4[k];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                encolarSiFondo(nx, ny);
            }
        }
    }

    struct Componente {
        int id;
        int minX, maxX, minY, maxY;
        int pixeles;
    };

    QVector<int> compId(w * h, 0);
    QVector<Componente> componentes;
    int idActual = 0;
    const int minAltoValido = std::max(12, h / 4);

    for (int py = 0; py < h; ++py) {
        for (int px = 0; px < w; ++px) {
            int idx = py * w + px;
            if (compId[idx] == 0 && qAlpha(img.pixel(px, py)) > 20) {
                ++idActual;
                Componente comp{idActual, px, px, py, py, 0};
                QQueue<QPoint> q;
                compId[idx] = idActual;
                q.enqueue(QPoint(px, py));

                while (!q.isEmpty()) {
                    QPoint p = q.dequeue();
                    ++comp.pixeles;
                    if (p.x() < comp.minX) comp.minX = p.x();
                    if (p.x() > comp.maxX) comp.maxX = p.x();
                    if (p.y() < comp.minY) comp.minY = p.y();
                    if (p.y() > comp.maxY) comp.maxY = p.y();

                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) continue;
                            int nx = p.x() + dx;
                            int ny = p.y() + dy;
                            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                int nIdx = ny * w + nx;
                                if (compId[nIdx] == 0 && qAlpha(img.pixel(nx, ny)) > 20) {
                                    compId[nIdx] = idActual;
                                    q.enqueue(QPoint(nx, ny));
                                }
                            }
                        }
                    }
                }

                int compW = comp.maxX - comp.minX + 1;
                int compH = comp.maxY - comp.minY + 1;
                if (comp.pixeles >= 60 && compW >= 8 && compH >= minAltoValido) {
                    componentes.append(comp);
                }
            }
        }
    }

    std::sort(componentes.begin(), componentes.end(), [](const Componente &a, const Componente &b) {
        return a.minX < b.minX;
    });

    for (const Componente &comp : componentes) {
        int cw = comp.maxX - comp.minX + 1;
        int ch = comp.maxY - comp.minY + 1;
        QImage recorte(cw, ch, QImage::Format_ARGB32);
        recorte.fill(Qt::transparent);

        for (int sy = comp.minY; sy <= comp.maxY; ++sy) {
            for (int sx = comp.minX; sx <= comp.maxX; ++sx) {
                if (compId[sy * w + sx] == comp.id) {
                    recorte.setPixel(sx - comp.minX, sy - comp.minY, img.pixel(sx, sy));
                }
            }
        }

        QPixmap pix = QPixmap::fromImage(recorte);
        if (altoObjetivo > 0) {
            pix = pix.scaledToHeight(altoObjetivo, Qt::SmoothTransformation);
        }
        resultado.append(pix);
    }

    return resultado;
}

void MainWindow::cargarSprites() {
    QPixmap hoja;
    QStringList rutasPosibles = {
        ":/Sprites.png",
        ":/imagenes/Sprites.png",
        QCoreApplication::applicationDirPath() + "/Sprites.png",
        QCoreApplication::applicationDirPath() + "/../Sprites.png",
        QCoreApplication::applicationDirPath() + "/../../../Sprites.png",
        "Sprites.png"
    };

    for (const QString &ruta : rutasPosibles) {
        if (hoja.load(ruta)) {
            spritesCargados = true;
            break;
        }
    }
    if (!spritesCargados) {
        throw std::runtime_error("No se encontro Sprites.png");
    }

    auto voltear = [](const QPixmap &p) {
        return p.transformed(QTransform().scale(-1, 1));
    };

    if (spritesCargados) {
        fondoSelva = recortarSinBordeBlanco(hoja, 0.453, 0.850, 0.160, 0.126)
        .scaled(960, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QVector<QPixmap> idles = extraerFilaSprites(hoja, 0.008, 0.062, 0.195, 0.072, 60);
        for (const QPixmap &f : idles) {
            if (f.width() <= f.height() * 1.1) {
                kapaxIdleDer.append(f);
                kapaxIdleIzq.append(voltear(f));
                break;
            }
        }

        QVector<QPixmap> caminarRaw = extraerFilaSprites(hoja, 0.008, 0.148, 0.195, 0.074, 60);
        for (const QPixmap &f : caminarRaw) {
            if (f.width() <= f.height() * 1.1) {
                kapaxCaminarDer.append(f);
                kapaxCaminarIzq.append(voltear(f));
            }
        }
        if (kapaxCaminarDer.isEmpty() && !kapaxIdleDer.isEmpty()) {
            kapaxCaminarDer = kapaxIdleDer;
            kapaxCaminarIzq = kapaxIdleIzq;
        }

        QVector<QPixmap> saltos = extraerFilaSprites(hoja, 0.008, 0.308, 0.195, 0.084, 60);
        for (const QPixmap &f : saltos) {
            if (f.width() <= f.height() * 1.15) {
                kapaxSaltarDer = f;
                kapaxSaltarIzq = voltear(f);
                break;
            }
        }

        QVector<QPixmap> aturdidos = extraerFilaSprites(hoja, 0.215, 0.148, 0.185, 0.074, 60);
        if (!aturdidos.isEmpty()) {
            kapaxAturdidoDer = aturdidos.last();
            kapaxAturdidoIzq = voltear(kapaxAturdidoDer);
        }

        QVector<QPixmap> monos = extraerFilaSprites(hoja, 0.436, 0.068, 0.275, 0.085, 56);
        if (!monos.isEmpty()) {
            monoIdleDer = monos.first();
            monoIdleIzq = voltear(monoIdleDer);
            monoLanzandoDer = (monos.size() >= 4) ? monos[3] : monos.last();
            monoLanzandoIzq = voltear(monoLanzandoDer);
        }

        QVector<QPixmap> piedras = extraerFilaSprites(hoja, 0.725, 0.056, 0.230, 0.042, 20);
        if (!piedras.isEmpty()) {
            spritePiedra = piedras.first();
        }

        QVector<QPixmap> suelos = extraerFilaSprites(hoja, 0.008, 0.464, 0.160, 0.064, 0);
        if (!suelos.isEmpty()) {
            spriteSuelo = suelos.first().scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        QVector<QPixmap> maderas = extraerFilaSprites(hoja, 0.168, 0.468, 0.150, 0.048, 0);
        if (!maderas.isEmpty()) {
            spriteMadera150 = maderas.first().scaled(150, 22, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            spriteMadera160 = maderas.first().scaled(160, 22, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        QVector<QPixmap> lodos = extraerFilaSprites(hoja, 0.212, 0.574, 0.195, 0.052, 0);
        if (!lodos.isEmpty()) {
            spriteLodo = lodos.last().scaled(115, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            spritesLodoRio = lodos;
        }

        QVector<QPixmap> viboras = extraerFilaSprites(hoja, 0.095, 0.684, 0.044, 0.078, 44);
        if (!viboras.isEmpty()) {
            spriteVibora = viboras.first();
        }

        QVector<QPixmap> giftsRow = extraerFilaSprites(hoja, 0.722, 0.348, 0.268, 0.058, 30);
        if (giftsRow.size() >= 3) {
            spriteGiftFortachon = giftsRow[0];
            spriteGiftSaltarin = giftsRow[2];
        }

        QVector<QPixmap> corazones = extraerFilaSprites(hoja, 0.728, 0.498, 0.030, 0.044, 22);
        if (!corazones.isEmpty()) {
            spriteCorazon = corazones.first();
        }

        QVector<QPixmap> vegRaw = extraerFilaSprites(hoja, 0.008, 0.556, 0.205, 0.110, 68);
        for (const QPixmap &v : vegRaw) {
            if (v.width() >= 24 && v.width() <= 95) {
                vegetacionOrilla.append(v);
            }
        }

        QVector<QPixmap> spritesCazador = extraerFilaSprites(hoja, 0.437, 0.295, 0.282, 0.098, 66);
        QVector<QPixmap> figurasCazador;
        for (const QPixmap &sprite : spritesCazador) {
            if (sprite.height() >= 45 && sprite.width() <= 90) {
                figurasCazador.append(sprite);
            }
        }
        if (!figurasCazador.isEmpty()) {
            cazadorIdleDer = figurasCazador.first();
            cazadorIdleIzq = voltear(cazadorIdleDer);
            cazadorLanzandoDer = figurasCazador.last();
            cazadorLanzandoIzq = voltear(cazadorLanzandoDer);
        }

        spriteAguaRio = recortarSinBordeBlanco(hoja, 0.515, 0.478, 0.038, 0.039)
                            .scaled(680, 220, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        spriteTroncoFlotante = recortarSinBordeBlanco(hoja, 0.458, 0.561, 0.037, 0.035)
                                   .scaled(120, 30, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QVector<QPixmap> vortices = extraerFilaSprites(hoja, 0.443, 0.621, 0.280, 0.074, 0);
        for (int i = 0; i < qMin(3, vortices.size()); ++i) {
            spritesVortice.append(vortices[i]);
        }

        spriteJabalina = recortarSinBordeBlanco(hoja, 0.730, 0.227, 0.046, 0.030)
                             .scaled(56, 18, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    QPixmap hojaNado;
    QStringList rutasNado = {
        ":/Kapax_Nadando.png",
        ":/Sprites de Natación de Kapax en 16 Cuadros.png",
        QCoreApplication::applicationDirPath() + "/Kapax_Nadando.png",
        QCoreApplication::applicationDirPath() + "/../Kapax_Nadando.png",
        QCoreApplication::applicationDirPath() + "/../../../Kapax_Nadando.png",
        "Kapax_Nadando.png",
        "Sprites de Natación de Kapax en 16 Cuadros.png"
    };

    for (const QString &ruta : rutasNado) {
        if (hojaNado.load(ruta)) {
            spritesNadoCargados = true;
            break;
        }
    }

    if (spritesNadoCargados) {
        const int celdaW = hojaNado.width() / 4;
        const int celdaH = hojaNado.height() / 4;

        for (int col = 0; col < 4; ++col) {
            QPixmap fArriba = limpiarFondoCelda(hojaNado.copy(col * celdaW, 0 * celdaH, celdaW, celdaH), 66);
            QPixmap fAbajo  = limpiarFondoCelda(hojaNado.copy(col * celdaW, 1 * celdaH, celdaW, celdaH), 66);
            QPixmap fIzq    = limpiarFondoCelda(hojaNado.copy(col * celdaW, 2 * celdaH, celdaW, celdaH), 66);
            QPixmap fDer    = limpiarFondoCelda(hojaNado.copy(col * celdaW, 3 * celdaH, celdaW, celdaH), 66);

            if (!fArriba.isNull()) kapaxNadarArriba.append(fArriba);
            if (!fAbajo.isNull())  kapaxNadarAbajo.append(fAbajo);
            if (!fIzq.isNull())    kapaxNadarIzq.append(fIzq);
            if (!fDer.isNull())    kapaxNadarDer.append(fDer);
        }

        if (kapaxNadarArriba.isEmpty()) {
            spritesNadoCargados = false;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }

    int tecla = event->key();

    // Silenciar / activar audio en cualquier pantalla con la tecla X
    if (tecla == Qt::Key_X) {
        audio->alternarSilencio();
        return;
    }

    // 1. Durante cinemáticas
    if (cinematicaActual != TipoCinematica::Ninguna) {
        if (tecla == Qt::Key_Space || tecla == Qt::Key_Return || tecla == Qt::Key_Enter || tecla == Qt::Key_Escape) {
            finalizarCinematica();
        }
        return;
    }

    // 2. Menú Principal (Jugar / Salir)
    if (estadoPantalla == EstadoPantalla::MenuPrincipal) {
        if (tecla == Qt::Key_1 || tecla == Qt::Key_Return || tecla == Qt::Key_Enter || tecla == Qt::Key_Space) {
            audio->reproducirEfecto("menu_confirmar");
            estadoPantalla = EstadoPantalla::MenuDificultad;
        } else if (tecla == Qt::Key_2 || tecla == Qt::Key_Escape) {
            close();
        }
        return;
    }

    // 3. Menú de Dificultad (Fácil / Normal / Difícil / Volver)
    if (estadoPantalla == EstadoPantalla::MenuDificultad) {
        if (tecla == Qt::Key_1 || tecla == Qt::Key_2 || tecla == Qt::Key_3) {
            audio->reproducirEfecto("menu_confirmar");
            short dif = (tecla == Qt::Key_1) ? 1 : (tecla == Qt::Key_2 ? 2 : 3);
            try {
                juego->setDificultad(dif);
                juego->iniciar(1);
            } catch (const std::invalid_argument &error) {
                qWarning() << error.what();
                return;
            } catch (const std::out_of_range &error) {
                qWarning() << error.what();
                return;
            } catch (const std::runtime_error &error) {
                qWarning() << error.what();
                return;
            }
            estadoPantalla = EstadoPantalla::Jugando;
            confirmandoSalidaMenu = false;
            mirandoDerecha = true;
            reproducirCinematica(TipoCinematica::Intro);
        } else if (tecla == Qt::Key_Escape || tecla == Qt::Key_Backspace || tecla == Qt::Key_M) {
            audio->reproducirEfecto("menu_seleccion");
            estadoPantalla = EstadoPantalla::MenuPrincipal;
        }
        return;
    }

    // 4. Confirmación antes de salir al Menú Principal durante la partida
    if (confirmandoSalidaMenu) {
        if (tecla == Qt::Key_S || tecla == Qt::Key_Y || tecla == Qt::Key_Return || tecla == Qt::Key_Enter) {
            volverAlMenuPrincipal();
        } else if (tecla == Qt::Key_N || tecla == Qt::Key_Escape) {
            confirmandoSalidaMenu = false;
            audio->establecerPausa(juego->estaPausado());
            audio->reproducirEfecto("menu_seleccion");
        }
        return;
    }

    // 5. Pantalla final (Victoria o Game Over)
    if (juego->juegoTerminado()) {
        if (tecla == Qt::Key_M) {
            volverAlMenuPrincipal();
        } else if (juego->hayVictoria()) {
            // Al ganar, reiniciar vuelve al Nivel 1 desde cero
            if (tecla == Qt::Key_R || tecla == Qt::Key_N) {
                teclasPresionadas.clear();
                mirandoDerecha = true;
                juego->iniciar(1);
                activarAudioNivelActual();
            }
        } else {
            // En Game Over: R reintenta el nivel actual, N reinicia el juego completo desde el Nivel 1
            if (tecla == Qt::Key_R) {
                teclasPresionadas.clear();
                mirandoDerecha = true;
                if (juego->getNivelActual() == 1) {
                    juego->iniciar(1);
                } else {
                    juego->reiniciarNivel();
                }
                activarAudioNivelActual();
            } else if (tecla == Qt::Key_N) {
                teclasPresionadas.clear();
                mirandoDerecha = true;
                juego->iniciar(1);
                activarAudioNivelActual();
            }
        }
        return;
    }

    // 6. Partida activa o en pausa
    teclasPresionadas.insert(tecla);
    Personaje *kapax = juego->getKapax();

    if (tecla == Qt::Key_P) {
        juego->pausar();
        audio->establecerPausa(juego->estaPausado());
        audio->reproducirEfecto("pausa");
    } else if (tecla == Qt::Key_R) {
        teclasPresionadas.clear();
        mirandoDerecha = true;
        juego->reiniciarNivel();
        activarAudioNivelActual();
    } else if (tecla == Qt::Key_M) {
        // Pedir confirmación antes de abandonar la partida
        confirmandoSalidaMenu = true;
        teclasPresionadas.clear();
        audio->establecerPausa(true);
        audio->reproducirEfecto("pausa");
    } else if (!juego->estaPausado() && kapax != nullptr) {
        if (tecla == Qt::Key_E && kapax->tieneGiftDisponible()) {
            bool eraInvuln = kapax->isInvulnerable();
            kapax->consumirGift();
            if (!eraInvuln && kapax->isInvulnerable()) {
                audio->reproducirEfecto("fortachon");
            } else {
                audio->reproducirEfecto("saltarin");
            }
        } else if (juego->getNivelActual() == 1 &&
                   (tecla == Qt::Key_Space || tecla == Qt::Key_W || tecla == Qt::Key_Up)) {
            bool estabaEnSuelo = kapax->isEnSuelo();
            float vyAntes = kapax->getVy();
            kapax->saltar();
            if (estabaEnSuelo && !kapax->isEnSuelo()) {
                audio->reproducirEfecto("salto");
            } else if (!estabaEnSuelo && kapax->getVy() < vyAntes - 50.0f) {
                audio->reproducirEfecto("doble_salto");
            }
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }
    teclasPresionadas.remove(event->key());
}

void MainWindow::actualizarFrame() {
    const float dt = 0.016f;
    tiempoAnimacion += dt;

    if (cinematicaActual != TipoCinematica::Ninguna) {
        return;
    }

    if (estadoPantalla == EstadoPantalla::MenuPrincipal) {
        renderizarMenuPrincipal();
        return;
    }
    if (estadoPantalla == EstadoPantalla::MenuDificultad) {
        renderizarMenuDificultad();
        return;
    }

    // Si está abierto el diálogo de confirmación, congelar la lógica y solo renderizar
    if (confirmandoSalidaMenu) {
        renderizarEscena();
        return;
    }

    Personaje *kapax = juego->getKapax();
    Nivel *nivelAntes = juego->getNivel();
    NivelRio *rioAntes = dynamic_cast<NivelRio*>(nivelAntes);

    short nivelPrevio = juego->getNivelActual();
    bool terminadoPrevio = juego->juegoTerminado();
    bool enSueloPrev = (kapax != nullptr) ? kapax->isEnSuelo() : false;
    short vidasPrev = (kapax != nullptr) ? kapax->getVidas() : 0;
    short saludPrev = (kapax != nullptr) ? kapax->getSalud() : 0;
    bool invulnPrev = (kapax != nullptr) ? kapax->isInvulnerable() : false;
    bool aturdidoPrev = (kapax != nullptr) ? kapax->isAturdido() : false;
    bool pesadoPrev = (kapax != nullptr) ? kapax->isPesado() : false;
    bool tieneGiftPrev = (kapax != nullptr) ? kapax->tieneGiftDisponible() : false;

    int enemigosAnimandoPrev = 0;
    if (nivelAntes != nullptr) {
        for (Enemigo *e : nivelAntes->getEnemigos()) {
            if (e != nullptr && e->estaAnimandoAtaque()) {
                ++enemigosAnimandoPrev;
            }
        }
    }

    if (!juego->estaPausado() && !juego->juegoTerminado() && kapax != nullptr) {
        short dirX = 0;
        short dirY = 0;
        if (teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left)) {
            dirX -= 1;
            mirandoDerecha = false;
        }
        if (teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right)) {
            dirX += 1;
            mirandoDerecha = true;
        }
        kapax->moverHorizontal(dirX);
        if (rioAntes != nullptr) {
            if (teclasPresionadas.contains(Qt::Key_W) || teclasPresionadas.contains(Qt::Key_Up)) {
                dirY -= 1;
            }
            if (teclasPresionadas.contains(Qt::Key_S) || teclasPresionadas.contains(Qt::Key_Down)) {
                dirY += 1;
            }
            kapax->moverVertical(dirY);
        }
    }

    juego->actualizar(dt);

    if (nivelPrevio == 1 && juego->getNivelActual() == 2) {
        audio->reproducirEfecto("nivel_completado");
        reproducirCinematica(TipoCinematica::Transicion);
        return;
    }
    if (!terminadoPrevio && juego->juegoTerminado()) {
        reproducirCinematica(juego->hayVictoria() ? TipoCinematica::Victoria : TipoCinematica::GameOver);
        return;
    }

    Nivel *nivelActual = juego->getNivel();
    NivelRio *rioActual = dynamic_cast<NivelRio*>(nivelActual);

    if (!juego->estaPausado() && !juego->juegoTerminado() && kapax != nullptr && nivelActual != nullptr) {
        if (rioActual == nullptr) {
            if (!enSueloPrev && kapax->isEnSuelo()) {
                audio->reproducirEfecto("aterrizaje", 180);
            } else if (kapax->isEnSuelo() && std::abs(kapax->getVx()) > 5.0f) {
                audio->reproducirEfecto("paso_tierra", 320);
            }
        } else {
            bool moviendo = (std::abs(kapax->getVx()) > 5.0f || std::abs(kapax->getVy()) > 5.0f);
            audio->reproducirEfecto("nado_brazada", moviendo ? 460 : 850);

            if (!rioViolentoActivo && rioActual->getProgreso() >= 0.55f) {
                rioViolentoActivo = true;
                audio->reproducirAmbiente("rio_violento.wav");
            }
        }

        if (!tieneGiftPrev && kapax->tieneGiftDisponible()) {
            audio->reproducirEfecto("recoger_gift");
        }
        if (!pesadoPrev && kapax->isPesado()) {
            audio->reproducirEfecto("pesado");
        }

        if (invulnPrev && !kapax->isInvulnerable()) {
            audio->reproducirEfecto("fin_poder");
        }

        if (!aturdidoPrev && kapax->isAturdido()) {
            audio->reproducirEfecto("aturdido", 300);
        }

        if (kapax->getVidas() < vidasPrev) {
            audio->reproducirEfecto("perder_vida");
        } else if (kapax->getSalud() < saludPrev) {
            bool porVibora = false;
            for (Trampa *t : nivelActual->getTrampas()) {
                if (t != nullptr && dynamic_cast<Vibora*>(t) != nullptr && t->colisionaCon(*kapax)) {
                    porVibora = true;
                    break;
                }
            }
            if (porVibora) {
                audio->reproducirEfecto("mordedura_vibora", 300);
            } else {
                audio->reproducirEfecto("impacto_proyectil", 200);
            }
        }

        for (Trampa *t : nivelActual->getTrampas()) {
            if (t == nullptr || !t->estaActivo() || !t->colisionaCon(*kapax)) {
                continue;
            }
            if (dynamic_cast<Lodazal*>(t) != nullptr) {
                audio->reproducirEfecto("lodazal", 1100);
            } else if (dynamic_cast<Resbaladero*>(t) != nullptr) {
                audio->reproducirEfecto("resbalon", 900);
            } else if (dynamic_cast<Remolino*>(t) != nullptr) {
                audio->reproducirEfecto("remolino", 1500);
            } else if (dynamic_cast<TroncoFlotante*>(t) != nullptr) {
                audio->reproducirEfecto("tronco_choque", 400);
            }
        }

        int enemigosAnimandoAhora = 0;
        for (Enemigo *e : nivelActual->getEnemigos()) {
            if (e != nullptr && e->estaAnimandoAtaque()) {
                ++enemigosAnimandoAhora;
            }
        }
        if (enemigosAnimandoAhora > enemigosAnimandoPrev) {
            audio->reproducirEfecto("lanzamiento", 220);
        }

        float restante = nivelActual->getTiempoRestante();
        if (restante > 0.5f && restante <= 15.0f) {
            audio->reproducirEfecto("tiempo_bajo", 3500);
        }
    }

    renderizarEscena();
}

void MainWindow::renderizarMenuPrincipal() {
    escena->clear();
    escena->setSceneRect(0.0f, 0.0f, 960.0f, 600.0f);
    setWindowTitle("La Travesía de Kapax - Menú Principal");

    if (spritesCargados && !fondoSelva.isNull()) {
        escena->addPixmap(fondoSelva)->setPos(0, 0);
    } else {
        escena->addRect(0, 0, 960, 600, QPen(Qt::NoPen), QBrush(QColor(13, 27, 42)));
    }

    escena->addRect(0, 0, 960, 600, QPen(Qt::NoPen), QBrush(QColor(8, 18, 28, 145)));

    escena->addRect(120, 75, 720, 450, QPen(QColor(244, 196, 101), 3), QBrush(QColor(13, 27, 42, 232)));
    escena->addRect(120, 75, 720, 118, QPen(Qt::NoPen), QBrush(QColor(9, 19, 30, 240)));
    escena->addLine(145, 193, 815, 193, QPen(QColor(244, 196, 101), 2));

    QGraphicsTextItem *tag = escena->addText("FANTASIAS ANIMADAS DE LA HISTORIA DE COLOMBIA");
    tag->setDefaultTextColor(QColor(244, 196, 101));
    tag->setFont(QFont("Arial", 9, QFont::Bold));
    tag->setPos(295, 90);

    QGraphicsTextItem *titulo = escena->addText("LA TRAVESIA DE KAPAX");
    titulo->setDefaultTextColor(Qt::white);
    titulo->setFont(QFont("Arial", 30, QFont::Bold));
    titulo->setPos(230, 110);

    QGraphicsTextItem *subtitulo = escena->addText("De la selva ribereña al indomable Rio Magdalena");
    subtitulo->setDefaultTextColor(QColor(175, 205, 225));
    subtitulo->setFont(QFont("Arial", 11));
    subtitulo->setPos(300, 160);

    if (spritesCargados && !kapaxCaminarDer.isEmpty()) {
        int idx = static_cast<int>(tiempoAnimacion * 6.0f) % kapaxCaminarDer.size();
        escena->addPixmap(kapaxCaminarDer[idx])->setPos(155, 106);
    }
    if (spritesCargados && !cazadorIdleIzq.isNull()) {
        escena->addPixmap(cazadorIdleIzq)->setPos(755, 102);
    }

    auto dibujarBoton = [&](float y, const QColor &acento, const QString &teclaTxt,
                            const QString &nombre, const QString &desc) {
        escena->addRect(220, y, 520, 78, QPen(acento, 2), QBrush(QColor(19, 38, 58, 225)));
        escena->addRect(238, y + 17, 52, 44, QPen(acento, 2), QBrush(QColor(10, 22, 34)));

        QGraphicsTextItem *k = escena->addText(teclaTxt);
        k->setDefaultTextColor(acento);
        k->setFont(QFont("Arial", 17, QFont::Bold));
        k->setPos(254, y + 22);

        QGraphicsTextItem *n = escena->addText(nombre);
        n->setDefaultTextColor(acento);
        n->setFont(QFont("Arial", 16, QFont::Bold));
        n->setPos(308, y + 13);

        QGraphicsTextItem *d = escena->addText(desc);
        d->setDefaultTextColor(QColor(220, 232, 240));
        d->setFont(QFont("Arial", 10));
        d->setPos(308, y + 43);
    };

    dibujarBoton(228, QColor(110, 231, 135), "1", "JUGAR",
                 "Comenzar la travesia y elegir nivel de dificultad  [1 / Enter]");
    dibujarBoton(326, QColor(255, 107, 107), "2", "SALIR",
                 "Cerrar el juego y volver al escritorio  [2 / Esc]");

    QString estadoAudio = audio->estaSilenciado() ? "AUDIO: OFF [X]" : "AUDIO: ON [X]";
    escena->addRect(175, 440, 610, 52, QPen(QColor(70, 105, 135)), QBrush(QColor(9, 19, 30, 220)));
    QGraphicsTextItem *controles = escena->addText(
        "Controles en partida:  [WASD / Flechas] Mover / Nadar   |   [Espacio] Saltar   |   [E] Gift\n"
        "[P] Pausa   |   [R] Reiniciar   |   [M] Menu Principal   |   " + estadoAudio);
    controles->setDefaultTextColor(QColor(185, 205, 220));
    controles->setFont(QFont("Arial", 9));
    controles->setPos(208, 449);
}

void MainWindow::renderizarMenuDificultad() {
    escena->clear();
    escena->setSceneRect(0.0f, 0.0f, 960.0f, 600.0f);
    setWindowTitle("La Travesía de Kapax - Selección de Dificultad");

    if (spritesCargados && !fondoSelva.isNull()) {
        escena->addPixmap(fondoSelva)->setPos(0, 0);
    } else {
        escena->addRect(0, 0, 960, 600, QPen(Qt::NoPen), QBrush(QColor(13, 27, 42)));
    }

    escena->addRect(0, 0, 960, 600, QPen(Qt::NoPen), QBrush(QColor(8, 18, 28, 145)));

    escena->addRect(120, 58, 720, 484, QPen(QColor(244, 196, 101), 3), QBrush(QColor(13, 27, 42, 232)));
    escena->addRect(120, 58, 720, 108, QPen(Qt::NoPen), QBrush(QColor(9, 19, 30, 240)));
    escena->addLine(145, 166, 815, 166, QPen(QColor(244, 196, 101), 2));

    QGraphicsTextItem *tag = escena->addText("PREPARACION DE LA TRAVESIA");
    tag->setDefaultTextColor(QColor(244, 196, 101));
    tag->setFont(QFont("Arial", 9, QFont::Bold));
    tag->setPos(370, 72);

    QGraphicsTextItem *titulo = escena->addText("SELECCIONA LA DIFICULTAD");
    titulo->setDefaultTextColor(Qt::white);
    titulo->setFont(QFont("Arial", 24, QFont::Bold));
    titulo->setPos(245, 96);

    QGraphicsTextItem *subtitulo = escena->addText("Elige el reto para la selva y el Rio Magdalena");
    subtitulo->setDefaultTextColor(QColor(175, 205, 225));
    subtitulo->setFont(QFont("Arial", 11));
    subtitulo->setPos(310, 138);

    if (spritesCargados && !kapaxCaminarDer.isEmpty()) {
        int idx = static_cast<int>(tiempoAnimacion * 6.0f) % kapaxCaminarDer.size();
        escena->addPixmap(kapaxCaminarDer[idx])->setPos(155, 86);
    }
    if (spritesCargados && !cazadorIdleIzq.isNull()) {
        escena->addPixmap(cazadorIdleIzq)->setPos(755, 82);
    }

    auto dibujarTarjeta = [&](float y, const QColor &acento, const QString &teclaTxt,
                              const QString &nombre, const QString &desc) {
        escena->addRect(175, y, 610, 68, QPen(acento, 2), QBrush(QColor(19, 38, 58, 225)));
        escena->addRect(190, y + 14, 46, 40, QPen(acento, 2), QBrush(QColor(10, 22, 34)));

        QGraphicsTextItem *k = escena->addText(teclaTxt);
        k->setDefaultTextColor(acento);
        k->setFont(QFont("Arial", 16, QFont::Bold));
        k->setPos(203, y + 18);

        QGraphicsTextItem *n = escena->addText(nombre);
        n->setDefaultTextColor(acento);
        n->setFont(QFont("Arial", 13, QFont::Bold));
        n->setPos(252, y + 10);

        QGraphicsTextItem *d = escena->addText(desc);
        d->setDefaultTextColor(QColor(220, 232, 240));
        d->setFont(QFont("Arial", 10));
        d->setPos(252, y + 36);
    };

    dibujarTarjeta(195, QColor(110, 231, 135), "1", "FACIL",
                   "Trampas suaves, enemigos menos precisos y corriente moderada.");
    dibujarTarjeta(278, QColor(244, 196, 101), "2", "NORMAL",
                   "Experiencia equilibrada en la selva y el rio Magdalena.");
    dibujarTarjeta(361, QColor(255, 107, 107), "3", "DIFICIL",
                   "Agente inteligente muy preciso, mas trampas y corriente feroz.");

    QString estadoAudio = audio->estaSilenciado() ? "AUDIO: OFF [X]" : "AUDIO: ON [X]";
    escena->addRect(175, 456, 610, 52, QPen(QColor(244, 196, 101, 160)), QBrush(QColor(9, 19, 30, 220)));
    QGraphicsTextItem *volver = escena->addText(
        "Presiona [1], [2] o [3] para iniciar   |   [ESC / M] Volver al Menu Principal\n"
        "[Espacio / Esc] Saltar cinematica   |   " + estadoAudio);
    volver->setDefaultTextColor(QColor(220, 232, 240));
    volver->setFont(QFont("Arial", 9, QFont::Bold));
    volver->setPos(230, 465);
}

void MainWindow::renderizarEscena() {
    escena->clear();

    Nivel *nivel = juego->getNivel();
    NivelRio *rio = dynamic_cast<NivelRio*>(nivel);
    if (rio != nullptr) {
        renderizarEscenaRio(rio);
        return;
    }

    NivelRibera *ribera = dynamic_cast<NivelRibera*>(nivel);
    Personaje *kapax = juego->getKapax();

    float scrollX = (ribera != nullptr) ? ribera->getScrollX() : 0.0f;
    escena->setSceneRect(scrollX, 0.0f, 960.0f, 600.0f);

    setWindowTitle("La Travesía de Kapax - Nivel 1: La Ribera del Magdalena");
    if (spritesCargados && !fondoSelva.isNull()) {
        escena->addPixmap(fondoSelva)->setPos(scrollX, 0.0f);
    } else {
        escena->addRect(scrollX, 0.0f, 960.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(135, 206, 235)));
    }

    if (ribera != nullptr) {
        float orillaX = ribera->getPosicionOrilla();
        escena->addRect(orillaX, 460.0f, 180.0f, 140.0f, QPen(Qt::NoPen), QBrush(QColor(30, 144, 255, 210)));
        QGraphicsTextItem *metaTxt = escena->addText("RIO MAGDALENA");
        metaTxt->setDefaultTextColor(Qt::white);
        metaTxt->setFont(QFont("Arial", 10, QFont::Bold));
        metaTxt->setPos(orillaX + 10.0f, 435.0f);

        float cpX = ribera->getPosicionCheckpoint();
        QColor colorBandera = ribera->isCheckpointAlcanzado() ? QColor(50, 220, 80) : QColor(230, 60, 60);
        escena->addRect(cpX, 430.0f, 6.0f, 70.0f, QPen(Qt::black), QBrush(Qt::lightGray));
        escena->addRect(cpX + 6.0f, 430.0f, 34.0f, 22.0f, QPen(Qt::black), QBrush(colorBandera));
        QGraphicsTextItem *cpTxt = escena->addText("CHECKPOINT");
        cpTxt->setDefaultTextColor(Qt::white);
        cpTxt->setFont(QFont("Arial", 8, QFont::Bold));
        cpTxt->setPos(cpX - 20.0f, 410.0f);

        for (Plataforma *plat : ribera->getPlataformas()) {
            if (plat == nullptr) continue;

            if (spritesCargados && !spriteSuelo.isNull() && !spriteMadera150.isNull()) {
                if (plat->getY() >= 490.0f) {
                    escena->addRect(plat->getX(), plat->getY() + 36.0f, plat->getAncho(), 70.0f,
                                    QPen(Qt::NoPen), QBrush(QColor(92, 58, 33)));
                    for (float bx = plat->getX(); bx + 40.0f <= plat->getX() + plat->getAncho() + 0.5f; bx += 40.0f) {
                        escena->addPixmap(spriteSuelo)->setPos(bx, plat->getY());
                    }
                } else {
                    const QPixmap &mad = (plat->getAncho() > 155.0f) ? spriteMadera160 : spriteMadera150;
                    escena->addPixmap(mad)->setPos(plat->getX(), plat->getY());
                }
            } else {
                QColor colorPlat = (plat->getY() >= 490.0f) ? QColor(101, 67, 33) : QColor(139, 90, 43);
                escena->addRect(plat->getX(), plat->getY(), plat->getAncho(), plat->getAlto(),
                                QPen(Qt::black, 1), QBrush(colorPlat));
            }
        }
    }

    if (nivel != nullptr) {
        for (Trampa *t : nivel->getTrampas()) {
            if (t == nullptr || !t->estaActivo()) continue;

            if (dynamic_cast<Lodazal*>(t) != nullptr) {
                if (spritesCargados && !spriteLodo.isNull()) {
                    escena->addPixmap(spriteLodo)->setPos(t->getX(), 493.0f);
                } else {
                    escena->addRect(t->getX(), 496.0f, t->getAncho(), 10.0f, QPen(Qt::NoPen), QBrush(QColor(78, 52, 46)));
                }
            } else if (dynamic_cast<Resbaladero*>(t) != nullptr) {
                escena->addRect(t->getX(), 498.0f, t->getAncho(), 7.0f,
                                QPen(Qt::NoPen), QBrush(QColor(130, 224, 255, 185)));
                escena->addRect(t->getX() + 4.0f, 499.0f, t->getAncho() - 8.0f, 2.0f,
                                QPen(Qt::NoPen), QBrush(QColor(255, 255, 255, 210)));
            } else if (dynamic_cast<Vibora*>(t) != nullptr) {
                if (spritesCargados && !spriteVibora.isNull()) {
                    float vibY = 500.0f - spriteVibora.height() + 4.0f;
                    escena->addPixmap(spriteVibora)->setPos(t->getX(), vibY);
                } else {
                    escena->addEllipse(t->getX(), t->getY(), t->getAncho(), t->getAlto(), QPen(Qt::darkGreen, 2), QBrush(QColor(76, 175, 80)));
                }
            }
        }

        for (Gift *g : nivel->getGifts()) {
            if (g == nullptr || !g->estaActivo()) continue;

            if (dynamic_cast<GiftFortachon*>(g) != nullptr && spritesCargados && !spriteGiftFortachon.isNull()) {
                escena->addPixmap(spriteGiftFortachon)->setPos(g->getX(), g->getY());
            } else if (dynamic_cast<GiftSaltarin*>(g) != nullptr && spritesCargados && !spriteGiftSaltarin.isNull()) {
                escena->addPixmap(spriteGiftSaltarin)->setPos(g->getX(), g->getY());
            } else {
                escena->addEllipse(g->getX(), g->getY(), g->getAncho(), g->getAlto(), QPen(Qt::yellow, 2), QBrush(QColor(0, 188, 212)));
            }
        }

        for (Enemigo *e : nivel->getEnemigos()) {
            if (e == nullptr) continue;

            if (e->estaActivo()) {
                float ramaY = e->getY() + e->getAlto();
                escena->addRect(e->getX() - 15.0f, ramaY, e->getAncho() + 30.0f, 10.0f,
                                QPen(Qt::NoPen), QBrush(QColor(93, 64, 55)));
                if (spritesCargados && !monoIdleDer.isNull()) {
                    bool mirarIzq = (kapax != nullptr && kapax->getX() < e->getX());
                    const QPixmap &sprMono = e->estaAnimandoAtaque()
                                                 ? (mirarIzq ? monoLanzandoIzq : monoLanzandoDer)
                                                 : (mirarIzq ? monoIdleIzq : monoIdleDer);
                    float drawX = e->getX() + (e->getAncho() - sprMono.width()) * 0.5f;
                    float drawY = ramaY - sprMono.height() + 3.0f;
                    escena->addPixmap(sprMono)->setPos(drawX, drawY);
                } else {
                    escena->addRect(e->getX(), e->getY(), e->getAncho(), e->getAlto(),
                                    QPen(Qt::black, 2), QBrush(QColor(121, 85, 72)));
                }
            }

            for (Proyectil *p : e->getProyectiles()) {
                if (p != nullptr && p->estaActivo()) {
                    if (spritesCargados && !spritePiedra.isNull()) {
                        escena->addPixmap(spritePiedra)->setPos(p->getX(), p->getY());
                    } else {
                        escena->addEllipse(p->getX(), p->getY(), p->getAncho(), p->getAlto(), QPen(Qt::black), QBrush(QColor(158, 158, 158)));
                    }
                }
            }
        }
    }

    if (kapax != nullptr && kapax->estaVivo()) {
        if (kapax->isInvulnerable()) {
            escena->addEllipse(kapax->getX() - 8.0f, kapax->getY() - 6.0f, kapax->getAncho() + 16.0f, kapax->getAlto() + 12.0f,
                               QPen(Qt::yellow, 3), QBrush(QColor(255, 235, 59, 60)));
        }

        if (spritesCargados && !kapaxIdleDer.isEmpty() && !kapaxCaminarDer.isEmpty()) {
            const QPixmap *sprPtr = nullptr;

            if (kapax->isAturdido() && !kapaxAturdidoDer.isNull()) {
                sprPtr = mirandoDerecha ? &kapaxAturdidoDer : &kapaxAturdidoIzq;
            } else if (!kapax->isEnSuelo() && !kapaxSaltarDer.isNull()) {
                sprPtr = mirandoDerecha ? &kapaxSaltarDer : &kapaxSaltarIzq;
            } else if (std::abs(kapax->getVx()) > 5.0f) {
                int idx = static_cast<int>(tiempoAnimacion * 8.0f) % kapaxCaminarDer.size();
                sprPtr = mirandoDerecha ? &kapaxCaminarDer[idx] : &kapaxCaminarIzq[idx];
            } else {
                sprPtr = mirandoDerecha ? &kapaxIdleDer[0] : &kapaxIdleIzq[0];
            }

            if (sprPtr != nullptr && !sprPtr->isNull()) {
                float drawX = kapax->getX() + (kapax->getAncho() - sprPtr->width()) * 0.5f;
                float drawY = (kapax->getY() + kapax->getAlto()) - sprPtr->height() + 2.0f;
                escena->addPixmap(*sprPtr)->setPos(drawX, drawY);
            }
        } else {
            escena->addRect(kapax->getX(), kapax->getY(), kapax->getAncho(), kapax->getAlto(),
                            QPen(Qt::black, 2), QBrush(QColor(230, 145, 56)));
        }
    }

    dibujarHUD(scrollX);
}

void MainWindow::renderizarEscenaRio(NivelRio *rio) {
    Personaje *kapax = juego->getKapax();
    escena->setSceneRect(0.0f, 0.0f, 960.0f, 600.0f);
    setWindowTitle("La Travesía de Kapax - Nivel 2: Nadando el Magdalena");

    const float izq = rio->getLimiteRioIzq();
    const float der = rio->getLimiteRioDer();
    const float scrollY = rio->getScrollY();

    escena->addRect(izq, 0.0f, der - izq, 600.0f, QPen(Qt::NoPen), QBrush(QColor(17, 91, 126)));
    if (!spriteAguaRio.isNull()) {
        const int altoAgua = spriteAguaRio.height();
        const int offsetAgua = static_cast<int>(std::fmod(scrollY / 0.60f, static_cast<float>(altoAgua)));
        for (int y = -altoAgua + offsetAgua; y < 600; y += altoAgua) {
            escena->addPixmap(spriteAguaRio)->setPos(izq, y);
        }
    }

    const int rangoX = static_cast<int>(der - izq - 40.0f);
    for (int i = 0; i < 16; ++i) {
        float sx = izq + 20.0f + static_cast<float>((i * 137) % rangoX);
        float sy = std::fmod((scrollY / 0.60f) + i * 71.0f, 660.0f) - 30.0f;
        escena->addRect(sx, sy, 3.0f, 26.0f, QPen(Qt::NoPen), QBrush(QColor(255, 255, 255, 65)));
    }

    escena->addRect(0.0f, 0.0f, izq, 600.0f, QPen(Qt::NoPen), QBrush(QColor(42, 102, 46)));
    escena->addRect(der, 0.0f, 960.0f - der, 600.0f, QPen(Qt::NoPen), QBrush(QColor(42, 102, 46)));
    escena->addRect(izq - 18.0f, 0.0f, 18.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(186, 158, 102)));
    escena->addRect(der, 0.0f, 18.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(186, 158, 102)));
    escena->addRect(izq - 24.0f, 0.0f, 6.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(68, 138, 56)));
    escena->addRect(der + 18.0f, 0.0f, 6.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(68, 138, 56)));

    const float ciclo = 700.0f;
    for (int i = 0; i < 14; ++i) {
        float py = std::fmod(scrollY + i * 50.0f, ciclo) - 50.0f;
        escena->addEllipse(izq - 4.0f, py, 10.0f, 22.0f, QPen(Qt::NoPen), QBrush(QColor(215, 245, 255, 110)));
        escena->addEllipse(der - 6.0f, py + 20.0f, 10.0f, 22.0f, QPen(Qt::NoPen), QBrush(QColor(215, 245, 255, 110)));
        escena->addEllipse(izq - 14.0f + (i % 3) * 3.0f, py + 10.0f, 7.0f, 5.0f,
                           QPen(Qt::NoPen), QBrush(QColor(135, 112, 74, 170)));
        escena->addEllipse(der + 4.0f + (i % 3) * 3.0f, py + 25.0f, 7.0f, 5.0f,
                           QPen(Qt::NoPen), QBrush(QColor(135, 112, 74, 170)));
        escena->addEllipse(48.0f + (i % 2) * 28.0f, py, 36.0f, 24.0f,
                           QPen(Qt::NoPen), QBrush(QColor(56, 122, 54, 140)));
        escena->addEllipse(der + 34.0f + (i % 2) * 26.0f, py + 15.0f, 36.0f, 24.0f,
                           QPen(Qt::NoPen), QBrush(QColor(56, 122, 54, 140)));
    }

    for (int i = 0; i < 7; ++i) {
        float ty = std::fmod(scrollY + i * 100.0f, ciclo) - 70.0f;
        if (!vegetacionOrilla.isEmpty()) {
            const QPixmap &vegIzq = vegetacionOrilla[i % vegetacionOrilla.size()];
            const QPixmap &vegDer = vegetacionOrilla[(i + 2) % vegetacionOrilla.size()];
            escena->addPixmap(vegIzq)->setPos(4.0f, ty);
            escena->addPixmap(vegDer)->setPos(956.0f - vegDer.width(), ty + 35.0f);
        } else {
            escena->addEllipse(8.0f, ty, 44.0f, 44.0f, QPen(Qt::NoPen), QBrush(QColor(27, 84, 36)));
            escena->addEllipse(906.0f, ty + 35.0f, 44.0f, 44.0f, QPen(Qt::NoPen), QBrush(QColor(27, 84, 36)));
        }
    }

    for (Trampa *t : rio->getTrampas()) {
        if (t == nullptr || !t->estaActivo()) {
            continue;
        }

        if (Remolino *rem = dynamic_cast<Remolino*>(t)) {
            const float cx = rem->getX() + rem->getAncho() * 0.5f;
            const float cy = rem->getY() + rem->getAlto() * 0.5f;
            const float radio = rem->getRadio();
            if (!spritesVortice.isEmpty()) {
                const int frame = static_cast<int>(tiempoAnimacion * 7.0f) % spritesVortice.size();
                const int diametro = qMax(1, static_cast<int>(radio * 1.8f));
                const QPixmap vortice = spritesVortice[frame].scaled(
                    diametro, diametro, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                escena->addPixmap(vortice)->setPos(
                    cx - vortice.width() * 0.5f, cy - vortice.height() * 0.5f);
            } else {
                escena->addEllipse(cx - radio, cy - radio, radio * 2.0f, radio * 2.0f,
                                   QPen(QColor(200, 230, 255, 90), 2), QBrush(QColor(10, 60, 110, 90)));
                for (int a = 0; a < 3; ++a) {
                    const float r = radio * (0.45f + 0.25f * a);
                    QRectF caja(cx - r, cy - r, r * 2.0f, r * 2.0f);
                    const float inicio = std::fmod(tiempoAnimacion * 240.0f + a * 120.0f, 360.0f);
                    QPainterPath arco;
                    arco.arcMoveTo(caja, inicio);
                    arco.arcTo(caja, inicio, 200.0);
                    escena->addPath(arco, QPen(QColor(235, 245, 255, 200), 3));
                }
                const float nucleo = rem->getRadioNucleo();
                escena->addEllipse(cx - nucleo, cy - nucleo, nucleo * 2.0f, nucleo * 2.0f,
                                   QPen(Qt::black, 2), QBrush(QColor(5, 20, 40)));
            }
        } else if (dynamic_cast<TroncoFlotante*>(t) != nullptr) {
            if (!spriteTroncoFlotante.isNull()) {
                QPixmap tronco = spriteTroncoFlotante.scaled(
                    qMax(1, static_cast<int>(t->getAncho())),
                    qMax(1, static_cast<int>(t->getAlto())),
                    Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                escena->addPixmap(tronco)->setPos(t->getX(), t->getY());
            } else {
                escena->addRect(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                QPen(QColor(60, 38, 20), 2), QBrush(QColor(121, 85, 50)));
            }
        } else if (dynamic_cast<Lodazal*>(t) != nullptr) {
            if (!spritesLodoRio.isEmpty()) {
                const int variante = std::abs(qRound((t->getX() + t->getY()) / 40.0f))
                                     % spritesLodoRio.size();
                const QPixmap lodo = spritesLodoRio[variante].scaled(
                    qMax(1, qRound(t->getAncho())), qMax(1, qRound(t->getAlto())),
                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
                escena->addPixmap(lodo)->setPos(
                    t->getX() + (t->getAncho() - lodo.width()) * 0.5f,
                    t->getY() + (t->getAlto() - lodo.height()) * 0.5f);
            } else {
                escena->addEllipse(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                   QPen(Qt::NoPen), QBrush(QColor(96, 70, 40, 215)));
                escena->addEllipse(t->getX() + t->getAncho() * 0.2f, t->getY() + t->getAlto() * 0.25f,
                                   t->getAncho() * 0.25f, t->getAlto() * 0.25f,
                                   QPen(Qt::NoPen), QBrush(QColor(70, 48, 28, 230)));
                escena->addEllipse(t->getX() + t->getAncho() * 0.55f, t->getY() + t->getAlto() * 0.5f,
                                   t->getAncho() * 0.2f, t->getAlto() * 0.25f,
                                   QPen(Qt::NoPen), QBrush(QColor(70, 48, 28, 230)));
            }
        }
    }

    for (Gift *g : rio->getGifts()) {
        if (g == nullptr || !g->estaActivo()) {
            continue;
        }
        if (dynamic_cast<GiftFortachon*>(g) != nullptr && !spriteGiftFortachon.isNull()) {
            escena->addPixmap(spriteGiftFortachon)->setPos(g->getX(), g->getY());
        } else if (dynamic_cast<GiftSaltarin*>(g) != nullptr && !spriteGiftSaltarin.isNull()) {
            escena->addPixmap(spriteGiftSaltarin)->setPos(g->getX(), g->getY());
        } else {
            escena->addEllipse(g->getX(), g->getY(), g->getAncho(), g->getAlto(),
                               QPen(Qt::yellow, 2), QBrush(QColor(0, 188, 212)));
        }
    }

    for (Enemigo *e : rio->getEnemigos()) {
        if (e == nullptr) {
            continue;
        }
        if (e->estaActivo()) {
            Cazador *cazador = dynamic_cast<Cazador*>(e);
            const float ex = e->getX();
            const float ey = e->getY();
            const float ew = e->getAncho();
            const float eh = e->getAlto();
            const bool orillaDerecha = cazador != nullptr && cazador->getOrilla() == 1;
            const QPixmap *sprite = nullptr;

            if (orillaDerecha) {
                sprite = e->estaAnimandoAtaque() && !cazadorLanzandoIzq.isNull()
                ? &cazadorLanzandoIzq : &cazadorIdleIzq;
            } else {
                sprite = e->estaAnimandoAtaque() && !cazadorLanzandoDer.isNull()
                ? &cazadorLanzandoDer : &cazadorIdleDer;
            }

            escena->addEllipse(ex - 4.0f, ey + eh - 10.0f, ew + 8.0f, 14.0f,
                               QPen(Qt::NoPen), QBrush(QColor(25, 58, 26, 150)));
            if (sprite != nullptr && !sprite->isNull()) {
                const float drawX = ex + (ew - sprite->width()) * 0.5f;
                const float drawY = ey + eh - sprite->height();
                escena->addPixmap(*sprite)->setPos(drawX, drawY);
            } else {
                escena->addRect(ex + 6.0f, ey + 18.0f, ew - 12.0f, eh - 22.0f,
                                QPen(Qt::black, 1), QBrush(QColor(170, 90, 40)));
                escena->addEllipse(ex + 8.0f, ey, ew - 16.0f, 22.0f,
                                   QPen(Qt::black, 1), QBrush(QColor(224, 172, 105)));
            }
        }

        for (Proyectil *p : e->getProyectiles()) {
            if (p == nullptr || !p->estaActivo()) {
                continue;
            }
            const float hx = p->getX() + p->getAncho() * 0.5f;
            const float hy = p->getY() + p->getAlto() * 0.5f;
            if (!spriteJabalina.isNull()) {
                QGraphicsPixmapItem *jabalina = escena->addPixmap(spriteJabalina);
                jabalina->setTransformOriginPoint(jabalina->boundingRect().center());
                jabalina->setPos(hx - static_cast<float>(jabalina->boundingRect().center().x()),
                                 hy - static_cast<float>(jabalina->boundingRect().center().y()));
                jabalina->setRotation(qRadiansToDegrees(std::atan2(p->getVy(), p->getVx())));
            } else {
                const float tx = hx - p->getVx() * 0.035f;
                const float ty = hy - p->getVy() * 0.035f;
                escena->addLine(tx, ty, hx, hy, QPen(QColor(235, 210, 150), 4));
            }
        }
    }

    if (kapax != nullptr && kapax->estaVivo()) {
        if (kapax->isInvulnerable()) {
            escena->addEllipse(kapax->getX() - 10.0f, kapax->getY() - 8.0f,
                               kapax->getAncho() + 20.0f, kapax->getAlto() + 16.0f,
                               QPen(Qt::yellow, 3), QBrush(QColor(255, 235, 59, 60)));
        }

        const QPixmap *sprite = nullptr;
        if (spritesNadoCargados && !kapaxNadarArriba.isEmpty()) {
            if (kapax->isAturdido() && !kapaxAturdidoDer.isNull()) {
                sprite = mirandoDerecha ? &kapaxAturdidoDer : &kapaxAturdidoIzq;
            } else {
                const float vx = kapax->getVx();
                const float vy = kapax->getVy();
                const bool moviendose = (std::abs(vx) > 5.0f || std::abs(vy) > 5.0f);
                const float velAnim = moviendose ? 8.0f : 4.5f;

                const QVector<QPixmap> *fila = &kapaxNadarArriba;
                if (std::abs(vx) > std::abs(vy) && std::abs(vx) > 5.0f) {
                    fila = (vx < 0.0f) ? &kapaxNadarIzq : &kapaxNadarDer;
                } else if (vy > 5.0f && !kapaxNadarAbajo.isEmpty()) {
                    fila = &kapaxNadarAbajo;
                }

                if (!fila->isEmpty()) {
                    const int idx = static_cast<int>(tiempoAnimacion * velAnim) % fila->size();
                    sprite = &(*fila)[idx];
                }
            }
        } else if (spritesCargados && !kapaxIdleDer.isEmpty()) {
            sprite = mirandoDerecha ? &kapaxIdleDer[0] : &kapaxIdleIzq[0];
        }

        if (sprite != nullptr && !sprite->isNull()) {
            const float drawX = kapax->getX() + (kapax->getAncho() - sprite->width()) * 0.5f;
            const float drawY = kapax->getY() + (kapax->getAlto() - sprite->height()) * 0.5f;
            escena->addPixmap(*sprite)->setPos(drawX, drawY);
        } else {
            escena->addRect(kapax->getX(), kapax->getY(), kapax->getAncho(), kapax->getAlto(),
                            QPen(Qt::black, 2), QBrush(QColor(230, 145, 56)));
        }
    }

    escena->addRect(0.0f, 44.0f, 960.0f * rio->getProgreso(), 4.0f,
                    QPen(Qt::NoPen), QBrush(QColor(244, 196, 101)));
    dibujarHUD(0.0f);
}

void MainWindow::dibujarHUD(float scrollX) {
    Personaje *kapax = juego->getKapax();
    Nivel *nivel = juego->getNivel();
    NivelRibera *ribera = dynamic_cast<NivelRibera*>(nivel);
    NivelRio *rio = dynamic_cast<NivelRio*>(nivel);

    escena->addRect(scrollX, 0.0f, 960.0f, 44.0f, QPen(Qt::NoPen), QBrush(QColor(13, 27, 42, 220)));
    escena->addLine(scrollX, 44.0f, scrollX + 960.0f, 44.0f, QPen(QColor(244, 196, 101, 180), 2));

    int vidas = (kapax != nullptr) ? kapax->getVidas() : 0;
    int salud = (kapax != nullptr) ? kapax->getSalud() : 0;
    int tiempo = (nivel != nullptr) ? static_cast<int>(nivel->getTiempoRestante()) : 0;

    if (spritesCargados && !spriteCorazon.isNull()) {
        for (int i = 0; i < vidas; ++i) {
            escena->addPixmap(spriteCorazon)->setPos(scrollX + 12.0f + i * 25.0f, 11.0f);
        }
    }

    QGraphicsTextItem *lblSalud = escena->addText("SALUD");
    lblSalud->setDefaultTextColor(QColor(244, 196, 101));
    lblSalud->setFont(QFont("Arial", 7, QFont::Bold));
    lblSalud->setPos(scrollX + 95.0f, 4.0f);

    QColor colorSalud = (salud == 3) ? QColor(80, 220, 120)
                                     : (salud == 2 ? QColor(244, 196, 101) : QColor(255, 90, 90));
    for (int s = 0; s < 3; ++s) {
        QColor relleno = (s < salud) ? colorSalud : QColor(40, 55, 72);
        escena->addRect(scrollX + 98.0f + s * 22.0f, 22.0f, 19.0f, 12.0f,
                        QPen(QColor(244, 196, 101, 160), 1), QBrush(relleno));
    }

    QString difTxt = (juego->getDificultad() == 1) ? "FACIL" : (juego->getDificultad() == 2 ? "NORMAL" : "DIFICIL");
    QString estadoTxt = "Normal";
    QColor colorEstado = Qt::white;
    if (kapax != nullptr) {
        if (kapax->isInvulnerable()) { estadoTxt = "FORTACHON"; colorEstado = QColor(255, 235, 59); }
        else if (kapax->isAturdido()) { estadoTxt = "ATURDIDO"; colorEstado = QColor(230, 130, 255); }
        else if (kapax->isPesado()) { estadoTxt = "PESADO"; colorEstado = QColor(190, 190, 190); }
    }

    QString infoCentral;
    if (rio != nullptr) {
        infoCentral = QString("TIEMPO: %1s   |   CORRIENTE: x%2   |   DIF: %3")
        .arg(tiempo)
            .arg(rio->getVelocidadCorriente() / rio->getVelocidadInicial(), 0, 'f', 1)
            .arg(difTxt);
    } else {
        infoCentral = QString("TIEMPO: %1s   |   DIFICULTAD: %2   |   NIVEL 1")
        .arg(tiempo).arg(difTxt);
    }

    QGraphicsTextItem *hudTexto = escena->addText(infoCentral);
    hudTexto->setDefaultTextColor(Qt::white);
    hudTexto->setFont(QFont("Arial", 9, QFont::Bold));
    hudTexto->setPos(scrollX + 185.0f, 6.0f);

    QString audioTag = audio->estaSilenciado() ? "X: Audio OFF" : "X: Mute";
    QGraphicsTextItem *estTexto = escena->addText("ESTADO: " + estadoTxt + "  |  [P: Pausa  R: Reiniciar  M: Menu  " + audioTag + "]");
    estTexto->setDefaultTextColor(colorEstado);
    estTexto->setFont(QFont("Arial", 8, QFont::Bold));
    estTexto->setPos(scrollX + 185.0f, 23.0f);

    bool tieneGift = (kapax != nullptr && kapax->tieneGiftDisponible());
    escena->addRect(scrollX + 805.0f, 6.0f, 142.0f, 32.0f,
                    QPen(tieneGift ? QColor(244, 196, 101) : QColor(70, 95, 120), 1),
                    QBrush(QColor(9, 19, 30, 220)));
    if (tieneGift && spritesCargados && !spriteGiftFortachon.isNull()) {
        escena->addPixmap(spriteGiftFortachon.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation))
        ->setPos(scrollX + 812.0f, 11.0f);
    }
    QGraphicsTextItem *giftLbl = escena->addText(tieneGift ? "GIFT LISTO [E]" : "SIN GIFT");
    giftLbl->setDefaultTextColor(tieneGift ? QColor(244, 196, 101) : QColor(140, 160, 180));
    giftLbl->setFont(QFont("Arial", 8, QFont::Bold));
    giftLbl->setPos(scrollX + (tieneGift ? 838.0f : 842.0f), 14.0f);

    // Diálogo de confirmación para volver al menú principal
    if (confirmandoSalidaMenu) {
        escena->addRect(scrollX, 44.0f, 960.0f, 556.0f, QPen(Qt::NoPen), QBrush(QColor(5, 12, 20, 150)));
        escena->addRect(scrollX + 180.0f, 185.0f, 600.0f, 205.0f,
                        QPen(QColor(244, 196, 101), 3), QBrush(QColor(13, 27, 42, 245)));

        QGraphicsTextItem *c1 = escena->addText("¿VOLVER AL MENU PRINCIPAL?");
        c1->setDefaultTextColor(QColor(244, 196, 101));
        c1->setFont(QFont("Arial", 20, QFont::Bold));
        c1->setPos(scrollX + 265.0f, 210.0f);

        QGraphicsTextItem *c2 = escena->addText("Se perdera el progreso actual de esta partida.");
        c2->setDefaultTextColor(QColor(220, 232, 240));
        c2->setFont(QFont("Arial", 12));
        c2->setPos(scrollX + 295.0f, 260.0f);

        escena->addRect(scrollX + 225.0f, 312.0f, 235.0f, 46.0f,
                        QPen(QColor(255, 107, 107), 2), QBrush(QColor(35, 22, 30)));
        QGraphicsTextItem *btnSi = escena->addText("[ S / ENTER ]  Si, salir");
        btnSi->setDefaultTextColor(QColor(255, 130, 130));
        btnSi->setFont(QFont("Arial", 11, QFont::Bold));
        btnSi->setPos(scrollX + 252.0f, 324.0f);

        escena->addRect(scrollX + 500.0f, 312.0f, 235.0f, 46.0f,
                        QPen(QColor(110, 231, 135), 2), QBrush(QColor(18, 42, 32)));
        QGraphicsTextItem *btnNo = escena->addText("[ N / ESC ]  Cancelar");
        btnNo->setDefaultTextColor(QColor(125, 240, 150));
        btnNo->setFont(QFont("Arial", 11, QFont::Bold));
        btnNo->setPos(scrollX + 532.0f, 324.0f);
        return;
    }

    if (ribera != nullptr && ribera->mostrandoAvisoCheckpoint()) {
        escena->addRect(scrollX + 310.0f, 65.0f, 340.0f, 48.0f,
                        QPen(QColor(110, 231, 135), 2), QBrush(QColor(13, 27, 42, 230)));
        QGraphicsTextItem *cpMsg = escena->addText("¡CHECKPOINT ALCANZADO!");
        cpMsg->setDefaultTextColor(QColor(110, 231, 135));
        cpMsg->setFont(QFont("Arial", 14, QFont::Bold));
        cpMsg->setPos(scrollX + 345.0f, 77.0f);
    }

    const bool avisoVida = (ribera != nullptr && ribera->mostrandoAvisoVida()) ||
                           (rio != nullptr && rio->mostrandoAvisoVida());
    if (avisoVida && !juego->juegoTerminado()) {
        escena->addRect(scrollX + 160.0f, 175.0f, 640.0f, 210.0f,
                        QPen(QColor(255, 95, 95), 3), QBrush(QColor(13, 27, 42, 238)));

        QGraphicsTextItem *t1 = escena->addText("¡PERDISTE UNA VIDA!");
        t1->setDefaultTextColor(QColor(255, 95, 95));
        t1->setFont(QFont("Arial", 28, QFont::Bold));
        t1->setPos(scrollX + 270.0f, 200.0f);

        QString quedanTxt = (vidas == 1) ? "TE QUEDA 1 VIDA" : QString("TE QUEDAN %1 VIDAS").arg(vidas);
        QGraphicsTextItem *t2 = escena->addText(quedanTxt);
        t2->setDefaultTextColor(QColor(244, 196, 101));
        t2->setFont(QFont("Arial", 22, QFont::Bold));
        t2->setPos(scrollX + 320.0f, 265.0f);

        QString respawnTxt;
        if (rio != nullptr) {
            respawnTxt = "Reapareciendo en el centro del RIO...";
        } else {
            respawnTxt = ribera->isCheckpointAlcanzado()
            ? "Reapareciendo en el CHECKPOINT..."
            : "Volviendo al INICIO del nivel...";
        }
        QGraphicsTextItem *t3 = escena->addText(respawnTxt);
        t3->setDefaultTextColor(Qt::white);
        t3->setFont(QFont("Arial", 13));
        t3->setPos(scrollX + 330.0f, 325.0f);
        return;
    }

    if (juego->estaPausado() || juego->juegoTerminado()) {
        escena->addRect(scrollX + 165.0f, 185.0f, 630.0f, 195.0f,
                        QPen(QColor(244, 196, 101), 3), QBrush(QColor(13, 27, 42, 240)));

        QString titulo;
        QString subtitulo;
        QString opciones;

        if (juego->estaPausado()) {
            titulo = "PAUSA";
            subtitulo = "La travesia esta detenida";
            opciones = "[P] Continuar   |   [R] Reiniciar nivel   |   [M] Menu principal";
        } else if (juego->hayVictoria()) {
            titulo = "¡TRAVESIA COMPLETADA!";
            subtitulo = "Kapax se convirtio en leyenda del Rio Magdalena";
            opciones = "[R] Jugar de nuevo (Nivel 1)   |   [M] Menu principal";
        } else {
            titulo = "GAME OVER";
            if (rio != nullptr) {
                subtitulo = "El Rio Magdalena se llevo a Kapax";
                opciones = "[R] Reintentar Nivel 2   |   [N] Nueva partida (Nivel 1)   |   [M] Menu principal";
            } else {
                subtitulo = "Te quedaste sin vidas o sin tiempo en la ribera";
                opciones = "[R] Reintentar desde Nivel 1   |   [M] Menu principal";
            }
        }

        QGraphicsTextItem *msgTit = escena->addText(titulo);
        msgTit->setDefaultTextColor(QColor(244, 196, 101));
        msgTit->setFont(QFont("Arial", 22, QFont::Bold));
        msgTit->setPos(scrollX + 205.0f, 208.0f);

        QGraphicsTextItem *msgSub = escena->addText(subtitulo);
        msgSub->setDefaultTextColor(Qt::white);
        msgSub->setFont(QFont("Arial", 12));
        msgSub->setPos(scrollX + 205.0f, 258.0f);

        escena->addLine(scrollX + 205.0f, 298.0f, scrollX + 755.0f, 298.0f,
                        QPen(QColor(244, 196, 101, 140), 1));

        QGraphicsTextItem *msgOp = escena->addText(opciones);
        msgOp->setDefaultTextColor(QColor(185, 215, 235));
        msgOp->setFont(QFont("Arial", 10, QFont::Bold));
        msgOp->setPos(scrollX + 205.0f, 318.0f);
    }
}