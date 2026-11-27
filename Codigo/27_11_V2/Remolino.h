#ifndef REMOLINO_H
#define REMOLINO_H

#include "Trampa.h"

// Trampa del nivel 2. Su modelo físico es un Vortice. Si el jugador llega al núcleo
// muere directamente; en la zona de influencia solo es arrastrado y girado.
// (x, y) es el CENTRO del remolino y "radio" su radio de influencia.
class Remolino : public Trampa {
private:
    float radioNucleo;
    float intensidad;

public:
    Remolino(float x = 0.0f, float y = 0.0f, float radio = 85.0f, float intensidad = 1.0f);
    ~Remolino() override;

    void actualizar(float dt) override;
    void aplicarEfecto(Personaje& p, float dt) override;

    float getRadioNucleo() const;
    float getIntensidad() const;
};

#endif // REMOLINO_H
