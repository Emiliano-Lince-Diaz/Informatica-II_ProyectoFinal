#include "Juego.h"
#include "NivelRibera.h"
#include "NivelRio.h"
#include <cstdlib>
#include <ctime>
#include <stdexcept>

Juego::Juego(short dificultad)
    : nivelActual(1),
    dificultad(dificultad),
    pausado(false),
    terminado(false),
    victoriaTotal(false),
    kapax(nullptr),
    nivel(nullptr) {
    if (dificultad < 1 || dificultad > 3) {
        throw std::invalid_argument("La dificultad debe estar entre 1 y 3");
    }
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
    if (n < 1 || n > 2) {
        throw std::out_of_range("El nivel solicitado debe ser 1 o 2");
    }
    if (kapax == nullptr) {
        throw std::runtime_error("No se puede cargar un nivel sin Personaje");
    }

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
    if (nuevaDificultad < 1 || nuevaDificultad > 3) {
        throw std::invalid_argument("La dificultad debe estar entre 1 y 3");
    }
    dificultad = nuevaDificultad;
}

bool Juego::juegoTerminado() const { return terminado; }
short Juego::getNivelActual() const { return nivelActual; }
short Juego::getDificultad() const { return dificultad; }
bool Juego::estaPausado() const { return pausado; }
bool Juego::hayVictoria() const { return victoriaTotal; }
Personaje* Juego::getKapax() const { return kapax; }
Nivel* Juego::getNivel() const { return nivel; }