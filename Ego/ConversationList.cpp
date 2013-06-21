#include "ConversationList.h"
/// Default constructor.
ConversationList::ConversationList() { m_numElements = 0; }

/// Adds a conversation to the array.
/** \param index The index of the Conversation.
	\param choice The conversation string of the Conversation.
	\param visible The visibility status of the Conversation.
*/
void ConversationList::AddElement(int index, std::string choice, bool visible) {
	Conversation c(choice, visible);
	m_array[index] = c;
	m_numElements++;
}

/// Getter for the number of valid elements stored in the array.
int ConversationList::GetNumElements() {
	return m_numElements;
}

/// Getter for a specific conversation.
/** \returns pointer to the conversation.
	Note that this function does not test for an out of bounds error.
*/
Conversation* ConversationList::GetConversation(int index) {
	return &m_array[index];
}

/// Sets a specific element's visibility.
void ConversationList::SetElementVisibility(int index, bool visible) {
	if(visible) { m_array[index].Show(); }
	else { m_array[index].Hide(); }
}

/// Getter to determine if the Conversation at index is visible.
bool ConversationList::IsVisible(int index) { return m_array[index].IsVisible(); }