#include "Sprite.h"
#include <iostream>
#include <sstream>
#include "Texture.h"
//////////////////////////////////////////////////////////////////////////////////
/// Default constructor.
Sprite::Sprite() {
	m_Tiles = NULL;
}
//////////////////////////////////////////////////////////////////////////////////
/// Default destructor.
Sprite::~Sprite() {
	
}
//////////////////////////////////////////////////////////////////////////////////
/// Specifies the Tile set to use, and which Texture within the tileset to use.
/** Example usage:
	m_Tiles.Create(&m_Graphics, 1);
	m_Tiles.Load(0, "Data\\Ego.png", 180, 240);
	m_Ego.UseTiles(&m_Tiles, 0);
	\return TRUE if the tileset is not null
*/
bool Sprite::UseTiles(Tile *Tiles, char TextureNum) {
	m_Tiles = Tiles;
	m_TextureNum = TextureNum;
	if(m_Tiles == NULL) return false;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the tile set used by this Sprite.
Tile* Sprite::GetTiles() const
{ 
	return m_Tiles; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the texture number used by this sprite.
char Sprite::GetTextureNum() const
{ 
	return m_TextureNum; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Creates a renderable sprite.
/** Create should be called only after UseTiles().
	\param TileNum Starting tile number to use (generally 0).
	\param name Name of the sprite (should be unique for parsing purposes)
	\param XPos X-Coordinate of the sprite.
	\param YPos Y-Coordinate of the sprite.
*/
void Sprite::Create(char TileNum, const std::string & name, float XPos, float YPos) {

	RECT BoundingRect;
	m_Width = m_Tiles->GetWidth(m_TextureNum);
	m_Height = m_Tiles->GetHeight(m_TextureNum);
	BoundingRect.left = 0;
	BoundingRect.right = m_Width;
	BoundingRect.top = 0;
	BoundingRect.bottom = m_Height;

	m_Name = name;
	m_Location.x = XPos;
	m_Location.y = YPos;
	m_curAnimation = 0;
	m_curFrame = 0;
	m_TileNum = TileNum;
	m_visible = true;
	m_OriginalRect = BoundingRect;
	m_XScale = 1.0000f;
	m_YScale = 1.0000f;
	m_autoAnimate = false;
	m_defaultAnimation = 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the current animation of the sprite.
/** \return FALSE if animationNumber is greater than the current number of animations. */
bool Sprite::SetAnimation(u32 animationNumber) {
	if(animationNumber >= m_Animations.size()) {
		m_curAnimation = m_defaultAnimation;
		return true;
	}
	m_curAnimation = animationNumber;
	m_curFrame = m_Animations[m_curAnimation].startingFrame;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the default animation of the sprite.
void Sprite::SetDefaultAnimation(int animationNumber) {
	m_defaultAnimation = animationNumber;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the default animation of the sprite.
int Sprite::GetDefaultAnimation() const {
	return m_defaultAnimation;
}
//////////////////////////////////////////////////////////////////////////////////
/// Creates a new animation sequence.
/** \param animationNumber The numberID of the animation.
	\param startFrame The starting frame in the Sprite's tileset.
	\param endFrame The ending frame in the Sprite's tileset.
	\param nOption Animation option (see enum AnimationOption ).
*/
bool Sprite::CreateAnimationSequence(u32 animationNumber, u32 startFrame, u32 endFrame, AnimationOption nOption) {
	if(startFrame > m_Tiles->GetNum(m_TextureNum)) return false;

	m_Animations.resize(animationNumber+1);
	AnimationRec rec(startFrame, endFrame, nOption);
	m_Animations[animationNumber] = rec;

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets m_autoAnimate to autoA.
void Sprite::SetAutoAnimate(bool autoA) {
	m_autoAnimate = autoA;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the state of m_autoAnimate.
bool Sprite::GetAutoAnimate() const {
	return m_autoAnimate;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the current animation number ID.
int Sprite::GetCurAnimation() const {
	return m_curAnimation;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the current frame in the current animation.
int Sprite::GetCurFrame() const {
	return m_curFrame;
}
//////////////////////////////////////////////////////////////////////////////////
/// Increments the current frame, and deals with any animation control codes
bool Sprite::incFrame() {
	m_curFrame++;
	// m_Animations[m_curAnimation][1] is number of total frames.  If we are equal to that,
	// then we should process the control code.
	if(m_curFrame >= m_Animations[m_curAnimation].endFrame) {
		switch(m_Animations[m_curAnimation].endOption) {
			case LOOP_ANIMATION:
				// simply set the curFrame back to the start frame.
				m_curFrame = m_Animations[m_curAnimation].startingFrame;
				break;
			case GOTO_NEXT_ANIMATION:
				// increment the current animation
				m_curAnimation++;
				// return false if there is no next animation.
				if(m_curAnimation > m_Animations.size()) return FALSE;
				// Set the current frame to the start frame of the (now) current animation
				m_curFrame = m_Animations[m_curAnimation].startingFrame;
				break;
			case MAINTAIN_LAST_FRAME:
				// set auto animte to false.
				SetAutoAnimate(false);
				break;
			case KILL_SPRITE:
				SetVisible(false);
				break;
			case GOTO_DEFAULT_ANIMATION:
				m_curFrame = m_Animations[m_defaultAnimation].startingFrame;
				break;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Decrements the current frame.
/** \return TRUE if the decrement is legal. */
bool Sprite::decFrame() {
	// if decrementing the frame would put the current frame before the starting frame,
	// return false.
	if(m_curFrame-1 < m_Animations[m_curAnimation].startingFrame) return false;
	m_curFrame--;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Updates the sprite (performs animation).
void Sprite::Update() {
	if(!m_visible) {
		return;
	}
	if(GetAutoAnimate()) { 
		incFrame();
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Renders the sprite.
/** If the sprite is invisible or dead, it is not rendered.
	\return TRUE if rendering is possible, or if the sprite is dead or invisible.
*/
bool Sprite::Render() const {
	if(m_Tiles == NULL) { TRACE("in Sprite::Render, tileset is Null!"); return false; }
	if(!m_visible) { return true; }

	// Simply uses class Tile's draw function, and draws the current frame
	// at the current scale and x,y coordinates.
	m_Tiles->Draw(m_TextureNum, m_curFrame, m_Location.x, m_Location.y, 0xFFFFFFFF, m_XScale, m_YScale);
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the X-position of the sprite.
float Sprite::GetXPos() const {
	return m_Location.x;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the Y-position of the sprite.
float Sprite::GetYPos() const {
	return m_Location.y;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the width of the sprite's tiles.
long Sprite::GetWidth() const {
	return m_Width;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the height of the sprite's tiles.
long Sprite::GetHeight() const {
	return m_Height;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the X, Y position to x, and y.
void Sprite::SetXYPos(float x, float y) {
	m_Location.x = x;
	m_Location.y = y;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the current X-Scale of this Sprite.
float Sprite::GetXScale() const {
	return m_XScale;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the current Y-Scale of this Sprite.
float Sprite::GetYScale() const {
	return m_YScale;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the current X-Scale to Scale.
/** The scale is absolute, not cumulative.
	SetXScale(0.5f) followed by SetXScale(0.5f), sets the absolute scale to 0.5 the original.
*/
void Sprite::SetXScale(float Scale) {
	float new_width = m_OriginalRect.right * Scale;
	m_Width = (int)new_width;
	m_XScale = Scale;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the current Y-Scale to Scale.
/** The scale is absolute, not cumulative.
	SetYScale(0.5f) followed by SetYScale(0.5f), sets the absolute scale to 0.5 the original.
*/
void Sprite::SetYScale(float Scale) {
	float new_height = m_OriginalRect.bottom * Scale;
	m_Height = (int)new_height;
	m_YScale = Scale;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the sprite's current AABB.  
/** This funtion is preferred over GetBoundingRect(). */
RECT Sprite::GetRect() const {
	RECT dst;
	dst.top = (long)m_Location.y;
	dst.left = (long)m_Location.x;
	dst.bottom = dst.top + m_Height;
	dst.right = dst.left + m_Width;
return dst;
}
//////////////////////////////////////////////////////////////////////////////////
/// Return the number of animation frames in the animation sequence animationNumber.
int Sprite::GetNumAnimationFrames(int animationNumber) const {
	return m_Animations[animationNumber].startingFrame - m_Animations[animationNumber].endFrame;
}
//////////////////////////////////////////////////////////////////////////////////
/// Return the visibility status of the sprite.
bool Sprite::GetIsVisible() const {
	return m_visible; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Set the visibility status of the sprite to v.
void Sprite::SetVisible(bool v) { 
	m_visible = v; 
}
//////////////////////////////////////////////////////////////////////////////////
void Sprite::Translate(float x, float y) {
	m_Location.x += x;
	m_Location.y += y;
}