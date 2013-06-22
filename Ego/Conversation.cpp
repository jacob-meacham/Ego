#include "Conversation.h"

/// Defualt constructor.
Conversation::Conversation() : m_visible(false) { }

/// Constructor which sets the conversation string and visibility status.
Conversation::Conversation(const std::string & choice, bool show) : m_visible(show) { 
	SetConversationString(choice); 
}

/// Setter for the conversation string
void Conversation::SetConversationString(const std::string & s) { m_conversationString = s; }

/// Getter for the conversation string.
const std::string & Conversation::GetConversationString() const { return m_conversationString; }

/// Makes the conversation string visible.
void Conversation::Show() { m_visible = true; }

/// Makes the conversation string invisible.
void Conversation::Hide() { m_visible = false; }

/// Returns the visibility status of the conversation string.
bool Conversation::IsVisible() const { return m_visible; }