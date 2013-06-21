#ifndef _sprite_h_included_
#define _sprite_h_included_
const int TOTAL_ANIMATIONS = 64; // Max number of animation states per sprite

#include <windows.h>
#include <list>
#include <string>
#include <sstream>
#include "Graphics_Core.h"
#include "System_Core.h"

/// Default animations, useful for all objects, and required for all actors.
enum DefaultAnimations {
	DEFAULT_TALKING = 0,
	TALKING_RIGHT_DOWN,
	TALKING_RIGHT_UP,
	TALKING_LEFT_DOWN,
	TALKING_LEFT_UP,
	TALKING_BACK,
	TALKING_FRONT,
	STANDING_RIGHT_DOWN,
	STANDING_RIGHT_UP,
	STANDING_LEFT_DOWN,
	STANDING_LEFT_UP,	
	STANDING_BACK,
	STANDING_FRONT,
	WALKING_RIGHT,
	WALKING_LEFT,
	WALKING_BACK,
	WALKING_FRONT,
	PICKING_UP_RIGHT_DOWN,
	PICKING_UP_RIGHT_UP,
	PICKING_UP_LEFT_DOWN,
	PICKING_UP_LEFT_UP,
	PICKING_UP_BACK,
	PICKING_UP_FRONT,
 };

/// All rendered objects extend from the Sprite base class.
/** The Sprite class handles all rendering of any game object.  This includes
	animations, scale, and position.
*/
class Sprite {
		
	protected:
		std::string		m_Name; ///< Base name of the sprite, through which it is known
		bool			m_Dead; ///< true if the sprite is dead (will not be rendered or updated).
		bool			m_visible; ///< true if the sprite is visible (will not be rendered).
		POINT			m_Location; ///< (x,y) location of the Sprite.
		long			m_Width; ///< Width of the sprite tile.
		long			m_Height; ///< Height of the sprite tile.
		RECT			m_OriginalRect; ///< Original position, width and height of the sprite
		bool			m_Collision; ///< True if there is a sprite-sprite collision.
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

		/// Structure which represents current Sprite movement.
		/** SpriteMovement is a 2-d vector detailing the number of pixels the sprite moves in
			the x and y direction every frame.
		*/
		struct SpriteMovement {
			long m_x, m_y;
		} m_Movement;
		

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
		Tile* GetTiles();

		// Returns the texture number used by this sprite.
		char GetTextureNum();

		// Creates a renderable sprite.
		void Create(char TileNum, std::string name, long Xpos, long Ypos); //Creates a sprite at screen location of type Type
		
		
		// Creates a new animation sequence.
		bool CreateAnimationSequence(int animationNumber, int startFrame, int numFrames, AnimationOption nOption);
		
		// Sets the current animation of the sprite.
		bool SetAnimation(int animationNumber);

		// Controls animation.
		bool incFrame();
		bool decFrame();

		//Kills the sprite.
		void Kill(); 

		// True if the sprite is dead.
		bool IsDead();

		// Checks if this sprite has collided with RECT a.
		bool CheckCollisions(RECT a);

		// Checks if the mouse is hovering over this sprite.
		bool CheckMouseCollision(long MouseX, long MouseY);

		// Performs animation/motion.
		virtual void Update();	

		// Renders the sprite.
		bool Render();
		
		// Setters.
		void SetCollision(bool collision);
		void SetDefaultAnimation(int animationNumber);
		void SetXScale(float Scale);
		void SetYScale(float Scale);
		void SetXYPos(long x, long y);
		void SetAutoAnimate(bool autoA);	
		void SetName(std::string name);
		void SetVisible(bool v);

		// Getters.		
		int GetDefaultAnimation();
		long GetXPos();
		long GetYPos();
		long GetWidth();
		long GetHeight();
		std::string GetName();
		float GetXScale();
		float GetYScale();
		RECT GetBoundingRect(POINT &pt);
		RECT GetRect();
		int GetNumAnimationFrames(int animationNumber);
		bool GetIsVisible();
		bool GetAutoAnimate();
		int GetCurAnimation();
		int GetCurFrame();
};
#endif