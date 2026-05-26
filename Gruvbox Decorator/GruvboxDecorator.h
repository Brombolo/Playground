#ifndef GRUVBOX_DECORATOR_H
#define GRUVBOX_DECORATOR_H

#include <Decorator.h>

class GruvboxDecorator : public BDecorator {
public:
    GruvboxDecorator(BRect rect, uint32 look, uint32 feel, uint32 flags);
    virtual ~GruvboxDecorator();

    virtual void Draw(BRect updateRect);
    virtual void DrawFrame(BRect rect);
    virtual void DrawTab(BRect rect);
    virtual void DrawClose(BRect rect);
    virtual void DrawZoom(BRect rect);
    
    // Metodi di utility per i colori
    static rgb_color kBg0, kBg1, kFg1, kOrange, kGray, kYellow;
};

extern "C" BDecorator* instantiate_decorator(BRect rect, uint32 look, uint32 feel, uint32 flags);

#endif
