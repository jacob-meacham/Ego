#ifndef _gameArea_h_included_
#define _gameArea_h_included_
#include <list>
#include "ConversationList.h"
#include "TextBox.h"
#include "Object.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

/// Base class for any areas of the game from which scripts can be processed.
/** both Room and Inventory are derived from this base class.  It gives functionality
	for setting conversation strings and rendering conversation trees, as well as handling
	the flags for script processing.  GameArea contains two pure virtual functions, which all
	derived classes must define: GameArea::FindObject(), and GameArea::GetInventory()
*/
class Ego;
class Inventory;
class Parser;
class GameArea {
	protected:
		std::list<Object>			m_objectList;	///< List of all interactive objects in the game area.
		std::list<Object>::iterator	iObject;		///< Standard iterator for m_objectList.
		ConversationList			m_conversationList; ///< List for rendering conversation trees.  See FillConversationChoices().
		std::list<TextBox>			m_activeChoices; ///< List for rendering conversation tree choices.
		Object						*m_curActionObject; ///< Pointer to the current selected object.
		Font						*m_font; ///< Pointer to the font used for rendering strings.
		std::string					m_curMouseObject; ///< Name of the object which the mouse is currently hovering over.
		std::string					m_curConversationString; ///< Current conversation string from script (if any).
		long						m_curConversationX; ///< X-Coordinate of the current conversation string.
		long						m_curConversationY; ///< Y-Coordinate of the current conversation string.
		bool						m_inScript; ///< Bool to determine if a script is currently being processed.
		Parser *					m_parser; ///< Parser to handle any object scripts.
		ActionType					m_curAction; ///< The current action.
		D3DCOLOR					m_curConversationStringColor; ///< Color of the conversation string.
		int							m_flags[256]; ///< Game area flags, used when processing .ent scripts.

	public:
		GameArea();
		virtual ~GameArea();

		// Sets the main font used for rendering all game area text (object descriptions, conversations, etc.).
		void SetFont(Font *font);

		// Adds an object to the main object list.
		void AddObject(Object o);
		
		// Removes an object from the main object list.
		bool RemoveObject(std::string objectName);

		// Sets the current mouse object to null, which forces it to not render.
		void ClearCurMouseObject();

		// Sets no object of attention for the game area.
		void ClearCurActionObject();
			
		// Adds a conversation choice to the main conversation list.  Used when first processing a script.
		void AddConversationChoice(int number, std::string choice, bool show);

		// Iterates through the main conversation list and finds the active choices for rendering speed.
		void FillConversationChoices();

		// Prints the active conversation choices to the screen.  Will only be rendered in script mode.
		void PrintConversationChoices();
		
		// Getters.
		Object* GetCurActionObject();
		ActionType GetGlobalAction();
		bool GetInScript();
		int GetFlag(int index);
		Conversation* GetConversationChoice(int number);
		
		// Setters.
		void SetFlag(int index, int d);
		void SetGlobalAction(ActionType action);
		void SetInScript(bool d);
		void SetConversationString(std::string conv, D3DCOLOR color, long x, long y);
		void ShowConversationChoice(int number);
		void HideConversationChoice(int number);
		void SetCurActionObject(Object* o);
		
		// virtual functions, required of all children.
		virtual Inventory* GetInventory() = 0;
		virtual Object* FindObject(std::string objectName) = 0;
		virtual Ego* GetEgo() = 0;

};
#endif