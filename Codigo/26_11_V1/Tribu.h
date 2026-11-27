#ifndef TRIBU_H
#define TRIBU_H

#include "Enemigo.h"

class Tribu : public Enemigo {
public:
    Tribu(float x, float y, const Dificultad& dificultad);
};

#endif // TRIBU_H
