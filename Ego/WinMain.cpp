#include <sstream>
#include "WinMain.h"
#include "Framework\Texture.h"
#include "Actor.h"
#include "Inventory.h"
#include "DataPackage.h"
#include "roomGrammar.h"
//////////////////////////////////////////////////////////////////////////////////
/// Default constructor.  Creates the window.
EgoApp::EgoApp() {
	x = 10;
	y = 10;
	width = 850;
	height = 650;
	dwLastTick = 0;
	dCurTime = 0;
	dLastTime = 0;
	fAnimationTimer = 0;
	fElpasedTime = 0;
	inInventory = false;
	inactiveCursor = LoadCursorFromFile("Data\\white-cursor.cur");
	activeCursor = LoadCursorFromFile("Data\\Red-cursor.cur");
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
	keyboard.Init(getWindow().getHWnd(), getWindow().getHInst(), windowed);
	mouse.Init(getWindow().getHWnd(), getWindow().getHInst(), windowed);

	// Create the main font.
	mainFont.create("Arial", 19, 1);

	// parse the first room, to determine the number of tiles to create.
	sRoom new_room;
	RoomGrammar room_grammar(new_room);
	iterator_t first("Data\\room0.dat");
	parse_info <iterator_t> info = parse(first, first.make_end(), room_grammar, space_p);
	if(!info.full) { 
		TRACE("Parsing Failed\n"); 
	}

	// Create tiles.
	tiles.Create((2*new_room.objectList.size())+1);
	inventoryTexture.load("Data\\Inventory.png");

	initVars(&new_room);
	initEgo();
	
	// Set the main room Ego and font.
	curRoom.Init(&ego, &mainFont);
	curRoom.SetScaling(1.3334f, 0.0002f);
	curRoom.SetGlobalAction(IS_WALK);

	ego.GetInventory()->SetGlobalAction(IS_WALK);

	// Load the first room.
	if(!LoadRoom("Data\\room0.dat")) {
		return false;
	}

	dwLastTick = timeGetTime();	
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
	int * vars = (int *)variables.GetPtr();
	
	memset(vars, 0, sizeof(int) * 256);
	for(int i = 256; i<256 + 40*new_room->objectList.size(); i++) {
		vars[i] = 1;
	}
	variables.Save("Room.var");
}
//////////////////////////////////////////////////////////////////////////////////
void EgoApp::initEgo() {
	// Load Ego, and his animations.
	tiles.Load(0, "Data\\Ego.png", 180, 240);
	ego.UseTiles(&tiles, 0);
	ego.CreateActor(0, "Ego", 100, 300);
	ego.CreateAnimationSequence(DEFAULT_TALKING, 60, 106, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(TALKING_RIGHT_DOWN, 92, 96, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(TALKING_RIGHT_UP, 48, 48, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(TALKING_LEFT_DOWN, 97, 101, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(TALKING_LEFT_UP, 60, 60, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(TALKING_BACK, 82, 82, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(TALKING_FRONT, 102, 106, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(STANDING_RIGHT_DOWN, 24, 23, Sprite::MAINTAIN_LAST_FRAME);
    ego.CreateAnimationSequence(STANDING_RIGHT_UP, 48, 47, Sprite::MAINTAIN_LAST_FRAME);
	ego.CreateAnimationSequence(STANDING_LEFT_DOWN, 36, 35, Sprite::MAINTAIN_LAST_FRAME);
	ego.CreateAnimationSequence(STANDING_LEFT_UP, 60, 59, Sprite::MAINTAIN_LAST_FRAME);
	ego.CreateAnimationSequence(STANDING_BACK, 82, 81, Sprite::MAINTAIN_LAST_FRAME);
	ego.CreateAnimationSequence(STANDING_FRONT, 72, 71, Sprite::MAINTAIN_LAST_FRAME);
	ego.CreateAnimationSequence(WALKING_RIGHT, 0, 11, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(WALKING_LEFT, 12, 23, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(WALKING_BACK, 0, 11, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(WALKING_FRONT, 0, 11, Sprite::LOOP_ANIMATION);
	ego.CreateAnimationSequence(PICKING_UP_RIGHT_DOWN, 24, 35, Sprite::GOTO_DEFAULT_ANIMATION);
	ego.CreateAnimationSequence(PICKING_UP_RIGHT_UP, 48, 59, Sprite::GOTO_DEFAULT_ANIMATION);
	ego.CreateAnimationSequence(PICKING_UP_LEFT_DOWN, 36, 47, Sprite::GOTO_DEFAULT_ANIMATION);
	ego.CreateAnimationSequence(PICKING_UP_LEFT_UP, 60, 71, Sprite::GOTO_DEFAULT_ANIMATION);
	ego.CreateAnimationSequence(PICKING_UP_BACK, 72, 81, Sprite::GOTO_DEFAULT_ANIMATION);
	ego.CreateAnimationSequence(PICKING_UP_FRONT, 82, 91, Sprite::GOTO_DEFAULT_ANIMATION);
	ego.SetAnimation(STANDING_RIGHT_DOWN);
	
	ego.SetTextColor(0xFFFFFFFF);
	ego.GetInventory()->SetFont(&mainFont);
	ego.GetInventory()->SetInventoryTiles(40);
	ego.SetName("Ego");
}
//////////////////////////////////////////////////////////////////////////////////
/// Called each frame.
/** This function renders at 60FPS.  It reads the input, updates the room/inventory,
	then renders the room/inventoryTexture.
*/
void EgoApp::onProcess() {
	// Update timer.
	dCurTime     = timeGetTime();
	fElpasedTime = (float)((dCurTime - dLastTime) * 0.001);
	dLastTime    = dCurTime;
	fAnimationTimer += fElpasedTime;
	if( fAnimationTimer >= 0.086f ) { 
		fAnimationTimer = 0.0f; // Target of 1/60th of a second (60 FPS) reached. Render a new frame.
	} else { 
		return; // It's too early. Return now and render nothing.
	}

	processInput();

	// if we are in the inventory, query.
	if(inInventory) {
		ego.GetInventory()->QueryInventory(mouse.GetXPos(), mouse.GetYPos(), mouse.GetLock(MOUSE_LBUTTON));
	}

	// otherwise, query the room.
	else {
		curRoom.QueryRoom(mouse.GetXPos(), mouse.GetYPos(), mouse.GetLock(MOUSE_LBUTTON));
	}

	// if we are not in the inventory, update the room.
	if(!inInventory) {
		// Update() only returns false is Ego has exited the room.  
		if(!curRoom.Update()) {
			// if this is the case, we must Load the next room.
			std::ostringstream roomScript;
			roomScript << "room" << curRoom.GetExitNum() << ".dat";
			LoadRoom(roomScript.str());
		}
	}

	if(gGraphics.beginScene()) {
		gGraphics.clear();
		gGraphics.beginSprite();

		backgroundTexture.draw(0, 0, 0, 0, 0, 0, 1.0f, 1.0f, 0xFFFFFFFF);

		// Render the room.
		curRoom.RenderRoom();
		if(inInventory) {
			inventoryTexture.draw(75, 0, 0, 0, 0, 0, 1.0f, 1.0f, 0xFFFFFFFF);
			ego.GetInventory()->RenderInventory();
		}

		gGraphics.endSprite();
		gGraphics.endScene();
	}
	
	gGraphics.display();
}
//////////////////////////////////////////////////////////////////////////////////
void EgoApp::processInput() {
	// Read keyboard and mouse state.
	keyboard.Read();
	mouse.Read();
	
	if(curRoom.GetEgo()->GetCurrentlyHeldItem() == 0) {
	SetCursor(inactiveCursor);
	}
		
	if(mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
		// if a left click, lock the mouse button
		mouse.SetLock(MOUSE_LBUTTON, TRUE);
	}

	if(mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
		mouse.SetLock(MOUSE_RBUTTON, TRUE);
		SetCursor(inactiveCursor);
		curRoom.GetEgo()->SetCurrentlyHeldItem(0);
	}

	if(keyboard.GetKeyState(DIK_P) == TRUE) {
		keyboard.SetLock(DIK_P);
		curRoom.GetEgo()->SetCurrentlyHeldItem(0);
		SetCursor(inactiveCursor);
	}

	// Toggle the inventory on I
	if(keyboard.GetKeyState(DIK_I) == TRUE) {
		if(!ego.GetInventory()->GetInScript() && !curRoom.GetInScript()) {
			keyboard.SetLock(DIK_I);
			if(inInventory) { 
				inInventory = false;
				ego.GetInventory()->SetActive(false);
			}
			else { 
				ego.GetInventory()->SetActive(true);
				inInventory = true;
				curRoom.ClearCurMouseObject();
			}
		}
	}

	if (!ego.GetCurrentlyHeldItem()) {
		// Change global action to Look on L
		if(keyboard.GetKeyState(DIK_L) == TRUE) {
			keyboard.SetLock(DIK_L, TRUE);
			curRoom.SetGlobalAction(IS_LOOK);
			ego.GetInventory()->SetGlobalAction(IS_LOOK);
		}

		// Change global action to Use on U
		if(keyboard.GetKeyState(DIK_U) == TRUE) {
			keyboard.SetLock(DIK_U, TRUE);
			curRoom.SetGlobalAction(IS_USE);
			ego.GetInventory()->SetGlobalAction(IS_USE);
		}

		// Change global action to Walk on W
		if(keyboard.GetKeyState(DIK_W) == TRUE) {
			keyboard.SetLock(DIK_W, TRUE);
			curRoom.SetGlobalAction(IS_WALK);
			ego.GetInventory()->SetGlobalAction(IS_WALK);
		}

		// Change global action to Talk on T
		if(keyboard.GetKeyState(DIK_T) == TRUE) {
			keyboard.SetLock(DIK_T, TRUE);
			curRoom.SetGlobalAction(IS_TALK);
			ego.GetInventory()->SetGlobalAction(IS_TALK);
		}
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
	
	backgroundTexture.load(new_room.bgFileName.c_str());
	
	// Setup flags
	DataPackage * dp_variables = DataPackage::Load("Room.var", NULL);
	int * flags = (int*)dp_variables->GetPtr();
	for(int j = 0; j<256; j++) {
		curRoom.SetFlag(j, flags[j]);
	}
	delete dp_variables;

	int size = new_room.objectList.size();
	for(int i = 0; i<size; i++) {
		const sObject & o = new_room.objectList.front();
		
		Object newRoomObject;
		tiles.Load(i+1, o.fileName.c_str(), o.width, o.height);
		newRoomObject.UseTiles(&tiles, i+1);
		newRoomObject.Create(0, (char*)(o.name.c_str()), o.loc.x, o.loc.y);
		newRoomObject.SetTextColor(0xFFFF0000);

		if(new_room.objectList.front().visible != 0 &&
		  (new_room.objectList.front().visibleFlag == -1 || 
		   curRoom.GetFlag(o.visibleFlag) != 0)) { newRoomObject.SetVisible(true); }
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

		curRoom.AddObject(newRoomObject);
		new_room.objectList.pop_front();
	}

	for(int i = 0; i<new_room.exitList.size(); i++) {
		curRoom.AddExit(new_room.exitList.front().loc, new_room.exitList.front().roomNumber);
		new_room.exitList.pop_front();
	}
	curRoom.SetHasEnterScript(new_room.hasOnEnterScript == 1); 

	ego.SetXYPos(100, 300);	

	DataPackage * dp_collision = DataPackage::Load(new_room.colMapFileName.c_str(), NULL);
	curRoom.EnterRoom(dp_collision, new_room.roomName);
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
EgoApp gApp;
System & gSystem = gApp;
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

	gApp.parseCommandLine(__argc, __argv);

	int rval = 0;
	if(gApp.onInit())
	{
		rval = 1;
		gApp.messageLoop();
	}

	return rval;
}


