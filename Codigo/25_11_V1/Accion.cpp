#include "Accion.h"

Accion::Accion()
    : animando(false),
    tiempoAnimacion(0.0f),
    ultimoContexto(0),
    ultimaEstrategia(0) {
}

Accion::~Accion() {
}

Proyectil* Accion::lanzar(float x, float y, float v0, float angulo, short tipo, short contexto, short estrategia) {
    ultimoContexto = contexto;
    ultimaEstrategia = estrategia;

    // Activar ventana de animación de ataque (0.35 segundos)
    animando = true;
    tiempoAnimacion = 0.35f;

    // Instanciar dinámicamente el proyectil con tiro parabólico
    return new Proyectil(x, y, v0, angulo, tipo, 1);
}

void Accion::actualizar(float dt) {
    if (animando) {
        tiempoAnimacion -= dt;
        if (tiempoAnimacion <= 0.0f) {
            animando = false;
            tiempoAnimacion = 0.0f;
        }
    }
}

bool Accion::estaAnimando() const { return animando; }
short Accion::getUltimoContexto() const { return ultimoContexto; }
short Accion::getUltimaEstrategia() const { return ultimaEstrategia; }