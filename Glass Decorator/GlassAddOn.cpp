#include <InterfaceDefs.h>
#include "GlassDecorator.h"

extern "C" float get_nth_decorator_priority(int32 n) { return 1.0; }

extern "C" DecorAddOn* instantiate_decorator_addon(image_id id, const char* name)
{
    return new (std::nothrow) DecorAddOn(id, name);
}

extern "C" Decorator* instantiate_decorator(Desktop* desktop, BRect rect)
{
    return new (std::nothrow) GlassDecorator(desktop, rect);
}
