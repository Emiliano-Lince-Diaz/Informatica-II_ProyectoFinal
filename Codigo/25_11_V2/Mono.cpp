#include "Mono.h"

Mono::Mono(float x, float y, const Dificultad& d)
    : Enemigo(x, y, d, 42.0f, 44.0f, 650.0f, 0),
    alturaRama(y) {
    // tipoProyectil = 0 (piedra / coco lanzado desde las ramas)
}

Mono::~Mono() {
}

void Mono::actualizar(float dt, Personaje& p) {
    // Mantiene su posición fija sobre la rama
    y = alturaRama;

    // Ejecuta la lógica de cadencia, agente inteligente y proyectiles de la clase base
    Enemigo::actualizar(dt, p);
}

float Mono::getAlturaRama() const {
    return alturaRama;
}