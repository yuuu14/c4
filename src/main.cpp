#include "stdafx.h"
#include "stdio.h"
#include "Lexer.h"
#include "Parser.h"
#include "SA.h"
#include "commands.h"

using namespace std;

void pretty_printing(AST_node* node, int num_tab, bool print_tab, bool print_newline);

string_t transformation(string_t a){
		if(a == "short"||a == "ushort"||a == "int" || a == "uint"||a == "long"||a == "ulong"||a == "float"||a == "double")
			return "constant";
		if(a == "operator")
			return "punctuator";
		if(a == "string"||a == "char"||a == "uchar")
			return "string-literal";
	return a;
}

string_t digraph(string_t s){
	if(s == "<%")
		return "{";
	else if(s=="%>")
		return "}";
	else if(s == "<:")
		return "[";
	else if(s == ":>")
		return "]";
	else
		return s;
}

bool Ture_print(string_t a){
	if(a == "constant"||a == "punctuator"||a == "string-literal"||a == "keyword"||a == "identifier"||a == "error"||a == "none")
		return true;
	return false;
}

void ugly_print(AST_node *node,int n_tab){
	int j = n_tab;
	for (; j>0; j--)
		std::cout << "   ";
	if(node->ptr_vec.size() != 0)
	std::cout << node->type << std::endl;
	else{
		std::cout << node->type<<" "<<node->token_index<< std::endl;
		return;
	}
	int i = 0;
	for (; i < node->ptr_vec.size(); i++) {
		ugly_print(node->ptr_vec.at(i), n_tab+1);	
	}
}

int main(int argc, char ** const argv) {
	if (argc < 3) {
		goto Error;
		
	}
	if (strncmp(CMD_TOKENIZE, argv[1], strlen(CMD_TOKENIZE)) == 0) {
		ifstream iff(argv[2]);
		ostringstream os;
		os << iff.rdbuf();
		CLexer l(os.str());
		while (l.next() != l_end)
		{
			string_t temp = transformation(LEX_STRING(l.get_type()));
			if(Ture_print(temp))//Ture_print(temp)
			{

				if(temp == "error"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
					argv[2],\
					l.get_line(), \
					l.get_column(), \
					"error:",\
					"token is invalid");
					return 1;
				}
				printf("%s:%d:%d: %s %s\n", \
					argv[2],\
					l.get_last_line(), \
					l.get_last_column(), \
					temp.c_str(),\
					l.current().c_str());
			}
		}
	}
	else if(strncmp(CMD_SA, argv[1], strlen(CMD_SA)) == 0){
		ifstream iff(argv[2]);
		ostringstream os;
		os << iff.rdbuf();
		CLexer l(os.str());
		CParser P;
		string_t shabi = "";
		int lll;
		int ccc;
		while (l.next() != l_end)
		{
			shabi = LEX_STRING(l.get_type());
			lll = l.get_last_line();
			ccc = l.get_last_column();
			string_t temp = transformation(LEX_STRING(l.get_type()));
			if(Ture_print(temp))//Ture_print(temp)
			{
				P.Token.push_back(token(temp,digraph(l.current())));
				P.position.push_back(posi(l.get_last_line(),l.get_last_column()));
			}
		}
		AST_node *node;
		node = new AST_node;
		node = P.translation_unit();
		if(node == NULL||P.index<P.Token.size()){
			//std::cout<<"error"<<std::endl;
			if(P.errortokenP < P.Token.size())
				fprintf(stderr, "%s:%d:%d: %s %s\n", \
					argv[2],\
					P.position[P.errortokenP].line, \
					P.position[P.errortokenP].column, \
					"error:",\
					"parser fail");
			else{
				if(shabi == "space"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						lll, \
						ccc+1, \
						"error:",\
						"parser fail");
				} 
				else if(shabi == "newline"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						lll+1, \
						1, \
						"error:",\
						"parser fail");
				}
				else if(P.Token.size()>0){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						P.position[P.Token.size()-1].line, \
						P.position[P.Token.size()-1].column+P.Token[P.Token.size()-1].content.size(), \
						"error:",\
						"parser fail");
				}
				else{
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						1, \
						1, \
						"error:",\
						"parser fail");
				}
			}
			return 1;
		}
		else {			
			class SA sa;
			sa.del_paren(node);
			//ugly_print(node, 0);
			sa.set_ast_tree(node);
			sa.semantic_analysis();
			if(sa.err_idx < 100000){
				fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						P.position[sa.err_idx].line, \
						P.position[sa.err_idx].column, \
						"error:",\
						"SA fail");
				return 1;
			}
			return 0;
		}
	}
	else if(strncmp(CMD_PARSE, argv[1], strlen(CMD_PARSE)) == 0){
		ifstream iff(argv[2]);
		ostringstream os;
		os << iff.rdbuf();
		CLexer l(os.str());
		CParser P;
		string_t shabi = "";
		int lll;
		int ccc;
		while (l.next() != l_end)
		{
			shabi = LEX_STRING(l.get_type());
			lll = l.get_last_line();
			ccc = l.get_last_column();
			string_t temp = transformation(LEX_STRING(l.get_type()));
			if(Ture_print(temp))//Ture_print(temp)
			{
				P.Token.push_back(token(temp,digraph(l.current())));
				P.position.push_back(posi(l.get_last_line(),l.get_last_column()));
			}
		}
		AST_node *node;
		node = new AST_node;
		node = P.translation_unit();
		if(node == NULL||P.index<P.Token.size()){
			//std::cout<<"error"<<std::endl;
			if(P.errortokenP < P.Token.size())
				fprintf(stderr, "%s:%d:%d: %s %s\n", \
					argv[2],\
					P.position[P.errortokenP].line, \
					P.position[P.errortokenP].column, \
					"error:",\
					"parser fail");
			else{
				if(shabi == "space"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						lll, \
						ccc+1, \
						"error:",\
						"parser fail");
				} 
				else if(shabi == "newline"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						lll+1, \
						1, \
						"error:",\
						"parser fail");
				}
				else if(P.Token.size()>0){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						P.position[P.Token.size()-1].line, \
						P.position[P.Token.size()-1].column+P.Token[P.Token.size()-1].content.size(), \
						"error:",\
						"parser fail");
				}
				else{
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						1, \
						1, \
						"error:",\
						"parser fail");
				}
			}
			return 1;
		}
		else {			
			return 0;
		}

	}
	else if(strncmp(CMD_AST, argv[1], strlen(CMD_AST)) == 0){
		ifstream iff(argv[2]);
		ostringstream os;
		os << iff.rdbuf();
		CLexer l(os.str());
		CParser P;
		string_t shabi = "";
		int lll;
		int ccc;
		while (l.next() != l_end)
		{
			shabi = LEX_STRING(l.get_type());
			lll = l.get_last_line();
			ccc = l.get_last_column();
			string_t temp = transformation(LEX_STRING(l.get_type()));
			if(Ture_print(temp))//Ture_print(temp)
			{
				P.Token.push_back(token(temp,digraph(l.current())));
				P.position.push_back(posi(l.get_last_line(),l.get_last_column()));
			}
		}
		AST_node *node;
		node = new AST_node;
		node = P.translation_unit();
		if(node == NULL){
			//std::cout<<"error"<<std::endl;
			if(P.errortokenP < P.Token.size()||P.index<P.Token.size())
				fprintf(stderr, "%s:%d:%d: %s %s\n", \
					argv[2],\
					P.position[P.errortokenP].line, \
					P.position[P.errortokenP].column, \
					"error:",\
					"parser fail");
			else{
				if(shabi == "space"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						lll, \
						ccc+1, \
						"error:",\
						"parser fail");
				} 
				else if(shabi == "newline"){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						lll+1, \
						1, \
						"error:",\
						"parser fail");
				}
				else if(P.Token.size()>0){
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						P.position[P.Token.size()-1].line, \
						P.position[P.Token.size()-1].column+P.Token[P.Token.size()-1].content.size(), \
						"error:",\
						"parser fail");
				}
				else{
					fprintf(stderr, "%s:%d:%d: %s %s\n", \
						argv[2],\
						1, \
						1, \
						"error:",\
						"parser fail");
				}
			}
			return 1;
		}
		else {
			pretty_printing(node, 0, true, true);
		}

	}
	else if(strncmp(CMD_COMPILE, argv[1], strlen(CMD_COMPILE)) == 0){
		return 0;
	}
	else {
		goto Error;
	}
	return 0;
	Error: printf("Error! usage: --tokenize or --parser or --print-ast <filename.c>\r\n");
	return -1;
}

void pretty_printing(AST_node* node, int num_tab, bool print_tab, bool print_newline) {
	// node->type is ;
	if (node->type == "=") 
		fprintf(stdout, " = ");
	
	else if (node->type == ":")
		fprintf(stdout, ": ");
	
	else if (node->type == ";")
		fprintf(stdout, ";\n");

	// node->type is ,
	else if (node->type == ",")
		fprintf(stdout, ", ");

	// token
	else if (node->ptr_vec.size() == 0) {
		int i = num_tab;
		if (print_tab)
			for (; i > 0; i--)
				fprintf(stdout, "\t");
		if (node->type != "")
			fprintf(stdout, "%s", node->type.c_str());		
	}

	// node->type is translation-unit
	else if (node->type == "translation_unit") {
		int idx = 0;
		for (; idx < node->ptr_vec.size() - 1; idx++) {
			pretty_printing(node->ptr_vec.at(idx), num_tab, true, false);
			fprintf(stdout, "\n");
		}
		pretty_printing(node->ptr_vec.at(idx), num_tab, true, false);
	}

	// node->type is function-definition
	else if (node->type == "function_definition") {
		pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
		fprintf(stdout, " ");
		pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
		fprintf(stdout, "\n");
		pretty_printing(node->ptr_vec.at(2), num_tab, true, true);
	}


	// node->type is struct-or-union-spe
	else if (node->type == "struct_or_union_specifier") {
		pretty_printing(node->ptr_vec.at(0), num_tab, true, false); // struct
		if (node->ptr_vec.size() > 2) { // the first derivation
			if (node->ptr_vec.at(1)->type == "{"||node->ptr_vec.at(1)->type == "<%") {
				fprintf(stdout, "\n");
				pretty_printing(node->ptr_vec.at(1), num_tab, true, false); // {
				fprintf(stdout, "\n"); 
				pretty_printing(node->ptr_vec.at(2), num_tab+1, true, false);
				pretty_printing(node->ptr_vec.at(3), num_tab, true, false); // }
			}
			else {
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // id
				fprintf(stdout, "\n");
				pretty_printing(node->ptr_vec.at(2), num_tab, true, false); // {
				fprintf(stdout, "\n"); 
				pretty_printing(node->ptr_vec.at(3), num_tab+1, true, false);
				pretty_printing(node->ptr_vec.at(4), num_tab, true, false); // }
			}
		}
		else {
			fprintf(stdout, " ");
			pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // id
		}
	}
	
	// node->type is labeled-statement
	else if (node->type == "label_statement") {
		pretty_printing(node->ptr_vec.at(0), 0, false, false);
		fprintf(stdout, ":");
		fprintf(stdout, "\n");
		pretty_printing(node->ptr_vec.at(2), num_tab, true, false);
	}

	// node->type is selection-statement
	else if (node->type == "selection_statement") {
		pretty_printing(node->ptr_vec.at(0), num_tab, print_tab, false); // if
		fprintf(stdout, " ");
		pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // (
		pretty_printing(node->ptr_vec.at(2), num_tab, false, false); // expr
		pretty_printing(node->ptr_vec.at(3), num_tab, false, false); // )
		
		if (node->ptr_vec.at(4)->ptr_vec.at(0)->type == "compound_statement") {
			fprintf(stdout, " ");
			if (node->ptr_vec.size() == 5) // if ( expr ) comp-stat
				pretty_printing(node->ptr_vec.at(4)->ptr_vec.at(0), num_tab, false, true); // comp-stat\n
			else { // if ( expr ) stat else stat
				pretty_printing(node->ptr_vec.at(4)->ptr_vec.at(0), num_tab, false, false); // comp-stat
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(5), num_tab, false, false); // else
				if (node->ptr_vec.at(6)->ptr_vec.at(0)->type == "selection_statement") {// else if ...
					fprintf(stdout, " ");
					pretty_printing(node->ptr_vec.at(6)->ptr_vec.at(0), num_tab, false, false);
				}
				else if (node->ptr_vec.at(6)->ptr_vec.at(0)->type == "compound_statement") { // else {...}\n
					fprintf(stdout, " ");
					pretty_printing(node->ptr_vec.at(6)->ptr_vec.at(0), num_tab, false, true);
				}
				else {
					fprintf(stdout, "\n");
					pretty_printing(node->ptr_vec.at(6), num_tab+1, true, false);
				}
			}
		}
		else {
			fprintf(stdout, "\n");
			if (node->ptr_vec.size() == 5) // if ( expr ) other-stat
				pretty_printing(node->ptr_vec.at(4), num_tab+1, true, false); // other-stat
			else {
				pretty_printing(node->ptr_vec.at(4)->ptr_vec.at(0), num_tab+1, true, false); // other-stat
				pretty_printing(node->ptr_vec.at(5), num_tab, true, false); // else
				if (node->ptr_vec.at(6)->ptr_vec.at(0)->type == "selection_statement") {// else if ...
					fprintf(stdout, " ");
					pretty_printing(node->ptr_vec.at(6)->ptr_vec.at(0), num_tab, false, false);
				}
				else if (node->ptr_vec.at(6)->ptr_vec.at(0)->type == "compound_statement") { // else {...}\n
					fprintf(stdout, " ");
					pretty_printing(node->ptr_vec.at(6)->ptr_vec.at(0), num_tab, false, true);
				}
				else {
					fprintf(stdout, "\n");
					pretty_printing(node->ptr_vec.at(6), num_tab+1, true, false);
				}
			}
		}
	}

	// node->type is compound-statement
	else if (node->type == "compound_statement") {
		pretty_printing(node->ptr_vec.at(0), num_tab, print_tab, false); // {
		fprintf(stdout, "\n");
		if (node->ptr_vec.size() == 2) //{}
			pretty_printing(node->ptr_vec.at(1), num_tab, true, false); // }
		else { // {...}
			pretty_printing(node->ptr_vec.at(1), num_tab+1, true, print_newline);
			pretty_printing(node->ptr_vec.at(2), num_tab, true, false); // }
		}
		if (print_newline)
			fprintf(stdout, "\n");
	}

	// node->type is iteration-statement
	else if (node->type == "iteration_statement") {
		pretty_printing(node->ptr_vec.at(0), num_tab, true, false); // while
		fprintf(stdout, " ");
		pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // (
		pretty_printing(node->ptr_vec.at(2), num_tab, false, false); // expr
		pretty_printing(node->ptr_vec.at(3), num_tab, false, false); // )
		if (node->ptr_vec.at(4)->ptr_vec.at(0)->type == "compound_statement") {
			fprintf(stdout, " ");
			pretty_printing(node->ptr_vec.at(4)->ptr_vec.at(0), num_tab, false, true); // {...}\n
		}
		else {
			fprintf(stdout, "\n");
			pretty_printing(node->ptr_vec.at(4), num_tab+1, true, false);
		}
	}

	// node->type is jump-statement
	else if (node->type == "jump_statement") {
		pretty_printing(node->ptr_vec.at(0), num_tab, true, false);
		if (node->ptr_vec.size() == 3) {
			fprintf(stdout, " ");
			pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
			pretty_printing(node->ptr_vec.at(2), num_tab, false, false);
		}
		else pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
	}

	//node->type is expression-statement
	else if (node->type == "expression_statement") {
		AST_node* empty_node = new AST_node;
		empty_node->type = "";
		pretty_printing(empty_node, num_tab, true, false);
		free(empty_node);
		pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
		if (node->ptr_vec.size() == 2)
			pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
	}

	// expressions
	// when node->ptr_vec.size() > 1 and node is log_or|log_and|equa|rel|add|mul
	else if (node->type == "multiplicative_expression" || \ 
			 node->type == "additive_expression" || \
			 node->type == "relational_expression" || \
			 node->type == "equality_expression" || \
			 node->type == "logical_AND_expression" || \
			 node->type == "logical_OR_expression") {
		if (node->ptr_vec.size() > 1) {
			int i = int(node->ptr_vec.size() / 2); // before the first ( tab should be added
			for (; i > 0; i--)
				fprintf(stdout, "(");
			
			pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
			for (i = 1; i < node->ptr_vec.size(); i += 2) {
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(i+1), num_tab, false, false);
				fprintf(stdout, ")");
			}
		}
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}

	else if (node->type == "conditional_expression") {
		if (node->ptr_vec.size() > 1) {
			fprintf(stdout, "(");
			pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
			fprintf(stdout, " ? ");
			pretty_printing(node->ptr_vec.at(2), num_tab, false, false);
			fprintf(stdout, " : ");
			pretty_printing(node->ptr_vec.at(4), num_tab, false, false);
			fprintf(stdout, ")");
		}
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}

	else if (node->type == "postfix_expression") {
		if (node->ptr_vec.size() > 1) {
			int i = 0;
			for (; i < node->ptr_vec.size(); i++) {
				if (node->ptr_vec.at(i)->type == "[" || \
				    node->ptr_vec.at(i)->type == "(" || \
					node->ptr_vec.at(i)->type == "." || \
					node->ptr_vec.at(i)->type == "->")
					fprintf(stdout, "(");
			}
			for (i = 0; i < node->ptr_vec.size(); i++) {				
				if (node->ptr_vec.at(i)->type == "]" || \
				    node->ptr_vec.at(i)->type == ")") {
					pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
					fprintf(stdout, ")");
				}
				else if (node->ptr_vec.at(i)->type == "." || \
					node->ptr_vec.at(i)->type == "->") {
					pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
					pretty_printing(node->ptr_vec.at(++i), num_tab, false, false);
					fprintf(stdout, ")");
				}
				else pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
			}
		}
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}

	else if (node->type == "unary_expression" || \
			 node->type == "assignment_expression") {
		if (node->ptr_vec.size() > 1) {
			int i = 0;
			fprintf(stdout, "(");
			if (node->ptr_vec.at(0)->type == "sizeof" && node->ptr_vec.at(1)->type == "unary_expression") {
				pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
			}
			else {
				for (; i < node->ptr_vec.size(); i++)
					pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
			}
			fprintf(stdout, ")");
		}
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}
	

	else if (node->type == "primary_expresstion") {
		if (node->ptr_vec.size() == 3)
			pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}

	// node->type is declaration
	else if (node->type == "declaration" || node->type == "struct_declaration" || \
			 node->type == "parameter_declaration") {
		pretty_printing(node->ptr_vec.at(0), num_tab, print_tab, false); // type
		if (node->ptr_vec.size() == 3) {
			fprintf(stdout, " ");
			pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // declarator
			pretty_printing(node->ptr_vec.at(2), num_tab, false, false); // ;
		}
		else if (node->type == "parameter_declaration") {
			if (node->ptr_vec.size() == 2) {
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // declarator
			}
		}
		else pretty_printing(node->ptr_vec.at(1), num_tab, false, false); // ;
	}

	else if (node->type == "declaration_specifiers" || node->type == "specifier_qualifier_list") {
		pretty_printing(node->ptr_vec.at(0), num_tab, print_tab, false);
		if (node->ptr_vec.size() > 1)
			for (int i = 1; i < node->ptr_vec.size(); i++) {
				fprintf(stdout, " ");
				pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
			}
	}

	// declarator
	// declarator -> (ptr_ dir-dec) : add par and how many depends on the number of pointer
	// dir-dec -> ( dec ) pretty_printing(dec)
	// dir-dec -> dir-dec ( para-list ) : add par
	else if (node->type == "declarator") {
		if (node->ptr_vec.size() == 2) {
			int num_ptr = node->ptr_vec.at(0)->ptr_vec.size();
			for (int i = num_ptr; i > 0; i--) {
				fprintf(stdout, "(");
				fprintf(stdout, "*");
			}
			pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
			for (int i = num_ptr; i > 0; i--)
				fprintf(stdout, ")");
		}		
		else pretty_printing(node->ptr_vec.at(0), num_tab, print_tab, false);
	}

	else if (node->type == "abstract_declarator") {
		if (node->ptr_vec.at(0)->type == "pointer") {
			if (node->ptr_vec.size() == 2)
				fprintf(stdout, "(");	
			int num_ptr = node->ptr_vec.at(0)->ptr_vec.size();
			for (int i = num_ptr; i > 0; i--) {
				fprintf(stdout, "(");
				fprintf(stdout, "*");
			}
			for (int i = num_ptr; i > 0; i--)
				fprintf(stdout, ")");
			if (node->ptr_vec.size() == 2) {
				pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
				fprintf(stdout, ")");
			}						
		}
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}

	else if (node->type == "direct_declarator" || node->type == "direct_abstract_declarator") {
		if (node->ptr_vec.at(0)->type == "(") {
			if (node->ptr_vec.at(1)->type == ")") { // when it is dir-abs it is allowed
				if (node->ptr_vec.size() == 2) {
					fprintf(stdout, "(");
					fprintf(stdout, ")");
				}
				else {
					fprintf(stdout, "(");
					for (int i = 2; i < node->ptr_vec.size(); i++)
						pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
					fprintf(stdout, ")");
				}
			}
			else if (node->ptr_vec.size() > 3) {
				fprintf(stdout, "(");
				pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
				for (int i = 3; i < node->ptr_vec.size(); i++)
					pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
				fprintf(stdout, ")");
			}
			else if (node->ptr_vec.size() == 3 && node->ptr_vec.at(1)->type == "parameter_list") {
				fprintf(stdout, "(");
				pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
				fprintf(stdout, ")");
			}
			else pretty_printing(node->ptr_vec.at(1), num_tab, false, false);
		}
		else if (node->ptr_vec.size() > 1) {
			fprintf(stdout, "(");
			for (int i = 0; i < node->ptr_vec.size(); i++)
				pretty_printing(node->ptr_vec.at(i), num_tab, false, false);
			fprintf(stdout, ")");

		}
		else pretty_printing(node->ptr_vec.at(0), num_tab, false, false);
	}

	// another
	else {
		int i = 0;
		for (; i < node->ptr_vec.size(); i++)
			pretty_printing(node->ptr_vec.at(i), num_tab, print_tab, print_newline);
	}
}