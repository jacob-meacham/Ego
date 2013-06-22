#pragma once

#include "Framework\Sprite.h"
#include <list>

enum ActionType {
	IS_NOACTION, 
	IS_WALK, 
	IS_LOOK, 
	IS_USE, 
	IS_USEITEM, 
	IS_TALK
};

enum FlagType {
	PER_CONVERSATION, 
	PER_ROOM, 
	PERMANENT
};

class Parser;
/// Any game object.
/** An object is any interactive game object that is primarily stationary (that is, does not need pathfinding).
	As with a Sprite, an Object is fully animatable.  Further, an Object has the ability to parse scripts
	based on an action done to it.  Finally, an object has 40 flags which may be set by script, and which can
	affect script execution through if() statements (See Parser for more information).
*/
class Object : public Sprite {
	private:
		std::string		strDescriptor; ///< A string describing the object.
		int				scriptFlags[40]; ///< Flags for scripting.

		POINT			walkCoordinates; ///< Coordinates that an actor should walk to if she/he wants to see this object.
		D3DCOLOR		textColor; ///< The text color of all conversation strings attached to this object.
		int				curTalkAnimation; ///< Talking animation for this object, default -1 (no talking animation).

		bool			hasOnStepScript; ///< true if the object has a onEnter script.
		bool			isEgoIn; ///< true if Ego is within the bounds of the object.
		std::list<std::string> useItemsOn; ///< List of objects that execute a special script when used upon this object.
	public:
		

		Object();
		virtual ~Object();
		
		// Executes the specified action with the parser.	
		void DoAction(ActionType action, Parser *p);

		// Adds an item that can be used upon this one.
		void AddUseItem(const std::string & itemName);
		
		// Ego has used a held item on this object - process it.
		void UseItem(const std::string & item, Parser *p);

		// Executes the current talking animation of this object.
		void DoTalkingAnimation();
		
		int GetFlag(int index) const;
		D3DCOLOR GetTextColor() const;
		const std::string & GetDescriptor() const;
		int GetCurTalkingAnimation() const;
		bool GetEgoIn() const;
		bool HasOnStep() const;
		long GetXWalkCoordinate() const;
		long GetYWalkCoordinate() const;
		
		// Checks if this object has collided with RECT a.
		bool CheckCollisions(const RECT & a) const;

		// Checks if the mouse is hovering over this object.
		bool CheckMouseCollision(long MouseX, long MouseY) const;

		void SetFlag(int index, int d);
		void SetDescriptor(const std::string & descriptor);
		void SetCurTalkingAnimation(int animationNumber);
		void SetTextColor(D3DCOLOR color);
		void SetHasOnStep(bool d);
		void SetEgoIn(bool d);
		void SetXYWalkCoordinates(long x, long y);
};