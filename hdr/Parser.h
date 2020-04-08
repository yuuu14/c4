#ifndef __PARSER_H
#define __PARSER_H

#include "types.h"
#include <vector>
#include <string>
using namespace std;

/* we use vector to store all token generated from lexer
 * as input of parser.
 */
struct token {
    std::string type;
    std::string content;
    token(string_t type,string_t content){
        this->type = type;
        this->content = content;
    }
};

struct posi{
    int line;
    int column;
    posi(int line,int column){
        this->line = line;
        this->column = column;
    }
};

struct AST_node {
    std::string type;
    std::vector<AST_node*> ptr_vec;
    int token_index;
};

class CParser {
public:
    CParser(){
        index = 0;
        errortokenP = 0;
    };
    ~CParser(){

    };
    std::vector<struct token> Token;
    std::vector<posi> position;
    unsigned int index; //store the position of the token processed
    int errortokenP;
    AST_node* translation_unit();

private:
    AST_node* creatnew(string_t content,int index);

    AST_node* external_declaration();
    AST_node* function_definition();
    AST_node* compound_statement();

    AST_node* type_specifier();
    AST_node* declaration_specifiers();
    AST_node* declarator();
    AST_node* declaration();
    AST_node* struct_specifier();
    AST_node* pointer();
    AST_node* direct_declarator();
    AST_node* parameter_list();
    AST_node* parameter_declaration();
    AST_node* abstract_declarator();
    AST_node* direct_abstract_declarator();

    AST_node* statement();
    AST_node* label_statement();
    AST_node* expression_statement();
    AST_node* selection_statement();
    AST_node* iteration_statement();
    AST_node* jump_statement();
    AST_node* block_item_list();
    AST_node* block_item();

    AST_node* multiplicative_expression();
    AST_node* additive_expression();
    AST_node* relational_expression();
    AST_node* equality_expression();
    AST_node* logical_AND_expression();
    AST_node* logical_OR_expression();
    AST_node* conditional_expression();
    AST_node* unary_expression();
    AST_node* assignment_expression();

    AST_node* struct_or_union_specifier();
    AST_node* struct_declaration_list();
    AST_node* struct_declaration();
    AST_node* specifier_qualifier_list();
    AST_node* struct_declarator_list();
    AST_node* struct_declarator();
    AST_node* expression();
    AST_node* primary_expresstion();
    AST_node* postfix_expression();
    std::vector<AST_node*> subpostfix_expression();
    AST_node* argument_expression_list();
    AST_node* unary_operator();
    AST_node* type_name();
};

#endif