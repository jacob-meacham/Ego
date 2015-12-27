#pragma once

#include "Math.h"
#include "DXUtil.h"
//////////////////////////////////////////////////////////////////////////////////
//																				//
// --------------------------------- Font ------------------------------------- //
//																				//
//////////////////////////////////////////////////////////////////////////////////
// Class which wraps DirectX support for rendering fonts.
class Font {
	private:
		ID3DXFont *pFont; ///< DirectX Font interface.

	public:
		Font() { pFont = NULL; }
		~Font() { freeFont(); }
		
		bool create(const char * pname, u32 size, bool bold = false, bool italic = false);
		void freeFont();

		// Prints the text in the font's specifications at the specified coordinates.
		bool render(const char * ptext, u32 x, u32 y, u32 width, u32 height, Color color = 0xFFFFFFFF, DWORD Format = 0) const;
		long calcHeight(const char * ptext, u32 x, u32 y, u32 width, u32 height) const;
};