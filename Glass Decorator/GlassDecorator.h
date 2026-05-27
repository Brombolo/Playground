/*
 * Copyright 2001-2015 Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Stephan Aßmus, superstippi@gmx.de
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		Ryan Leavengood, leavengood@gmail.com
 *		Philippe Saint-Pierre, stpere@gmail.com
 *		John Scipione, jscipione@gmail.com
 *		Ingo Weinhold, ingo_weinhold@gmx.de
 *		Clemens Zeidler, haiku@clemens-zeidler.de
 *		Joseph Groover <looncraz@looncraz.net>
 *		Brombolo
 */
#ifndef GLASS_DECORATOR_H
#define GLASS_DECORATOR_H


#include "TabDecorator.h"
#include "DecorManager.h"


class Desktop;
class ServerBitmap;

class GlassDecorAddOn : public DecorAddOn {
public:
								GlassDecorAddOn(image_id id, const char* name);

protected:
	virtual Decorator*			_AllocateDecorator(DesktopSettings& settings,
									BRect rect, Desktop* desktop);
};

class GlassDecorator: public TabDecorator {
public:
								GlassDecorator(DesktopSettings& settings,
									BRect frame, Desktop* desktop);
	virtual						~GlassDecorator();

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
	virtual	void				_GetFootprint(BRegion* region);

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


#endif	// GLASS_DECORATOR_H
