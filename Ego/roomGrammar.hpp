#ifndef _roomGrammar_hpp_included_
#define _roomGrammar_hpp_included_
#include <list>
#include <boost/spirit.hpp>
using namespace boost::spirit;
using namespace std;
#define ErrorT(x) MessageBox(NULL, x, "Error", MB_OK);
    
typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;

/// Structure which defines an animation sequence for a room script.
struct sAnimSequence {
	int animationNumber; ///< The number of the animation.
	int startFrame; ///< The index of the animation's start frame.
	int numFrames; ///< The length of the animation.
	std::string animationOption; ///< The animation control code (see Sprite::AnimationOption).
};

/// Structure which defines an object for a room script.
struct sObject {
	string fileName; ///< The graphic for this object.
	string name; ///< The name of this object.
	string descriptor; ///< The descriptor for this object.
	long width; ///< Width of this object's tiles.
	long height; ///< Height of this object's tiles.
	POINT loc; ///< starting location for this object.
	POINT walkCoord; ///< Coor
	int visible; ///< The visibility status of this object.
	int visibleFlag; ///< Used for if statements in room scripts.
	list<sAnimSequence> animList; ///< List of animations.
	int hasOnStepScript; ///< True if the object has an onStep script.
	list<string> useItemOnList; ///< List of object that execute a special script when used on this object.
	/// Default constructor: set visibility to not visible.
	sObject() { visible = 0; visibleFlag = -1; hasOnStepScript = 0; useItemOnList.clear(); }
};
/// Structure which defines exits for a room script.
struct sExit {
	int roomNumber; ///< Number that this exit point to.
	RECT loc; ///< Location rectangle of this exit.
};

/// Structure which defines a room for a room script.
struct sRoom {
	string bgFileName; ///< The background file for this room.
	string colMapFileName; ///< The collision map for this room.
	string roomName; ///< Name of the room.
	int hasOnEnterScript; ///< True if the room has an onEnter script.
	list<sObject> objectList; ///< A list of objects in this room.
	list<sExit> exitList; ///< A list of exits in this room.
	sRoom() { hasOnEnterScript = 0; }
};

/// Functor to add an animation to a room grammar object.
struct AddAnimation
{
    AddAnimation(sObject& object_, sAnimSequence& anim_)
    : object(object_), anim(anim_) {}

    void operator()(iterator_t const, iterator_t const) const
	{
		object.animList.push_back(anim);
    }

    sObject &object;
	sAnimSequence &anim;
};

/// Functor to add an object to a room grammar object.
struct AddObject
{
    AddObject(sRoom& room_, sObject& object_)
    : room(room_), object(object_) {}

    void operator()(iterator_t const, iterator_t const) const
    {
		room.objectList.push_back(object);
		object.visibleFlag = -1;
		object.animList.clear();
		object.useItemOnList.clear();
    }

    sObject &object;
	sRoom &room;
};

/// Functor to add an exit to a room grammar object.
struct AddExit
{
    AddExit(sRoom& room_, sExit& exit_)
    : room(room_), exit(exit_) {}
	
    void operator()(iterator_t const, iterator_t const) const
    {
		room.exitList.push_back(exit);
    }

    sExit &exit;
	sRoom &room;
};

/// Functor to add an use item string to a room grammar object.
struct AddUseItem
{
	AddUseItem(sObject& object_)
		: object(object_) {}
	void operator()(iterator_t const begin, iterator_t const end) const
	{
		object.useItemOnList.push_back(string(begin, end));
	}
	sObject &object;
};

/// Grammar to parse room scripts.
struct RoomGrammar : public boost::spirit::grammar<RoomGrammar> {
	RoomGrammar(sRoom &room_) :
		room(room_) {}

		template <typename ScannerT>
		struct definition {
			rule<ScannerT> Room, FileName, CollisionMap, RoomName, 
						   Object, Exit, Coordinates, WalkCoordinates, Rect, 
						   AnimationSequence, WidthHeight, UseItemString;
			sObject tempObject;
			sExit tempExit;
			sAnimSequence tempAnimation;
			/// Structure which defines the grammar of an AdventureScript.
			definition(RoomGrammar const& self) {

				Room = *(space_p) >> FileName[assign_a(self.room.bgFileName)] >> CollisionMap[assign_a(self.room.colMapFileName)] >> RoomName
					>> (real_p[assign_a(self.room.hasOnEnterScript)] | *(space_p)) >> *(Object[AddObject(self.room, tempObject)]) >> *(Exit[AddExit(self.room, tempExit)]) >> *(space_p);
				FileName = +(anychar_p - '.') >> str_p(".png");
				CollisionMap = +(anychar_p - '.') >> str_p(".col");
				RoomName = lexeme_d[+(anychar_p - ';')][assign_a(self.room.roomName)] >> ch_p(';');
				Object = '{' >> FileName[assign_a(tempObject.fileName)] >> lexeme_d[+(anychar_p - ';')][assign_a(tempObject.name)] >> ';' >> lexeme_d[+(anychar_p - ';')][assign_a(tempObject.descriptor)] >> ';' 
						>> Coordinates >> WidthHeight >> WalkCoordinates >> ((real_p[assign_a(tempObject.visible)]) | (str_p("if(") >> real_p[assign_a(tempObject.visibleFlag)] >> ch_p(')') 
						>> real_p[assign_a(tempObject.visible)])) >> (real_p[assign_a(tempObject.hasOnStepScript)] | *(space_p)) >> *(UseItemString) >> *(AnimationSequence[AddAnimation(tempObject, tempAnimation)]) >> '}';
				Exit = '{' >> Rect >> ',' >> real_p[assign_a(tempExit.roomNumber)] >>  '}';
				Rect = real_p[assign_a(tempExit.loc.left)] >> ',' >> real_p[assign_a(tempExit.loc.top)] >> ',' >> real_p[assign_a(tempExit.loc.right)] >> ',' >> real_p[assign_a(tempExit.loc.bottom)];
				Coordinates = real_p[assign_a(tempObject.loc.x)] >> ',' >> real_p[assign_a(tempObject.loc.y)];
				WidthHeight = real_p[assign_a(tempObject.width)] >> ',' >> real_p[assign_a(tempObject.height)];
				WalkCoordinates = real_p[assign_a(tempObject.walkCoord.x)] >> ',' >> real_p[assign_a(tempObject.walkCoord.y)];
				UseItemString = (+alnum_p)[AddUseItem(tempObject)] >> ch_p(';');
				AnimationSequence = '{' >> 
					real_p[assign_a(tempAnimation.animationNumber)] >> ',' >>
					real_p[assign_a(tempAnimation.startFrame)] >> ',' >>
					real_p[assign_a(tempAnimation.numFrames)] >> ',' >>
					(str_p("LOOP_ANIMATION") | str_p("GOTO_NEXT_ANIMATION") |
					str_p("MAINTAIN_LAST_FRAME") | str_p("KILL_SPRITE") | str_p("GOTO_DEFAULT_ANIMATION"))[assign_a(tempAnimation.animationOption)]
				>> '}';
			}
			
			rule<ScannerT> const& start() const {
				return Room;
			}
		};
		sRoom &room;
	};
#endif