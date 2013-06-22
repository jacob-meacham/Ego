#include "GameArea.h"
#include "Framework\Define.h"
#include "ScriptParser.h"
//////////////////////////////////////////////////////////////////////////////////
GameArea::GameArea() : pParser(new Parser()) {

}
//////////////////////////////////////////////////////////////////////////////////
GameArea::~GameArea() {
	delete pParser;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the main font used for rendering all game area text (object descriptions, conversations, etc.).
void GameArea::SetFont(const Font *font) { pFont = font; }
//////////////////////////////////////////////////////////////////////////////////
/// Adds an object to the main object list.
Object & GameArea::AddObject(const Object & o) {
	objectList.push_front(o);
	return objectList.front();
}
//////////////////////////////////////////////////////////////////////////////////
/// Removes an object from the main object list.
/** \return true if the object is found and removed, false otherwise.
*/
bool GameArea::RemoveObject(const std::string & objectName) {
	bool found = false;
	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) {
		objectList.erase(iObject);
		found = true;
		break;
		}
	}
	return found;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the current mouse object to null, which forces it to not render.
void GameArea::ClearCurMouseObject() {
	curMouseObject.clear();
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets no object of attention for the game area.
void GameArea::ClearCurActionObject() { curActionObject = 0; }
//////////////////////////////////////////////////////////////////////////////////
/// Adds a conversation choice to the main conversation list.  Used when first processing a script.
void GameArea::AddConversationChoice(int number, const std::string & choice, bool show) { 
	conversationList.AddElement(number, choice, show);
}
//////////////////////////////////////////////////////////////////////////////////
/// Iterates through the main conversation list and finds the active choices for rendering speed.
/** conversationList holds all of the possible leaves in the conversation tree.  However, only a few
	of these will be shown at a time.  Therefore, this function is called to choose only the active choices
	from the full conversation list and place them in activeChoices.
*/
void GameArea::FillConversationChoices() {
	int y = 400; // the first choice is rendered at x = 0, y = 400.
	activeChoices.clear(); // Clear any previous active choices, as they are no longer active
	for(int i = 0; i < conversationList.GetNumElements(); i++) { // iterate through each element
		if(conversationList.IsVisible(i)) { // if the conversation leaf is visible, then it is active.
			TextBox t(pFont); // create a text box to represent the conversation leaf.
			t.SetText(i, conversationList.GetConversation(i)->GetConversationString(), 0xFFFFFFFF, 0, y, 500);
			activeChoices.push_front(t);
			y += t.GetHeight();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Prints the active conversation choices to the screen.  Will only be rendered in script mode.
void GameArea::PrintConversationChoices() const {
	for(std::list<TextBox>::const_iterator iText = activeChoices.begin(); iText != activeChoices.end(); iText++) {
		(*iText).PrintTextBox();
		//(*iText).SetColor(0xFFFFFFFF);
		}
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the game area's script mode.
/** \param d The script mode. True means that the game area is waiting for script input.
Fals means that no script is being processed.
*/
void GameArea::SetInScript(bool d) { 
	inScript = d; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the status of the script mode.
bool GameArea::GetInScript() const { 
	return inScript; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the current conversation string.
/** This string will only be rendered in script mode (see inScript).
	\param conv The string to render.
	\param color The color of the conversation string.
	\param x The X-Coordinate of the upper left corner of the text box.
	\param y The Y-Coordinate of the upper left corner of the text box.
*/
void GameArea::SetConversationString(std::string conv, D3DCOLOR color, long x, long y) { 
	curConversationStringColor = color;
	curConversationString = conv;
	curConversationX = x;
	curConversationY = y;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns a pointer to a conversation in the main conversation list.
Conversation* GameArea::GetConversationChoice(int number) {
	return conversationList.GetConversation(number); 
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the visibility of a conversation in the main conversation list to true.
void GameArea::ShowConversationChoice(int number) { 
	conversationList.SetElementVisibility(number, true); 
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the visibility of a conversation in the main conversation list to false.
void GameArea::HideConversationChoice(int number) { 
	conversationList.SetElementVisibility(number, false); 
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the global action (Use, Talk, Look, or Walk) for the game area.
void GameArea::SetGlobalAction(ActionType action) {
	curAction = action;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the global action for the game area.
ActionType GameArea::GetGlobalAction() const { 
	return curAction; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the current object of attention in the game area.
Object* GameArea::GetCurActionObject() { 
	if(!curActionObject) { 
		TRACE("Object is NULL!"); 
	}

	return curActionObject; 
}
//////////////////////////////////////////////////////////////////////////////////
const Object* GameArea::GetCurActionObject() const { 
	if(!curActionObject) { 
		TRACE("Object is NULL!"); 
	}

	return curActionObject; 
}
//////////////////////////////////////////////////////////////////////////////////
void GameArea::SetCurActionObject(Object* o) {
	curActionObject = o;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns a room flag at the specified index.
int GameArea::GetFlag(int index) const { 
	return gameFlags[index]; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets a room flag at the specified index.
void GameArea::SetFlag(int index, int d) { 
	gameFlags[index] = d; 
}