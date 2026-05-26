#ifndef GLASS_DECORATOR_H
#define GLASS_DECORATOR_H

#include "DefaultDecorator.h"

class GlassDecorator : public DefaultDecorator {
public:
    GlassDecorator(DesktopSettings& settings, BRect rect, Desktop* desktop);
    virtual ~GlassDecorator();

protected:
    virtual void _DrawTab(Decorator::Tab* tab, BRect rect);
    virtual void _DrawFrame(BRect rect);
    virtual void _DrawClose(Decorator::Tab* tab, bool direct, BRect rect);

private:
    rgb_color fGlassBase;
    rgb_color fGlassHighlight;
    rgb_color fGlassShadow;
};

#endif
