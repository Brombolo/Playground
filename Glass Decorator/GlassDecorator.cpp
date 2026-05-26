#include "GlassDecorator.h"
#include <GradientLinear.h>
#include <View.h>

GlassDecorator::GlassDecorator(DesktopSettings& settings, BRect rect, Desktop* desktop)
    : Decorator(settings, rect)
{
    // Colore azzurrato vetrato e semi-trasparente
    fGlassBase = (rgb_color){ 160, 200, 240, 150 }; 
    fGlassHighlight = (rgb_color){ 255, 255, 255, 210 };
    fGlassShadow = (rgb_color){ 80, 110, 150, 180 };
}

GlassDecorator::~GlassDecorator() {}

void GlassDecorator::MoveBy(float x, float y)
{
    // Sposta i confini geometrici della finestra
    fFrame.OffsetBy(x, y);
}

void GlassDecorator::ResizeBy(float x, float y)
{
    // Ridimensiona i confini geometrici della finestra
    fFrame.right += x;
    fFrame.bottom += y;
}

bool GlassDecorator::SetSettings(const BMessage& settings) { return false; }
bool GlassDecorator::GetSettings(BMessage& settings) const { return false; }

void GlassDecorator::Draw()
{
    Draw(fFrame);
}

void GlassDecorator::Draw(BRect rect)
{
    // Metodo principale di rendering invocato dall'app_server
    _DrawFrame(fFrame);
    
    // Disegna ciascun tab associato alla finestra (Haiku supporta più tab per via dello stack)
    for (int32 i = 0; i < CountTabs(); i++) {
        Decorator::Tab* tab = TabAt(i);
        if (tab != NULL) {
            // Calcola l'area del tab corrente basandoti sulle funzioni di Haiku
            BRect tabRect = tab->Frame();
            _DrawTab(tab, tabRect);
        }
    }
}

Decorator::Region GlassDecorator::RegionAt(BPoint where) const
{
    // Identifica quale parte della finestra l'utente sta cliccando (Bordi, Tab, Chiusura)
    if (fFrame.Contains(where))
        return REGION_WINDOW;
    return REGION_NONE;
}

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
    
    // Logica per arrotondare o squadrare i lati in base allo Stack & Tile
    if (tab->tiling == AS_TAB_LOCATION_MIDDLE) {
        fDrawingEngine->FillRect(rect, gradient);
    } else if (tab->tiling == AS_TAB_LOCATION_START) {
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
        BRect rightHalf = rect;
        rightHalf.left = rect.left + rect.Width() / 2;
        fDrawingEngine->FillRect(rightHalf, gradient);
    } else if (tab->tiling == AS_TAB_LOCATION_END) {
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
        BRect leftHalf = rect;
        leftHalf.right = rect.left + rect.Width() / 2;
        fDrawingEngine->FillRect(leftHalf, gradient);
    } else {
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
    }
}

void GlassDecorator::_DrawFrame(BRect rect)
{
    fDrawingEngine->StrokeRect(rect, fGlassHighlight);
    BRect inner = rect.InsetByCopy(1, 1);
    fDrawingEngine->StrokeRect(inner, fGlassShadow);
}
