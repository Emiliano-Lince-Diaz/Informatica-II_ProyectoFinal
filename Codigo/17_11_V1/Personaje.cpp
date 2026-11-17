#include "Personaje.h"
#include <iostream>

Personaje::Personaje(float x, float y)
    : Entidad(x, y, 40.0f, 60.0f), // Dimensiones base estimadas del sprite
    vidas(3), salud(3), velocidadNormal(200.0f), velocidadActual(200.0f),
    esFuerte(false), tiempoFuerte(0.0f), estadoPesado(false),
    dobleSaltoDisponible(false), estaAturdido(false), tiempoAturdido(0.0f),
    fuerzaSalto(450.0f), gravedad(980.0f), enElSuelo(false), saltosRealizados(0) {
    // Kapax inicia con 3 vidas y una barra de salud que aguanta 3 golpes[cite: 29]
}

Personaje::~Personaje() {}

void Personaje::actualizar(float dt) {
    // Actualizar temporizador de aturdimiento
    if (estaAturdido) {
        tiempoAturdido -= dt;
        if (tiempoAturdido <= 0.0f) {
            estaAturdido = false;
            tiempoAturdido = 0.0f;
        }
        return; // Si está aturdido, no se aplican otras lógicas de movimiento o estado activo[cite: 30]
    }

    // Actualizar temporizador del estado Fortachón
    if (esFuerte) {
        tiempoFuerte -= dt;
        if (tiempoFuerte <= 0.0f) {
            esFuerte = false;
            tiempoFuerte = 0.0f;
        }
    }

    // Actualizar posición base sumando la velocidad instantánea
    x += vx * dt;
    y += vy * dt;
}

void Personaje::moverHorizontal(float dir) {
    if (estaAturdido) return;
    vx = dir * velocidadActual;
}

void Personaje::moverVertical(float dir) {
    if (estaAturdido) return;
    vy = dir * velocidadActual;
}

void Personaje::saltar() {
    if (estaAturdido) return;

    if (enElSuelo) {
        vy = -fuerzaSalto;
        enElSuelo = false;
        saltosRealizados = 1;
    } else if (dobleSaltoDisponible && saltosRealizados == 1) {
        vy = -fuerzaSalto;
        saltosRealizados = 2;
        dobleSaltoDisponible = false; // Se consume el doble salto[cite: 30]
    }
}

void Personaje::aplicarGravedad(float dt) {
    if (!enElSuelo) {
        vy += gravedad * dt;
    } else {
        vy = 0.0f;
    }
}

void Personaje::recibirDano(short cant) {
    if (esFuerte) {
        // Fortachón otorga invulnerabilidad a un solo ataque[cite: 29]
        esFuerte = false;
        tiempoFuerte = 0.0f;
        return;
    }

    salud -= cant;
    if (salud <= 0) {
        vidas--;
        salud = 3; // Restaurar salud para la siguiente vida
        if (vidas <= 0) {
            desactivar(); // Game Over
        }
    }
}

void Personaje::aturdir(float seg) {
    estaAturdido = true;
    tiempoAturdido = seg;
    vx = 0.0f; // Detener movimiento horizontal
}

void Personaje::activarFortachon(float seg) {
    esFuerte = true;
    tiempoFuerte = seg;
}

void Personaje::aplicarPeso() {
    estadoPesado = true;
    velocidadActual = velocidadNormal * 0.5f; // Penalización que lo vuelve lento[cite: 29]
}

void Personaje::activarDobleSalto() {
    dobleSaltoDisponible = true;
}

void Personaje::reaparecer(float nx, float ny) {
    x = nx;
    y = ny;
    vx = 0.0f;
    vy = 0.0f;
    salud = 3;
    estadoPesado = false;
    velocidadActual = velocidadNormal;
    enElSuelo = false;
}

void Personaje::setEnElSuelo(bool estado) {
    enElSuelo = estado;
    if (enElSuelo) saltosRealizados = 0;
}
bool Personaje::isAturdido() const { return estaAturdido; }
short Personaje::getVidas() const { return vidas; }
short Personaje::getSalud() const { return salud; }