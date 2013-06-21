#ifndef _winMain_h_included_
#define _winMain_h_included_
#include <iostream>
#include <sstream>
#include <string>
#include "Framework\DXUtil.h"
#include "Framework\GraphicsDevice.h"
#include "Framework\System.h"
#include "Framework\Input.h"
#include "Room.h"
#include "roomGrammar.h"
#include "Inventory.h"
#define LEAN_AND_MEAN
#define MAX_ITEMS 15
using namespace boost::spirit;

 typedef char                    char_t;
 typedef file_iterator <char_t>  iterator_t;

/// Entry point for this game.
 /** EgoApp extends from SystemCore, and, as such, handles Windows messages,
	 input devices, rendering, and updating of the room which Ego is in.
 */


class EgoApp : public System
{
	private:
		GraphicsDevice  m_Graphics; ///< The graphics device.
		Texture			m_Background; ///< The background of the current room.
		Texture			m_Inventory; ///< The background of the main inventory.
		Tile			m_Tiles; ///< The tile set to use.
		Ego				m_Ego; ///< The main Ego instance.
		Room			m_curRoom; ///< The current room.
		Keyboard		m_Keyboard; ///< The keyboard input device.
		Mouse			m_Mouse; ///< The mouse input device.
		DataPackage *   m_DPCollision; ///< Data package to load collision maps into rooms.
		DWORD			cmSize; ///< DWORD to store size of the collision map data package.
		Font			mainFont; ///< The game's main font.
		char			*collisionData; ///< a pointer to the current collision map.
		//char			*m_Name; ///< Name of this game.
		DWORD			g_dwLastTick; ///< Time of last tick.
		DWORD			g_dCurTime; ///< Current system time.
		DWORD			g_dLastTime; ///< Time at the beginning of last tick.
		float			g_fAnimationTimer; ///< Timer to determine when to update/render the game.
		float			g_fElpasedTime; ///< Time elapsed since last render.
		sRoom			m_newRoom; ///< Structure filled when a new room script is parsed.
		RoomGrammar		*m_roomGrammar; ///< Grammar to parse a room script.
		string			m_curAction; ///< String representing Ego's current action.
		bool			m_inInventory; ///< true if the game is currently in the inventory.
		DataPackage *	m_DPVariables; ///< Data package to load room/object variables.
		int*			m_vars; ///< integer pointer for m_DPVariables.
		int*			test; ///< integer pointer for m_DPVariables.
		DWORD			m_VariablesSize; ///< DWORD to store the size of the variables data package.
		HCURSOR			m_inactive; ///< Cursor when the cursor is not on a hotspot.
		HCURSOR			m_active; ///< Cursor when the cursor is on a hotspot.


		virtual void onProcess();
		virtual void shutdown();
	public:
		EgoApp();
		virtual bool onInit();
		
		bool LoadRoom(std::string roomName);
};

#endif