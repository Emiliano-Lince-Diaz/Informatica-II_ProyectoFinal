#include "Juego.h"
#include "NivelRibera.h"
#include "NivelRio.h"
#include <cstdlib>
#include <ctime>

Juego::Juego(short dificultad)
    : nivelActual(1),
    dificultad(dificultad),
    pausado(false),
    terminado(false),
    victoriaTotal(false),
    kapax(nullptr),
    nivel(nullptr) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Juego::~Juego() {
    if (nivel != nullptr) {
        delete nivel;
        nivel = nullptr;
    }
    if (kapax != nullptr) {
        delete kapax;
        kapax = nullptr;
    }
}

void Juego::iniciar(short nivelInicial) {
    terminado = false;
    victoriaTotal = false;
    pausado = false;

    if (kapax != nullptr) {
        delete kapax;
        kapax = nullptr;
    }
    kapax = new Personaje(60.0f, 400.0f);

    cargarNivel(nivelInicial);
}

void Juego::cargarNivel(short n) {
    if (nivel != nullptr) {
        delete nivel;
        nivel = nullptr;
    }

    nivelActual = n;

    if (n == 1) {
        nivel = new NivelRibera(kapax, dificultad, 960.0f);
        nivel->cargar();
    } else if (n == 2) {
        nivel = new NivelRio(kapax, dificultad, 960.0f);
        nivel->cargar();
    }
}

void Juego::actualizar(float dt) {
    if (pausado || terminado || nivel == nullptr) {
        return;
    }

    nivel->actualizar(dt);

    if (nivel->estaCompletado()) {
        siguienteNivel();
    } else if (nivel->estaFallido()) {
        terminado = true;
        victoriaTotal = false;
    }
}

void Juego::siguienteNivel() {
    if (nivelActual == 1) {
        cargarNivel(2);
        if (nivel == nullptr) {
            // Si el Nivel 2 aún no está conectado, mantener nivelActual en 1 para que R reinicie bien
            nivelActual = 1;
            victoriaTotal = true;
            terminado = true;
        }
    } else {
        victoriaTotal = true;
        terminado = true;
    }
}

void Juego::reiniciarNivel() {
    terminado = false;
    victoriaTotal = false;
    pausado = false;

    if (kapax != nullptr) {
        delete kapax;
        kapax = nullptr;
    }
    kapax = new Personaje(60.0f, 400.0f);

    cargarNivel(nivelActual);
    if (nivel == nullptr) {
        cargarNivel(1);
    }
}

void Juego::pausar() {
    if (!terminado) {
        pausado = !pausado;
    }
}

void Juego::setDificultad(short nuevaDificultad) {
    if (nuevaDificultad < 1) nuevaDificultad = 1;
    if (nuevaDificultad > 3) nuevaDificultad = 3;
    dificultad = nuevaDificultad;
}

bool Juego::juegoTerminado() const { return terminado; }
short Juego::getNivelActual() const { return nivelActual; }
short Juego::getDificultad() const { return dificultad; }
bool Juego::estaPausado() const { return pausado; }
bool Juego::hayVictoria() const { return victoriaTotal; }
Personaje* Juego::getKapax() const { return kapax; }
Nivel* Juego::getNivel() const { return nivel; }