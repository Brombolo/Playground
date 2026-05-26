#include "GlassDecorator.h"
#include <GradientLinear.h>
#include <View.h>

GlassDecorator::GlassDecorator(Desktop* desktop, BRect rect)
    : DefaultDecorator(desktop, rect)
{
    fGlassBase = (rgb_color){ 160, 200, 240, 150 }; 
    fGlassHighlight = (rgb_color){ 255, 255, 255, 210 };
    fGlassShadow = (rgb_color){ 80, 110, 150, 180 };
}

GlassDecorator::~GlassDecorator() {}

void GlassDecorator::_DrawTab(BRect rect)
{
    BGradientLinear gradient;
    gradient.SetStart(rect.LeftTop());
    gradient.SetEnd(rect.LeftBottom());
    gradient.AddColor(fGlassHighlight, 0.0);
    gradient.AddColor(fGlassBase, 0.6);
    gradient.AddColor(fGlassShadow, 1.0);

    // Determine rounding based on stack position
    float radius = 6.0;
    
    if (fTabLocation == AS_TAB_LOCATION_MIDDLE) {
        fDrawingEngine->FillRect(rect, gradient);
    } else if (fTabLocation == AS_TAB_LOCATION_START) {
        // Round only the left side (simplified for this example)
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
        BRect rightHalf = rect;
        rightHalf.left = rect.left + rect.Width()/2;
        fDrawingEngine->FillRect(rightHalf, gradient);
    } else if (fTabLocation == AS_TAB_LOCATION_END) {
        // Round only the right side
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
        BRect leftHalf = rect;
        leftHalf.right = rect.left + rect.Width()/2;
        fDrawingEngine->FillRect(leftHalf, gradient);
    } else {
        // Standard single window
        fDrawingEngine->FillRoundRect(rect, radius, radius, gradient);
    }

    _DrawTitle(fTabRect);
}

void GlassDecorator::_DrawFrame(BRect rect)
{
    fDrawingEngine->StrokeRect(rect, fGlassHighlight);
    BRect inner = rect.InsetByCopy(1, 1);
    fDrawingEngine->StrokeRect(inner, fGlassShadow);
}

void GlassDecorator::_DrawButton(BRect rect, int32 type, int32 state, bool highlighted)
{
    rgb_color c = highlighted ? (rgb_color){255,255,255,255} : fGlassBase;
    fDrawingEngine->FillEllipse(rect, c);
    fDrawingEngine->StrokeEllipse(rect, fGlassHighlight);
}
