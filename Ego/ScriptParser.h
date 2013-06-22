#pragma once
#include "scriptGrammar.h"
#include <boost/spirit/tree/ast.hpp>
using namespace std;

// Typedefs for boost::spirit
typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t; ///< Abstract Syntax Tree iterator.

class Ego;
class GameArea;
class Object;

/// Main parsing class, used to parse, interpret, and execute .sc files.
/** Parser will create an AST from a .sc file.  Then, through ExecuteNextLine(), it can
	navigate the AST and perform the necessary actions.
*/
class Parser {
	private:
		GameArea					*pGameArea; ///< GameArea which owns this parser.
		Object*						currentObject; ///< Current active object.

		AdventureScript				adventureScript; ///< Main AdventureScript.
		iter_t						iScript; ///< Global Script iterator.
		iter_t						iConversation; ///< Iterator for dealing with conversations.
		iter_t						iIfBlock; ///< Iterator for dealing with if-blocks.

		tree_parse_info<iterator_t> info; ///< Spirit parse structure containing the AST.
		
		DWORD						dwLastTick; ///< Size of the last tick.
		DWORD						dCurTime; ///< Current time.
		DWORD						dLastTime; ///< Time of the beginning of the last tick.
		float						fConversationTimer; ///< Timer for execution.
		float						fElpasedTime; ///< Time elapsed in timer.
		float						executionTime; ///< Amount of time to execute the current line.
		bool						waiting; ///< true if the parser is waiting for conversation input
				
		void SetWaiting(bool d);

		void handleIdentifier();
		void handleDoAnimation();
		void handleGetItem();
		void handleLoseItem();
		void handleWait();
		void handleKillObject();
		void handleCreateObject();
		void handleSetVariable();
		void handleConversation();
		void handleShowHideBlock();
		void handleIfElse();
	public:
		Parser(GameArea *p);

		// Parses a file.
		bool ParseFile(string & fileName);

		// Executes the next line of the script.
		void ExecuteNextLine();

		// True if the line is finished executing.
		bool FinishedLine();

		// True if in conversation and waiting for a choice from the user.
		bool WaitingForInput() const;

		// Fills the conversation list.
		void FillConversationList();

		// Jumps to the conversation node.
		void JumpToConversation(int number);
};