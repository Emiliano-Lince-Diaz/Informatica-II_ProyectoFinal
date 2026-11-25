#ifndef TRONCOFLOTANTE_H
#define TRONCOFLOTANTE_H

#include "Trampa.h"

class Personaje;

class TroncoFlotante : public Trampa {
private:
    float velocidadCorriente;

public:
    TroncoFlotante(float x, float y, float velocidad);

    void actualizar(float dt) override;
    void aplicarEfecto(Personaje& p, float dt) override;
    void setVelocidadCorriente(float velocidad);
};

#endif // TRONCOFLOTANTE_H
