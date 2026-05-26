#include "GlassDecorator.h"
#include <GradientLinear.h>
#include <View.h>

GlassDecorator::GlassDecorator(DesktopSettings& settings, BRect rect, Desktop* desktop)
    : DefaultDecorator(settings, rect, desktop)
{
    // Colore base azzurrato e semi-trasparente
    fGlassBase = (rgb_color){ 160, 200, 240, 150 }; 
    fGlassHighlight = (rgb_color){ 255, 255, 255, 210 };
    fGlassShadow = (rgb_color){ 80, 110, 150, 180 };
}

GlassDecorator::~GlassDecorator() {}

void GlassDecorator::_DrawTab(Decorator::Tab* tab, BRect rect)
{
    if (tab == NULL) return;

    BGradientLinear gradient;
    gradient.SetStart(rect.LeftTop());
    gradient.SetEnd(rect.LeftBottom());
    gradient.AddColor(fGlassHighlight, 0.0);
    gradient.AddColor(fGlassBase, 0.6);
    gradient.AddColor(fGlassShadow, 1.0);

    float radius = 6.0;
    
    // Controlliamo la posizione del Tab all'interno del gruppo "Stack & Tile"
    // Per arrotondare solo i bordi esterni o lasciarli piatti se uniti
    if (tab->tiling == AS_TAB_LOCATION_MIDDLE) {
        fDrawingEngine->FillRect(rect, gradient);
    } else if (tab->tiling == AS_TAB_LOCATION_START) {
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
        BRect rightHalf = rect;
        rightHalf.left = rect.left + rect.Width() / 2;
        fDrawingEngine->FillRect(rightHalf, gradient); // Appiattisce il lato destro di contatto
    } else if (tab->tiling == AS_TAB_LOCATION_END) {
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
        BRect leftHalf = rect;
        leftHalf.right = rect.left + rect.Width() / 2;
        fDrawingEngine->FillRect(leftHalf, gradient);  // Appiattisce il lato sinistro di contatto
    } else {
        // Finestra singola standard: angoli totalmente arrotondati
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
    }

    // Richiama il disegno nativo del titolo del testo sopra il nostro vetro
    _DrawTitle(tab, rect);
}

void GlassDecorator::_DrawFrame(BRect rect)
{
    // Un frame semplice effetto specchio/vetro per i bordi
    fDrawingEngine->StrokeRect(rect, fGlassHighlight);
    BRect inner = rect.InsetByCopy(1, 1);
    fDrawingEngine->StrokeRect(inner, fGlassShadow);
}

void GlassDecorator::_DrawClose(Decorator::Tab* tab, bool direct, BRect rect)
{
    // Esempio minimale per un bottone di chiusura personalizzato stile bolla di vetro
    fDrawingEngine->FillEllipse(rect, fGlassBase);
    fDrawingEngine->StrokeEllipse(rect, fGlassHighlight);
}
