#ifndef _room_h_included_
#define _room_h_included_
#include <list>
#include "ConversationList.h"
#include "Ego.h"
#include "Exit.h"
#include "TextBox.h"
#include "GameArea.h"


#define MAX_CHOICES 10;
//class Inventory;
/// Main area of the game
/** This adventure game is made up of a series of rooms.  As such, Room is the most important class.
	It handles movement of the main character, scripts, and interation.
*/
class Room : public GameArea {
	protected:
		std::string					m_roomName; ///< Name of the room.
		std::list<Exit>				m_ExitList; ///< List of all exits from the room.
		std::list<Exit>::iterator	iExit; ///< standard iterator for the list of exits.
		Ego							m_Ego; ///< Main character instance.
		char*						m_collisionMap; ///< pointer to the collision map data.
		bool						m_ActiveExit; ///< true if there is an exit that is active.
		int							m_ActiveRoomNumber; ///< the room number of the active exit (if any).
		DataPackage					m_DPVariables; ///< DataPackage to load/save all object and room flags.
		int*						m_vars; ///< int* for use with m_DPVariables.
		bool						m_hasEnterScript; ///< True if the room has an enter script.
		float						m_zeroScale; ///< Scale that actors will be if they are at the bottom of the screen.
		float						m_scalingFactor; ///< Factor to scale back as the actors move away from zero scale.

	public:
		// Constructor.
		Room();

		// Sets the rooms Ego and font.
		void Set(Ego Dude, Font *font);

		// Enters the room with the roomName and collision map.
		void EnterRoom(char *cm, std::string roomName);
		
		// Leaves this room.	
		bool LeaveRoom();

		// Adds an exit.
		void AddExit(RECT location, int rmNumber);

		// Removes an exit.
		bool RemoveExit(int number);

		// Relays input to the room for processing.
		void QueryRoom(long mouseX, long mouseY, bool lClick);

		// Updates the room.
		bool Update();
	    
		// Finds an object within the room.
		Object* FindObject(std::string objectName);

		// Renders the room and everything within.
		void RenderRoom();
		

		// Getters.
		Inventory* GetInventory();
		std::string GetName();
		int GetExitNum();
		Ego* GetEgo();
		bool GetExit();
		bool GetHasEnterScript();

		// Setters.
		void SetExit(bool active, int number = -1);
		void SetHasEnterScript(bool d);
		void SetScaling(float zeroScale, float scalingFactor);
		
};
#endif
