#ifndef EXPERIENCIA_H
#define EXPERIENCIA_H

struct Experiencia {
    short contexto;
    short estrategia;
    short resultado;       // 1: acertó, 2: pasó cerca, 3: falló
    float distanciaMinima;
    float errorX;
    float recompensa;

    Experiencia(short ctx = 0, short est = 0, short res = 3, float distMin = 0.0f, float errX = 0.0f, float rec = 0.0f)
        : contexto(ctx),
        estrategia(est),
        resultado(res),
        distanciaMinima(distMin),
        errorX(errX),
        recompensa(rec) {
    }
};

#endif // EXPERIENCIA_H