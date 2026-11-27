QT += widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GestorAudio.cpp \
    Accion.cpp \
    AgenteLanzador.cpp \
    Aprendizaje.cpp \
    Cazador.cpp \
    Dificultad.cpp \
    Enemigo.cpp \
    Entidad.cpp \
    Friccion.cpp \
    Gift.cpp \
    GiftFortachon.cpp \
    GiftSaltarin.cpp \
    Juego.cpp \
    Lodazal.cpp \
    Modelofisico.cpp \
    Mono.cpp \
    MovimientoParabolico.cpp \
    Nivel.cpp \
    NivelRibera.cpp \
    NivelRio.cpp \
    Percepcion.cpp \
    Personaje.cpp \
    Plataforma.cpp \
    Proyectil.cpp \
    Razonamiento.cpp \
    Remolino.cpp \
    Resbaladero.cpp \
    Trampa.cpp \
    TroncoFlotante.cpp \
    Vibora.cpp \
    Vortice.cpp \
    Viscosidad.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    GestorAudio.h \
    Accion.h \
    AgenteLanzador.h \
    Aprendizaje.h \
    Cazador.h \
    Dificultad.h \
    Enemigo.h \
    Entidad.h \
    Experiencia.h \
    Friccion.h \
    Gift.h \
    GiftFortachon.h \
    GiftSaltarin.h \
    Juego.h \
    Lodazal.h \
    Modelofisico.h \
    Mono.h \
    MovimientoParabolico.h \
    Nivel.h \
    NivelRibera.h \
    NivelRio.h \
    Percepcion.h \
    Personaje.h \
    Plataforma.h \
    Proyectil.h \
    Razonamiento.h \
    Remolino.h \
    Resbaladero.h \
    Trampa.h \
    TroncoFlotante.h \
    Vibora.h \
    Vortice.h \
    Viscosidad.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    Sprites.qrc \
    Audio_Kapax/audio.qrc \
    videos/videos.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
