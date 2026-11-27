#include "Vibora.h"
#include "Personaje.h"

Vibora::Vibora(float x, float y, float tiempoAturdir)
    : Trampa(x, y, 36.0f, 28.0f, 2, 0.0f), tiempoAturdimiento(tiempoAturdir) {
    // Daño inicializado en 2 barras de salud según la especificación de Juego.pdf
}

Vibora::~Vibora() {
}

void Vibora::aplicarEfecto(Personaje& p, float dt) {
    (void)dt;
    if (!activo || !p.estaVivo()) {
        return;
    }

    p.recibirDano(dano);
    p.aturdir(tiempoAturdimiento);

    // Se desactiva tras morder para no aplicar daño infinito en el mismo frame
    desactivar();
}

float Vibora::getTiempoAturdimiento() const {
    return tiempoAturdimiento;
}