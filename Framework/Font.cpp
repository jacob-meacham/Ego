#include "Font.h"
#include "GraphicsDevice.h"

/// Creates the specified font.
/** \param Graphics GraphicsCore object to attach the font.
	\param Name Name of the font.  These include "Times New Roman" and "Courier"
	\param Size Point size of the font.
	\param Bold TRUE if the font is bold (default false).
	\param Italic TRUE if the font is italic (default false).
	\param Underline TRUE if the font is underlined (default false).
	\return TRUE if the font is created successfully.
*/
bool Font::create(const char * pname, u32 size, bool bold, bool italic)
{
	D3DXFONT_DESC desc;

	// Error Checking.
	if(!pname)
		return false;

	if(!gGraphics.getDeviceCOM())
		return false;

	// Clear out the font structure
	ZeroMemory(&desc, sizeof(D3DXFONT_DESC));

	// Set the font name and height
	strcpy_s(desc.FaceName, pname);
	desc.Height = -((int)size);
	desc.Weight = (bold == TRUE) ? 700 : 0;
	desc.Italic = italic;
	desc.CharSet = DEFAULT_CHARSET;
	desc.Quality = 0;
	desc.PitchAndFamily = 0;

	pSprite = gGraphics.getSpriteCOM();

  // Create the font object
	if(FAILED(D3DXCreateFontIndirect(gGraphics.getDeviceCOM(), &desc, &pFont))) {
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Frees the Font object.
void Font::free()
{
	DX_RELEASE(pFont);
}
//////////////////////////////////////////////////////////////////////////////////
/// Prints Text to the screen in a textbox (not the class Textbox), using the font's properties.
/** \param Text text to print.
	\param XPos x-coordinate that represents the upper-left corner of the textbox.
	\param YPos y-coordinate that represents the upper-left corner of the textbox.
	\param Width Width of the textbox.
	\param Height Height of the textbox.
	\param Color Color of the printed text. (D3DCOLOR expects an 8 bit hex number).
	\param Format Format of the printed font.  This is achieved via flags, which can be combined using bitwise OR  
	Some of the more useful ones include:
	DT_WORDBREAK
	DT_BOTTOM
	DT_CENTER
	DT_LEFT
	DT_RIGHT
	DT_TOP
	\return TRUE if printing is successful.
*/
bool Font::render(const char * ptext, u32 x, u32 y, u32 width, u32 height, Color color, DWORD Format)
{
	if(!pFont)
		return false;

	RECT Rect;
	Rect.left   = x;
	Rect.top    = y;
	Rect.right  = Rect.left + width;
	Rect.bottom = Rect.top + height;
	if(FAILED(pFont->DrawText(pSprite, ptext, -1, &Rect, Format, color))) 
	{
		return false;
	}

	return true;
}