QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Accion.cpp \
    AgenteLanzador.cpp \
    Aprendizaje.cpp \
    Dificultad.cpp \
    Enemigo.cpp \
    Entidad.cpp \
    Friccion.cpp \
    Gift.cpp \
    GiftFortachon.cpp \
    GiftSaltarin.cpp \
    Modelofisico.cpp \
    MovimientoParabolico.cpp \
    Nivel.cpp \
    Percepcion.cpp \
    Personaje.cpp \
    Proyectil.cpp \
    Razonamiento.cpp \
    Trampa.cpp \
    Viscosidad.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Accion.h \
    AgenteLanzador.h \
    Aprendizaje.h \
    Dificultad.h \
    Enemigo.h \
    Entidad.h \
    Experiencia.h \
    Friccion.h \
    Gift.h \
    GiftFortachon.h \
    GiftSaltarin.h \
    Modelofisico.h \
    MovimientoParabolico.h \
    Nivel.h \
    Percepcion.h \
    Personaje.h \
    Proyectil.h \
    Razonamiento.h \
    Trampa.h \
    Viscosidad.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
