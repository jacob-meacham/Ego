#include "Texture.h"
//////////////////////////////////////////////////////////////////////////////////
Texture::Texture() {
	nWidth = nHeight = 0;
	pTexture = NULL;
}
//////////////////////////////////////////////////////////////////////////////////
bool Texture::free() 
{
	DX_RELEASE(pTexture);
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Loads a texture into m_Texture from file.
/** \param Graphics The GraphicsCore to which this texture is attached.
	\param FileName Name of the image file to use.
	\return TRUE if the file is loaded properly.
*/
bool Texture::load(const char * pfilename)
{
	if(!pfilename)
		return false;

	if(FAILED(D3DXCreateTextureFromFileEx(
							gGraphics.getDeviceCOM(), 
							pfilename, 
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
							&pTexture))) 
	{
		TRACE("Could not create texture, %s\n", pfilename);
		ASSERT_FAILED;

		return false; 
	}
	
	nWidth = width();	
	nHeight = height();
		
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter which calculates the width of the texture, using D3DSURFACE_DESC.
u32 Texture::width() const
{
  D3DSURFACE_DESC d3dsd;

  if(!pTexture)   
     return 0;

  if(FAILED(pTexture->GetLevelDesc(0, &d3dsd))) 
    return 0;

  return d3dsd.Width;
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter which calculates the height of the texture, using D3DSURFACE_DESC.
u32 Texture::height() const
{
  D3DSURFACE_DESC d3dsd;

  if(!pTexture) {
    return 0;
  }

  if(FAILED(pTexture->GetLevelDesc(0, &d3dsd))) {
    return 0;
  }

  return d3dsd.Height;
}
//////////////////////////////////////////////////////////////////////////////////
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
bool Texture::draw(float destx, float desty, 
				  long srcx, long srcy, 
				  long width, long height, 
				  float scalex, float scaley,
				  Color color, const Vector2 * pcenter,
				  float angle) const
{
  if(!pTexture) 
	return false;

  ID3DXSprite *psprite = gGraphics.getSpriteCOM();
  if(!psprite)
	  return false;

  // Set the source rectangle.
  RECT Rect;
  Rect.left = srcx;
  Rect.top  = srcy;
  Rect.right = Rect.left + width;
  Rect.bottom = Rect.top + height;
  
  D3DXMATRIX transform;
  D3DXMatrixTransformation2D(&transform, &D3DXVECTOR2(0, 0), NULL,
						&D3DXVECTOR2(scalex, scaley), pcenter, angle, &D3DXVECTOR2(destx, desty));
  
  psprite->SetTransform(&transform); 
  if(FAILED(psprite->Draw(pTexture, &Rect, NULL, NULL, color)))
			return false;

  return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Default Constructor.
Tile::Tile() {
	aWidths = aHeights = aColumns = NULL;
	pTextures = NULL;
	nTextures = 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Default Destructor.
Tile::~Tile() {
	Free();
}
//////////////////////////////////////////////////////////////////////////////////
/// Create a Tileset, with the specified number of textures.
/** \param Graphics The parent GraphicsCore object.
	\param NumTextures The number of texture that this Tileset will have
	\return TRUE if the tileset is created successfully.
*/
bool Tile::Create(u32 NumTextures) 
{
	nTextures = NumTextures;
	
	pTextures = new Texture[nTextures];
	if(!pTextures)
		return false;

	// Create space for the width, height, and columns of m_NumTextures
	aWidths = new u32[nTextures];
	aHeights = new u32[nTextures];
	aColumns = new u32[nTextures];

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Frees a tileset.
void Tile::Free() {
	// Simply need to call Texture Free() for each texture.
	for(u32 i = 0; i < nTextures; i++)
		pTextures[i].free();

	// Delete pointers
	delete [] pTextures;
	delete [] aWidths;
	delete [] aHeights;
	delete [] aColumns;

	pTextures = NULL;
	aWidths = NULL;
	aHeights = NULL;
	aColumns = NULL;
	
	nTextures = 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Load a tileset from File into one of the texture slots created using Tile::Create().
/** \param TextureNum Index of the texture to be loaded
	\param Filename Filename of the texture to load
	\param TileWidth width of the tiles in FileName.
	\param TileHeight height of the tiles if FileName.
	NOTE: The tiles in any file are assumed to be the same size.
*/
bool Tile::Load(u32 TextureNum, const char *Filename, short TileWidth, short TileHeight)
{
	if(!pTextures)
		return false;

	if(TextureNum >= nTextures) {
		TRACE("Error in Tile::Load, only %i textures", nTextures);
		return false;
	}

	if(!pTextures[TextureNum].load(Filename))
		return false;

	aWidths[TextureNum] = TileWidth;
	aHeights[TextureNum] = TileHeight;

	// The columns are the width of the texture divided by the width of the tiles in the texture.
	aColumns[TextureNum] = pTextures[TextureNum].width() / aWidths[TextureNum];
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Free a texture from the tileset.
bool Tile::FreeTexture(u32 TextureNum) {
	if(TextureNum >= nTextures || !pTextures) return false;

	pTextures[TextureNum].free();
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Width of the WHOLE tileset in TextureNum.
u32 Tile::GetWidth(u32 TextureNum) const {
	if(TextureNum >= nTextures || !pTextures) return 0;

	return aWidths[TextureNum];
}
//////////////////////////////////////////////////////////////////////////////////
/// Height of the WHOLE tileset in TextureNum.
u32 Tile::GetHeight(u32 TextureNum) const {
	if(TextureNum >= nTextures || !pTextures) return 0;

	return aHeights[TextureNum];
}
//////////////////////////////////////////////////////////////////////////////////
/// Number of tiles in the tileset.
u32 Tile::GetNum(u32 TextureNum) const {
	if(TextureNum >= nTextures || !pTextures) return 0;

	return aColumns[TextureNum] * (pTextures[TextureNum].height() / aHeights[TextureNum]);
}
//////////////////////////////////////////////////////////////////////////////////
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
bool Tile::Draw(u32 TextureNum, u32 TileNum, float ScreenX,
				  float ScreenY, D3DCOLOR Color,
				  float XScale, float YScale,
				  float rotationCenterX, float rotationCenterY,
				  float angle) const {
	
	long SrcX, SrcY;

	// Make sure that the texture is not out of bounds.
	if(TextureNum >= nTextures || !pTextures) {
		return false;
	}

	// The source coordinates in the texture file is calculated by moving to
	// the correct column and then multiplying by either the width or the height.
	SrcX = (TileNum % aColumns[TextureNum]) * aWidths[TextureNum];
	SrcY = (TileNum / aColumns[TextureNum]) * aHeights[TextureNum];

	Vector2 rotationCenter;
	rotationCenter.x = rotationCenterX;
	rotationCenter.y = rotationCenterY;

	// Then, we can just use Texture::Blit().
	if(!pTextures[TextureNum].draw(ScreenX, ScreenY, SrcX, SrcY,
								   aWidths[TextureNum], aHeights[TextureNum],
								   XScale, YScale, Color, &rotationCenter, angle)) 
	{
		return false; 
	}
	return true;
}