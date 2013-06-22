#pragma once
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
		void AddElement(int index, const std::string & choice, bool visible);
		
		// Setters.
		void SetElementVisibility(int index, bool visible);

		// Getters.
		int GetNumElements() const;
		Conversation* GetConversation(int index);
		const Conversation * GetConversation(int index) const;
		bool IsVisible(int index) const;
};