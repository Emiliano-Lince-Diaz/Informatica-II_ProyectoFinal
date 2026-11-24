#include "Trampa.h"

Trampa::Trampa(float x, float y, float ancho, float alto, short dano, float radio)
    : Entidad(x, y, ancho, alto), dano(dano), radio(radio), modelo(nullptr) {
}

Trampa::~Trampa() {
    if (modelo != nullptr) {
        delete modelo;
        modelo = nullptr;
    }
}

void Trampa::actualizar(float dt) {
    (void)dt;
    // Las trampas del nivel 1 son estáticas en el mapa; las subclases móviles (como TroncoFlotante en Nivel 2) sobrescriben este método
}

short Trampa::getDano() const { return dano; }
float Trampa::getRadio() const { return radio; }
ModeloFisico* Trampa::getModelo() const { return modelo; }