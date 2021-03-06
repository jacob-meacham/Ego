#pragma once

#include <string>
#include <windows.h>
#include <vector>
#include "Math.h"

class Tile;
/// All rendered objects extend from the Sprite base class.
/** The Sprite class handles all rendering of any game object.  This includes
	animations, scale, and position.
*/
class Sprite {
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

	protected:
		struct AnimationRec {
			int startingFrame;
			int endFrame;
			AnimationOption endOption;

			AnimationRec() : 
				startingFrame(0), endFrame(0), endOption(MAINTAIN_LAST_FRAME) {
			}

			AnimationRec(int start, int end, AnimationOption option) : 
				startingFrame(start), endFrame(end), endOption(option) {
			}
		};

		std::string		m_Name; ///< Base name of the entity, through which it is known.

		long			m_Width; ///< Width of the sprite tile.
		long			m_Height; ///< Height of the sprite tile.
		RECT			m_OriginalRect; ///< Original position, width and height of the sprite
		Vector2			m_Location;

		float			m_XScale; ///< X-scaling factor. 1.0f is no scaling.
		float			m_YScale; ///< Y-scaling factor. 1.0f is no scaling
						
		Tile			*m_Tiles; ///< Pointer to the tile set that this sprite uses
		char			m_TileNum; ///< Current tile number in the texture.
		char			m_TextureNum; ///< Texture number from the Tile.
		
		bool			m_visible;
		int				m_curFrame; ///< Current frame of animation.
		u32				m_curAnimation; ///< Current animation.
		std::vector<AnimationRec> m_Animations; ///< Array where all animations are stored.  
		bool			m_autoAnimate; ///< true if the sprite should animate itself.
		int				m_defaultAnimation; 
		///< Animation that the sprite will return to on an AnimationOption code of GOTO_DEFAULT_ANIMATION.

	public:
		// Constructor/Destructor.
		Sprite();
		virtual ~Sprite();
		
		//Specifies which tileset to use
		bool UseTiles(Tile *Tiles, char TextureNum); 

		// Returns the tile set used by this Sprite.
		Tile* GetTiles() const;

		// Returns the texture number used by this sprite.
		char GetTextureNum() const;

		// Creates a renderable sprite.
		void Create(char TileNum, const std::string & name, float Xpos, float Ypos);
		
		// Creates a new animation sequence.
		bool CreateAnimationSequence(u32 animationNumber, u32 startFrame, u32 numFrames, AnimationOption nOption);
		
		// Sets the current animation of the sprite.
		bool SetAnimation(u32 animationNumber);

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
		
		void SetName(const std::string & name) { m_Name = name; }
		const std::string & GetName() const { return m_Name; }
};