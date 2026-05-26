#ifndef GLASS_DECORATOR_H
#define GLASS_DECORATOR_H

#include "DefaultDecorator.h"

class GlassDecorator : public DefaultDecorator {
public:
    GlassDecorator(Desktop* desktop, BRect rect);
    virtual ~GlassDecorator();

protected:
    virtual void _DrawTab(BRect rect);
    virtual void _DrawFrame(BRect rect);
    virtual void _DrawButton(BRect rect, int32 type, int32 state, bool highlighted);

private:
    rgb_color fGlassBase;
    rgb_color fGlassHighlight;
    rgb_color fGlassShadow;
};

#endif
