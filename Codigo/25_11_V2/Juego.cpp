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

void Juego::iniciar() {
    terminado = false;
    victoriaTotal = false;
    pausado = false;

    if (kapax != nullptr) {
        delete kapax;
    }
    kapax = new Personaje(60.0f, 400.0f);

    cargarNivel(1);
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
        nivel = new NivelRio(kapax, dificultad);
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
    } else {
        victoriaTotal = true;
        terminado = true;
    }
}

void Juego::reiniciarNivel() {
    terminado = false;
    victoriaTotal = false;
    pausado = false;

    if (kapax == nullptr || !kapax->estaVivo()) {
        delete kapax;
        kapax = new Personaje(60.0f, 400.0f);
    }

    cargarNivel(nivelActual);
}

void Juego::pausar() {
    if (!terminado) {
        pausado = !pausado;
    }
}

bool Juego::juegoTerminado() const { return terminado; }
short Juego::getNivelActual() const { return nivelActual; }
bool Juego::estaPausado() const { return pausado; }
bool Juego::hayVictoria() const { return victoriaTotal; }
Personaje* Juego::getKapax() const { return kapax; }
Nivel* Juego::getNivel() const { return nivel; }