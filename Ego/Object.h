#ifndef _object_h_included_
#define _object_h_included_
#include "Framework\Sprite.h"
//#include <boost/spirit/iterator/file_iterator.hpp>
//using namespace boost::spirit;
class Parser;
enum ActionType {IS_NOACTION, IS_WALK, IS_LOOK, IS_USE, IS_USEITEM, IS_TALK};
enum FlagType {PER_CONVERSATION, PER_ROOM, PERMANENT};
//typedef char                    char_t;
//typedef file_iterator <char_t>  iterator_t;
/// Any game object.
/** An object is any interactive game object that is primarily stationary (that is, does not need pathfinding).
	As with a Sprite, an Object is fully animatable.  Further, an Object has the ability to parse scripts
	based on an action done to it.  Finally, an object has 40 flags which may be set by script, and which can
	affect script execution through if() statements (See Parser for more information).
*/
class Object : public Sprite {
	private:
		std::string		m_descriptor; ///< A string describing the object.
		
		D3DCOLOR		m_textColor; ///< The text color of all conversation strings attached to this object.
		int				m_flags[40]; ///< Flags for scripting.
		bool			m_hasOnStepScript; ///< true if the object has a onEnter script.
		bool			m_isEgoIn; ///< true if Ego is within the bounds of the object.
		POINT			m_walkCoordinates; ///< Coordinates that an actor should walk to if she/he wants to see this object.
		int				m_curTalkAnimation; ///< Talking animation for this object, default -1 (no talking animation).

	public:
		std::list<std::string> m_useItemsOn; ///< List of objects that execute a special script when used upon this object.
		// Constructor/Destructor.
		Object();
		~Object();
		
		// Executes the specified action with the parser.	
		void DoAction(ActionType action, Parser *p);

		// Adds an item that can be used upon this one.
		void AddUseItem(std::string itemName);
		
		// Ego has used a held item on this object - process it.
		void UseItem(std::string item, Parser *p);

		// Executes the current talking animation of this object.
		void DoTalkingAnimation();
		
		// Getters. 
		int GetFlag(int index);
		D3DCOLOR GetTextColor();
		std::string GetDescriptor();
		int GetCurTalkingAnimation();
		bool GetEgoIn();
		bool HasOnStep();
		long GetXWalkCoordinate();
		long GetYWalkCoordinate();
		
		// Checks if this object has collided with RECT a.
		bool CheckCollisions(RECT a);

		// Checks if the mouse is hovering over this object.
		bool CheckMouseCollision(long MouseX, long MouseY);

		// Setters
		void SetFlag(int index, int d);
		void SetDescriptor(std::string descriptor);
		void SetCurTalkingAnimation(int animationNumber);
		void SetTextColor(D3DCOLOR color);
		void SetHasOnStep(bool d);
		void SetEgoIn(bool d);
		void SetXYWalkCoordinates(long x, long y);
};
#endif