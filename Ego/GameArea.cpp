#include "GameArea.h"
#define Error(x) MessageBox(NULL, x, "Error", MB_OK);


/// Sets the main font used for rendering all game area text (object descriptions, conversations, etc.).
void GameArea::SetFont(Font *font) { m_font = font; }

/// Adds an object to the main object list.
void GameArea::AddObject(Object o) {
	m_objectList.push_front(o);
}

/// Removes an object from the main object list.
/** \return true if the object is found and removed, false otherwise.
*/
bool GameArea::RemoveObject(std::string objectName) {
	bool found = false;
	for(iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) {
		m_objectList.erase(iObject);
		found = true;
		break;
		}
	}
	return found;
}

/// Sets the current mouse object to null, which forces it to not render.
void GameArea::ClearCurMouseObject() {
	m_curMouseObject = "";
}

/// Sets no object of attention for the game area.
void GameArea::ClearCurActionObject() { m_curActionObject = 0; }

/// Adds a conversation choice to the main conversation list.  Used when first processing a script.
void GameArea::AddConversationChoice(int number, std::string choice, bool show) { 
	m_conversationList.AddElement(number, choice, show);
}

/// Iterates through the main conversation list and finds the active choices for rendering speed.
/** m_conversationList holds all of the possible leaves in the conversation tree.  However, only a few
	of these will be shown at a time.  Therefore, this function is called to choose only the active choices
	from the full conversation list and place them in m_activeChoices.
*/
void GameArea::FillConversationChoices() {
	int y = 400; // the first choice is rendered at x = 0, y = 400.
	m_activeChoices.clear(); // Clear any previous active choices, as they are no longer active
	for(int i = 0; i < m_conversationList.GetNumElements(); i++) { // iterate through each element
		if(m_conversationList.IsVisible(i)) { // if the conversation leaf is visible, then it is active.
			TextBox t; // create a text box to represent the conversation leaf.
			t.Create(m_font);
			t.SetText(i, m_conversationList.GetConversation(i)->GetConversationString(), 0xFFFFFFFF, 0, y, 500);
			m_activeChoices.push_front(t);
			y += t.GetHeight();
		} // end if
	} // end for
}

/// Prints the active conversation choices to the screen.  Will only be rendered in script mode.
void GameArea::PrintConversationChoices() {
	std::list<TextBox>::iterator iText;
	for(iText = m_activeChoices.begin(); iText != m_activeChoices.end(); iText++) {
		(*iText).PrintTextBox();
		(*iText).SetColor(0xFFFFFFFF);
		}
}

/// Sets the game area's script mode.
/** \param d The script mode. True means that the game area is waiting for script input.
Fals means that no script is being processed.
*/
void GameArea::SetInScript(bool d) { m_inScript = d; }



/// Returns the status of the script mode.
bool GameArea::GetInScript() { return m_inScript; }

/// Sets the current conversation string.
/** This string will only be rendered in script mode (see m_inScript).
	\param conv The string to render.
	\param color The color of the conversation string.
	\param x The X-Coordinate of the upper left corner of the text box.
	\param y The Y-Coordinate of the upper left corner of the text box.
*/
void GameArea::SetConversationString(std::string conv, D3DCOLOR color, long x, long y) { 
	m_curConversationStringColor = color;
	m_curConversationString = conv;
	m_curConversationX = x;
	m_curConversationY = y;
}





/// Returns a pointer to a conversation in the main conversation list.
Conversation* GameArea::GetConversationChoice(int number) { return m_conversationList.GetConversation(number); }

/// Sets the visibility of a conversation in the main conversation list to true.
void GameArea::ShowConversationChoice(int number) { m_conversationList.SetElementVisibility(number, true); }

/// Sets the visibility of a conversation in the main conversation list to false.
void GameArea::HideConversationChoice(int number) { m_conversationList.SetElementVisibility(number, false); }


/// Sets the global action (Use, Talk, Look, or Walk) for the game area.
void GameArea::SetGlobalAction(ActionType action) {
	m_curAction = action;
}

/// Returns the global action for the game area.
ActionType GameArea::GetGlobalAction() { return m_curAction; }

/// Returns the current object of attention in the game area.
Object* GameArea::GetCurActionObject() { 
	if(m_curActionObject == 0) { Error("Object is NULL!"); }
	return m_curActionObject; 
}

void GameArea::SetCurActionObject(Object* o) {
	m_curActionObject = o;
}



/// Returns a room flag at the specified index.
int GameArea::GetFlag(int index) { return m_flags[index]; }

/// Sets a room flag at the specified index.
void GameArea::SetFlag(int index, int d) { m_flags[index] = d; }