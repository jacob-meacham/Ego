#pragma once
#include <list>
#include "Ego.h"
#include "Exit.h"
#include "GameArea.h"

class DataPackage;

//class Inventory;
/// Main area of the game
/** This adventure game is made up of a series of rooms.  As such, Room is the most important class.
	It handles movement of the main character, scripts, and interation.
*/
class Room : public GameArea {
	protected:
		std::string					roomName; ///< Name of the room.		
		Ego *						pEgo; ///< Main character instance.

		bool						hasEnterScript; ///< True if the room has an enter script.

		std::list<Exit>				exitList; ///< List of all exits from the room.
		bool						activeExit; ///< true if there is an exit that is active.
		int							activeRoomNumber; ///< the room number of the active exit (if any).

		const DataPackage *			dpCollision;
		char *						collisionMap; ///< pointer to the collision map data.
		
		float						zeroScale; ///< Scale that actors will be if they are at the bottom of the screen.
		float						scalingFactor; ///< Factor to scale back as the actors move away from zero scale.

	public:
		// Constructor.
		Room();
		~Room();

		// Sets the rooms Ego and font.
		void Init(Ego * pego, const Font *font);

		// Enters the room with the roomName and collision map.
		void EnterRoom(const DataPackage * dp_collision, const std::string & roomName);
		
		// Leaves this room.	
		bool LeaveRoom();

		// Adds an exit.
		void AddExit(const RECT & location, int rmNumber);

		// Removes an exit.
		bool RemoveExit(int number);

		// Relays input to the room for processing.
		void QueryRoom(long mouseX, long mouseY, bool lClick);

		// Updates the room.
		bool Update();
	    
		// Finds an object within the room.
		Object * FindObject(const std::string & objectName);
		Ego * GetEgo();

		// Renders the room and everything within.
		void RenderRoom() const;
		
		const std::string & GetName() const;
		bool GetExit() const;
		int GetExitNum() const;
		bool GetHasEnterScript() const;

		void SetExit(bool active, int number = -1);
		void SetHasEnterScript(bool d);
		void SetScaling(float zeroScale, float scalingFactor);
		
};
