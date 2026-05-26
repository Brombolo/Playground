#ifndef GLASS_DECORATOR_H
#define GLASS_DECORATOR_H

#include <Decorator.h>

class Desktop;

class GlassDecorator : public Decorator {
public:
    GlassDecorator(DesktopSettings& settings, BRect rect, Desktop* desktop);
    virtual ~GlassDecorator();

    // Metodi geometrici richiesti dalla classe base Decorator
    virtual void                MoveBy(float x, float y);
    virtual void                ResizeBy(float x, float y);
    virtual bool                SetSettings(const BMessage& settings);
    virtual bool                GetSettings(BMessage& settings) const;
    virtual void                Draw(BRect rect);
    virtual void                Draw();
    virtual Region              RegionAt(BPoint where) const;

protected:
    // Funzioni di disegno interne per simulare il vetro
    virtual void                _DrawTab(Decorator::Tab* tab, BRect rect);
    virtual void                _DrawFrame(BRect rect);

private:
    rgb_color fGlassBase;
    rgb_color fGlassHighlight;
    rgb_color fGlassShadow;
};

#endif
