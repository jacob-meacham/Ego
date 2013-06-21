#pragma once

#include "GraphicsDevice.h"
/////////////////////////////////////////////////////////////////////////
/// The main drawing class.
/** Texture is used to draw a single texture onto the screen, via the GraphicsDevice.   */
class Texture {
	protected:
		IDirect3DTexture9 *  pTexture; ///< Pointer to the DirectX texture object.
        u32 nWidth, nHeight; ///< Width and Height of the texture.

	public:
		Texture();
		~Texture() { free(); }

		// Loads a texture from a file.
		bool load(const char * pfilename);

		// Frees the Texture object.
		bool free();

		// Draws the texture to the screen.
		bool draw(float destx, float desty, 
				  long srcx, long srcy, 
				  long width, long height, 
				  float scalex, float scaley,
				  Color color, const Vector2 * pcenter = NULL,
				  float angle = 0.0f) const;
		
		u32 width() const;
		u32 height() const;
};
/////////////////////////////////////////////////////////////////////////
class Tile {
	protected:
		Texture  *  pTextures;	///< Pointer to the Textures stored in this tileset.
		
		u32			nTextures; ///< Number of textures in this instance.
		u32 *		aWidths;	///< Widths of the textures.
		u32 *		aHeights; ///< Heights of the textures.
		u32 *		aColumns; ///< Columns of textures.

	public:
		// Constructor/Destructor
		Tile();
		~Tile();

		// Creates a Tile with the specified number of textures.
		bool Create(u32 NumTextures);
		
		// Frees the Tile.
		void Free();

		// Loads a Texture from file into the specified texture number.
		bool Load(u32 TextureNum, const char *Filename, short TileWidth,
				  short TileHeight);

		// Frees the Texture at the specified index.
		bool FreeTexture(u32 TextureNum);

		// Getters.
		u32 GetWidth(u32 TextureNum) const;
		u32 GetHeight(u32 TextureNum) const;
		u32 GetNum(u32 TextureNum) const;

		// Setters.
		bool SetTransparent(bool Enabled);

		// Draws the specified texture number at the specified screen coordinates.
		bool Draw(u32 TextureNum, u32 TileNum, float ScreenX,
				  float ScreenY, D3DCOLOR Color = 0xFFFFFFFF,
				  float XScale = 1.0f, float YScale = 1.0f,
				  float rotationCenterX = 0.0f, float rotationCenterY = 0.0f,
				  float angle = 0.0f) const;
};