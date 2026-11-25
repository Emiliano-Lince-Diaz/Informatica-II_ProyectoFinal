#ifndef NIVELRIO_H
#define NIVELRIO_H

#include "Nivel.h"

class NivelRio : public Nivel {
private:
    float velocidadCorriente;
    float scrollY;

public:
    NivelRio(Personaje* p, short nivelDificultad);

    void cargar() override;
    void actualizar(float dt) override;
    bool verificarVictoria() override;

    float getVelocidadCorriente() const;
    float getScrollY() const;
};

#endif // NIVELRIO_H
