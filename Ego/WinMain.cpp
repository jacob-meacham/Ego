#include <sstream>
#include "WinMain.h"
#include "DataPackage.h"
#include "Inventory.h"
#include "roomGrammar.h"
//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////
/// Called before the game enters the message pump.
/** This function initializes graphics and input devices, loads the main font,
	and the main Ego instance, and then loads the first room.
*/
bool EgoApp::onInit() {
	if(!System::onInit()) {
		return false;
	}

	// Begin the input devices.
	m_Keyboard.Init(getWindow().getHWnd(), getWindow().getHInst(), windowed);
	m_Mouse.Init(getWindow().getHWnd(), getWindow().getHInst(), windowed);

	// Create the main font.
	mainFont.create("Arial", 19, 1);

	// parse the first room, to determine the number of tiles to create.
	sRoom new_room;
	RoomGrammar room_grammar(new_room);
	iterator_t first("room0.dat");
	parse_info <iterator_t> info = parse(first, first.make_end(), room_grammar, space_p);
	if(!info.full) { TRACE("Parsing Failed"); }

	// Create tiles.
	m_Tiles.Create((2*new_room.objectList.size())+1);
	m_Inventory.load("Data\\Inventory.png");

	//initVars(&new_room);
	initEgo();
	
	// Set the main room Ego and font.
	m_curRoom.Set(m_Ego, &mainFont);
	m_curRoom.SetScaling(1.3334f, 0.0002f);

	// Load the first room.
	if(!LoadRoom("room0.dat")) {
		return false;
	}

	m_curRoom.SetGlobalAction(IS_WALK);
	m_curRoom.GetInventory()->SetGlobalAction(IS_WALK);
	g_dwLastTick = timeGetTime();
	
	return true; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Shutdown function, called after the message pump has completed.
void EgoApp::shutdown() {
	System::shutdown();
}
//////////////////////////////////////////////////////////////////////////////////
void EgoApp::initVars(const sRoom * new_room) {
	DataPackage variables(sizeof(int)*(256 + 40*new_room->objectList.size()));
	int * m_vars = (int *)variables.GetPtr();
	
	memset(m_vars, 0, sizeof(int) * 256);
	for(int i = 256; i<256 + 40*new_room->objectList.size(); i++) {
		m_vars[i] = 1;
	}
	variables.Save("Room.var");
}
//////////////////////////////////////////////////////////////////////////////////
void EgoApp::initEgo() {
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
}
//////////////////////////////////////////////////////////////////////////////////
/// Called each frame.
/** This function renders at 60FPS.  It reads the input, updates the room/inventory,
	then renders the room/inventory.
*/
void EgoApp::onProcess() {
	return;
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

	processInput();

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
			// if this is the case, we must Load the next room.
			std::ostringstream roomScript;
			roomScript << "room" << m_curRoom.GetExitNum() << ".dat";
			LoadRoom(roomScript.str());
		}
	}

	if(gGraphics.beginScene()) {
		gGraphics.clear();
		gGraphics.beginSprite();
		m_Background.draw(0, 0, 0, 0, 0, 0, 1.0f, 1.0f, 0xFFFFFFFF);

		// Render the room.
		m_curRoom.RenderRoom();
		if(m_inInventory) {
			m_Inventory.draw(75, 0, 0, 0, 0, 0, 1.0f, 1.0f, 0xFFFFFFFF);
			m_curRoom.GetEgo()->GetInventory()->RenderInventory();
		}

		gGraphics.endSprite();
		gGraphics.endScene();
	}
	
	gGraphics.display();
}
//////////////////////////////////////////////////////////////////////////////////
void EgoApp::processInput() {
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
}
//////////////////////////////////////////////////////////////////////////////////
/// Function to load a room from a room script.  Returns true if the load is successful.
bool EgoApp::LoadRoom(const std::string & roomName) {
	iterator_t first(roomName.c_str());
    if (!first)
    {
       TRACE("Unable to open file!");
       return false;
    }

	sRoom new_room;
	RoomGrammar room_grammar(new_room);
	parse_info <iterator_t> info = parse(first, first.make_end(), room_grammar, space_p);
	
	if(!info.full) { 
		TRACE("Parsing Failed"); 
		return false;
	}
	
	m_Background.load(new_room.bgFileName.c_str());
	
	// Setup flags
	DataPackage * dp_variables = DataPackage::Load("Room.var", NULL);
	int * flags = (int*)dp_variables->GetPtr();
	for(int j = 0; j<256; j++) {
		m_curRoom.SetFlag(j, flags[j]);
	}
	delete dp_variables;

	int size = new_room.objectList.size();
	for(int i = 0; i<size; i++) {
		const sObject & o = new_room.objectList.front();
		
		Object newRoomObject;
		m_Tiles.Load(i+1, o.fileName.c_str(), o.width, o.height);
		newRoomObject.UseTiles(&m_Tiles, i+1);
		newRoomObject.Create(0, (char*)(o.name.c_str()), o.loc.x, o.loc.y);
		newRoomObject.SetTextColor(0xFFFF0000);

		if(new_room.objectList.front().visible != 0 &&
		  (new_room.objectList.front().visibleFlag == -1 || 
		   m_curRoom.GetFlag(o.visibleFlag) != 0)) { newRoomObject.SetVisible(true); }
		else { newRoomObject.SetVisible(false); }
		if(o.hasOnStepScript == 1) { newRoomObject.SetHasOnStep(true); }
		else { newRoomObject.SetHasOnStep(false); }
		newRoomObject.SetDescriptor(o.descriptor);
		newRoomObject.SetXYWalkCoordinates(o.walkCoord.x, o.walkCoord.y);
		int l = 0;
		for(int k=40*(i+1); k<40*(i+2); k++) {
			newRoomObject.SetFlag(l, flags[k]);
			l++;
		}

		if(o.useItemOnList.size() != 0) {
			for(std::list<std::string>::const_iterator iString = o.useItemOnList.begin(); iString != o.useItemOnList.end(); iString++) {
				newRoomObject.AddUseItem((*iString));
			}
		}

		if(o.animList.size() != 0) {
			for(std::list<sAnimSequence>::const_iterator iAnim = o.animList.begin(); iAnim != o.animList.end(); iAnim++) {
				Sprite::AnimationOption animOp;
				if((*iAnim).animationOption.compare("GOTO_NEXT_ANIMATION") == 0) { animOp = Sprite::GOTO_NEXT_ANIMATION; }
				else if((*iAnim).animationOption.compare("GOTO_DEFAULT_ANIMATION") == 0) { animOp = Sprite::GOTO_DEFAULT_ANIMATION; }
				else if((*iAnim).animationOption.compare("LOOP_ANIMATION") == 0) { animOp = Sprite::LOOP_ANIMATION; }
				else if((*iAnim).animationOption.compare("KILL_SPRITE") == 0) { animOp = Sprite::KILL_SPRITE; }
				else if((*iAnim).animationOption.compare("MAINTAIN_LAST_FRAME") == 0) { animOp = Sprite::MAINTAIN_LAST_FRAME; }
				newRoomObject.CreateAnimationSequence((*iAnim).animationNumber, (*iAnim).startFrame, (*iAnim).numFrames, animOp);
				newRoomObject.SetAnimation(0);
			}
		}

		m_curRoom.AddObject(newRoomObject);
		new_room.objectList.pop_front();
	}

	for(int i = 0; i<new_room.exitList.size(); i++) {
		m_curRoom.AddExit(new_room.exitList.front().loc, new_room.exitList.front().roomNumber);
		new_room.exitList.pop_front();
	}
	m_curRoom.SetHasEnterScript(new_room.hasOnEnterScript == 1); 

	m_Ego.SetXYPos(100, 300);	

	DataPackage * dp_collision = DataPackage::Load(new_room.colMapFileName.c_str(), NULL);
	m_curRoom.EnterRoom(dp_collision, new_room.roomName);
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
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


