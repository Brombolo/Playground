#include "GruvboxDecorator.h"
#include <View.h>

// Palette Ufficiale Gruvbox Dark
rgb_color GruvboxDecorator::kBg0      = {40, 40, 40, 255};   // #282828 (Sfondo)
rgb_color GruvboxDecorator::kBg3      = {102, 92, 84, 255};  // #665c54 (Elementi secondari)
rgb_color GruvboxDecorator::kFg1      = {235, 219, 178, 255}; // #ebdbb2 (Testo chiaro)
rgb_color GruvboxDecorator::kOrange   = {204, 36, 29, 255};   // #cc241d (Rosso/Arancio scuro per chiusura)
rgb_color GruvboxDecorator::kGray     = {146, 131, 116, 255}; // #928374 (Bordi)
rgb_color GruvboxDecorator::kAqua     = {104, 157, 106, 255}; // #689d6a (Tab della finestra - Look bilanciato)

GruvboxDecorator::GruvboxDecorator(BRect rect, uint32 look, uint32 feel, uint32 flags)
    : BDecorator(rect, look, feel, flags) {}

GruvboxDecorator::~GruvboxDecorator() {}

void GruvboxDecorator::Draw(BRect updateRect) {
    DrawFrame(updateRect);
    DrawTab(updateRect);
}

void GruvboxDecorator::DrawFrame(BRect rect) {
    // Bordo finestra: usiamo il grigio neutro di Gruvbox
    fView->SetHighColor(kGray);
    fView->StrokeRect(fFrame); 
}

void GruvboxDecorator::DrawTab(BRect rect) {
    BRect tabRect = fTabRect;
    
    // Niente giallo elettrico! Usiamo l'Aqua di Gruvbox per la tab attiva
    fView->SetHighColor(kAqua);
    fView->FillRect(tabRect);
    
    // Testo scuro sulla tab per massimo contrasto (stile Gruvbox)
    fView->SetHighColor(kBg0);
    fView->SetLowColor(kAqua);
    
    DrawTitle(tabRect);
}

void GruvboxDecorator::DrawClose(BRect rect) {
    // Pulsante di chiusura: Rosso Gruvbox
    fView->SetHighColor(kOrange);
    fView->FillRect(rect);
    // X scura
    fView->SetHighColor(kBg0);
    fView->StrokeLine(rect.LeftTop(), rect.RightBottom());
    fView->StrokeLine(rect.LeftBottom(), rect.RightTop());
}

void GruvboxDecorator::DrawZoom(BRect rect) {
    // Pulsante Zoom/Ingrandimento: Grigio scuro
    fView->SetHighColor(kBg3);
    fView->FillRect(rect);
}

BDecorator* instantiate_decorator(BRect rect, uint32 look, uint32 feel, uint32 flags) {
    return new GruvboxDecorator(rect, look, feel, flags);
}
