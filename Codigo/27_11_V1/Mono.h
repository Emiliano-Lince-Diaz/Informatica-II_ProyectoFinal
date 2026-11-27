#ifndef MONO_H
#define MONO_H

#include "Enemigo.h"

class Mono : public Enemigo {
private:
    float alturaRama;

public:
    Mono(float x, float y, const Dificultad& d);
    ~Mono() override;

    void actualizar(float dt, Personaje& p) override;
    float getAlturaRama() const;
};

#endif // MONO_H