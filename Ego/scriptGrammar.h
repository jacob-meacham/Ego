#include <boost/spirit.hpp>
#include <boost/spirit/tree/ast.hpp>

using namespace boost::spirit;
using namespace std;

/// Grammar for .sc files.  Uses boost::spirit.
struct AdventureScript : public boost::spirit::grammar<AdventureScript> {
	// Grammar rule IDs, for the AST generated by class Parser.
	static const int IdentifierID = 1;
	static const int ConversationHeaderID = 2;
	static const int StatementListID = 3;
	static const int StatementID = 4; 
	static const int ConversationStringID = 5;
	static const int ChoiceID = 6;
	static const int ShowHideID = 7;
	static const int ChoiceStringID = 8;
	static const int ConversationListID = 9;
	static const int ActionID = 10;
	static const int BeginConversationID = 11;
	static const int EndConversationID = 12;
	static const int ConditionID = 13;
	static const int DoAnimationID = 14;
	static const int GetItemID = 15;
	static const int LoseItemID = 16;
	static const int WaitID = 17;
	static const int KillObjectID = 18;
	static const int CreateObjectID = 19;
	static const int SetVariableID = 20;
	static const int IfBlockID = 21;
	static const int ElseBlockID = 22;
	static const int IfElseBlockID = 23;

	template <typename ScannerT>
	struct definition {
		rule<ScannerT, parser_context<>, parser_tag<ActionID> >  Action;
		rule<ScannerT, parser_context<>, parser_tag<StatementListID> > StatementList;
		rule<ScannerT, parser_context<>, parser_tag<StatementID> > Statement;
		rule<ScannerT, parser_context<>, parser_tag<ConversationStringID> > ConversationString;
		rule<ScannerT, parser_context<>, parser_tag<ChoiceID> > Choice;
		rule<ScannerT, parser_context<>, parser_tag<ShowHideID> > ShowHide;
		rule<ScannerT, parser_context<>, parser_tag<ChoiceStringID> > ChoiceString;
		rule<ScannerT, parser_context<>, parser_tag<ConversationListID> > ConversationList;
		rule<ScannerT, parser_context<>, parser_tag<IdentifierID> > Identifier;
		rule<ScannerT, parser_context<>, parser_tag<ConversationHeaderID> > ConversationHeader;
		rule<ScannerT, parser_context<>, parser_tag<BeginConversationID> > BeginConversation;
		rule<ScannerT, parser_context<>, parser_tag<EndConversationID> > EndConversation;
		rule<ScannerT, parser_context<>, parser_tag<DoAnimationID> > DoAnimation;
		rule<ScannerT, parser_context<>, parser_tag<GetItemID> > GetItem;
		rule<ScannerT, parser_context<>, parser_tag<LoseItemID> > LoseItem;
		rule<ScannerT, parser_context<>, parser_tag<WaitID> > Wait;
		rule<ScannerT, parser_context<>, parser_tag<KillObjectID> > KillObject;
		rule<ScannerT, parser_context<>, parser_tag<CreateObjectID> > CreateObject;
		rule<ScannerT, parser_context<>, parser_tag<SetVariableID> > SetVariable;
		rule<ScannerT, parser_context<>, parser_tag<ConditionID> > Condition;
		rule<ScannerT, parser_context<>, parser_tag<IfBlockID> > IfBlock; 
		rule<ScannerT, parser_context<>, parser_tag<IfBlockID> > ElseBlock; 
		rule<ScannerT, parser_context<>, parser_tag<IfElseBlockID> > IfElseBlock;
		
		definition(AdventureScript const& self) {
			Action = root_node_d[StatementList] >> str_p("stop");
			StatementList = (+Statement >> BeginConversation >> discard_node_d[ch_p('{')] >> *(ShowHide) >> discard_node_d[ch_p('}')] >> *(Choice) >>  EndConversation) |
			+(Statement);
			Statement = ConversationString | DoAnimation | GetItem | LoseItem | Wait | KillObject | CreateObject | SetVariable | IfElseBlock | IfBlock;
			IfBlock = discard_node_d[str_p("if")] >> discard_node_d[ch_p('(')] >> Condition >> discard_node_d[ch_p(')')] >> ch_p('{') >> +Statement >> ch_p('}');
			IfElseBlock = IfBlock >> ElseBlock;
			ElseBlock = discard_node_d[str_p("else")] >> ch_p('{') >> +Statement >> ch_p('}');
			Condition = real_p;
			DoAnimation = discard_node_d[str_p("DoAnimation")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')];
			GetItem = discard_node_d[str_p("GetItem")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')] | 
						discard_node_d[str_p("GetItem")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')];
			LoseItem = discard_node_d[str_p("LoseItem")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')];
			Wait = discard_node_d[str_p("Wait")] >> discard_node_d[ch_p('(')] >> real_p >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')];
			KillObject = discard_node_d[str_p("KillObject")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')];
			CreateObject = (discard_node_d[str_p("CreateObject")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] 
							>> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')]) | 
							(discard_node_d[str_p("CreateObject")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')]);
			SetVariable = discard_node_d[str_p("SetVariable")] >> discard_node_d[ch_p('(')] >> leaf_node_d[+(alnum_p)] >> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(',')] >> real_p >> discard_node_d[ch_p(')')] >> discard_node_d[ch_p(';')];
			ConversationString =  root_node_d[Identifier] >> leaf_node_d [lexeme_d[+(anychar_p - ';')]] >> discard_node_d[ch_p(';')];
			Identifier = leaf_node_d [+(alnum_p)] >> discard_node_d[ch_p(':')];
			Choice = root_node_d[ConversationHeader] >> ChoiceString >> ConversationList;
			ConversationList = (+Statement >> discard_node_d[ch_p('{')] >> *(ShowHide) >> discard_node_d[ch_p('}')] >> str_p("end")) | (+Statement >> str_p("stop"));
			ShowHide = (leaf_node_d[str_p("show")] >> real_p) | (leaf_node_d[str_p("hide")] >> real_p);
			ConversationHeader = discard_first_node_d['@' >> real_p];
			ChoiceString = leaf_node_d [ lexeme_d[+(anychar_p - ';')]] >> discard_node_d[ch_p(';')];
			BeginConversation = str_p("begin conversation");
			EndConversation = str_p("end conversation");
		}
			
		rule<ScannerT, parser_context<>, parser_tag<ActionID> > const& start() const {
			return Action;
		}
	};
};