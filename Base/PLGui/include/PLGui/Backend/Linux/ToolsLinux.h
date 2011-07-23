/*********************************************************\
 *  File: ToolsLinux.h                                   *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


#ifndef __PLGUI_TOOLSLINUX_H__
#define __PLGUI_TOOLSLINUX_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/PLCore.h>
#include <PLGraphics/Color/Color4.h>
#include "PLGui/PLGui.h"
#include "PLGui/PLGuiLinuxIncludes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace PLMath {
	class Vector2i;
}
namespace PLGraphics {
	class Image;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLGui {


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Tool functions for the Linux GUI implementation
*/
class ToolsLinux {


	//[-------------------------------------------------------]
	//[ Public static functions                               ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Convert color to X color
		*
		*  @param[in] cColor
		*    Color
		*  @param[in] nColorDepth
		*    Color depth (currently supported: 16, 24)
		*
		*  @return
		*    Color value
		*/
		PLGUI_API static long GetXColor(const PLGraphics::Color3 &cColor, int nColorDepth);

		/**
		*  @brief
		*    Set hints for not applying a window decoration for a window
		*
		*  @param[in] pDisplay
		*    Display
		*  @param[in] nWindow
		*    Window
		*  @param[in] bDecorate
		*    'true' to apply default window decoration, else 'false'
		*
		*  @return
		*    'true' if all went fine, else 'false'
		*/
		PLGUI_API static bool SetNoWindowDecoration(Display *pDisplay, ::Window nWindow, bool bDecorate);

		/**
		*  @brief
		*    Create X pixmap from image
		*
		*  @param[in]  pDisplay
		*    Display to use, must be valid
		*  @param[in]  cImage
		*    Image to use
		*  @param[in]  cColorKey
		*    Color key to use (pass Color4::Transparent to use no color key)
		*  @param[out] pPixmap
		*    Will receive the pixmap
		*  @param[out] pMaskPixmap
		*    Will receive the mask pixmap
		*
		*  @return
		*    'true' if all went fine, else 'false'
		*
		*  @note
		*    - Creates a X pixmap of the image
		*    - Use XFreePixmap() to free the pixmap
		*/
		PLGUI_API static bool CreatePixmapFromImage(Display *pDisplay, PLGraphics::Image &cImage, Pixmap &pPixmap, Pixmap &pMaskPixmap, const PLGraphics::Color4 &cColorKey = PLGraphics::Color4::Transparent);

		/**
		*  @brief
		*    Create X cursor from image
		*
		*  @param[in]  pDisplay
		*    Display to use, must be valid
		*  @param[in]  cImage
		*    Image to use
		*  @param[in]  vHotspot
		*    Position of hotstop
		*  @param[out] pCursor
		*    Will receive the cursor
		*
		*  @return
		*    'true' if all went fine, else 'false'
		*
		*  @note
		*    - Creates a X cursor of the image
		*    - Use XFreeCursor() to free the cursor
		*/
		PLGUI_API static bool CreateCursorFromImage(Display *pDisplay, PLGraphics::Image &cImage, const PLMath::Vector2i &vHotspot, ::Cursor &pCursor);

		/**
		*  @brief
		*    Create empty X cursor
		*
		*  @param[in]  pDisplay
		*    Display to use, must be valid
		*
		*  @return
		*    Cursor
		*/
		PLGUI_API static ::Cursor CreateEmptyCursor(Display *pDisplay);


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLGui


#endif // __PLGUI_TOOLSLINUX_H__