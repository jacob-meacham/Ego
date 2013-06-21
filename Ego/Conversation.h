#ifndef _conversation_h_included_
#define _conversation_h_included_
#include <string>
/// class to hold a single conversation string.
/** Conversation is used in the interactive conversations 
	between the main character and other objects.
*/
class Conversation {
	private:
		std::string		m_conversationString; ///< The converastion string to be displayed.
		bool			m_visible; ///< boolean which determines the visibility status of m_conversationString.
	public:
		// Constructors.
		Conversation();
		Conversation(std::string choice, bool show);

		// Setters.
		void SetConversationString(std::string s);
		void Show();
		void Hide();

		// Getters.
		std::string GetConversationString();
		bool IsVisible();
};
#endif