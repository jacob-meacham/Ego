#pragma once
#include <list>
#include "ConversationList.h"
#include "TextBox.h"
#include "Object.h"

class Ego;
class Inventory;
class Parser;

/// Base class for any areas of the game from which scripts can be processed.
/** both Room and Inventory are derived from this base class.  It gives functionality
	for setting conversation strings and rendering conversation trees, as well as handling
	the flags for script processing.  GameArea contains two pure virtual functions, which all
	derived classes must define: GameArea::FindObject(), and GameArea::GetInventory()
*/
class GameArea {
	protected:
		const Font * pFont; ///< Pointer to the font used for rendering strings.

		std::list<Object> objectList;	///< List of all interactive objects in the game area.
		Object * curActionObject; ///< Pointer to the current selected object.
		ActionType curAction; ///< The current action.
		std::string curMouseObject; ///< Name of the object which the mouse is currently hovering over.

		ConversationList conversationList; ///< List for rendering conversation trees.  See FillConversationChoices().
		std::list<TextBox> activeChoices; ///< List for rendering conversation tree choices.		
		std::string	curConversationString; ///< Current conversation string from script (if any).
		long curConversationX; ///< X-Coordinate of the current conversation string.
		long curConversationY; ///< Y-Coordinate of the current conversation string.
		bool inScript; ///< Bool to determine if a script is currently being processed.
		D3DCOLOR curConversationStringColor; ///< Color of the conversation string.

		Parser * pParser; ///< Parser to handle any object scripts.		
		int	gameFlags[256]; ///< Game area flags, used when processing .ent scripts.
	public:
		GameArea();
		virtual ~GameArea();

		// Sets the main font used for rendering all game area text (object descriptions, conversations, etc.).
		void SetFont(const Font * font);

		// Adds an object to the main object list.
		Object & AddObject(const Object & o);
		
		// Removes an object from the main object list.
		bool RemoveObject(const std::string & objectName);

		// Sets the current mouse object to null, which forces it to not render.
		void ClearCurMouseObject();

		// Sets no object of attention for the game area.
		void ClearCurActionObject();
			
		// Adds a conversation choice to the main conversation list.  Used when first processing a script.
		void AddConversationChoice(int number, const std::string & choice, bool show);

		// Iterates through the main conversation list and finds the active choices for rendering speed.
		void FillConversationChoices();

		// Prints the active conversation choices to the screen.  Will only be rendered in script mode.
		void PrintConversationChoices() const;
		
		// Getters.
		Object* GetCurActionObject();
		const Object* GetCurActionObject() const;
		ActionType GetGlobalAction() const;
		bool GetInScript() const;
		int GetFlag(int index) const;
		Conversation* GetConversationChoice(int number);
		const Conversation* GetConversationChoice(int number) const;
		
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
		virtual Object* FindObject(const std::string & objectName) = 0;
		virtual Ego* GetEgo() = 0;
};