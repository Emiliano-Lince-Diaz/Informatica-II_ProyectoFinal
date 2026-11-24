#ifndef GIFTFORTACHON_H
#define GIFTFORTACHON_H

#include "Gift.h"

class GiftFortachon : public Gift {
private:
    float duracionInvulnerable;

public:
    GiftFortachon(float x = 0.0f, float y = 0.0f, float duracion = 6.0f);
    ~GiftFortachon() override;

    void aplicarEfecto(Personaje& p) override;
};

#endif // GIFTFORTACHON_H