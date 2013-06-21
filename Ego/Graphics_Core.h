#ifndef _graphicsCore_h_included_
#define _graphicsCore_h_included_
#include "DXUtil.h"
/// Main DirectX graphic class.
/** This class is used to set the directX devices, as well as to render
frames.  The class is based off of code found in Jim Adams'
Programming Role Playing Games with DirectX.
*/
class GraphicsCore {
	protected:
		HWND					m_hWnd; ///< Handle to rendering window.
		ID3DXSprite				*m_Sprite; ///< Sprite Interface.
		IDirect3D8				*m_d3d; ///< Main DirectX Interface.
		IDirect3DDevice8		*m_d3dDevice; ///< Hardware device Interface.
		D3DDISPLAYMODE			m_d3dDisplay; ///< DirectX Display structure.
		D3DPRESENT_PARAMETERS	m_d3dpp; ///< DirectX Paramaters structure.
		BOOL					m_Windowed; ///< boolean for windowed mode.
		RECT					ClientRect; ///< Rectangle which defines the window.
		long					m_Width; ///< Window width.
		long					m_Height; ///< Window height.
		D3DFORMAT				m_BPP; ///< DirectX Format structure.
		BOOL					m_HAL; ///< boolean for hardware acceleration.


	public:
		// Constructor.
		GraphicsCore();
		
		// Initializes the DirectX interface.
		BOOL Initialize();

		// Sets the hardware graphics mode.
		BOOL SetGraphics(HWND hwnd, BOOL Windowed, long Width, long Height);

		// Determines the format is supported by the hardware.
		BOOL CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);

		// Destructor
		BOOL Shutdown();

		BOOL BeginScene();
		BOOL EndScene();

		BOOL BeginSprite();
		BOOL EndSprite();

		// Blit to screen.
		BOOL Display();

		// Clear backbuffer.
		BOOL Clear(long Color = D3DCOLOR_RGBA(0, 0, 192, 255), float ZBuffer = 1.0f);

		// Getters.
		IDirect3DDevice8 *GetDeviceCOM();
		ID3DXSprite *GetSpriteCOM();

};

/// The main drawing class.
/** Texture is used to draw a single texture onto the screen, via the GraphicsCore.  
The class is based off of code found in Jim Adams' Programming Role Playing Games with DirectX. */
class Texture {
	protected:
		GraphicsCore		*m_Graphics; ///< Pointer to the GraphicsCore object.
		IDirect3DTexture8   *m_Texture; ///< Pointer to the DirectX texture object.
        unsigned long        m_Width, m_Height; ///< Width and Height of the texture.

	public:

		// Constructor
		Texture();

		// Destructor.
		~Texture() { Free(); }

		// Loads a texture from a file.
		BOOL Load(GraphicsCore *Graphics, char *FileName);

		// Frees the Texture object.
		BOOL Free();

		// Draws the texture to the screen.
		BOOL Blit(long DestX, long DestY, 
					long SrcX, long SrcY, 
					long Width, long Height, 
					float XScale, float YScale, 
					D3DCOLOR Color);
		
		// Getters.
		long GetWidth();
		long GetHeight();
};

/// Creates a tilesets of Textures, useful for animation.
/** Stores a number of Texture objects and can render any of them.  
The class is based off of code found in Jim Adams' Programming Role Playing Games with DirectX.
*/
class Tile {
	protected:
		GraphicsCore *m_Graphics; ///< Pointer to the GraphicsCore object
		Texture   *m_Textures;	///< Pointer to the Textures stored in this tileset.
		
		long		  m_NumTextures; ///< Number of textures in this instance.
		long		 *m_Widths;	///< Widths of the textures.
		long		 *m_Heights; ///< Heights of the textures.
		long		 *m_Columns; ///< Columns of textures.

	public:
		// Constructor/Destructor
		Tile();
		~Tile();

		// Creates a Tile with the specified number of textures.
		BOOL Create(GraphicsCore *Graphics, long NumTextures);
		
		// Frees the Tile.
		BOOL Free();

		// Loads a Texture from file into the specified texture number.
		BOOL Load(long TextureNum, char *Filename, short TileWidth,
				  short TileHeight);

		// Frees the Texture at the specified index.
		BOOL FreeTexture(long TextureNum);

		// Getters.
		long GetWidth(long TextureNum);
		long GetHeight(long TextureNum);
		long GetNum(long TextureNum);

		// Setters.
		BOOL SetTransparent(BOOL Enabled);

		// Draws the specified texture number at the specified screen coordinates.
		BOOL Draw(long TextureNum, long TileNum, long ScreenX,
				  long ScreenY, D3DCOLOR Color = 0xFFFFFFFF,
				  float XScale = 1.0f, float YScale = 1.0f);
};

/// Class which wraps DirectX support for rendering fonts.
/** The class is based off of code found in Jim Adams'
Programming Role Playing Games with DirectX.
*/
class Font {
	private:
		ID3DXFont *m_Font; ///< DirectX Font interface.
	public:
		// Constructor/Destructor.
		Font();
		~Font();
		
		// Creates a font in the specified GraphicsCore.
		BOOL Create(GraphicsCore *Graphics, char *Name, long Size, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE);

		// Frees the font.
		BOOL Free();

		// Calls which surround printing the font to the screen.
		BOOL BeginFont();
		BOOL EndFont();

		// Prints the text in the font's specifications at the specified coordinates.
		BOOL Print(char *Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);

		// Getter.
		ID3DXFont* GetFontCOM();
};
#endif




