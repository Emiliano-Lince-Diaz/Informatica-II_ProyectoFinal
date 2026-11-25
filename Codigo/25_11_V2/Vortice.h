#ifndef VORTICE_H
#define VORTICE_H

#include "Trampa.h"

class Personaje;

class Vortice : public Trampa {
private:
    float velocidadCorriente;

public:
    Vortice(float x, float y, float diametro, float velocidad);

    void actualizar(float dt) override;
    void aplicarEfecto(Personaje& p, float dt) override;
    void setVelocidadCorriente(float velocidad);
};

#endif // VORTICE_H
