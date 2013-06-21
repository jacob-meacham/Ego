#include "Conversation.h"

/// Defualt constructor.
Conversation::Conversation() { SetConversationString("NULL!!"); m_visible = false; }

/// Constructor which sets the conversation string and visibility status.
Conversation::Conversation(std::string choice, bool show) { SetConversationString(choice); m_visible = show; }

/// Setter for the conversation string
void Conversation::SetConversationString(std::string s) { m_conversationString = s; }

/// Getter for the conversation string.
std::string Conversation::GetConversationString() { return m_conversationString; }

/// Makes the conversation string visible.
void Conversation::Show() { m_visible = true; }

/// Makes the conversation string invisible.
void Conversation::Hide() { m_visible = false; }

/// Returns the visibility status of the conversation string.
bool Conversation::IsVisible() { return m_visible; }