#include "WinMain.h"
#include "DataPackage.h"
#include "Inventory.h"
#include <sstream>
#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default constructor.  Creates the window.
EgoApp::EgoApp() {	
	x = 10;
	y = 10;
	width = 800;
	height = 600;
	g_dwLastTick = 0;
	g_dCurTime = 0;
	g_dLastTime = 0;
	g_fAnimationTimer = 0;
	g_fElpasedTime = 0;
	m_inInventory = false;
	m_inactive = LoadCursorFromFile("white-cursor.cur");
	m_active = LoadCursorFromFile("red-cursor.cur");
	windowed = true;
}

/// Called before the game enters the message pump.
/** This function initializes graphics and input devices, loads the main font,
	and the main Ego instance, and then loads the first room.
*/
bool EgoApp::onInit() {
	if(!System::onInit()) {
		return false;
	}

	// Create room grammar.
	m_roomGrammar = new RoomGrammar(m_newRoom);

	// Begin the input devices.
	m_Keyboard.Init(getWindow().getHWnd(), getWindow().getHInst(), windowed);
	m_Mouse.Init(getWindow().getHWnd(), getWindow().getHInst(), windowed);

	// Create the main font.
	mainFont.create("Arial", 19, 1);

	// Load the inventory texture.
	m_Inventory.load("Data\\Inventory.png");

	// parse the first room, to determine the number of tiles to create.
	iterator_t first("room0.dat");
    if (!first) {
       return false;
    }

    iterator_t last = first.make_end();
	parse_info <iterator_t> info = parse(first, last, *(m_roomGrammar), space_p);
	if(!info.full) { Error("Parsing Failed"); }


	DataPackage variables(sizeof(int)*(256 + 40*m_newRoom.objectList.size()));
	m_vars = (int *)variables.GetPtr();
	
	memset(m_vars, 0, sizeof(int) * 256);
	for(int i = 256; i<256 + 40*m_newRoom.objectList.size(); i++) {
		m_vars[i] = 1;
	}

	variables.Save("Room.var");

	// Create tiles.
	m_Tiles.Create((2*m_newRoom.objectList.size())+1);
	
	// Load Ego, and his animations.
	m_Tiles.Load(0, "Data\\Ego.png", 180, 240);
	m_Ego.UseTiles(&m_Tiles, 0);
	m_Ego.CreateActor(0, "Ego", 100, 300);
	m_Ego.CreateAnimationSequence(DEFAULT_TALKING, 60, 106, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(TALKING_RIGHT_DOWN, 92, 96, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(TALKING_RIGHT_UP, 48, 48, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(TALKING_LEFT_DOWN, 97, 101, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(TALKING_LEFT_UP, 60, 60, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(TALKING_BACK, 82, 82, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(TALKING_FRONT, 102, 106, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(STANDING_RIGHT_DOWN, 24, 23, Sprite::MAINTAIN_LAST_FRAME);
    m_Ego.CreateAnimationSequence(STANDING_RIGHT_UP, 48, 47, Sprite::MAINTAIN_LAST_FRAME);
	m_Ego.CreateAnimationSequence(STANDING_LEFT_DOWN, 36, 35, Sprite::MAINTAIN_LAST_FRAME);
	m_Ego.CreateAnimationSequence(STANDING_LEFT_UP, 60, 59, Sprite::MAINTAIN_LAST_FRAME);
	m_Ego.CreateAnimationSequence(STANDING_BACK, 82, 81, Sprite::MAINTAIN_LAST_FRAME);
	m_Ego.CreateAnimationSequence(STANDING_FRONT, 72, 71, Sprite::MAINTAIN_LAST_FRAME);
	m_Ego.CreateAnimationSequence(WALKING_RIGHT, 0, 11, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(WALKING_LEFT, 12, 23, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(WALKING_BACK, 0, 11, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(WALKING_FRONT, 0, 11, Sprite::LOOP_ANIMATION);
	m_Ego.CreateAnimationSequence(PICKING_UP_RIGHT_DOWN, 24, 35, Sprite::GOTO_DEFAULT_ANIMATION);
	m_Ego.CreateAnimationSequence(PICKING_UP_RIGHT_UP, 48, 59, Sprite::GOTO_DEFAULT_ANIMATION);
	m_Ego.CreateAnimationSequence(PICKING_UP_LEFT_DOWN, 36, 47, Sprite::GOTO_DEFAULT_ANIMATION);
	m_Ego.CreateAnimationSequence(PICKING_UP_LEFT_UP, 60, 71, Sprite::GOTO_DEFAULT_ANIMATION);
	m_Ego.CreateAnimationSequence(PICKING_UP_BACK, 72, 81, Sprite::GOTO_DEFAULT_ANIMATION);
	m_Ego.CreateAnimationSequence(PICKING_UP_FRONT, 82, 91, Sprite::GOTO_DEFAULT_ANIMATION);
	m_Ego.SetAnimation(STANDING_RIGHT_DOWN);
	
	m_Ego.SetTextColor(0xFFFFFFFF);
	m_Ego.GetInventory()->SetFont(&mainFont);
	m_Ego.GetInventory()->SetInventoryTiles(&gGraphics, 40);
	m_Ego.SetName("Ego");
	
	// Set the main room Ego and font.
	m_curRoom.Set(m_Ego, &mainFont);
	m_curRoom.SetScaling(1.3334f, 0.0002f);

	// Load the first room.
	if(!LoadRoom("room0.dat")) return FALSE;
	m_curRoom.SetGlobalAction(IS_WALK);
	m_curRoom.GetInventory()->SetGlobalAction(IS_WALK);
	g_dwLastTick = timeGetTime();
	
	return TRUE; 
}


/// Shutdown function, called after the message pump has completed.
void EgoApp::shutdown() {
	System::shutdown();

	delete m_DPCollision;
	delete m_DPVariables;
}

/// Called each frame.
/** This function renders at 60FPS.  It reads the input, updates the room/inventory,
	then renders the room/inventory.
*/
void EgoApp::onProcess() {
	// Update timer.
	g_dCurTime     = timeGetTime();
	g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
	g_dLastTime    = g_dCurTime;
	g_fAnimationTimer += g_fElpasedTime;
	if( g_fAnimationTimer >= 0.086f ) { 
		g_fAnimationTimer = 0.0f; // Target of 1/60th of a second (60 FPS) reached. Render a new frame.
	} else { 
		return; // It's too early. Return now and render nothing.
	}

	// Read keyboard and mouse state.
	m_Keyboard.Read();
	m_Mouse.Read();
	
	if(m_curRoom.GetEgo()->GetCurrentlyHeldItem() == 0) {
	SetCursor(m_inactive);
	}
		
	if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
		// if a left click, lock the mouse button
		m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
	}

	if(m_Mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
		m_Mouse.SetLock(MOUSE_RBUTTON, TRUE);
		SetCursor(m_inactive);
		m_curRoom.GetEgo()->SetCurrentlyHeldItem(0);
	}

	if(m_Keyboard.GetKeyState(DIK_P) == TRUE) {
		m_Keyboard.SetLock(DIK_P);
		m_curRoom.GetEgo()->SetCurrentlyHeldItem(0);
		SetCursor(m_inactive);
	}

	// Toggle the inventory on I
	if(m_Keyboard.GetKeyState(DIK_I) == TRUE) {
		if(!m_curRoom.GetInventory()->GetInScript() && !m_curRoom.GetInScript()) {
			m_Keyboard.SetLock(DIK_I);
			if(m_inInventory) { m_inInventory = false; }
			else { 
				m_inInventory = true;
				m_curRoom.ClearCurMouseObject();
			}
		}
	}

	// Change global action to Look on L
	if(m_Keyboard.GetKeyState(DIK_L) == TRUE) {
		m_Keyboard.SetLock(DIK_L, TRUE);
		m_curRoom.SetGlobalAction(IS_LOOK);
		m_curRoom.GetInventory()->SetGlobalAction(IS_LOOK);
	}

	// Change global action to Use on U
	if(m_Keyboard.GetKeyState(DIK_U) == TRUE) {
		m_Keyboard.SetLock(DIK_U, TRUE);
		m_curRoom.SetGlobalAction(IS_USE);
		m_curRoom.GetInventory()->SetGlobalAction(IS_USE);
	}

	// Change global action to Walk on W
	if(m_Keyboard.GetKeyState(DIK_W) == TRUE) {
		m_Keyboard.SetLock(DIK_W, TRUE);
		m_curRoom.SetGlobalAction(IS_WALK);
		m_curRoom.GetInventory()->SetGlobalAction(IS_WALK);
	}

	// Change global action to Talk on T
	if(m_Keyboard.GetKeyState(DIK_T) == TRUE) {
		m_Keyboard.SetLock(DIK_T, TRUE);
		m_curRoom.SetGlobalAction(IS_TALK);
		m_curRoom.GetInventory()->SetGlobalAction(IS_TALK);
	}

	// if we are in the inventory, query.
	if(m_inInventory) {
		m_curRoom.GetInventory()->QueryInventory(m_Mouse.GetXPos(), m_Mouse.GetYPos(), m_Mouse.GetLock(MOUSE_LBUTTON));
	}

	// otherwise, query the room.
	else {
		m_curRoom.QueryRoom(m_Mouse.GetXPos(), m_Mouse.GetYPos(), m_Mouse.GetLock(MOUSE_LBUTTON));
	}

	// if we are not in the inventory, update the room.
	if(!m_inInventory) {
		// Update() only returns false is Ego has exited the room.  
		if(!m_curRoom.Update()) {
			// Not needed for engine.
			Error("Thank you for playing this room of Ego: Adventure, and congratulations!");
			Error("Stay tuned to htpp://uoregon.edu/~jmeacha3/ for more Ego updates.");
			// if this is the case, we must Load the next room.
			std::ostringstream roomScript;
			roomScript << "room" << m_curRoom.GetExitNum() << ".dat";
			LoadRoom(roomScript.str());
		}
	}

	// Begin render state.
	if(gGraphics.beginScene()) {
		gGraphics.clear();
		gGraphics.beginSprite();
		// Draw the background.
		m_Background.draw(0, 0, 0, 0, 0, 0, 1.0f, 1.0f, 0xFFFFFFFF);

		// Render the room.
		m_curRoom.RenderRoom();
		if(m_inInventory) {
			// If the inventory is visible, draw the inventory.
			m_Inventory.draw(75, 0, 0, 0, 0, 0, 1.0f, 1.0f, 0xFFFFFFFF);
			// And then render the inventory.
			m_curRoom.GetEgo()->GetInventory()->RenderInventory();
		}
		gGraphics.endSprite();
		gGraphics.endScene();
	}
	// Display graphics.
	gGraphics.display();
}

/// Function to load a room from a room script.  Returns true if the load is successful.
bool EgoApp::LoadRoom(std::string roomName) {
	m_newRoom.objectList.clear();
	m_newRoom.exitList.clear();
	iterator_t first(roomName.c_str());
    if (!first)
    {
       Error("Unable to open file!");
       return false;
    }

	iterator_t last = first.make_end();

	parse_info <iterator_t> info = parse(first, last, *(m_roomGrammar), space_p);
	
	if(!info.full) { 
		Error("Parsing Failed"); 
		return false;
	}
	
	m_Background.load(m_newRoom.bgFileName.c_str());

	m_DPCollision = DataPackage::Load(m_newRoom.colMapFileName.c_str(), &cmSize);
	collisionData = (char*)m_DPCollision->GetPtr();
	
	int size = m_newRoom.objectList.size();
	
	m_DPVariables = DataPackage::Load("Room.var", &m_VariablesSize);
	test = (int*)m_DPVariables->GetPtr();
	for(int j = 0; j<256; j++) {
		m_curRoom.SetFlag(j, test[j]);
	}
	for(int i = 0; i<size; i++) {
		sObject o = m_newRoom.objectList.front();
		Object tempRoomObject;
		m_Tiles.Load(i+1, (char*)(o.fileName.c_str()), o.width, o.height);
		tempRoomObject.UseTiles(&m_Tiles, i+1);
		tempRoomObject.Create(0, (char*)(o.name.c_str()), o.loc.x, o.loc.y);
		tempRoomObject.SetTextColor(0xFFFF0000);

		if(m_newRoom.objectList.front().visible != 0 &&
		  (m_newRoom.objectList.front().visibleFlag == -1 || 
		   m_curRoom.GetFlag(o.visibleFlag) != 0)) { tempRoomObject.SetVisible(true); }
		else { tempRoomObject.SetVisible(false); }
		if(o.hasOnStepScript == 1) { tempRoomObject.SetHasOnStep(true); }
		else { tempRoomObject.SetHasOnStep(false); }
		tempRoomObject.SetDescriptor(o.descriptor);
		tempRoomObject.SetXYWalkCoordinates(o.walkCoord.x, o.walkCoord.y);
		int l = 0;
		for(int k=40*(i+1); k<40*(i+2); k++) {
			tempRoomObject.SetFlag(l, test[k]);
			l++;
		}
		if(o.useItemOnList.size() != 0) {
			for(std::list<std::string>::iterator iString = o.useItemOnList.begin(); iString != o.useItemOnList.end(); iString++) {
				tempRoomObject.AddUseItem((*iString));
			}
		}
		if(o.animList.size() != 0) {
			for(std::list<sAnimSequence>::iterator iAnim = o.animList.begin(); iAnim != o.animList.end(); iAnim++) {
				Sprite::AnimationOption animOp;
				if((*iAnim).animationOption.compare("GOTO_NEXT_ANIMATION") == 0) { animOp = Sprite::GOTO_NEXT_ANIMATION; }
				else if((*iAnim).animationOption.compare("GOTO_DEFAULT_ANIMATION") == 0) { animOp = Sprite::GOTO_DEFAULT_ANIMATION; }
				else if((*iAnim).animationOption.compare("LOOP_ANIMATION") == 0) { animOp = Sprite::LOOP_ANIMATION; }
				else if((*iAnim).animationOption.compare("KILL_SPRITE") == 0) { animOp = Sprite::KILL_SPRITE; }
				else if((*iAnim).animationOption.compare("MAINTAIN_LAST_FRAME") == 0) { animOp = Sprite::MAINTAIN_LAST_FRAME; }
				tempRoomObject.CreateAnimationSequence((*iAnim).animationNumber, (*iAnim).startFrame, (*iAnim).numFrames, animOp);
				tempRoomObject.SetAnimation(0);
			}
		}
		m_curRoom.AddObject(tempRoomObject);
		m_newRoom.objectList.pop_front();
	}
	int exitSize = m_newRoom.exitList.size();
	for(int i = 0; i<exitSize; i++) {
		m_curRoom.AddExit(m_newRoom.exitList.front().loc, m_newRoom.exitList.front().roomNumber);
		m_newRoom.exitList.pop_front();
	}
	
	m_Ego.SetXYPos(100, 300);
	if(m_newRoom.hasOnEnterScript == 1) { m_curRoom.SetHasEnterScript(true); }
	else { m_curRoom.SetHasEnterScript(false); }
	m_curRoom.EnterRoom(collisionData, m_newRoom.roomName);	
	return true;
}

/// Entry point for a Windows application.  Calls SystemCore::Run
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag); // Set flag to the new value

	_crtBreakAlloc = -1; // Sets a user breakpoint at the specified allocation.
#endif 

	EgoApp app;
	app.parseCommandLine(::__argc, ::__argv);

	int rval = 0;
	if(app.onInit())
	{
		rval = 1;
		app.messageLoop();
	}

	return rval;
}


