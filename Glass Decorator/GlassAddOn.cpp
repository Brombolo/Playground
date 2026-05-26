#include <InterfaceDefs.h>
#include "GlassDecorator.h"

extern "C" float get_nth_decorator_priority(int32 n) { return 1.0; }

extern "C" DecorAddOn* instantiate_decorator_addon(image_id id, const char* name)
{
    return new (std::nothrow) DecorAddOn(id, name);
}

extern "C" Decorator* instantiate_decorator(DesktopSettings& settings, BRect rect, Desktop* desktop)
{
    return new (std::nothrow) GlassDecorator(settings, rect, desktop);
}
