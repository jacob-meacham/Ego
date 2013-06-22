#pragma once
#include "Framework\Texture.h"
#include "Framework\System.h"
#include "Framework\Input.h"
#include "Room.h"

/// Entry point for this game.
 /** EgoApp extends from SystemCore, and, as such, handles Windows messages,
	 input devices, rendering, and updating of the room which Ego is in.
 */
class DataPackage;
struct sRoom;
class EgoApp : public System
{
	private:
		// Data
		Ego				m_Ego; ///< The main Ego instance.
		Room			m_curRoom; ///< The current room.
		std::string		m_curAction; ///< String representing Ego's current action.
		bool			m_inInventory; ///< true if the game is currently in the inventory.

		// Input
		Keyboard		m_Keyboard; ///< The keyboard input device.
		Mouse			m_Mouse; ///< The mouse input device.
		
		// Rendering
		Texture			m_Background; ///< The background of the current room.
		Texture			m_Inventory; ///< The background of the main inventory.
		Tile			m_Tiles; ///< The tile set to use.
		Font			mainFont; ///< The game's main font.

		DWORD			g_dwLastTick; ///< Time of last tick.
		DWORD			g_dCurTime; ///< Current system time.
		DWORD			g_dLastTime; ///< Time at the beginning of last tick.
		float			g_fAnimationTimer; ///< Timer to determine when to update/render the game.
		float			g_fElpasedTime; ///< Time elapsed since last render.
		
		HCURSOR			m_inactive; ///< Cursor when the cursor is not on a hotspot.
		HCURSOR			m_active; ///< Cursor when the cursor is on a hotspot.

		void initVars(const sRoom * new_room);
		void initEgo();

		void processInput();
		virtual void onProcess();
		virtual void shutdown();
	public:
		EgoApp();
		virtual bool onInit();
		
		bool LoadRoom(const std::string & roomName);
};

extern EgoApp gApp;