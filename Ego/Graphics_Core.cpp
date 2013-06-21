#include "Graphics_Core.h"
#include <stdio.h>
#include <sstream>

#define Error(x) MessageBox(NULL, x, "Error", MB_OK);
/// Default Constructor.
GraphicsCore::GraphicsCore() {
	
	m_hWnd = NULL;
	m_d3d = NULL;
	m_d3dDevice = NULL;

	m_Width = 0;
	m_Height = 0;

	m_Windowed = TRUE;
	
}
///First function to call.  Produces the necessary D3D interface in m_d3d.
/** \return TRUE if the interface is not NULL.
*/
BOOL GraphicsCore::Initialize() {

	m_d3d = Direct3DCreate8(D3D_SDK_VERSION); //Create a D3D interface
	if(m_d3d == NULL) { Error("could not initialize"); return FALSE; }
	return TRUE;
}

///Set Graphic mode, given a window handle.  
/** \param hWnd Handle to a Windows window.
	\param Windowed If Windowed is TRUE, then the graphics mode is not windowed.
	\param Width The width of the window.  If left at 0, the width is automatically calculated.
	\param Height The height of the window.  If left at 0, the height is automatically calculated.
	\return TRUE if the graphics are set successfully.
*/
BOOL GraphicsCore::SetGraphics(HWND hWnd, BOOL Windowed, long Width, long Height) {	
	m_hWnd = hWnd;
	m_Windowed = Windowed;
	
	if(!m_hWnd) return FALSE;
	if(!m_d3d) return FALSE;

	if(FAILED(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3dDisplay))) { //Get usable display modes
		return FALSE;
	}

	if(!Width) { //If no width given, then...
		
		if(Windowed == FALSE) { //if not windowed, just use Display resolution
			m_Width = m_d3dDisplay.Width;	
		} 
		else { //if windowed, use the size of the window
			GetClientRect(m_hWnd, &ClientRect); 
			m_Width = ClientRect.right;
		}
	} // end if(!Width)
	else {
		m_Width = Width;
	}

	if(!Height) { //If no height given, then
		
		if(Windowed == FALSE) { //if not windowd, use Display resolution
			m_Height = m_d3dDisplay.Height;
			
		} 
		else { //if windowed, use the size of the window
			GetClientRect(m_hWnd, &ClientRect);
			m_Height = ClientRect.bottom;
		}
	} // end if(!Height)
	else {
		m_Height = Height;
	}

	if(Windowed == TRUE) { //If it is windowed, use the current resolution settings.
		m_BPP = m_d3dDisplay.Format;
	}
	else { //otherwise use the typical resolution format (ie 32 bit).
		m_BPP = D3DFMT_X8R8G8B8;
		if(CheckFormat(m_BPP, Windowed, TRUE) == FALSE) {
			return FALSE;
		}
	}
	
	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	// These are standard D3DPRESENT_PARAMETERS settings.
	if(Windowed == TRUE) {
		m_d3dpp.Windowed = TRUE;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.BackBufferFormat = m_BPP;
	}
	else if(Windowed == FALSE) {
		m_d3dpp.Windowed = FALSE;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dpp.BackBufferWidth  = m_Width;
		m_d3dpp.BackBufferHeight = m_Height;
		m_d3dpp.BackBufferFormat = m_BPP;
	}
	
	//Create device, using software vertex processing and HAL.
	if(FAILED(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&m_d3dpp, &m_d3dDevice))) { return FALSE; }

	//Create Sprite interface, using newly minted device.
	if(FAILED(D3DXCreateSprite(m_d3dDevice, &m_Sprite))) { return FALSE; }

	// if all creations are successful, return true.
	return TRUE;
}

/// Function used to check whether a particular format is supported by the hardwore and/or software emulation.
BOOL GraphicsCore::CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL)
{
  if(FAILED(m_d3d->CheckDeviceType(D3DADAPTER_DEFAULT, 
              (HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
              Format, Format, Windowed)))
    return FALSE;

  return TRUE;
}


/// Destructor.
BOOL GraphicsCore::Shutdown() {
	if(m_Sprite) {
		m_Sprite->Release();
		m_Sprite = NULL;
	}
	if(m_d3dDevice) {
		m_d3dDevice->Release();
		m_d3dDevice = NULL;
	}
	if(m_d3d) {
		m_d3d->Release();
		m_d3d = NULL;
	}
	if(m_hWnd) m_hWnd = NULL;
		
	return TRUE;
}

/// Sets the graphic device up so that it is ready to render a frame.
/** NOTE: Should be coupled with a call to EndScene() after any rendering is complete.
	\return TRUE if the scene is set succesfully.
*/
BOOL GraphicsCore::BeginScene() {
	if(m_d3dDevice == NULL) { Error("Device is NULL"); return FALSE; }

	if(FAILED(m_d3dDevice->BeginScene())) { Error("BeginScene() is False!"); return FALSE; }
  return TRUE;
}

/// Returns the graphic device to its former state after a call to BeginScene()
/** NOTE: Should be coupled with a call to BeginScene().
	\return TRUE if the graphics device is reset successfully.
*/
BOOL GraphicsCore::EndScene() {
	if(m_d3dDevice == NULL) { return FALSE; }

	if(FAILED(m_d3dDevice->EndScene())) { return FALSE; }

  return TRUE;
}

/// Sets the sprite COM to render sprites.
/** NOTE: Should be coupled with a call to EndSprite() after any rendering is complete.
	\return TRUE if the sprite COM is set successfully.
*/
BOOL GraphicsCore::BeginSprite()
{
	if(m_Sprite == NULL) { Error("Sprite is NULL"); return FALSE; }
	if(FAILED(m_Sprite->Begin())) { return FALSE; }
  
  return TRUE;
}

/// Sets the sprite COM to finish rendering sprites.
/** NOTE: Should be coupled with a call to BeginSprite().
	\return TRUE if the sprite COM is reset successfully.
*/
BOOL GraphicsCore::EndSprite()
{
	if(m_Sprite == NULL) { Error("Sprite is null"); return FALSE; }

	if(FAILED(m_Sprite->End())) { return FALSE; }

  return TRUE;
}

/// Presents the backbuffer, and anything rendered to it.
/** \return TRUE if the backbuffer is flipped successfully.
*/
BOOL GraphicsCore::Display()
{
	if(m_d3dDevice == NULL) { Error("Device is Null"); return FALSE; }

	if(FAILED(m_d3dDevice->Present(NULL, NULL, NULL, NULL))) { Error("Couldn't Display"); return FALSE; }

  return TRUE;
}

/// Clears the backbuffer
BOOL GraphicsCore::Clear(long Color, float ZBuffer)
{
	if(m_d3dDevice == NULL) { return FALSE; }

	if(FAILED(m_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, ZBuffer, 0))) { return FALSE; }

  return TRUE; 
}

/// Returns a pointer to the graphic device.
IDirect3DDevice8 *GraphicsCore::GetDeviceCOM() {
	return m_d3dDevice;
}

/// Returns a pointer to the sprite device.
ID3DXSprite *GraphicsCore::GetSpriteCOM() {
	return m_Sprite;
}

///////////////////////////////////////////////////////////
//                  Sprite Core                          //
///////////////////////////////////////////////////////////

/// Default Constructor.
Texture::Texture() {
	m_Width = m_Height = 0;
	m_Texture = NULL;
	m_Graphics = NULL;
}

/// Destructor.
BOOL Texture::Free() {
	if(m_Texture) {
		m_Texture->Release();
		m_Texture = NULL;
	}
	if(m_Graphics) {  m_Graphics = NULL; }
  m_Width = m_Height = 0;
  return TRUE;
}

/// Loads a texture into m_Texture from file.
/** \param Graphics The GraphicsCore to which this texture is attached.
	\param FileName Name of the image file to use.
	\return TRUE if the file is loaded properly.
*/
BOOL Texture::Load(GraphicsCore *Graphics, char *FileName) {
	m_Graphics = Graphics;
	
	// Checks for any errors in the passed graphic device.
	if(m_Graphics == NULL) { 
		Error("m_Graphics is NULL"); 
		return FALSE; 
	}
	if(Graphics->GetDeviceCOM() == NULL) { 
		Error("Couldn't get the COM"); 
		return FALSE; 
	}
	if(FileName == NULL) { 
		Error("FileName is NULL"); 
		return FALSE; 
	}

	if(FAILED(D3DXCreateTextureFromFileEx(
							Graphics->GetDeviceCOM(), 
							FileName, 
							D3DX_DEFAULT,
                            D3DX_DEFAULT,
                            1,
                            0,
                            D3DFMT_UNKNOWN,
                            D3DPOOL_DEFAULT,
                            D3DX_FILTER_LINEAR,
			                D3DX_FILTER_LINEAR,
			                0xFF00FF30,
                            NULL,
                            NULL,
							&m_Texture))) {
		Error("Could not Create Texture");
		return FALSE; 
	}
	
	m_Width = GetWidth();	
	m_Height = GetHeight();
		
	return TRUE;

}

/// Getter which calculates the width of the texture, using D3DSURFACE_DESC.
long Texture::GetWidth()
{
  D3DSURFACE_DESC d3dsd;

  if(m_Texture == NULL)   
     return 0;

  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd))) 
    return 0;

  return d3dsd.Width;
}

/// Getter which calculates the height of the texture, using D3DSURFACE_DESC.
long Texture::GetHeight()
{
  D3DSURFACE_DESC d3dsd;

  if(m_Texture == NULL) {
    return 0;
  }

  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd))) {
    return 0;
  }

  return d3dsd.Height;
}

/// Blits a sprite to the screen.  
/** This function should mostly only be called by Draw() in the Tile class.  It renders any subset of the texture
	\param DestX The x-coordinate to render the texture.  This coordinate represents the top-left corner.
	\param DestY The y-coordinate to render the texture.  This coordinate represents the top-left corner.
	\param SrcX The x-coordinate of the subset of the texture to render.
	\param SrcY the y-coordinate of the subset of the texture to render.
	\param Width the width of the subset to render.
	\param Height the height of the subset to render.
	\param XScale the scale with which to render the texture.
	\param YScale the scale with which to render the texture.
	\param Color the color to consider transparent in the texture.
	\return TRUE if the texture is drawn successfully.
*/
BOOL Texture::Blit(long DestX, long DestY, 
					long SrcX, long SrcY, 
					long Width, long Height, 
					float XScale, float YScale, 
					D3DCOLOR Color)
{
  RECT Rect;
  ID3DXSprite *Sprite;

  if(m_Texture == NULL) { return FALSE; }
  if(m_Graphics == NULL) { return FALSE; }
  if((Sprite = m_Graphics->GetSpriteCOM()) == NULL) { return FALSE; }

  if(!Width) { Width = m_Width; }
  if(!Height) { Height = m_Height; }

  // Set the source rectangle.
  Rect.left = SrcX;
  Rect.top  = SrcY;
  Rect.right = Rect.left + Width;
  Rect.bottom = Rect.top + Height;
  m_Graphics->GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
  m_Graphics->GetDeviceCOM()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  m_Graphics->GetDeviceCOM()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
  if(FAILED(Sprite->Draw(m_Texture,                          
                         &Rect, &D3DXVECTOR2(XScale, YScale),                 
                         NULL, 0.0f, &D3DXVECTOR2((float)DestX, (float)DestY), Color)))
    return FALSE;
  return TRUE;
}


/// Default Constructor.
Tile::Tile() {
	m_Graphics = NULL;
	m_Widths = m_Heights = m_Columns = 0;
	m_Textures = NULL;
	m_NumTextures = 0;
}

/// Default Destructor.
Tile::~Tile() {
	Free();
}

/// Create a Tileset, with the specified number of textures.
/** \param Graphics The parent GraphicsCore object.
	\param NumTextures The number of texture that this Tileset will have
	\return TRUE if the tileset is created successfully.
*/
BOOL Tile::Create(GraphicsCore *Graphics, long NumTextures) {
	m_Graphics = Graphics;
	m_NumTextures = NumTextures;
	
	
	if(m_Graphics == NULL) return FALSE;
	if(m_NumTextures == NULL) return FALSE;
	
	// Create m_NumTextures Textures
	m_Textures = new Texture[m_NumTextures];
	
	if(m_Textures == NULL) return FALSE;

	// Create space for the width, height, and columns of m_NumTextures
	m_Widths = new long[m_NumTextures];
	m_Heights = new long[m_NumTextures];
	m_Columns = new long[m_NumTextures];

	return TRUE;
}

/// Frees a tileset.
BOOL Tile::Free() {
	// Simply need to call Texture Free() for each texture.
	if(m_NumTextures != 0) {
		for(short i=0; i<m_NumTextures; i++) {
			m_Textures[i].Free();
		}
	}

	// Delete pointers
	delete [] m_Textures;
	delete [] m_Widths;
	delete [] m_Heights;
	delete [] m_Columns;
	if(m_Textures) {m_Textures = NULL; }
	m_Widths = m_Heights = m_Columns = NULL;
	m_NumTextures = 0;

	return TRUE;
}

/// Load a tileset from File into one of the texture slots created using Tile::Create().
/** \param TextureNum Index of the texture to be loaded
	\param Filename Filename of the texture to load
	\param TileWidth width of the tiles in FileName.
	\param TileHeight height of the tiles if FileName.
	NOTE: The tiles in any file are assumed to be the same size.
*/
BOOL Tile::Load(long TextureNum, char *Filename, short TileWidth, short TileHeight) {
	if(TextureNum >= m_NumTextures) {
		Error("Error in Tile Load, wrong texture number");
		return FALSE;
	}

	if(m_Textures == NULL) {
		Error("Texture pointer is null!");
		return FALSE;
	}

	if(m_Textures[TextureNum].Load(m_Graphics, Filename) == FALSE) { 
		Error("Error in Tile Load, could not open file"); 
		return FALSE; 
	}

	m_Widths[TextureNum] = TileWidth;
	m_Heights[TextureNum] = TileHeight;

	// The columns are the width of the texture divided by the width of the tiles in the texture.
	m_Columns[TextureNum] = m_Textures[TextureNum].GetWidth() / m_Widths[TextureNum];
	return TRUE;
}

/// Free a texture from the tileset.
BOOL Tile::FreeTexture(long TextureNum) {
	if(TextureNum >= m_NumTextures || m_Textures == NULL) return FALSE;

	m_Textures[TextureNum].Free();
	return TRUE;
}

/// Width of the WHOLE tileset in TextureNum.
long Tile::GetWidth(long TextureNum) {
	if(TextureNum >= m_NumTextures || m_Textures == NULL) return 0;

	return m_Widths[TextureNum];
}
/// Height of the WHOLE tileset in TextureNum.
long Tile::GetHeight(long TextureNum) {
	if(TextureNum >= m_NumTextures || m_Textures == NULL) return 0;

	return m_Heights[TextureNum];
}

/// Number of tiles in the tileset.
long Tile::GetNum(long TextureNum) {
	if(TextureNum >= m_NumTextures || m_Textures == NULL) return 0;

	return m_Columns[TextureNum] * (m_Textures[TextureNum].GetHeight() / m_Heights[TextureNum]);
}

/// Draw the specified tile from the specified tileset onto the screen, using the Texture::Blit().
/** \param TextureNum The index of the Texture from the tileset.
	\param TileNum The number of the tile in the tileset.  The tiles are numbered left-to-right, 
	top-bottom, starting with 0.
	\param ScreenX x-coordinate at which we draw the tile.
	\param ScreenY y-coordinate at which we draw the tile.
	\param Color The transparent color in this tile.
	\param XScale Scale to draw the tile.
	\param YScale Scale to draw the tile.
*/
BOOL Tile::Draw(long TextureNum, long TileNum, long ScreenX, 
				long ScreenY, D3DCOLOR Color,
				float XScale, float YScale) {
	
	long SrcX, SrcY;
	
	// Check to make sure that we still have a graphics device
	if(m_Graphics == NULL) {
		return FALSE;
	}

	// Make sure that the texture is not out of bounds.
	if(TextureNum >= m_NumTextures || m_Textures == NULL) {
		return FALSE;
	}

	// The source coordinates in the texture file is calculated by moving to
	// the correct column and then multiplying by either the width or the height.
	SrcX = (TileNum % m_Columns[TextureNum]) * m_Widths[TextureNum];
	SrcY = (TileNum / m_Columns[TextureNum]) * m_Heights[TextureNum];

	// Then, we can just use Texture::Blit().
	if(m_Textures[TextureNum].Blit(ScreenX, ScreenY, SrcX, SrcY,
								   m_Widths[TextureNum], m_Heights[TextureNum],
								   XScale, YScale, Color) == FALSE) {
									   return FALSE; }
	return TRUE;
}

/// Default Constructor.
Font::Font()
{
  m_Font = NULL;
}

/// Default Destructor.
Font::~Font()
{
  Free();
}

/// Getter to retrieve the Font COM
ID3DXFont *Font::GetFontCOM()
{
  return m_Font;
}

/// Creates the specified font.
/** \param Graphics GraphicsCore object to attach the font.
	\param Name Name of the font.  These include "Times New Roman" and "Courier"
	\param Size Point size of the font.
	\param Bold TRUE if the font is bold (default false).
	\param Italic TRUE if the font is italic (default false).
	\param Underline TRUE if the font is underlined (default false).
	\return TRUE if the font is created successfully.
*/
BOOL Font::Create(GraphicsCore *Graphics, char *Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline)
{
  //Uses the standard LOGFONT structure.
	LOGFONT lf;

	// Error Checking.
	if(Graphics == NULL || Name == NULL) {
		return FALSE;
	}
	if(Graphics->GetDeviceCOM() == NULL) {
		return FALSE;
	}

	// Clear out the font structure
	ZeroMemory(&lf, sizeof(LOGFONT));

	// Set the font name and height
	strcpy_s(lf.lfFaceName, Name);
	lf.lfHeight = -Size;
	lf.lfWeight = (Bold == TRUE) ? 700 : 0;
	lf.lfItalic = Italic;
	lf.lfUnderline = Underline;

  // Create the font object
	if(FAILED(D3DXCreateFontIndirect(Graphics->GetDeviceCOM(), &lf, &m_Font))) {
		return FALSE;
	}
	return TRUE;
}

/// Frees the Font object.
BOOL Font::Free()
{
	if(m_Font) {
		m_Font->Release();
		m_Font = NULL;
	}
  
  return TRUE;
}

/// Sets the Font up so that it is ready to print.
/** NOTE: Should be coupled with a call to EndFont() after any printing is complete.
	\return TRUE if the font is set succesfully.
*/
BOOL Font::BeginFont()
{
	if(m_Font == NULL) {
		return FALSE;
	}
	if(FAILED(m_Font->Begin())) {
		return FALSE;
	}
	return TRUE;
}

/// Resets the Font when it is done printing.
/** NOTE: Should be coupled with a call to BeginFont().
	\return TRUE if the font is reset succesfully.
*/
BOOL Font::EndFont()
{
	if(m_Font == NULL) {
		return FALSE;
	}
	if(FAILED(m_Font->End())) {
		return FALSE;
	}
	return TRUE;
}

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

BOOL Font::Print(char *Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format)
{
	RECT Rect;

	if(m_Font == NULL) {
		return FALSE;
	}

	Rect.left   = XPos;
	Rect.top    = YPos;
	Rect.right  = Rect.left + Width;
	Rect.bottom = Rect.top + Height;
	if(FAILED(m_Font->DrawText(Text, -1, &Rect, Format, Color))) {
		return FALSE;
	}
	return TRUE;
}
	



