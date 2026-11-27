#ifndef TRONCOFLOTANTE_H
#define TRONCOFLOTANTE_H

#include "Trampa.h"

// Trampa del nivel 2: tronco que baja arrastrado por el río. Matar al contacto.
// La corriente base la aplica NivelRio; el tronco solo añade su deriva propia
// (velocidad extra relativa al agua) y un leve vaivén lateral.
class TroncoFlotante : public Trampa {
private:
    float velocidadDeriva;

public:
    TroncoFlotante(float x = 0.0f, float y = 0.0f, float ancho = 100.0f, float alto = 28.0f);
    ~TroncoFlotante() override;

    void aplicarEfecto(Personaje& p, float dt) override;
    void actualizar(float dt) override;

    float getVelocidadDeriva() const;
};

#endif // TRONCOFLOTANTE_H
