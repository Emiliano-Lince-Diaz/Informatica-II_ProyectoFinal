#include "Personaje.h"
#include "Gift.h"

Personaje::Personaje(float x, float y)
    : Entidad(x, y, 40.0f, 60.0f),
    vidas(3),
    salud(3),
    saltosMaximos(1),
    saltosRestantes(1),
    enSuelo(false),
    invulnerable(false),
    tiempoInvulnerable(0.0f),
    aturdido(false),
    tiempoAturdido(0.0f),
    pesado(false),
    factorVelocidad(1.0f),
    velocidadBase(215.0f),
    giftDisponible(nullptr),
    fuerzaSalto(510.0f),
    gravedad(980.0f) {
}

Personaje::~Personaje() {
}

void Personaje::actualizar(float dt) {
    if (!activo) {
        return;
    }

    if (aturdido) {
        tiempoAturdido -= dt;
        vx = 0.0f;
        if (tiempoAturdido <= 0.0f) {
            aturdido = false;
            tiempoAturdido = 0.0f;
        }
    }

    if (invulnerable) {
        tiempoInvulnerable -= dt;
        if (tiempoInvulnerable <= 0.0f) {
            invulnerable = false;
            tiempoInvulnerable = 0.0f;
        }
    }

    x += vx * dt;
    y += vy * dt;

    factorVelocidad = pesado ? 0.5f : 1.0f;
}

void Personaje::moverHorizontal(short dir) {
    if (aturdido || !activo) {
        vx = 0.0f;
        return;
    }
    vx = static_cast<float>(dir) * velocidadBase * factorVelocidad;
}

void Personaje::moverVertical(short dir) {
    if (aturdido || !activo) {
        vy = 0.0f;
        return;
    }
    vy = static_cast<float>(dir) * velocidadBase * factorVelocidad;
}

void Personaje::saltar() {
    if (aturdido || !activo) {
        return;
    }

    if (enSuelo) {
        vy = -fuerzaSalto;
        enSuelo = false;
        saltosRestantes = saltosMaximos - 1;
    } else if (saltosRestantes > 0) {
        vy = -fuerzaSalto;
        saltosRestantes--;
        saltosMaximos = 1;
    }
}

void Personaje::aterrizar(float ySuelo) {
    y = ySuelo - alto;
    vy = 0.0f;
    enSuelo = true;
    saltosRestantes = saltosMaximos;
}

void Personaje::aplicarGravedad(float dt) {
    if (!enSuelo) {
        vy += gravedad * dt;
    }
}

void Personaje::recibirDano(short cantidad) {
    if (!activo) {
        return;
    }

    if (invulnerable) {
        invulnerable = false;
        tiempoInvulnerable = 0.0f;
        return;
    }

    salud -= cantidad;
    if (salud <= 0) {
        vidas--;
        if (vidas > 0) {
            salud = 3;
        } else {
            salud = 0;
            desactivar();
        }
    }
}

void Personaje::recogerGift(Gift* g) {
    if (g == nullptr || !g->estaActivo()) {
        return;
    }

    if (giftDisponible != nullptr) {
        aplicarPeso();
    }

    giftDisponible = g;
    g->marcarRecogido();
}

void Personaje::consumirGift() {
    if (giftDisponible != nullptr) {
        giftDisponible->aplicarEfecto(*this);
        giftDisponible = nullptr;

        if (pesado) {
            pesado = false;
            factorVelocidad = 1.0f;
        }
    }
}

void Personaje::aturdir(float seg) {
    aturdido = true;
    tiempoAturdido = seg;
    vx = 0.0f;
}

void Personaje::activarInvulnerabilidad(float seg) {
    invulnerable = true;
    tiempoInvulnerable = seg;
}

void Personaje::activarFortachon(float seg) {
    activarInvulnerabilidad(seg);
}

void Personaje::activarDobleSalto() {
    saltosMaximos = 2;
    saltosRestantes = 2;
}

void Personaje::aplicarPeso() {
    pesado = true;
    factorVelocidad = 0.5f;
}

void Personaje::modificarFactorVelocidad(float nuevoFactor) {
    factorVelocidad = nuevoFactor;
}

void Personaje::reaparecer(float nx, float ny) {
    x = nx;
    y = ny;
    vx = 0.0f;
    vy = 0.0f;
    salud = 3;
    enSuelo = false;
    aturdido = false;
    tiempoAturdido = 0.0f;
    invulnerable = false;
    tiempoInvulnerable = 0.0f;
    pesado = false;
    factorVelocidad = 1.0f;
    saltosMaximos = 1;
    saltosRestantes = 1;
    giftDisponible = nullptr;
}

bool Personaje::estaVivo() const {
    return vidas > 0 && activo;
}

short Personaje::estadoActual() const {
    if (invulnerable) return 4;
    if (aturdido) return 3;
    if (factorVelocidad < 0.9f) return 2;
    if (!enSuelo) return 1;
    return 0;
}

bool Personaje::isEnSuelo() const { return enSuelo; }
bool Personaje::isAturdido() const { return aturdido; }
bool Personaje::isInvulnerable() const { return invulnerable; }
bool Personaje::isPesado() const { return pesado; }
bool Personaje::tieneGiftDisponible() const { return giftDisponible != nullptr; }
short Personaje::getVidas() const { return vidas; }
short Personaje::getSalud() const { return salud; }

void Personaje::setEnElSuelo(bool estado) {
    enSuelo = estado;
    if (enSuelo) {
        saltosRestantes = saltosMaximos;
    }
}