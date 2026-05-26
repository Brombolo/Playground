/*
 * Copyright 2001-2020 Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */
#ifndef DEFAULT_DECORATOR_H
#define DEFAULT_DECORATOR_H

#include "TabDecorator.h"

class Desktop;
class ServerBitmap;

class DefaultDecorator: public TabDecorator {
public:
								DefaultDecorator(DesktopSettings& settings,
									BRect frame, Desktop* desktop);
	virtual						~DefaultDecorator();

	virtual	void				GetComponentColors(Component component,
									uint8 highlight, ComponentColors _colors,
									Decorator::Tab* tab = NULL);

	virtual void				UpdateColors(DesktopSettings& settings);

protected:
	virtual	void				_DrawFrame(BRect rect);

	virtual	void				_DrawTab(Decorator::Tab* tab, BRect r);
	virtual	void				_DrawTitle(Decorator::Tab* tab, BRect r);
	virtual	void				_DrawClose(Decorator::Tab* tab, bool direct,
									BRect rect);
	virtual	void				_DrawZoom(Decorator::Tab* tab, bool direct,
									BRect rect);
	virtual	void				_DrawMinimize(Decorator::Tab* tab, bool direct,
									BRect rect);
	virtual	void				_DrawResizeKnob(BRect r, bool full,
									const ComponentColors& color);

private:
 			void				_DrawButtonBitmap(ServerBitmap* bitmap,
 									bool direct, BRect rect);
			void				_DrawBlendedRect(DrawingEngine *engine,
									const BRect rect, bool down,
									const ComponentColors& colors);
			ServerBitmap*		_GetBitmapForButton(Decorator::Tab* tab,
									Component item, bool down, int32 width,
									int32 height);

			void				_GetComponentColors(Component component,
									ComponentColors _colors,
									Decorator::Tab* tab = NULL);
};

#endif	// DEFAULT_DECORATOR_H
