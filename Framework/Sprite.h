#pragma once

const int TOTAL_ANIMATIONS = 64; // Max number of animation states per sprite

#include <list>
#include <string>
#include <sstream>
#include "PhysicsCore.h"

class Tile;
/// All rendered objects extend from the Sprite base class.
/** The Sprite class handles all rendering of any game object.  This includes
	animations, scale, and position.
*/
//class PhysicalEntity;
class Sprite : public PhysicalEntity {
		
	protected:
		long			m_Width; ///< Width of the sprite tile.
		long			m_Height; ///< Height of the sprite tile.
		Vector2			m_Location;
		bool			m_visible;
		RECT			m_OriginalRect; ///< Original position, width and height of the sprite
		Tile			*m_Tiles; ///< Pointer to the tile set that this sprite uses
		char			m_TileNum; ///< Current tile number in the texture.
		char			m_TextureNum; ///< Texture number from the Tile.
		float			m_XScale; ///< X-scaling factor. 1.0f is no scaling.
		float			 m_YScale; ///< Y-scaling factor. 1.0f is no scaling.
		int				m_curFrame; ///< Current frame of animation.
		int				m_curAnimation; ///< Current animation.
		int				m_numAnimations; ///< Total number of animations.
		int				m_Animations[TOTAL_ANIMATIONS][3]; ///< Array where all animations are stored.  
		/**< [][0] = starting Frame, [][1] = number of Frames, [][2] = end control code (see enum AnimationOption) */
		bool			m_autoAnimate; ///< true if the sprite should animate itself.
		int				m_defaultAnimation; 
		///< Animation that the sprite will return to on an AnimationOption code of GOTO_DEFAULT_ANIMATION.

	public:
		
		/// enum for animation control codes.
		enum AnimationOption
		{
			LOOP_ANIMATION = 200, ///< returns to the first frame of animation.  Loops infinitely.
			GOTO_NEXT_ANIMATION, ///< Goes to the next animation.  Should not be set as the control code of the last animation.
			MAINTAIN_LAST_FRAME, ///< Maintains the last frame of animation.
			KILL_SPRITE, ///< When the last frame is reached, the sprite is destroyed.
			GOTO_DEFAULT_ANIMATION ///< Goes to the default animation (animation #0)
		};

		// Constructor/Destructor.
		Sprite();
		~Sprite();

		// Frees a sprite's memory.
		void Free();
		
		//Specifies which tileset to use
		bool UseTiles(Tile *Tiles, char TextureNum); 

		// Returns the tile set used by this Sprite.
		Tile* GetTiles() const;

		// Returns the texture number used by this sprite.
		char GetTextureNum() const;

		// Creates a renderable sprite.
		void Create(char TileNum, std::string name, float Xpos, float Ypos);

		virtual void Collide(PhysicalEntity *other);

		// Checks if this Sprite is an entity of type e.
		virtual bool checkType(EntityType e) const;
		
		// Creates a new animation sequence.
		bool CreateAnimationSequence(int animationNumber, u32 startFrame, u32 numFrames, AnimationOption nOption);
		
		// Sets the current animation of the sprite.
		bool SetAnimation(int animationNumber);

		// Controls animation.
		bool incFrame();
		bool decFrame();

		// Performs animation/motion.
		virtual void Update();

		// Renders the sprite.
		bool Render() const;
		
		// Setters.
		void SetDefaultAnimation(int animationNumber);
		void SetXScale(float Scale);
		void SetYScale(float Scale);
		void SetXYPos(float x, float y);
		void SetAutoAnimate(bool autoA);	
		void SetVisible(bool v);

		void Translate(float x, float y);

		// Getters.		
		int GetDefaultAnimation() const;
		float GetXPos() const;
		float GetYPos() const;
		long GetWidth() const;
		long GetHeight() const;
		float GetXScale() const;
		float GetYScale() const;

		RECT GetRect() const;
		int GetNumAnimationFrames(int animationNumber) const;
		bool GetIsVisible() const;
		bool GetAutoAnimate() const;
		int GetCurAnimation() const;
		int GetCurFrame() const;
};