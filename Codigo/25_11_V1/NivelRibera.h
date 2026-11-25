#ifndef NIVELRIBERA_H
#define NIVELRIBERA_H

#include "Nivel.h"
#include "Plataforma.h"
#include <vector>

class NivelRibera : public Nivel {
private:
    float scrollX;
    float limiteScrollIzq;
    float limiteScrollDer;
    float anchoMundo;
    float posicionOrilla;
    float anchoVentana;
    std::vector<Plataforma*> plataformas;

public:
    NivelRibera(Personaje* p, short nivelDificultad, float anchoVista = 960.0f);
    ~NivelRibera() override;

    void cargar() override;
    void actualizar(float dt) override;
    bool verificarVictoria() override;

    void actualizarScroll();
    void resolverPlataformas(float dt);

    // Getters para la vista gráfica de Qt
    float getScrollX() const;
    float getAnchoMundo() const;
    float getPosicionOrilla() const;
    const std::vector<Plataforma*>& getPlataformas() const;
};

#endif // NIVELRIBERA_H