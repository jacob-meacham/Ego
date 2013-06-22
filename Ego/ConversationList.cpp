#include "ConversationList.h"
ConversationList::ConversationList() : numElements(0) { 

}
//////////////////////////////////////////////////////////////////////////////////
/// Adds a conversation to the array.
/** \param index The index of the Conversation.
	\param choice The conversation string of the Conversation.
	\param visible The visibility status of the Conversation.
*/
void ConversationList::AddElement(int index, const std::string & choice, bool visible) {
	Conversation c(choice, visible);
	conversationArray[index] = c;
	numElements++;
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter for the number of valid elements stored in the array.
int ConversationList::GetNumElements() const {
	return numElements;
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter for a specific conversation.
/** \returns pointer to the conversation.
	Note that this function does not test for an out of bounds error.
*/
Conversation* ConversationList::GetConversation(int index) {
	return &conversationArray[index];
}
//////////////////////////////////////////////////////////////////////////////////
const Conversation * ConversationList::GetConversation(int index) const {
	return &conversationArray[index];
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets a specific element's visibility.
void ConversationList::SetElementVisibility(int index, bool visible) {
	if(visible) { 
		conversationArray[index].Show(); 
	} else { 
		conversationArray[index].Hide(); 
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter to determine if the Conversation at index is visible.
bool ConversationList::IsVisible(int index) const { return conversationArray[index].IsVisible(); }