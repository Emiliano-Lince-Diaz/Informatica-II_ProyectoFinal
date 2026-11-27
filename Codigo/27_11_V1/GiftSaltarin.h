#ifndef GIFTSALTARIN_H
#define GIFTSALTARIN_H

#include "Gift.h"

class GiftSaltarin : public Gift {
private:
    short saltosExtra;

public:
    GiftSaltarin(float x = 0.0f, float y = 0.0f, short extra = 1);
    ~GiftSaltarin() override;

    void aplicarEfecto(Personaje& p) override;
};

#endif // GIFTSALTARIN_H