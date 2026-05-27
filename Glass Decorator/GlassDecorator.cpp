/*
 * Copyright 2001-2020 Haiku, Inc.
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
 *		Nahuel Tello <nhtello@unarix.com.ar>
 *		Brombolo
 */


/*!	Glass decorator for dark mode theme */


#include "GlassDecorator.h"

#include <algorithm>
#include <cmath>
#include <new>
#include <stdio.h>

#include <Autolock.h>
#include <Debug.h>
#include <GradientLinear.h>
#include <Rect.h>
#include <Region.h>
#include <View.h>

#include <WindowPrivate.h>

#include "BitmapDrawingEngine.h"
#include "DesktopSettings.h"
#include "DrawingEngine.h"
#include "DrawState.h"
#include "FontManager.h"
#include "PatternHandler.h"
#include "ServerBitmap.h"


//#define DEBUG_DECORATOR
#ifdef DEBUG_DECORATOR
#	define STRACE(x) printf x
#else
#	define STRACE(x) ;
#endif


static const float kBorderResizeLength = 22.0;


GlassDecorAddOn::GlassDecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
GlassDecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)GlassDecorator(settings, rect, desktop);
}



static inline uint8
blend_color_value(uint8 a, uint8 b, float position)
{
	int16 delta = (int16)b - a;
	int32 value = a + (int32)(position * delta);
	if (value > 255)
		return 255;
	if (value < 0)
		return 0;

	return (uint8)value;
}


//	#pragma mark -


// TODO: get rid of DesktopSettings here, and introduce private accessor
//	methods to the Decorator base class
GlassDecorator::GlassDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	TabDecorator(settings, rect, desktop)
{
	// TODO: If the decorator was created with a frame too small, it should
	// resize itself!

	STRACE(("GlassDecorator:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));

	// Forzare uno spessore minimo per l'effetto vetro
	fBorderWidth = 5;
}


GlassDecorator::~GlassDecorator()
{
	STRACE(("GlassDecorator: ~GlassDecorator()\n"));
}


// #pragma mark - Public methods


/*!	Returns the frame colors for the specified decorator component.

	The meaning of the color array elements depends on the specified component.
	For some components some array elements are unused.

	\param component The component for which to return the frame colors.
	\param highlight The highlight set for the component.
	\param colors An array of colors to be initialized by the function.
*/
void
GlassDecorator::GetComponentColors(Component component, uint8 highlight,
	ComponentColors _colors, Decorator::Tab* _tab)
{
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);

	// Simulated Glass base colors
	rgb_color focusCeleste = {120, 180, 235, 255};
	rgb_color unfocusCeleste = {170, 190, 210, 255};
	rgb_color focusFrame = {90, 150, 215, 255};
	rgb_color unfocusFrame = {150, 170, 190, 255};

	switch (component) {
		case COMPONENT_TAB:
			if (highlight != 0) {
				_colors[COLOR_TAB_FRAME_LIGHT] = tint_color(focusCeleste, 1.0);
				_colors[COLOR_TAB_FRAME_DARK] = tint_color(focusCeleste, 1.2);
				_colors[COLOR_TAB] = tint_color(focusCeleste, 1.0);
				_colors[COLOR_TAB_LIGHT] = tint_color(focusCeleste, 0.5);
				_colors[COLOR_TAB_BEVEL] = tint_color(focusCeleste, 0.7);
				_colors[COLOR_TAB_SHADOW] = tint_color(focusCeleste, 1.3);
				_colors[COLOR_TAB_TEXT] = tint_color(fFocusTextColor, 0.5);
			}
			else if (tab && tab->buttonFocus) {
				_colors[COLOR_TAB_FRAME_LIGHT] = tint_color(focusCeleste, 1.0);
				_colors[COLOR_TAB_FRAME_DARK] = tint_color(focusCeleste, 1.2);
				_colors[COLOR_TAB] = focusCeleste;
				_colors[COLOR_TAB_LIGHT] = tint_color(focusCeleste, 0.5);
				_colors[COLOR_TAB_BEVEL] = tint_color(focusCeleste, 0.7);
				_colors[COLOR_TAB_SHADOW] = tint_color(focusCeleste, 1.3);
				_colors[COLOR_TAB_TEXT] = fFocusTextColor;
			} else {
				_colors[COLOR_TAB_FRAME_LIGHT] = tint_color(unfocusCeleste, 1.0);
				_colors[COLOR_TAB_FRAME_DARK] = tint_color(unfocusCeleste, 1.2);
				_colors[COLOR_TAB] = unfocusCeleste;
				_colors[COLOR_TAB_LIGHT] = tint_color(unfocusCeleste, 0.5);
				_colors[COLOR_TAB_BEVEL] = tint_color(unfocusCeleste, 0.7);
				_colors[COLOR_TAB_SHADOW] = tint_color(unfocusCeleste, 1.3);
				_colors[COLOR_TAB_TEXT] = fNonFocusTextColor;
			}
			break;

		case COMPONENT_CLOSE_BUTTON:
		case COMPONENT_ZOOM_BUTTON:
			if (highlight != 0 || (tab && tab->buttonFocus)) {
				_colors[COLOR_BUTTON] = focusCeleste;
				_colors[COLOR_BUTTON_LIGHT] = tint_color(focusCeleste, 0.5);
			} else {
				_colors[COLOR_BUTTON] = unfocusCeleste;
				_colors[COLOR_BUTTON_LIGHT] = tint_color(unfocusCeleste, 0.5);
			}
			break;

		case COMPONENT_TOP_BORDER:
			if (tab && tab->buttonFocus) {
				_colors[0] = tint_color(focusCeleste, 1.2); 
				_colors[1] = tint_color(focusCeleste, 1.0); 
				_colors[2] = tint_color(focusCeleste, 1.0); 
				_colors[3] = tint_color(focusCeleste, 1.0); 
				_colors[4] = tint_color(focusFrame, 1.1); 
				_colors[5] = tint_color(focusFrame, 1.1); 
			} else {
				_colors[0] = tint_color(unfocusCeleste, 1.2); 
				_colors[1] = tint_color(unfocusCeleste, B_NO_TINT);
				_colors[2] = tint_color(unfocusCeleste, B_NO_TINT);
				_colors[3] = tint_color(unfocusCeleste, B_NO_TINT);
				_colors[4] = tint_color(unfocusFrame, 1.1); 
				_colors[5] = tint_color(unfocusFrame, 1.1); 
			}
			if (highlight == HIGHLIGHT_RESIZE_BORDER) {
				for (int32 i = 0; i < 6; i++) {
					_colors[i].red = 150;
					_colors[i].green = 150;
					_colors[i].blue = 255;
				}
			}
			break;
		case COMPONENT_RESIZE_CORNER:
		case COMPONENT_LEFT_BORDER:
		case COMPONENT_RIGHT_BORDER:
		case COMPONENT_BOTTOM_BORDER:
		default:
		{
			if (tab && tab->buttonFocus) {
				_colors[0] = tint_color(focusFrame, 1.2);
				_colors[1] = tint_color(focusFrame, B_NO_TINT);
				_colors[2] = tint_color(focusFrame, B_NO_TINT);
				_colors[3] = tint_color(focusFrame, B_NO_TINT);
				_colors[4] = tint_color(focusFrame, 1.05); 
				_colors[5] = tint_color(focusFrame, 1.1); 
				_colors[6] = tint_color(focusCeleste, 1.2); 
			} else {
				_colors[0] = tint_color(unfocusFrame, 1.25);
				_colors[1] = tint_color(unfocusFrame, B_NO_TINT);
				_colors[2] = tint_color(unfocusFrame, B_NO_TINT);
				_colors[3] = tint_color(unfocusFrame, B_NO_TINT);
				_colors[4] = tint_color(unfocusFrame, 1.05); 
				_colors[5] = tint_color(focusFrame, 1.3); 
				_colors[6] = tint_color(unfocusCeleste, 1.2); 
			}

			if (highlight == HIGHLIGHT_RESIZE_BORDER) {
				for (int32 i = 0; i < 6; i++) {
					_colors[i].red = 150;
					_colors[i].green = 150;
					_colors[i].blue = 255;
				}
			}
			break;
		}
	}
}


void
GlassDecorator::UpdateColors(DesktopSettings& settings)
{
	TabDecorator::UpdateColors(settings);
}


// #pragma mark - Protected methods


void
GlassDecorator::_DrawFrame(BRect rect)
{
	STRACE(("_DrawFrame(%f,%f,%f,%f)\n", rect.left, rect.top,
		rect.right, rect.bottom));

	// NOTE: the DrawingEngine needs to be locked for the entire
	// time for the clipping to stay valid for this decorator

	if (fTopTab->look == B_NO_BORDER_WINDOW_LOOK)
		return;

	if (fBorderWidth <= 0)
		return;

	// TODO: While this works, it does not look so crisp at higher resolutions.
#define COLORS_INDEX(i, borderWidth, nominalLimit) int32((float(i) / float(borderWidth)) * nominalLimit)

	// Draw the border frame
	BRect r = BRect(fTopBorder.LeftTop(), fBottomBorder.RightBottom());
	switch ((int)fTopTab->look) {
		case B_TITLED_WINDOW_LOOK:
		case B_DOCUMENT_WINDOW_LOOK:
		{

			// left
			if (rect.Intersects(fLeftBorder.InsetByCopy(0, -fBorderWidth))) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_LEFT_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 5);
					fDrawingEngine->StrokeLine(
						BPoint(r.left + i, r.top + i),
						BPoint(r.left + i, r.bottom - i),
						colors[colorsIndex]);
				}
				// redraw line to be part of tab title
				fDrawingEngine->StrokeLine(BPoint(r.left, r.top),
					BPoint(r.left, r.top + 4), colors[6]);
			}
			// bottom
			if (rect.Intersects(fBottomBorder)) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_BOTTOM_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 5);
					fDrawingEngine->StrokeLine(
						BPoint(r.left + i, r.bottom - i),
						BPoint(r.right - i, r.bottom - i),
						colors[colorsIndex]);
				}
			}
			// right
			if (rect.Intersects(fRightBorder.InsetByCopy(0, -fBorderWidth))) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_RIGHT_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 5);
					fDrawingEngine->StrokeLine(
						BPoint(r.right - i, r.top + i),
						BPoint(r.right - i, r.bottom - i),
						colors[colorsIndex]);
				}
				// redraw line to be part of tab title
				fDrawingEngine->StrokeLine(BPoint(r.right, r.top),
					BPoint(r.right, r.top + 4),
					colors[6]);
			}
			// top
			if (rect.Intersects(fTopBorder)) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_TOP_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 3);
					fDrawingEngine->StrokeLine(
						BPoint(r.left + i, r.top + i),
						BPoint(r.right - i, r.top + i),
						colors[colorsIndex * 2]);
				}
				if (fTitleBarRect.IsValid() && fTopTab->look != kLeftTitledWindowLook) {
					// Complete with gradient effect
					for (int8 i = 1; i < fBorderWidth; i++) {
						if (i<fBorderWidth)
						{
							fDrawingEngine->StrokeLine(BPoint(r.left + 1, r.top + i),
								BPoint(r.right - 1, r.top + i), tint_color(colors[3], (i*0.01+1)));
						}
						else
						{
							fDrawingEngine->StrokeLine(BPoint(r.left + 1, r.top + i),
								BPoint(r.right - 1, r.top + i), tint_color(colors[3], 1.1));
						}
					}
				}
			}
			break;
		}

		case B_MODAL_WINDOW_LOOK:
		case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
			// top
			if (rect.Intersects(fTopBorder)) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_LEFT_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 3);
					fDrawingEngine->StrokeLine(
						BPoint(r.left + i, r.top + i),
						BPoint(r.right - i, r.top + i),
						colors[colorsIndex]);
				}
				if (fTitleBarRect.IsValid() && fTopTab->look != kLeftTitledWindowLook) {
					// grey along the bottom of the tab
					// (overwrites "white" from frame)
					const int overdraw = (int)ceilf(fBorderWidth / 5.0f);
					for (int i = 1; i <= overdraw; i++) {
						fDrawingEngine->StrokeLine(
							BPoint(fTitleBarRect.left + 2, fTitleBarRect.bottom + i),
							BPoint(fTitleBarRect.right - 2, fTitleBarRect.bottom + i),
							colors[1]);
					}
				}
			}
			// left
			if (rect.Intersects(fLeftBorder.InsetByCopy(0, -fBorderWidth))) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_LEFT_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 3);
					fDrawingEngine->StrokeLine(
						BPoint(r.left + i, r.top + i),
						BPoint(r.left + i, r.bottom - i),
						colors[colorsIndex]);
				}
				if (fTopTab->look == kLeftTitledWindowLook
					&& fTitleBarRect.IsValid()) {
					// grey along the right side of the tab
					// (overwrites "white" from frame)
					fDrawingEngine->StrokeLine(
						BPoint(fTitleBarRect.right + 1,
							fTitleBarRect.top + 2),
						BPoint(fTitleBarRect.right + 1,
							fTitleBarRect.bottom - 2), colors[1]);
				}
			}
			// bottom
			if (rect.Intersects(fBottomBorder)) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_BOTTOM_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 3);
					fDrawingEngine->StrokeLine(
						BPoint(r.left + i, r.bottom - i),
						BPoint(r.right - i, r.bottom - i),
						colors[colorsIndex]);
				}
			}
			// right
			if (rect.Intersects(fRightBorder.InsetByCopy(0, -fBorderWidth))) {
				ComponentColors colors;
				_GetComponentColors(COMPONENT_RIGHT_BORDER, colors, fTopTab);

				for (int8 i = 0; i < fBorderWidth; i++) {
					const int8 colorsIndex = COLORS_INDEX(i, fBorderWidth, 3);
					fDrawingEngine->StrokeLine(
						BPoint(r.right - i, r.top + i),
						BPoint(r.right - i, r.bottom - i),
						colors[colorsIndex]);
				}
			}
			break;
		}

		case B_BORDERED_WINDOW_LOOK:
		{
			// TODO: Draw the borders individually!
			ComponentColors colors;
			_GetComponentColors(COMPONENT_LEFT_BORDER, colors, fTopTab);

			fDrawingEngine->StrokeRect(r, colors[5]);
			break;
		}

		default:
			// don't draw a border frame
			break;
	}

	// Draw the resize knob if we're supposed to
	if (!(fTopTab->flags & B_NOT_RESIZABLE)) {
		r = fResizeRect;

		ComponentColors colors;
		_GetComponentColors(COMPONENT_RESIZE_CORNER, colors, fTopTab);

		switch ((int)fTopTab->look) {
			case B_DOCUMENT_WINDOW_LOOK:
			{
				if (fOutlinesDelta.x != 0 || fOutlinesDelta.y != 0) {
					r.Set(fFrame.right - 13, fFrame.bottom - 13,
						fFrame.right + 3, fFrame.bottom + 3);

					if (rect.Intersects(r))
						_DrawResizeKnob(r, false, colors);
				}

				if (rect.Intersects(fResizeRect)) {
					_DrawResizeKnob(fResizeRect, fTopTab && IsFocus(fTopTab),
						colors);
				}

				break;
			}

			case B_TITLED_WINDOW_LOOK:
			case B_FLOATING_WINDOW_LOOK:
			case B_MODAL_WINDOW_LOOK:
			case kLeftTitledWindowLook:
			{
				if (!rect.Intersects(BRect(
						fRightBorder.right - fBorderResizeLength,
						fBottomBorder.bottom - fBorderResizeLength,
						fRightBorder.right - 1,
						fBottomBorder.bottom - 1)))
					break;

				fDrawingEngine->StrokeLine(
					BPoint(fRightBorder.left,
						fBottomBorder.bottom - fBorderResizeLength),
					BPoint(fRightBorder.right - 1,
						fBottomBorder.bottom - fBorderResizeLength),
					colors[0]);
				fDrawingEngine->StrokeLine(
					BPoint(fRightBorder.right - fBorderResizeLength,
						fBottomBorder.top),
					BPoint(fRightBorder.right - fBorderResizeLength,
						fBottomBorder.bottom - 1),
					colors[0]);

				break;
			}

			default:
				// don't draw resize corner
				break;
		}
	}
}


void
GlassDecorator::_DrawResizeKnob(BRect rect, bool full,
	const ComponentColors& colors)
{
	float x = rect.right -= 4;
	float y = rect.bottom -= 4;

	BGradientLinear gradient;
	gradient.SetStart(rect.LeftTop());
	gradient.SetEnd(rect.RightBottom());
	gradient.AddColor(colors[1], 0);
	gradient.AddColor(colors[2], 255);

	fDrawingEngine->FillRect(rect, gradient);

	BPoint offset1(rect.Width(), rect.Height()),
		offset2(rect.Width() - 2, rect.Height() - 1);
	fDrawingEngine->StrokeLine(BPoint(x, y) - offset1,
		BPoint(x - offset1.x, y - 1), tint_color(colors[1], 1.05));
	fDrawingEngine->StrokeLine(BPoint(x, y) - offset1,
		BPoint(x - 1, y - offset1.y), tint_color(colors[1], 1.05));


	if (!full)
		return;

	for (int8 i = 1; i <= 4; i++) {
		for (int8 j = 1; j <= i; j++) {
			BPoint pt1(x - (3 * j) + 1, y - (3 * (5 - i)) + 1);
			BPoint pt2(x - (3 * j) + 2, y - (3 * (5 - i)) + 2);
			fDrawingEngine->StrokePoint(pt1, tint_color(colors[1], 1.6));
			fDrawingEngine->StrokePoint(pt2, tint_color(colors[1], 0.6));
		}
	}
}


/*!	\brief Actually draws the tab

	This function is called when the tab itself needs drawn. Other items,
	like the window title or buttons, should not be drawn here.

	\param tab The \a tab to update.
	\param rect The area of the \a tab to update.
*/
void
GlassDecorator::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f,%.1f,%.1f,%.1f)\n",
		invalid.left, invalid.top, invalid.right, invalid.bottom));
	const BRect& tabRect = tab->tabRect;
	// If a window has a tab, this will draw it and any buttons which are
	// in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;

	ComponentColors colors;
	_GetComponentColors(COMPONENT_TAB, colors, tab);

	bool roundLeft = (fTabList.IndexOf(tab) == 0);
	bool roundRight = (fTabList.IndexOf(tab) == fTabList.CountItems() - 1);

	float leftOffset = roundLeft ? 3.0f : 0.0f;
	float rightOffset = roundRight ? 3.0f : 0.0f;

	if (tab && tab->buttonFocus) {
		// outer frame
		fDrawingEngine->StrokeLine(BPoint(tabRect.left, tabRect.top + leftOffset), tabRect.LeftBottom(),
			colors[COLOR_TAB_FRAME_DARK]);
		fDrawingEngine->StrokeLine(BPoint(tabRect.left + leftOffset, tabRect.top), BPoint(tabRect.right - rightOffset, tabRect.top),
			colors[COLOR_TAB_FRAME_DARK]);
		if (roundLeft) {
			fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + 2), BPoint(tabRect.left + 2, tabRect.top + 1), colors[COLOR_TAB_FRAME_DARK]);
		}
		if (roundRight) {
			fDrawingEngine->StrokeLine(BPoint(tabRect.right - 2, tabRect.top + 1), BPoint(tabRect.right - 1, tabRect.top + 2), colors[COLOR_TAB_FRAME_DARK]);
		}
		
		if (tab->look != kLeftTitledWindowLook) {
			fDrawingEngine->StrokeLine(BPoint(tabRect.right, tabRect.top + rightOffset), tabRect.RightBottom(),
				colors[COLOR_TAB_FRAME_DARK]);
		} else {
			fDrawingEngine->StrokeLine(tabRect.LeftBottom(),
				tabRect.RightBottom(), fFocusFrameColor);
		}
	} else {
		// outer frame
		fDrawingEngine->StrokeLine(BPoint(tabRect.left, tabRect.top + leftOffset), BPoint(tabRect.left, tabRect.bottom - 1),
			colors[COLOR_TAB_FRAME_DARK]);
		fDrawingEngine->StrokeLine(BPoint(tabRect.left + leftOffset, tabRect.top), BPoint(tabRect.right - rightOffset, tabRect.top),
			colors[COLOR_TAB_FRAME_DARK]);
		if (roundLeft) {
			fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + 2), BPoint(tabRect.left + 2, tabRect.top + 1), colors[COLOR_TAB_FRAME_DARK]);
		}
		if (roundRight) {
			fDrawingEngine->StrokeLine(BPoint(tabRect.right - 2, tabRect.top + 1), BPoint(tabRect.right - 1, tabRect.top + 2), colors[COLOR_TAB_FRAME_DARK]);
		}

		if (tab->look != kLeftTitledWindowLook) {
			fDrawingEngine->StrokeLine(BPoint(tabRect.right, tabRect.top + rightOffset), BPoint(tabRect.right, tabRect.bottom - 1),
				colors[COLOR_TAB_FRAME_DARK]);
		} else {
			fDrawingEngine->StrokeLine(tabRect.LeftBottom(),
				tabRect.RightBottom(), fFocusFrameColor);
		}
	}

	float tabBottom = tabRect.bottom;
	if (fTopTab != tab)
		tabBottom -= 1;

	// bevel
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + leftOffset),
		BPoint(tabRect.left + 1,
			tabBottom - (tab->look == kLeftTitledWindowLook ? 1 : 0)),
		colors[COLOR_TAB]);
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1 + leftOffset, tabRect.top + 1),
		BPoint(tabRect.right - 1 - rightOffset,
			tabRect.top + 1),
		tint_color(colors[COLOR_TAB], 0.9));

	if (roundLeft) {
		fDrawingEngine->StrokePoint(BPoint(tabRect.left + 2, tabRect.top + 2), colors[COLOR_TAB]);
	}
	if (roundRight) {
		fDrawingEngine->StrokePoint(BPoint(tabRect.right - 2, tabRect.top + 2), colors[COLOR_TAB]);
	}

	if (tab->look != kLeftTitledWindowLook) {
		fDrawingEngine->StrokeLine(BPoint(tabRect.right - 1, tabRect.top + rightOffset),
			BPoint(tabRect.right - 1, tabBottom),
			colors[COLOR_TAB]);
	} else {
		fDrawingEngine->StrokeLine(
			BPoint(tabRect.left + 2, tabRect.bottom - 1),
			BPoint(tabRect.right, tabRect.bottom - 1),
			colors[COLOR_TAB]);
	}

	// fill (Simulated Glass)
	BGradientLinear gradient;
	gradient.SetStart(tabRect.LeftTop());
	if (tab->look != kLeftTitledWindowLook)
		gradient.SetEnd(tabRect.LeftBottom());
	else
		gradient.SetEnd(tabRect.RightTop());

	if (tab && tab->buttonFocus) {
		gradient.AddColor(tint_color(colors[COLOR_TAB], 0.3), 0);
		gradient.AddColor(tint_color(colors[COLOR_TAB], 0.7), 120);
		gradient.AddColor(tint_color(colors[COLOR_TAB], 1.1), 121); // Specular reflection
		gradient.AddColor(tint_color(colors[COLOR_TAB], 0.95), 255);
	} else {
		gradient.AddColor(tint_color(colors[COLOR_TAB], 0.6), 0);
		gradient.AddColor(tint_color(colors[COLOR_TAB], 0.9), 120);
		gradient.AddColor(tint_color(colors[COLOR_TAB], 1.2), 121);
		gradient.AddColor(tint_color(colors[COLOR_TAB], 1.05), 255);
	}

	if (tab->look != kLeftTitledWindowLook) {
		fDrawingEngine->FillRect(BRect(tabRect.left + 2, tabRect.top + 2,
			tabRect.right - 2, tabBottom), gradient);
	} else {
		fDrawingEngine->FillRect(BRect(tabRect.left + 2, tabRect.top + 2,
			tabRect.right, tabRect.bottom - 2), gradient);
	}

	// Inner white bevel (thickness)
	rgb_color whiteBevel = {255, 255, 255, 255};
	if (tab->look != kLeftTitledWindowLook) {
		fDrawingEngine->StrokeLine(BPoint(tabRect.left + 2, tabRect.top + 2), BPoint(tabRect.right - 2, tabRect.top + 2), whiteBevel);
		fDrawingEngine->StrokeLine(BPoint(tabRect.left + 2, tabRect.top + 2), BPoint(tabRect.left + 2, tabBottom), whiteBevel);
	}

	if (roundLeft) {
		fDrawingEngine->StrokeLine(BPoint(tabRect.left, tabRect.top), BPoint(tabRect.left + 2, tabRect.top), B_TRANSPARENT_COLOR);
		fDrawingEngine->StrokeLine(BPoint(tabRect.left, tabRect.top + 1), BPoint(tabRect.left + 1, tabRect.top + 1), B_TRANSPARENT_COLOR);
		fDrawingEngine->StrokePoint(BPoint(tabRect.left, tabRect.top + 2), B_TRANSPARENT_COLOR);
	}
	if (roundRight) {
		fDrawingEngine->StrokeLine(BPoint(tabRect.right - 2, tabRect.top), BPoint(tabRect.right, tabRect.top), B_TRANSPARENT_COLOR);
		fDrawingEngine->StrokeLine(BPoint(tabRect.right - 1, tabRect.top + 1), BPoint(tabRect.right, tabRect.top + 1), B_TRANSPARENT_COLOR);
		fDrawingEngine->StrokePoint(BPoint(tabRect.right, tabRect.top + 2), B_TRANSPARENT_COLOR);
	}

	_DrawTitle(tab, tabRect);

	_DrawButtons(tab, invalid);
}


/*!	\brief Actually draws the title

	The main tasks for this function are to ensure that the decorator draws
	the title only in its own area and drawing the title itself.
	Using B_OP_COPY for drawing the title is recommended because of the marked
	performance hit of the other drawing modes, but it is not a requirement.

	\param tab The \a tab to update.
	\param rect area of the title to update.
*/
void
GlassDecorator::_DrawTitle(Decorator::Tab* _tab, BRect rect)
{
	STRACE(("_DrawTitle(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);

	const BRect& tabRect = tab->tabRect;
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;

	ComponentColors colors;
	_GetComponentColors(COMPONENT_TAB, colors, tab);

	fDrawingEngine->SetDrawingMode(B_OP_OVER);
	fDrawingEngine->SetHighColor(colors[COLOR_TAB_TEXT]);
	fDrawingEngine->SetFont(fDrawState.Font());

	// figure out position of text
	font_height fontHeight;
	fDrawState.Font().GetHeight(fontHeight);

	BPoint titlePos;
	if (tab->look != kLeftTitledWindowLook) {
		titlePos.x = closeRect.IsValid() ? closeRect.right + tab->textOffset
			: tabRect.left + tab->textOffset;
		titlePos.y = floorf(((tabRect.top + 2.0) + tabRect.bottom
			+ fontHeight.ascent + fontHeight.descent) / 2.0
			- fontHeight.descent + 0.5);
	} else {
		titlePos.x = floorf(((tabRect.left + 2.0) + tabRect.right
			+ fontHeight.ascent + fontHeight.descent) / 2.0
			- fontHeight.descent + 0.5);
		titlePos.y = zoomRect.IsValid() ? zoomRect.top - tab->textOffset
			: tabRect.bottom - tab->textOffset;
	}

	fDrawingEngine->DrawString(tab->truncatedTitle, tab->truncatedTitleLength,
		titlePos);

	fDrawingEngine->SetDrawingMode(B_OP_COPY);
}


/*!	\brief Actually draws the close button

	Unless a subclass has a particularly large button, it is probably
	unnecessary to check the update rectangle.

	\param _tab The \a tab to update.
	\param direct Draw without double buffering.
	\param rect The area of the button to update.
*/
void
GlassDecorator::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawClose(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);

	int32 index = (tab->buttonFocus ? 0 : 1) + (tab->closePressed ? 0 : 2);
	ServerBitmap* bitmap = tab->closeBitmaps[index];
	if (bitmap == NULL) {
		bitmap = _GetBitmapForButton(tab, COMPONENT_CLOSE_BUTTON,
			tab->closePressed, rect.IntegerWidth(), rect.IntegerHeight());
		tab->closeBitmaps[index] = bitmap;
	}

	_DrawButtonBitmap(bitmap, direct, rect);
}


/*!	\brief Actually draws the zoom button

	Unless a subclass has a particularly large button, it is probably
	unnecessary to check the update rectangle.

	\param _tab The \a tab to update.
	\param direct Draw without double buffering.
	\param rect The area of the button to update.
*/
void
GlassDecorator::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawZoom(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	if (rect.IntegerWidth() < 1)
		return;

	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	int32 index = (tab->buttonFocus ? 0 : 1) + (tab->zoomPressed ? 0 : 2);
	ServerBitmap* bitmap = tab->zoomBitmaps[index];
	if (bitmap == NULL) {
		bitmap = _GetBitmapForButton(tab, COMPONENT_ZOOM_BUTTON,
			tab->zoomPressed, rect.IntegerWidth(), rect.IntegerHeight());
		tab->zoomBitmaps[index] = bitmap;
	}

	_DrawButtonBitmap(bitmap, direct, rect);
}


void
GlassDecorator::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}


// #pragma mark - Private methods


void
GlassDecorator::_DrawButtonBitmap(ServerBitmap* bitmap, bool direct,
	BRect rect)
{
	if (bitmap == NULL)
		return;

	bool copyToFrontEnabled = fDrawingEngine->CopyToFrontEnabled();
	fDrawingEngine->SetCopyToFrontEnabled(direct);
	drawing_mode oldMode;
	fDrawingEngine->SetDrawingMode(B_OP_OVER, oldMode);
	fDrawingEngine->DrawBitmap(bitmap, rect.OffsetToCopy(0, 0), rect);
	fDrawingEngine->SetDrawingMode(oldMode);
	fDrawingEngine->SetCopyToFrontEnabled(copyToFrontEnabled);
}


/*!	\brief Draws a framed rectangle with a gradient.
	\param engine The drawing engine to use.
	\param rect The rectangular area to draw in.
	\param down The rectangle should be drawn recessed or not.
	\param colors A button color array of the colors to be used.
*/
void
GlassDecorator::_DrawBlendedRect(DrawingEngine* engine, const BRect rect,
	bool down, const ComponentColors& colors)
{
	rgb_color darkColor = tint_color(colors[COLOR_BUTTON], 1.3);
	rgb_color lightColor = tint_color(colors[COLOR_BUTTON], 0.7);
	rgb_color fillColor = tint_color(colors[COLOR_BUTTON], 0.9);

	if (down) {
		darkColor = tint_color(colors[COLOR_BUTTON], 1.45);
		lightColor = tint_color(colors[COLOR_BUTTON], 0.85);
		fillColor = tint_color(colors[COLOR_BUTTON], 1.1);
	}

	engine->FillRect(rect, fillColor);

	// outline engraved (inset bevel)
	engine->StrokeLine(rect.LeftTop(), rect.RightTop(), darkColor);
	engine->StrokeLine(rect.LeftTop(), rect.LeftBottom(), darkColor);
	engine->StrokeLine(BPoint(rect.right, rect.top + 1), rect.RightBottom(), lightColor);
	engine->StrokeLine(BPoint(rect.left + 1, rect.bottom), rect.RightBottom(), lightColor);
}


ServerBitmap*
GlassDecorator::_GetBitmapForButton(Decorator::Tab* tab, Component item,
	bool down, int32 width, int32 height)
{
	// TODO: the list of shared bitmaps is never freed
	struct decorator_bitmap {
		Component			item;
		bool				down;
		int32				width;
		int32				height;
		rgb_color			baseColor;
		rgb_color			lightColor;
		UtilityBitmap*		bitmap;
		decorator_bitmap*	next;
	};

	static BLocker sBitmapListLock("decorator lock", true);
	static decorator_bitmap* sBitmapList = NULL;

	ComponentColors colors;
	_GetComponentColors(item, colors, tab);

	BAutolock locker(sBitmapListLock);

	// search our list for a matching bitmap
	// TODO: use a hash map instead?
	decorator_bitmap* current = sBitmapList;
	while (current) {
		if (current->item == item && current->down == down
			&& current->width == width && current->height == height
			&& current->baseColor == colors[COLOR_BUTTON]
			&& current->lightColor == colors[COLOR_BUTTON_LIGHT]) {
			return current->bitmap;
		}

		current = current->next;
	}

	static BitmapDrawingEngine* sBitmapDrawingEngine = NULL;

	// didn't find any bitmap, create a new one
	if (sBitmapDrawingEngine == NULL)
		sBitmapDrawingEngine = new(std::nothrow) BitmapDrawingEngine();
	if (sBitmapDrawingEngine == NULL
		|| sBitmapDrawingEngine->SetSize(width, height) != B_OK)
		return NULL;

	BRect rect(0, 0, width - 1, height - 1);

	STRACE(("GlassDecorator creating bitmap for %s %s at size %ldx%ld\n",
		item == COMPONENT_CLOSE_BUTTON ? "close" : "zoom",
		down ? "down" : "up", width, height));
	switch (item) {
		case COMPONENT_CLOSE_BUTTON:
			if (tab && tab->buttonFocus)
				_DrawBlendedRect(sBitmapDrawingEngine, rect, down, colors);
			else
				_DrawBlendedRect(sBitmapDrawingEngine, rect, true, colors);
			break;

		case COMPONENT_ZOOM_BUTTON:
		{
			sBitmapDrawingEngine->FillRect(rect, B_TRANSPARENT_COLOR);
				// init the background

			float inset = floorf(width / 4.0);
			BRect zoomRect(rect);
			zoomRect.left += inset;
			zoomRect.top += inset;
			if (tab && tab->buttonFocus)
				_DrawBlendedRect(sBitmapDrawingEngine, zoomRect, down, colors);
			else
				_DrawBlendedRect(sBitmapDrawingEngine, zoomRect, true, colors);

			inset = floorf(width / 2.1);
			zoomRect = rect;
			zoomRect.right -= inset;
			zoomRect.bottom -= inset;
			if (tab && tab->buttonFocus)
				_DrawBlendedRect(sBitmapDrawingEngine, zoomRect, down, colors);
			else
				_DrawBlendedRect(sBitmapDrawingEngine, zoomRect, true, colors);
			break;
		}

		default:
			break;
	}

	UtilityBitmap* bitmap = sBitmapDrawingEngine->ExportToBitmap(width, height,
		B_RGB32);
	if (bitmap == NULL)
		return NULL;

	// bitmap ready, put it into the list
	decorator_bitmap* entry = new(std::nothrow) decorator_bitmap;
	if (entry == NULL) {
		delete bitmap;
		return NULL;
	}

	entry->item = item;
	entry->down = down;
	entry->width = width;
	entry->height = height;
	entry->bitmap = bitmap;
	entry->baseColor = colors[COLOR_BUTTON];
	entry->lightColor = colors[COLOR_BUTTON_LIGHT];
	entry->next = sBitmapList;
	sBitmapList = entry;

	return bitmap;
}


void
GlassDecorator::_GetComponentColors(Component component,
	ComponentColors _colors, Decorator::Tab* tab)
{
	// get the highlight for our component
	Region region = REGION_NONE;
	switch (component) {
		case COMPONENT_TAB:
			region = REGION_TAB;
			break;
		case COMPONENT_CLOSE_BUTTON:
			region = REGION_CLOSE_BUTTON;
			break;
		case COMPONENT_ZOOM_BUTTON:
			region = REGION_ZOOM_BUTTON;
			break;
		case COMPONENT_LEFT_BORDER:
			region = REGION_LEFT_BORDER;
			break;
		case COMPONENT_RIGHT_BORDER:
			region = REGION_RIGHT_BORDER;
			break;
		case COMPONENT_TOP_BORDER:
			region = REGION_TOP_BORDER;
			break;
		case COMPONENT_BOTTOM_BORDER:
			region = REGION_BOTTOM_BORDER;
			break;
		case COMPONENT_RESIZE_CORNER:
			region = REGION_RIGHT_BOTTOM_CORNER;
			break;
	}

	return GetComponentColors(component, RegionHighlight(region), _colors, tab);
}


extern "C" DecorAddOn* (instantiate_decor_addon)(image_id id, const char* name)
{
	return new (std::nothrow)GlassDecorAddOn(id, name);
}

