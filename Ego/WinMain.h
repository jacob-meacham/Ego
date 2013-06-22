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
		Ego				ego; ///< The main Ego instance.
		Room			curRoom; ///< The current room.
		std::string		curAction; ///< String representing Ego's current action.
		bool			inInventory; ///< true if the game is currently in the inventory.

		// Input
		Keyboard		keyboard; ///< The keyboard input device.
		Mouse			mouse; ///< The mouse input device.
		
		// Rendering
		Texture			backgroundTexture; ///< The background of the current room.
		Texture			inventoryTexture; ///< The background of the main inventory.
		Tile			tiles; ///< The tile set to use.
		Font			mainFont; ///< The game's main font.

		DWORD			dwLastTick; ///< Time of last tick.
		DWORD			dCurTime; ///< Current system time.
		DWORD			dLastTime; ///< Time at the beginning of last tick.
		float			fAnimationTimer; ///< Timer to determine when to update/render the game.
		float			fElpasedTime; ///< Time elapsed since last render.
		
		HCURSOR			inactiveCursor; ///< Cursor when the cursor is not on a hotspot.
		HCURSOR			activeCursor; ///< Cursor when the cursor is on a hotspot.

		void initVars(const sRoom * new_room);
		void initEgo();

		void processInput();
		virtual void onProcess();
		virtual void shutdown();
	public:
		EgoApp();
		virtual bool onInit();
		
		bool LoadRoom(const std::string & roomName);

		Ego * getEgo() { return &ego; }
};

extern EgoApp gApp;