#ifndef _parser_h_included_
#define _parser_h_included_
#include "scriptGrammar.hpp"
#include <boost/spirit/tree/ast.hpp>
#include <mmsystem.h> // for timeGetTime()
using namespace std;

// Typedefs for boost::spirit
typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t; ///< Abstract Syntax Tree iterator.

class GameArea;
//class Tiles;
class Object;

/// Main parsing class, used to parse, interpret, and execute .sc files.
/** Parser will create an AST from a .sc file.  Then, through ExecuteNextLine(), it can
	navigate the AST and perform the necessary actions.
*/
class Parser {
	private:
		AdventureScript				m_script; ///< Main AdventureScript.
		iter_t						m_iScript; ///< Global Script iterator.
		iter_t						m_iConversation; ///< Iterator for dealing with conversations.
		iter_t						m_iIfBlock; ///< Iterator for dealing with if-blocks.
		tree_parse_info<iterator_t> info; ///< Spirit parse structure containing the AST.
		GameArea					*parent; ///< GameArea which owns this parser.
		DWORD						m_dwLastTick; ///< Size of the last tick.
		DWORD						m_dCurTime; ///< Current time.
		DWORD						m_dLastTime; ///< Time of the beginning of the last tick.
		float						m_fConversationTimer; ///< Timer for execution.
		float						m_fElpasedTime; ///< Time elapsed in timer.
		float						m_ExecutionTime; ///< Amount of time to execute the current line.
		bool						m_Waiting; ///< true if the parser is waiting for conversation input
		Object*						m_currentObject; ///< Current active object.
		
		void SetWaiting(bool d);


	public:

		// Constructors.
		Parser();
		Parser(GameArea *p);

		// Sets the parent of this parser.
		void SetParent(GameArea *p);

		// Parses a file.
		void ParseFile(string fileName);

		// Executes the next line of the script.
		void ExecuteNextLine();

		// True if the line is finished executing.
		bool FinishedLine();

		// True if in conversation and waiting for a choice from the user.
		bool WaitingForInput();

		// Fills the conversation list.
		void FillConversationList();

		// Jumps to the conversation node.
		void JumpToConversation(int number);
};
#endif
