#include "ScriptParser.h"
#include "GameArea.h"
#include "Inventory.h"

#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Standard constructor.  Resets all timer variables.
Parser::Parser() { 
	m_dwLastTick = 0;
    m_dCurTime = 0;
	m_dLastTime = 0;
	m_fConversationTimer = 0;
	m_fElpasedTime = 0;
	m_ExecutionTime = 0;
	m_currentObject = NULL;
}

/// Constructor which sets the parent of the Parser.
Parser::Parser(GameArea *p) { parent = p; }

/// Sets the parent of the Parser to p.
void Parser::SetParent(GameArea *p) { parent = p; }

/// Creates an AST of the fileName (must be a .sc file).
void Parser::ParseFile(string fileName) {
	// create a file iterator
	iterator_t first(fileName.c_str());
	if(!first) {
		// if the file cannot be opened, use the generic script instead.
		int index = fileName.find('_', 0);
		// keep the action, but change the object name to generic.
		fileName.replace(0, index, "generic");
		
		iterator_t temp(fileName.c_str());
		first = temp;
	}
    // make an iterator to the end of the file
	iterator_t last = first.make_end();

	// fill the AST, using AdventureScript as the grammar, and spaces as the skip parser
	// (see boost::spirit for more information on spirit).
	info = ast_parse(first, last, m_script, space_p);
	//TODO: Error checking needed! if(info.full) { }
	if(!info.full) { 
		/* REMOVE REMOVE REMOVE!!!!!
		stringstream file;
		file << "Error parsing " << fileName;
		Error((char*)file.str().c_str());
		*/
		int index = fileName.find('_', 0);
		// keep the action, but change the object name to generic.
		fileName.replace(0, index, "generic");
		iterator_t temp(fileName.c_str());
		first = temp;
		// make an iterator to the end of the file
		iterator_t last = first.make_end();

		// fill the AST, using AdventureScript as the grammar, and spaces as the skip parser
		// (see boost::spirit for more information on spirit).
		info = ast_parse(first, last, m_script, space_p);

	}
	
	// Set the global iterator to the beginning of the AST.
	m_iScript = info.trees.begin()->children.begin();
	
	SetWaiting(false);
	m_ExecutionTime = 0.0f;
	
}

/// Executes the next line of the script, regardless of execution time.
/** ExecuteNextLine() is the most important function of Parser.  It should be called by
	its parent GameArea only when FinishedLine() returns true.  ExecuteNextLine() makes
	extensive use of GameArea::FindObject() to set conversation strings, animations, and flags.
	For more information on how the AST is created, see AdventureScript.
*/
void Parser::ExecuteNextLine() {
	// reset execution time
	m_ExecutionTime = 0.0f;
	parent->SetConversationString("", 0x00000000, 0, 0);

	// Converstaion String 
	// Only conversation strings begin with identifiers.
	if(m_iScript->value.id() == AdventureScript::IdentifierID) {
			// Find the object which this identifier points to.
			Object *temp = parent->FindObject(string(m_iScript->value.begin(), m_iScript->value.end()));
			if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << string(m_iScript->value.begin(), m_iScript->value.end());
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}

			// calculate the x and y coordinates at which to print the conversation string.
			long x = temp->GetXPos() - 20;
			long y = temp->GetYPos() - 20;
			if(x < 0) { x = 10; }
			if(y < 0) { y = 10; }

			// Set the color of the string as the color of the object.
			D3DCOLOR color = temp->GetTextColor();

			// create the string.
			string conv(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());

			// set the parent's current conversation string
			parent->SetConversationString(conv, color, x, y);
			temp->DoTalkingAnimation();
			m_currentObject = temp;

			// m_ExecutionTime is dependendent on the length of the string.
			m_ExecutionTime = ((float)(conv.length()) * 0.05f) + .5f;
			
						
			// increment the iterator.
			m_iScript++;
			return;
	}
	// End Conversation String

	// DoAnimation
	else if(m_iScript->value.id() == AdventureScript::DoAnimationID) { 
		// first, find the object.
		Object *temp;
		temp = parent->FindObject(string(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end()));
		if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << string(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}
		
		// next, find the number of the animation
		string animationNumberString((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
		
		// then, the number of seconds the parser should wait before executing the next line.
		string numSecondsToWait((m_iScript->children.begin()+2)->value.begin(), (m_iScript->children.begin()+2)->value.end());
		
		int animationNumber = atoi(animationNumberString.c_str());
		temp->SetAnimation(animationNumber);
		temp->SetAutoAnimate(true);	
		
		// increment the iterator.
		m_iScript++;

		// set execution time to the appropriate time.
		m_ExecutionTime = (float)(atoi(numSecondsToWait.c_str()));
		return;
	}

	// GetItem
	else if(m_iScript->value.id() == AdventureScript::GetItemID) {
		if(m_iScript->children.size() == 0) {
			// find the object in question
			string object(m_iScript->value.begin(), m_iScript->value.end());
			std::stringstream inventoryObject;
			inventoryObject << object << "Inventory";
			Object *temp = parent->FindObject(inventoryObject.str());
			if(temp == 0) {
				temp = parent->FindObject(object);
			}
			if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << object;
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}
	
			// add that object to the main inventory.
			parent->GetInventory()->AddInventoryObject((*temp));

			// no time is needed to execute this line.
			m_ExecutionTime = 0.0f;
		}
		else {
			// find the object in question
			string object(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());
			std::stringstream inventoryObject;
			inventoryObject << object << "Inventory";
			Object *temp = parent->FindObject(inventoryObject.str());
			if(temp == 0) {
				temp = parent->FindObject(object);
			}
			if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << object;
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}
			// add that object to the main inventory.
			parent->GetInventory()->AddInventoryObject((*temp));

			// Then, find out what the animationString says.
			string animationString((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
			int animationNumber = atoi(animationString.c_str());

			// if the number is 0, then do no animation.
			if(animationNumber = 0) {
				m_ExecutionTime = 0.0f;
			}
			// if the number is one, do default pickup animation.
			else if(animationNumber = 1) {
				parent->GetEgo()->DoPickupAnimation();
				m_ExecutionTime = 0.8f;
			}

			// else, do the animation pointed to by animationString.
			else {
				parent->GetEgo()->SetAnimation(animationNumber);
				parent->GetEgo()->SetAutoAnimate(true);
				m_ExecutionTime = parent->GetEgo()->GetNumAnimationFrames(animationNumber) * 0.5f;
			}
		}

		// increment the iterator.
		m_iScript++;
		return;
	}

	// LoseItem
	else if(m_iScript->value.id() == AdventureScript::LoseItemID) { 
		// simply remove the object in question.
		parent->GetInventory()->LoseObject(string(m_iScript->value.begin(), m_iScript->value.end()));
		m_iScript++;
		
		// no time is needed to execute this line.
		m_ExecutionTime = 0.0f;
		return;	
	}

	// Wait
	else if(m_iScript->value.id() == AdventureScript::WaitID) {
		string wait(m_iScript->value.begin(), m_iScript->value.end());
		
		// wait the specified number of seconds.
		m_ExecutionTime = (float)atoi(wait.c_str());

		// increment the iterator.
		m_iScript++;
		return;
	}

	// KillObject
	else if(m_iScript->value.id() == AdventureScript::KillObjectID) { 
		// find the object in question, and set its visibility status to false.
		Object* temp = parent->FindObject(string(m_iScript->value.begin(), m_iScript->value.end()));
		if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << string(m_iScript->value.begin(), m_iScript->value.end());
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
		}
		temp->SetVisible(false);
		m_iScript++;
		m_ExecutionTime = 0.0f;
		return;
	}

	// CreateObject
	else if(m_iScript->value.id() == AdventureScript::CreateObjectID) { 
		// find the object in question and set its visibilty status to true.
		if(m_iScript->children.size() == 0) {
			Object* temp = parent->FindObject(string(m_iScript->value.begin(), m_iScript->value.end()));
			if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << string(m_iScript->value.begin(), m_iScript->value.end());
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}
			temp->SetVisible(true);

		}
		else {
			Object* temp = parent->FindObject(string(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end()));
			if(temp == 0) {
				std::stringstream s;
				s << "Couldn't find " << string(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}
			temp->SetVisible(true);
			string x((m_iScript->children.begin() + 1)->value.begin(), (m_iScript->children.begin() + 1)->value.end());
			string y((m_iScript->children.begin() + 2)->value.begin(), (m_iScript->children.begin() + 2)->value.end());
			temp->SetXYPos(atoi(x.c_str()), atoi(y.c_str()));
		}
		m_iScript++; 
		m_ExecutionTime = 0.0f;
		return;
	}

	// SetVariable
	else if(m_iScript->value.id() == AdventureScript::SetVariableID) { 
		// find variable type (ROOM or an Object name).
		string variableType(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());
		// find variable number.
		string variableNum((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
		// find value.
		string setTo((m_iScript->children.begin()+2)->value.begin(), (m_iScript->children.begin()+2)->value.end());

		// if the variable to be set is a room variable, simply set the parent's flag.
		if(variableType.compare("ROOM") == 0) {
			parent->SetFlag(atoi(variableNum.c_str()), atoi(setTo.c_str()));
		}
		else {
			// otherwise, find the object in question, and set the appropriate flag.
			Object* o = parent->FindObject(variableType);
			if(o == 0) {
				std::stringstream s;
				s << "Couldn't find " << variableType;
				Error((char*)s.str().c_str());
				m_ExecutionTime = 0.0f;
				m_iScript++;
				return;
			}
			o->SetFlag(atoi(variableNum.c_str()), atoi(setTo.c_str()));
		}
		// increment the iterator.
		m_iScript++;
		m_ExecutionTime = 0.0f;
		return;
	}

	// If Block
	else if(m_iScript->value.id() == AdventureScript::IfBlockID) {
		// find the condition.
		string conditionString(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());

		// determine if the condition is false.
		if(parent->GetCurActionObject()->GetFlag(atoi(conditionString.c_str())) == 0) {
			// if it is, we can iterate past the if block.
			m_iScript++;
			m_ExecutionTime = 0.0f;
			return;
		}
		else {
			// otherwise, save the position of the global iterator
			m_iIfBlock = m_iScript;

			// and set the iterator to the beginning of the if block.
			m_iScript = (m_iScript->children.begin()+2);
			m_ExecutionTime = 0.0f;
			return;
		}
	}

	// If-Else Block
	else if(m_iScript->value.id() == AdventureScript::IfElseBlockID) {
		m_iIfBlock = m_iScript;
		
		string conditionString(m_iScript->children.begin()->children.begin()->value.begin(), m_iScript->children.begin()->children.begin()->value.end());
		if(parent->GetCurActionObject()->GetFlag(atoi(conditionString.c_str())) == 0) {
			// set m_iScript to its else block child.
			m_iScript = (m_iScript->children.begin()+1);
			// set m_iScript to else block's first statement child.
			m_iScript = (m_iScript->children.begin()+1);
			
		}
		else {
			// set m_iScript to its if block child.
			m_iScript = m_iScript->children.begin();
			// set m_iScript to if block's first statement child.
			m_iScript = (m_iScript->children.begin()+2);
		}
		m_ExecutionTime = 0.0f;
		return;
	}


	// End of an if or an else block.
	// } is only seen at the end of an if or else block
	else if(string(m_iScript->value.begin(), m_iScript->value.end()).compare("}") == 0) {
		// reset the global iterator to its position before the if block.
		m_iScript = m_iIfBlock;
		
		// iterate past the if block.
		m_iScript++;
		m_ExecutionTime = 0.0f;
		return;
	}

	// Start of Conversation
	else if(m_iScript->value.id() == AdventureScript::BeginConversationID) {
		// save the position of the iterator, and fill the global conversation list.
		iter_t temp = m_iScript;
		FillConversationList();

		// reset the position of the iterator, and execute the show/hide statements (see AdventureScript for grammar).
		m_iScript = temp;
		m_iScript++;

		while(m_iScript->value.id() == AdventureScript::ShowHideID) {
			// find the show/hide string
			string visible(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());
			if(visible.compare("show") == 0) {
				// find the number to show.
				string choice((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
				// show the choice.
				parent->ShowConversationChoice(atoi(choice.c_str()));
			}
			else {
				// find the number to hide.
				string choice((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
				// hide the choice.
				parent->HideConversationChoice(atoi(choice.c_str()));
				
			}
			// increment the iterator.
			m_iScript++;
		}
		// fill the parent's conversation choices.
		parent->FillConversationChoices();

		// the Parser is now waiting for input to proceed.
		SetWaiting(true);
	}
	// End Start of Conversation

	// Conversation Header
	else if(m_iScript->value.id() == AdventureScript::ConversationHeaderID) {
		// Sets the global script to the beginning of the conversation string.
		m_iScript = m_iScript->children.begin()+1;
		m_iScript = m_iScript->children.begin();

	}
	// End Conversation Header

	// Show/Hide Block
	else if(m_iScript->value.id() == AdventureScript::ShowHideID) {
		while(m_iScript->value.id() == AdventureScript::ShowHideID) {
			// find the show/hide string
			string visible(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());
			if(visible.compare("show") == 0) {
				// find the number to show.
				string choice((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
				// show the choice.
				parent->ShowConversationChoice(atoi(choice.c_str()));
			}
			else {
				// find the number to hide.
				string choice((m_iScript->children.begin()+1)->value.begin(), (m_iScript->children.begin()+1)->value.end());
				// hide the choice.
				parent->HideConversationChoice(atoi(choice.c_str()));
				
			}
			m_iScript++;
		}
		// refill the parent's conversation choices.
		parent->FillConversationChoices();

		// The Parser is now waiting for input.
		SetWaiting(true);
	}
	// End Show/Hide Block

	// Stop Command -- if we ever see stop, the script is at an end.
	else if(string(m_iScript->value.begin(), m_iScript->value.end()).compare("stop") == 0) { 
		// clear the current object of attention, and set the parent's script status to false.
		parent->ClearCurActionObject();
		parent->SetInScript(false);
		return;
	}
	// End Stop Command
}

/// Determines if the Parser has finished parsing a line of script.
/** \return true if the execution time has been reached.
*/
bool Parser::FinishedLine() {
	// set timer variables.
	m_dCurTime = timeGetTime();
	m_fElpasedTime = (float)((m_dCurTime - m_dLastTime) * 0.001);
	m_dLastTime    = m_dCurTime;
	m_fConversationTimer += m_fElpasedTime;

	// if we are above the execution time, return true.
	if( m_fConversationTimer >= m_ExecutionTime ) {
			m_fConversationTimer = 0.0f;
			if(m_currentObject != NULL) {
				m_currentObject->SetAnimation(m_currentObject->GetDefaultAnimation());
				m_currentObject = NULL;
			}
			return true;
	}

	// else, return false.
	return false;
}

/// Fills the parent GameArea's global conversation list.
/** This function parses the entire conversation contained in the script and adds each choice to
	the GameArea's list.
*/
void Parser::FillConversationList() {
	// first, there will be some show/hide statements.  we must iterate past them.
	while(m_iScript->value.id() != AdventureScript::ConversationHeaderID) { m_iScript++; }
	
	// Set an iterator to the beginning of the conversation.
	m_iConversation = m_iScript;
	
	// while still in conversation...
	while(m_iScript->value.id() != AdventureScript::EndConversationID) {
		// determine choice number.
		string choiceNumber(m_iScript->value.begin(), m_iScript->value.end());

		// determine choice string.
		string choiceString(m_iScript->children.begin()->value.begin(), m_iScript->children.begin()->value.end());

		// Add the choice, default visibilty is not visible.
		parent->AddConversationChoice(atoi(choiceNumber.c_str()), choiceString, false);

		// increment the iterator.
		m_iScript++;
	} // end while.
}

/// Returns the waiting status of the iterator.
bool Parser::WaitingForInput() { return m_Waiting; }

/// Sets the waiting status of the iterator to d.
void Parser::SetWaiting(bool d) { m_Waiting = d; }


/// Jumps the iterator to a conversation choice.
void Parser::JumpToConversation(int number) {
	// set the global iterator to the beginning of the conversation block.
	m_iScript = m_iConversation;

	// Determine the choice number of the script choice
	string choiceNumber(m_iScript->value.begin(), m_iScript->value.end());

	// compare it with the number to jump to.
	// while this comparison is not true...
	while(atoi(choiceNumber.c_str()) != number) {
		// increment the iterator.
		m_iScript++;
		choiceNumber.clear();
		// Determine the new choice number.
		choiceNumber = string(m_iScript->value.begin(), m_iScript->value.end());
	}
	// Set waiting to false, since there may be some commands to execute.
	SetWaiting(false);
}