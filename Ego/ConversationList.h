#ifndef _conversationList_h_included_
#define _conversationList_h_included_
#include "Conversation.h"

/// Composed of a fixed sized array of conversations.
class ConversationList {
	private:
		Conversation	m_array[40]; ///< Conversation array.  A conversation tree should never span 40 branches.
		int				m_numElements; ///< The number of valid elements in m_Array.
	public:
		// Constructor
		ConversationList();

		// Add an element to the array.
		void AddElement(int index, std::string choice, bool visible);
		
		// Setters.
		void SetElementVisibility(int index, bool visible);

		// Getters.
		int GetNumElements();
		Conversation* GetConversation(int index);
		bool IsVisible(int index);
};
#endif