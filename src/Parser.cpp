#include "stdafx.h"
#include "Parser.h"
#include <string>
#include <algorithm>
using namespace std;

int max(unsigned int a,int b){
	if(a>b)
		return a;
	return b;
}

AST_node* CParser::creatnew(string_t content,int index){
	AST_node *node;
	node = new AST_node;
	node->type = content;
	node->token_index = index;
	return node;
}

AST_node* CParser::function_definition() {
    AST_node* node;
    int idx = index;
    
    auto dec_spe = declaration_specifiers();
    if (dec_spe == NULL) {
        index = idx;
        return nullptr;
    }
    auto dec = declarator();
    if (dec == NULL) { 
        index = idx;
        return nullptr;
    }
    auto comp_stat = compound_statement();
    if (comp_stat != NULL) {
        node = new AST_node;
        node->type = "function_definition";
        node->ptr_vec.push_back(dec_spe);
        node->ptr_vec.push_back(dec);
        node->ptr_vec.push_back(comp_stat);
        return node;
    }
    else {
        index = idx;
        return nullptr;
    }
}

AST_node* CParser::declaration_specifiers() {
    AST_node* node;
    int idx = index;

    auto type_spec = type_specifier();
    if (type_spec == NULL) {
        index = idx;
        return nullptr;
    }
    else {
        node = new AST_node;
        node->type = "declaration_specifiers";
    }
    while (type_spec != NULL) {
        idx = index;
        node->ptr_vec.push_back(type_spec);
        type_spec = type_specifier();
    }
    index = idx;
    return node;
}

AST_node* CParser::declarator() {
    AST_node* node;
    int idx = index;
    
    auto ptr = pointer();  
    auto dir_dec = direct_declarator();
    if (dir_dec == nullptr) {
        index = idx;
        return nullptr;
    }
    else {
        node = new AST_node;
        node->type = "declarator";
        if (ptr != nullptr)
            node->ptr_vec.push_back(ptr);
        node->ptr_vec.push_back(dir_dec);
        return node;
    }
}

AST_node* CParser::direct_declarator() {
    AST_node* node;
    int idx = index;

    if (index >= Token.size()) 
        return nullptr;
    else if (Token[index].type == "identifier") {
        index++;
        errortokenP = max(index,errortokenP);
        AST_node* id = new AST_node;
        id->type = Token[index-1].content;
		id->token_index = index-1;
        node = new AST_node;
        node->type = "direct_declarator";
        node->ptr_vec.push_back(id);
    }
    else if (Token[index].content == "(") {
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        auto dec = declarator();
        if (dec == nullptr) {
            index = idx;
            return nullptr;
        }
        else{
            if (index >= Token.size() || Token[index].content != ")"){
                index = idx;
            	return nullptr;
            } 
                
            else {
				int iii = index;
                index++;
                errortokenP = max(index,errortokenP);
                AST_node* left_par = new AST_node;
                AST_node* right_par = new AST_node;
                left_par->type = "(";
				left_par->token_index = ii;
                right_par->type = ")";
				right_par->token_index = iii;
                node = new AST_node;
                node->type = "direct_declarator";
				if(dec->ptr_vec.size()==1){
					auto temp = dec->ptr_vec[0];//diret-declarator
					if(temp->ptr_vec.size()==3){
						free(node);
						return dec->ptr_vec[0];
					}
				}
                node->ptr_vec.push_back(left_par);
                node->ptr_vec.push_back(dec);
                node->ptr_vec.push_back(right_par);
            }
        }
    }
    else{
    	return nullptr;
    } 
    idx = index; 
    
    if (index >= Token.size()) 
        return node;         //return nullptr OR node???
    while (Token[index].content == "(") {
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        auto para_list = parameter_list();
        if (para_list == nullptr) {
            index = idx;
            return node;
        }
        else {
            if (index >= Token.size() || Token[index].content != ")"){
                index = idx;
                return node;
            } 
            else {
				int iii = index;
                index++;
                errortokenP = max(index,errortokenP);           
                AST_node* left_par = new AST_node;
                AST_node* right_par = new AST_node;
                left_par->type = "(";
				left_par->token_index = ii;
                right_par->type = ")";
				right_par->token_index = iii;                                
                node->ptr_vec.push_back(left_par);
                node->ptr_vec.push_back(para_list);
                node->ptr_vec.push_back(right_par);
            }
        }
    }
    return node;
}

AST_node* CParser::pointer() {
    AST_node* node;
    int idx = index;

    if (index >= Token.size()) 
        return nullptr;
    if (Token[index].content == "*") {
		int ii = index;
        index++; 
        errortokenP = max(index,errortokenP);       
        AST_node* ptr_node = new AST_node;
        ptr_node->type = "ptr_";
		ptr_node->token_index = ii;
        node = new AST_node;
        node->type = "pointer";
        node->ptr_vec.push_back(ptr_node);
        if (index >= Token.size()){
            return node;
        } 
        while (Token[index].content == "*") {
			int iii = index;
            index++;
            errortokenP = max(index,errortokenP);
            if (index >= Token.size()){
                return node;
            }
			AST_node* ptr_node1 = new AST_node;
       		ptr_node1->type = "ptr_";
			ptr_node1->token_index = iii; 
            node->ptr_vec.push_back(ptr_node1);
        }
        return node;
    }
    else {
        return nullptr;
    }
}

AST_node* CParser::parameter_list() {
    AST_node* node;
    int idx = index;

    auto para_dec = parameter_declaration();
    if (para_dec == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "parameter_list";
    node->ptr_vec.push_back(para_dec);
    idx = index;
    if (index >= Token.size()) 
        return node;
    while (Token[index].content == ",") {
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        para_dec = parameter_declaration();
        if (para_dec == nullptr) {
            index = idx;
            return node;
        }
        else {
            AST_node* comma_node = new AST_node;
            comma_node->type = ",";
			comma_node->token_index = ii;
            node->ptr_vec.push_back(comma_node);
            node->ptr_vec.push_back(para_dec);
        }
        if (index >= Token.size()) 
            return node;        
    }
    return node;
}

AST_node* CParser::parameter_declaration() {   
    AST_node* node;
    int idx, temp;
	temp = index;
	idx = index;

    auto dec_spec = declaration_specifiers();
    if (dec_spec == nullptr) {
        index = idx;
        return nullptr;
    }
    temp = index;
    node = new AST_node;
    node->type = "parameter_declaration";
    node->ptr_vec.push_back(dec_spec);

    auto dec = declarator();
    if (dec == nullptr) {
        index = temp;
        auto abst_dec = abstract_declarator();
        if (abst_dec == nullptr)
            return node;
        else {
            node->ptr_vec.push_back(abst_dec);
            return node;
        }
    }
    else {
        node->ptr_vec.push_back(dec);
        return node;
    }
}

AST_node* CParser::abstract_declarator() {
    AST_node* node;
    int idx = index;

    auto ptr = pointer();
    auto dir_abst_dec = direct_abstract_declarator();
    if (dir_abst_dec == nullptr) {
        if (ptr == nullptr) {
            index = idx;
            return nullptr;
        }
        else {
            node = new AST_node;
            node->type = "abstract_declarator";
            node->ptr_vec.push_back(ptr);
            return node;
        }
    }
    else {
        node = new AST_node;
        node->type = "abstract_declarator";
        if (ptr != nullptr)
            node->ptr_vec.push_back(ptr);
        node->ptr_vec.push_back(dir_abst_dec);
        return node;
    }
}

AST_node* CParser::direct_abstract_declarator() {
    AST_node* node;
    int idx = index;
    node = new AST_node;
    node->type = "direct_abstract_declarator";
    bool flag = false;
    if (index >= Token.size()) 
        return nullptr;
	AST_node* abst_dec;
    if (Token[index].content == "(") {
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        abst_dec = abstract_declarator();
        if (abst_dec == nullptr)
            index = idx;
        else {
            if (index >= Token.size()) 
                return nullptr;
            if (Token[index].content == ")") {
				int iii = index;
                index++;
                errortokenP = max(index,errortokenP);
                AST_node* left_par = new AST_node;
                AST_node* right_par = new AST_node;
                left_par->type = "(";
				left_par->token_index = ii;
                right_par->type = ")";
				right_par->token_index = iii;                             
                node->ptr_vec.push_back(left_par);
                node->ptr_vec.push_back(abst_dec);
                node->ptr_vec.push_back(right_par);
                flag = !flag;
            }
            else {
                index = idx;
                free(node);
                return nullptr;
            }
        }
    }
    if (index >= Token.size()){
        if(flag){
			if(abst_dec->ptr_vec[0]->type == "direct_abstract_declarator"){
					if(abst_dec->ptr_vec[0]->ptr_vec.size()==3&&abst_dec->ptr_vec[0]->ptr_vec[1]->type=="abstract_declarator"){
						free(node);
						return abst_dec->ptr_vec[0];
					}
				}
			return node;
		}
        return nullptr;
    } 
    idx = index;
    while (Token[index].content == "(" || Token[index].content == "["||Token[index].content == "<:") {
        string_t ttt = Token[index].content;
		int ii = index;
		index++;
        errortokenP = max(index,errortokenP);
        if (Token[index-1].content == "["||Token[index-1].content == "<:") {
            if (Token[index].content == "*") {
				int iii = index;
                index++;
                errortokenP = max(index,errortokenP);
                if (Token[index].content == "]"||Token[index].content == ":>") {
					int iiii = index;
					index++;
                    AST_node* left_br = new AST_node;
                    AST_node* ptr = new AST_node;
                    AST_node* right_br = new AST_node;
                    left_br->type = ttt;
                    ptr->type = "ptr_";
                    right_br->type = Token[index-1].content;
					left_br->token_index = ii;
                    ptr->token_index = iii;
                    right_br->token_index = iiii;
                    node->ptr_vec.push_back(left_br);
                    node->ptr_vec.push_back(ptr);
                    node->ptr_vec.push_back(right_br);
                }
                else {
                    index = idx;
                    break;
                }
            }
            else {
                index = idx;
                break;
            }
        }
        else { // ( para-list opt)
            auto para_list = parameter_list();
            if (Token[index].content != ")") {
                index = idx;
                break;
            }
            else {
				int iii = index;
                index++;
                errortokenP = max(index,errortokenP);
                AST_node* left_par = new AST_node;
                AST_node* right_par = new AST_node;
                left_par->type = "(";
                right_par->type = ")";
				left_par->token_index = ii;
                right_par->token_index = iii;   
                node->ptr_vec.push_back(left_par);
                if (para_list != nullptr)
                    node->ptr_vec.push_back(para_list);
                node->ptr_vec.push_back(right_par);
            }
        }
        if (index >= Token.size())
            break;
    }
    if(node->ptr_vec.size() != 0){
		if(flag&&node->ptr_vec.size()==3){
			if(abst_dec->ptr_vec[0]->type == "direct_abstract_declarator"){
					if(abst_dec->ptr_vec[0]->ptr_vec.size()==3&&abst_dec->ptr_vec[0]->ptr_vec[1]->type=="abstract_declarator"){
						free(node);
						return abst_dec->ptr_vec[0];
					}
				}
		}
		return node;
	}
        
    index = idx;
    return nullptr;
}

//Statements
AST_node* CParser::statement() {
    AST_node* node = new AST_node;
    node->type = "statement";
    int idx = index;

    auto label_stat = label_statement();
    if (label_stat == nullptr) {
        index = idx;
        auto comp_stat = compound_statement();
        if (comp_stat == nullptr) {
            index = idx;
            auto expr_stat = expression_statement();
            if (expr_stat == nullptr) {
                index = idx;
                auto sel_stat = selection_statement();
                if (sel_stat == nullptr) {
                    index = idx;
                    auto iter_stat = iteration_statement();
                    if (iter_stat == nullptr) {
                        index = idx;
                        auto jump_stat = jump_statement();
                        if (jump_stat == nullptr) {
                            index = idx;
                            free(node);
                            return nullptr;
                        }
                        else {
                            node->ptr_vec.push_back(jump_stat);
                            return node;
                        }
                    }
                    else {
                        node->ptr_vec.push_back(iter_stat);
                        return node;                     
                    }                    
                }
                else {
                    node->ptr_vec.push_back(sel_stat);
                    return node;
                }                       
            }
            else {
                node->ptr_vec.push_back(expr_stat);
                return node;
            }
        }
        else {
            node->ptr_vec.push_back(comp_stat);
            return node;
        }
    }
    else {
        node->ptr_vec.push_back(label_stat);
        return node;
    }
}

AST_node* CParser::label_statement() {
    AST_node* node;
    int idx = index;
    if (index >= Token.size()){
        return nullptr;
    }
    if (Token[index].type == "identifier") {
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        AST_node* id = new AST_node;
        id->type = Token[index-1].content;
		id->token_index = ii;
        if (index >= Token.size()){
            index = idx;
            return nullptr;
        }
        if (Token[index].content == ":") {
			int iii = index;
            index++;
            errortokenP = max(index,errortokenP);
            AST_node* colon = new AST_node;
            colon->type = ":";
			colon->token_index = iii;
            auto stat = statement();
            if (stat == nullptr) {
                index = idx;
                free(id);
                free(colon);
                return nullptr;
            }
            else {
                node = new AST_node;
                node->type = "label_statement";
                node->ptr_vec.push_back(id);
                node->ptr_vec.push_back(colon);
                node->ptr_vec.push_back(stat);
                return node;            
            }
        }
        else {
            index = idx;
            free(id);
            return nullptr;
        }
    }
    else{
        index = idx;
        return nullptr;
    }
}

AST_node* CParser::compound_statement() {
    AST_node* node;
    int idx = index;
    if (index >= Token.size()){
        return nullptr;
    }
    if (Token[index].content == "{"||Token[index].content == "<%") {
		string_t ttt = Token[index].content;
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        auto blck_item_list = block_item_list();
        if (index >= Token.size()) {
            return nullptr;
        }
        if (Token[index].content == "}"||Token[index].content == "%>") {
			index++;
            errortokenP = max(index,errortokenP);
            node = new AST_node;
            node->type = "compound_statement";
            AST_node* left_brac = new AST_node;
            AST_node* right_brac = new AST_node;
            left_brac->type = ttt;
			left_brac->token_index = ii;
            right_brac->type = Token[index-1].content;
			right_brac->token_index = index-1;
            node->ptr_vec.push_back(left_brac);
            if (blck_item_list != nullptr)
                node->ptr_vec.push_back(blck_item_list);
            node->ptr_vec.push_back(right_brac);
            return node;
        }
        else {
            index = idx;
            return nullptr;
        }
    }
    else {
        index = idx;
        return nullptr;
    }
}

AST_node* CParser::block_item_list() {
    AST_node* node;
    int idx = index;

    auto blck_item = block_item();
    if (blck_item == nullptr) {
        index = idx;
        return nullptr;
    }
    else {
        node = new AST_node;
        node->type = "block_item_list";
        node->ptr_vec.push_back(blck_item);
    }
    while (blck_item != nullptr) {
        blck_item = block_item();
        if (blck_item != nullptr) {
            node->ptr_vec.push_back(blck_item);
        }
    }
    return node;
}

//Expressions
AST_node* CParser::multiplicative_expression() {
    AST_node* node;
    int idx = index;

    auto una_expr = unary_expression();
    if (una_expr == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "multiplicative_expression";
    node->ptr_vec.push_back(una_expr);
    AST_node* mul = new AST_node;
    mul->type = "*";

    if (index >= Token.size()){
        if(node->ptr_vec.size()>1)
				return node;
			else{
				if(node->ptr_vec[0]->ptr_vec.size()==1){
					return node->ptr_vec[0]->ptr_vec[0];
				} else{
					return node->ptr_vec[0];
				}
			}
    }
    while (Token[index].content == "*") {
        idx = index;
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        una_expr = unary_expression();
        if (una_expr == nullptr) {
            index = idx;
            if(node->ptr_vec.size()>1)
				return node;
			else{
				if(node->ptr_vec[0]->ptr_vec.size()==1){
					return node->ptr_vec[0]->ptr_vec[0];
				} else{
					return node->ptr_vec[0];
				}
			}
        }
		mul->token_index = ii;
        node->ptr_vec.push_back(mul);
        node->ptr_vec.push_back(una_expr);
        if (index >= Token.size()){
            if(node->ptr_vec.size()>1)
				return node;
			else{
				if(node->ptr_vec[0]->ptr_vec.size()==1){
					return node->ptr_vec[0]->ptr_vec[0];
				} else{
					return node->ptr_vec[0];
				}
			}
		}
    }
	if(node->ptr_vec.size()>1)
    	return node;
	else{
		if(node->ptr_vec[0]->ptr_vec.size()==1){
			return node->ptr_vec[0]->ptr_vec[0];
		} else{
			return node->ptr_vec[0];
		}
	}
}

AST_node* CParser::additive_expression() {
    AST_node* node;
    int idx = index, flag;//1:+ 2:-

    auto mul_expr = multiplicative_expression();
    if (mul_expr == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "additive_expression";
    node->ptr_vec.push_back(mul_expr);
    AST_node* add = new AST_node;
    AST_node* sub = new AST_node;
    add->type = "+";
    sub->type = "-";

    if (index >= Token.size()){
        return node;
    }
    while (Token[index].content == "+" || Token[index].content == "-") {
		int ii = index;
        idx = index;
        index++;
        errortokenP = max(index,errortokenP);
        if (Token[index-1].content == "+") flag = 1;
        else flag = 2;
        mul_expr = multiplicative_expression();
        if (mul_expr == nullptr) {
            index = idx;
            if(node->ptr_vec.size() >1)
				return node;
			else{
				if(node->ptr_vec[0]->ptr_vec.size()>1)
					return node->ptr_vec[0];
				else
					return node->ptr_vec[0]->ptr_vec[0];
			}
        }
		add->token_index = ii;
		sub->token_index = ii;
        if (flag == 1) node->ptr_vec.push_back(add);
        else node->ptr_vec.push_back(sub);
        node->ptr_vec.push_back(mul_expr);

        if (index >= Token.size()){
            if(node->ptr_vec.size() >1)
				return node;
			else{
				if(node->ptr_vec[0]->ptr_vec.size()>1)
					return node->ptr_vec[0];
				else
					return node->ptr_vec[0]->ptr_vec[0];
			}
        }
    }
	if(node->ptr_vec.size() > 1)
    	return node;
	else{
		if(node->ptr_vec[0]->ptr_vec.size()>1)
			return node->ptr_vec[0];
		else
			return node->ptr_vec[0]->ptr_vec[0];
	}

}

AST_node* CParser::relational_expression() {
    AST_node* node;
    int idx = index;

    auto add_expr = additive_expression();
    if (add_expr == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "relational_expression";
    node->ptr_vec.push_back(add_expr);
    AST_node* comp = new AST_node;
    comp->type = "<";
    
    if (index >= Token.size()){
        if(node->ptr_vec.size() >1)
			return node;
		else{
			return node->ptr_vec[0];
		}
    }
    while (Token[index].content == "<") {
        idx = index;
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        add_expr = additive_expression();
        if (add_expr == nullptr) {
            index = idx;
            if(node->ptr_vec.size() >1)
				return node;
			else{
				return node->ptr_vec[0];
			}
        }
		comp->token_index = ii;
        node->ptr_vec.push_back(comp);
        node->ptr_vec.push_back(add_expr);
        if (index >= Token.size()){
            if(node->ptr_vec.size() >1)
				return node;
			else{
				return node->ptr_vec[0];
			}
        }
    }
	if(node->ptr_vec.size() >1)
    	return node;
	else{
		return node->ptr_vec[0];
	}
}

AST_node* CParser::equality_expression() {
    AST_node* node;
    int idx = index, flag;//1:== 2:!=

    auto rel_expr = relational_expression();
    if (rel_expr == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "equality_expression";
    node->ptr_vec.push_back(rel_expr);
    AST_node* equa = new AST_node;
    AST_node* ineq = new AST_node;
    equa->type = "==";
    ineq->type = "!=";  

    if (index >= Token.size()){
        if(node->ptr_vec.size() >1)
			return node;
		else{
			return node->ptr_vec[0];
		}
    }
    while (Token[index].content == "==" || Token[index].content == "!=") {
        idx = index;
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        if (Token[index-1].content == "==") flag = 1;
        else flag = 2;
        rel_expr = relational_expression();
        if (rel_expr == nullptr) {
            index = idx;
            if(node->ptr_vec.size() >1)
				return node;
			else{
				return node->ptr_vec[0];
			}
        }
		equa->token_index = ii;
		ineq->token_index = ii;
        if (flag == 1) node->ptr_vec.push_back(equa);
        else node->ptr_vec.push_back(ineq);
        node->ptr_vec.push_back(rel_expr);

        if (index >= Token.size()){
            if(node->ptr_vec.size() >1)
				return node;
			else{
				return node->ptr_vec[0];
			}
        }
    }
    if(node->ptr_vec.size() >1)
    	return node;
	else{
		return node->ptr_vec[0];
	}
}

AST_node* CParser::logical_AND_expression() {
    AST_node* node;
    int idx = index;

    auto equ_expr = equality_expression();
    if (equ_expr == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "logical_AND_expression";
    node->ptr_vec.push_back(equ_expr);
    AST_node* andd = new AST_node;
    andd->type = "&&";

    if(index >= Token.size()){
        if(node->ptr_vec.size() >1)
			return node;
		else{
			return node->ptr_vec[0];
		}
    }

    while (Token[index].content == "&&") {
        idx = index;
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        equ_expr = equality_expression();
        if (equ_expr == nullptr) {
            index = idx;
            if(node->ptr_vec.size() >1)
    			return node;
			else{
				return node->ptr_vec[0];
			}
        }
		andd->token_index = ii;
        node->ptr_vec.push_back(andd);
        node->ptr_vec.push_back(equ_expr);
        if(index >= Token.size()){
            if(node->ptr_vec.size() >1)
    			return node;
			else{
				return node->ptr_vec[0];
			}
        }
    }
    if(node->ptr_vec.size() >1)
    	return node;
	else{
		return node->ptr_vec[0];
	}
}

AST_node* CParser::logical_OR_expression() {
    AST_node* node;
    int idx = index;

    auto log_and_expr = logical_AND_expression();
    if (log_and_expr == nullptr) {
        index = idx;
        return nullptr;
    }
    node = new AST_node;
    node->type = "logical_OR_expression";
    node->ptr_vec.push_back(log_and_expr);
    AST_node* oror = new AST_node;
    oror->type = "||";

    if(index >= Token.size()){
        return node;
    }

    while (Token[index].content == "||") {
        idx = index;
		int ii = index;
        index++;
        errortokenP = max(index,errortokenP);
        log_and_expr = logical_AND_expression();
        if (log_and_expr == nullptr) {
            index = idx;
            if(node->ptr_vec.size() >1)
    			return node;
			else{
				return node->ptr_vec[0];
			}
        }
		oror->token_index = ii;
        node->ptr_vec.push_back(oror);
        node->ptr_vec.push_back(log_and_expr);
        if(index >= Token.size()){
            if(node->ptr_vec.size() >1)
    			return node;
			else{
				return node->ptr_vec[0];
			}
        }
    }
    if(node->ptr_vec.size() >1)
    	return node;
	else
		return node->ptr_vec[0];
}

AST_node* CParser::conditional_expression() {
    AST_node* node = new AST_node;
    node->type = "conditional_expression";
    int idx = index;
    AST_node* ques = new AST_node;
    AST_node* colon = new AST_node;
    ques->type = "?";
    colon->type = ":";

    auto log_or_expr = logical_OR_expression();
    if (log_or_expr == nullptr) {
        index = idx;
        free(node);
        return nullptr;
    }
    node->ptr_vec.push_back(log_or_expr);
    if(index >= Token.size()){
        if(node->ptr_vec.size() >1)
			return node;
		else
			return node->ptr_vec[0];
    }
    if (Token[index].content == "?") {
        idx = index;
        index++;
        errortokenP = max(index,errortokenP);
        auto asgn_expr = assignment_expression();
        if (asgn_expr == nullptr) {
            index = idx;
            return node;
        }
        if(index >= Token.size()){
            index = idx;
            if(node->ptr_vec.size() >1)
				return node;
			else
				return node->ptr_vec[0];
        }
        if (Token[index].content == ":") {
            index++;
            errortokenP = max(index,errortokenP);
            auto cond_expr = conditional_expression();
            if (cond_expr == nullptr) {
                index = idx;
                if(node->ptr_vec.size() >1)
					return node;
				else
					return node->ptr_vec[0];
            }
            node->ptr_vec.push_back(ques);
            node->ptr_vec.push_back(asgn_expr);
            node->ptr_vec.push_back(colon);
            node->ptr_vec.push_back(cond_expr);
            if(node->ptr_vec.size() >1)
				return node;
			else
				return node->ptr_vec[0];
        }
        else {
            index = idx;
            if(node->ptr_vec.size() >1)
				return node;
			else
				return node->ptr_vec[0];
        }
    }
    else {
		if(node->ptr_vec.size() >1)
			return node;
		else
			return node->ptr_vec[0];
	}
}


AST_node* CParser::translation_unit(){
	int idx = index;
	auto ex_node = external_declaration();
	if(ex_node != NULL){
		AST_node* node;
		node = new AST_node;
		node->type = "translation_unit";
		while(ex_node != NULL){
			node->ptr_vec.push_back(ex_node);
			ex_node = external_declaration();
		}
		return node;
	} else{
		index = idx;
		return nullptr;
	}
}

AST_node* CParser::external_declaration(){
	int idx = index;
	auto fd_node = function_definition();
	if(fd_node != NULL){
		AST_node* node;
		node = new AST_node;
		node->type = "external_declaration";
		node->ptr_vec.push_back(fd_node);
		return node;
	}
	index = idx;
	auto de_node = declaration();
	if(de_node != NULL){
		AST_node* node;
		node = new AST_node;
		node->type = "external_declaration";
		node->ptr_vec.push_back(de_node);
		return node;
	}
	index = idx;
	return nullptr;
}

AST_node* CParser::declaration(){
	int idx = index;
	auto dec_spe = declaration_specifiers();
	if(dec_spe == NULL){
		index = idx;
		return nullptr;
	}
	auto dec = declarator();
	if(index >= Token.size()||Token[index].content != ";"){
		index = idx;
		return nullptr;
	}
	int ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	AST_node *node;
	node = new AST_node;
	node->type = "declaration";
	node->ptr_vec.push_back(dec_spe);
	if(dec != nullptr)
		node->ptr_vec.push_back(dec);
	node->ptr_vec.push_back(creatnew(";",ii));
	return node;
}

AST_node* CParser::type_specifier(){
	int idx = index;
	if(index >= Token.size())
		return nullptr;
	auto current_token = Token[index];
	if(current_token.content == "void"||current_token.content == "char"||current_token.content == "int"){
		AST_node *temp;
		temp = new AST_node;
		temp->type = current_token.content;
		temp->token_index = index;
		AST_node *node;
		node = new AST_node;
		node -> type = "type_specifier";
		node -> ptr_vec.push_back(temp);
		index++;
		errortokenP = max(index,errortokenP);
		return node;
	}
	auto sous = struct_or_union_specifier();
	if(sous != NULL){
		AST_node* node;
		node = new AST_node;
		node->type = "type_specifier";
		node->ptr_vec.push_back(sous);
		return node;
	} else{
		index = idx;
		return nullptr;
	}
}

AST_node* CParser::struct_or_union_specifier(){
	int idx = index;
	if(index >= Token.size())
		return nullptr;
	auto current_token = Token[index];
	if(current_token.content == "struct"){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		if(index >= Token.size()){
			index = idx;
			return nullptr;
		}
		auto current_token1 = Token[index];
		if(current_token1.type == "identifier"){
			int iii = index;
			index++;
			errortokenP = max(index,errortokenP);
			idx = index;
			if(index >= Token.size()||(Token[index].content != "{"&&Token[index].content != "<%")){
				index = idx;
                AST_node *stru;
				stru = new AST_node;
				stru->type = "struct";
				stru->token_index = ii;
				AST_node *identif;
				identif = new AST_node;
				identif->type = current_token1.content;
				identif->token_index = iii;
				AST_node *node;
				node = new AST_node;
				node->type = "struct_or_union_specifier";
				node->ptr_vec.push_back(stru);
				node->ptr_vec.push_back(identif);
				return node;
			}
			int iiii = index;
			string_t ttt = Token[index].content;
			index++;
			errortokenP = max(index,errortokenP);
			auto sdl = struct_declaration_list();
			if(sdl==NULL){
				index = idx;
				AST_node *stru;
				stru = new AST_node;
				stru->type = "struct";
				stru->token_index = ii;
				AST_node *identif;
				identif = new AST_node;
				identif->type = current_token1.content;
				identif->token_index = iii;
				AST_node *node;
				node = new AST_node;
				node->type = "struct_or_union_specifier";
				node->ptr_vec.push_back(stru);
				node->ptr_vec.push_back(identif);
				return node;
			}
			if(index >= Token.size()||(Token[index].content != "}"&&Token[index].content != "%>")){
				index = idx;
				AST_node *stru;
				stru = new AST_node;
				stru->type = "struct";
				stru->token_index = ii;
				AST_node *identif;
				identif = new AST_node;
				identif->type = current_token1.content;
				identif->token_index = iii;
				AST_node *node;
				node = new AST_node;
				node->type = "struct_or_union_specifier";
				node->ptr_vec.push_back(stru);
				node->ptr_vec.push_back(identif);
				return node;
			}
			else{
				int iiiii = index;
				index++;
				errortokenP = max(index,errortokenP);
				AST_node *stru;
				stru = new AST_node;
				stru->type = "struct";
				stru->token_index = ii;
				AST_node *identif;
				identif = new AST_node;
				identif->type = current_token1.content;
				identif->token_index = iii;
				AST_node *l;
				l = new AST_node;
				l->type = ttt;
				l->token_index = iiii;
				AST_node *r;
				r = new AST_node;
				r->type = Token[index-1].content;
				r->token_index = iiiii;
				AST_node *node;
				node = new AST_node;
				node->type = "struct_or_union_specifier";
				node->ptr_vec.push_back(stru);
				node->ptr_vec.push_back(identif);
				node->ptr_vec.push_back(l);
				node->ptr_vec.push_back(sdl);
				node->ptr_vec.push_back(r);
				return node;
			}
		} else{
			if(current_token1.content == "{"||current_token1.content == "<%"){
				int iii = index;
				index++;
				errortokenP = max(index,errortokenP);
				if(index>=Token.size()){
					index = idx;
					return nullptr;
				}
				auto sdl = struct_declaration_list();
				if(sdl == NULL){
					index = idx;
					return nullptr;
				}
				if(index >= Token.size()){
					index = idx;
					return nullptr;
				}
				if(Token[index].content == "}"||Token[index].content == "%>"){
					int iiii = index;
					index++;
					errortokenP = max(index,errortokenP);
					AST_node *stru;
					stru = new AST_node;
					stru->type = "struct";
					stru->token_index = ii;
					AST_node *l;
					l = new AST_node;
					l->type = current_token1.content;
					l->token_index = iii;
					AST_node *r;
					r = new AST_node;
					r->type = Token[index-1].content;
					r->token_index = iiii;
					AST_node *node;
					node = new AST_node;
					node->type = "struct_or_union_specifier";
					node->ptr_vec.push_back(stru);
					node->ptr_vec.push_back(l);
					node->ptr_vec.push_back(sdl);
					node->ptr_vec.push_back(r);
					return node;
				} else {
					index = idx;
					return nullptr;
				}

			}
			else{
				index = idx;
				return nullptr;
			}
		}
	}else{
		return nullptr;
	}
}

AST_node* CParser::struct_declaration_list(){
	int idx = index;
	auto sd = struct_declaration();
	if(sd==NULL){
		index = idx;
		return nullptr;
	}
	AST_node *node;
	node = new AST_node;
	node->type = "struct_declaration_list";
	idx = index;
	while(sd!=NULL){
		node->ptr_vec.push_back(sd);
		sd = struct_declaration();
	}
	return node;
}

AST_node* CParser::struct_declaration(){
	int idx = index;
	auto sql = specifier_qualifier_list();
	if(sql == NULL){
		index = idx;
		return nullptr;
	}
	auto sdl = struct_declarator_list();
	if(sdl == NULL){
		if(index >= Token.size()||Token[index].content != ";"){
			index = idx;
			return nullptr;
		}
		int ii = index;
		AST_node *node;
		node = new AST_node;
		node->type = "struct_declaration";
		node->ptr_vec.push_back(sql);
		AST_node *fen;
		fen = new AST_node;
		fen->type = ";";
		fen->token_index = ii;
		node->ptr_vec.push_back(fen);
		index++;
		errortokenP = max(index,errortokenP);
		return node;
	}else{
		if(index >= Token.size()||Token[index].content != ";"){
			index = idx;
			return nullptr;
		}
		int ii = index;
		AST_node *node;
		node = new AST_node;
		node->type = "struct_declaration";
		node->ptr_vec.push_back(sql);
		node->ptr_vec.push_back(sdl);
		AST_node *fen;
		fen = new AST_node;
		fen->type = ";";
		fen->token_index = index;
		node->ptr_vec.push_back(fen);
		index++;
		errortokenP = max(index,errortokenP);
		return node;
	}
}

AST_node* CParser::specifier_qualifier_list(){
	int idx = index;
	auto tsp = type_specifier();
	if(tsp == NULL){
		index = idx;
		return nullptr;
	}
	idx = index;
	auto sql = specifier_qualifier_list();
	AST_node *node;
	node = new AST_node;
	node->type = "specifier_qualifier_list";
	node->ptr_vec.push_back(tsp);
	if(sql == NULL){
		index = idx;
		return node;
	}
	node->ptr_vec.push_back(sql);
	return node;

}

AST_node* CParser::struct_declarator_list(){
	int idx = index;
	auto sde = struct_declarator();
	if(sde == NULL){
		index = idx;
		return nullptr;
	}
	AST_node *node;
	node = new AST_node;
	node->type = "struct_declarator_list";
	node->ptr_vec.push_back(sde);
	idx = index;
	while(1){
		idx = index;
		if(index >= Token.size()||Token[index].content != ","){
			index = idx;
			break;
		}
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		sde = struct_declarator();
		if(sde == NULL){
			index = idx;
			break;
		}
		node->ptr_vec.push_back(creatnew(",",ii));
		node->ptr_vec.push_back(sde);
	}
	return node;
}

AST_node* CParser::struct_declarator(){
	int idx = index;
	auto dec = declarator();
	if(dec == NULL){
		if(index >= Token.size()||Token[index].content != ":"){
			index = idx;
			return nullptr;
		}
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto ce = conditional_expression();
		if(ce==NULL){
			index = idx;
			return nullptr;
		}
		AST_node* node;
		node = new AST_node;
		node->ptr_vec.push_back(creatnew(":",ii));
		node->ptr_vec.push_back(ce);
		return node;
	} else {
		idx = index;
		AST_node *node;
		node = new AST_node;
		node->type = "struct_declarator";
		node->ptr_vec.push_back(dec);
		if(index >= Token.size()||Token[index].content != ":"){
			return node;
		}
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto ce = conditional_expression();
		if(ce == NULL){
			index = idx;
			return node;
		}
		AST_node* dou;
		dou = new AST_node;
		dou->type = ":";
		dou->token_index = ii;
		node->ptr_vec.push_back(dou);
		node->ptr_vec.push_back(ce);
		return node;
	}
}

AST_node* CParser::block_item(){
	int idx = index;
	auto de = declaration();
	if(de != NULL){
		AST_node *node;
		node = new AST_node;
		node->type = "block_item";
		node->ptr_vec.push_back(de);
		return node;
	}
	index = idx;
	auto state = statement();
	if(state != NULL){
		AST_node *node;
		node = new AST_node;
		node->type = "block_item";
		node->ptr_vec.push_back(state);
		return node;
	}
	index = idx;
	return nullptr;

}

AST_node* CParser::expression_statement(){
	int idx = index;
	auto expre = assignment_expression();
	if(expre == NULL){
		index = idx;
		if(index >= Token.size()||Token[index].content != ";"){
			index = idx;
			return nullptr;
		}
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node *fen;
		fen = new AST_node;
		fen->type = ";";
		fen->token_index = ii;
		AST_node* node;
		node = new AST_node;
		node->type = "expression_statement";
		node->ptr_vec.push_back(fen);
		return node;
	}
	if(index >= Token.size()||Token[index].content != ";"){
		index = idx;
		return nullptr;
	}
	int ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	AST_node *fen;
	fen = new AST_node;
	fen->type = ";";
	fen->token_index = ii;
	AST_node* node;
	node = new AST_node;
	node->type = "expression_statement";
	node->ptr_vec.push_back(expre);
	node->ptr_vec.push_back(fen);
	return node;
}

AST_node* CParser::selection_statement(){
	int idx = index;
	if(index >= Token.size()||Token[index].content != "if"){
		index = idx;
		return nullptr;
	}
	int ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	if(index >= Token.size()||Token[index].content != "("){
		index = idx;
		return nullptr;
	}
	int iii = index;
	index++;
	errortokenP = max(index,errortokenP);
	auto expre = assignment_expression();
	if(expre == NULL){
		index = idx;
		return nullptr;
	}
	if(index >= Token.size()||Token[index].content != ")"){
		index = idx;
		return nullptr;
	}
	int iiii = index;
	index++;
	errortokenP = max(index,errortokenP);
	auto state = statement();
	if(state == NULL){
		index = idx;
		return nullptr;
	}
	idx = index;
	AST_node* node;
	node = new AST_node;
	node->type = "selection_statement";
	node->ptr_vec.push_back(creatnew("if",ii));
	node->ptr_vec.push_back(creatnew("(",iii));
	node->ptr_vec.push_back(expre);
	node->ptr_vec.push_back(creatnew(")",iiii));
	node->ptr_vec.push_back(state);
	if(index >= Token.size()||Token[index].content != "else"){
		index = idx;
		return node;
	}
	ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	auto state1 = statement();
	if(state1 == NULL){
		index = idx;
		return node;
	}
	node->ptr_vec.push_back(creatnew("else",ii));
	node->ptr_vec.push_back(state1);
	return node;
}

AST_node* CParser::iteration_statement(){
	int idx = index;
	if(index >= Token.size()||Token[index].content != "while"){
		index = idx;
		return nullptr;
	}
	int ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	if(index >= Token.size()||Token[index].content != "("){
		index = idx;
		return nullptr;
	}
	int iii = index;
	index++;
	errortokenP = max(index,errortokenP);
	auto expre = assignment_expression();
	if(expre == NULL){
		index = idx;
		return nullptr;
	}
	if(index >= Token.size()||Token[index].content != ")"){
		index = idx;
		return nullptr;
	}
	int iiii = index;
	index++;
	errortokenP = max(index,errortokenP);
	auto state = statement();
	if(state == NULL){
		index = idx;
		return nullptr;
	}
	AST_node *node;
	node = new AST_node;
	node->type = "iteration_statement";
	node->ptr_vec.push_back(creatnew("while",ii));
	node->ptr_vec.push_back(creatnew("(",iii));
	node->ptr_vec.push_back(expre);
	node->ptr_vec.push_back(creatnew(")",iiii));
	node->ptr_vec.push_back(state);
	return node;
}

AST_node* CParser::jump_statement(){
	int idx = index;
	if(index >= Token.size()){
		index = idx;
		return nullptr;
	}
	auto current_token = Token[index];
	int ii = index;
	if(current_token.content == "continue"||current_token.content == "break"){
		index++;
		errortokenP = max(index,errortokenP);
		if(index >= Token.size()||Token[index].content != ";"){
			index = idx;
			return nullptr;
		}
		int iii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node* node;
		node = new AST_node;
		node->type = "jump_statement";
		node->ptr_vec.push_back(creatnew(current_token.content,ii));
		node->ptr_vec.push_back(creatnew(";",iii));
		return node;
	} 
	else if(current_token.content == "goto"){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		if(index >= Token.size()||Token[index].type != "identifier"){
			index = idx;
			return nullptr;
		}
		int iii = index;
		index++;
		errortokenP = max(index,errortokenP);
		if(index >= Token.size()||Token[index].content != ";"){
			index = idx;
			return nullptr;
		}
		int iiii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node* node;
		node = new AST_node;
		node->type = "jump_statement";
		node->ptr_vec.push_back(creatnew(current_token.content,ii));
		node->ptr_vec.push_back(creatnew(Token[index-2].content,iii));
		node->ptr_vec.push_back(creatnew(";",iiii));
		return node;
	} 
	else if(current_token.content == "return"){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto expre = assignment_expression();
		if(index >= Token.size()||Token[index].content != ";"){
			index = idx;
			return nullptr;
		}
		int iii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node *node;
		node = new AST_node;
		node->type = "jump_statement";
		node->ptr_vec.push_back(creatnew(current_token.content,ii));
		if(expre != NULL)
			node->ptr_vec.push_back(expre);
		node->ptr_vec.push_back(creatnew(";",iii));
		return node;
	} 
	else {
		index = idx;
		return nullptr;
	}
}


AST_node* CParser::assignment_expression(){
	int idx = index;
	auto ue = unary_expression();
	if(ue == NULL){
		index = idx;
		auto cexpres = conditional_expression();
		if(cexpres == NULL){
			index = idx;
			return nullptr;
		}
		AST_node *node;
		node = new AST_node;
		node->type = "assignment_expression";
		node->ptr_vec.push_back(cexpres);
		if(node->ptr_vec.size()>1)
			return node;
		else
			return node->ptr_vec[0];
	}
	if(index >= Token.size()||Token[index].content != "="){
		index = idx;
		auto cexpres = conditional_expression();
		if(cexpres == NULL){
			index = idx;
			return nullptr;
		}
		AST_node *node;
		node = new AST_node;
		node->type = "assignment_expression";
		node->ptr_vec.push_back(cexpres);
		if(node->ptr_vec.size()>1)
			return node;
		else
			return node->ptr_vec[0];		
	}
	int ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	auto ae = assignment_expression();
	if(ae == NULL){
		index = idx;
		auto cexpres = conditional_expression();
		if(cexpres == NULL){
			index = idx;
			return nullptr;
		}
		AST_node *node;
		node = new AST_node;
		node->type = "assignment_expression";
		node->ptr_vec.push_back(cexpres);
		if(node->ptr_vec.size()>1)
			return node;
		else
			return node->ptr_vec[0];
	}
	AST_node *node;
	node = new AST_node;
	node->type = "assignment_expression";
	node->ptr_vec.push_back(ue);
	node->ptr_vec.push_back(creatnew("=",ii));
	node->ptr_vec.push_back(ae);
	if(node->ptr_vec.size()>1)
		return node;
	else
		return node->ptr_vec[0];
}

AST_node* CParser::primary_expresstion(){
	int idx = index;
	if(index >= Token.size()){
		return nullptr;
	}
	if(Token[index].type == "identifier"||(Token[index].type == "constant"&&Token[index].content != "08"&&Token[index].content != "09")||Token[index].type == "string-literal"){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node *node;
		node = new AST_node;
		node->type = "primary_expresstion";
		node->ptr_vec.push_back(creatnew(Token[index-1].content,ii));
		return node;
	}
	else if(Token[index].content == "("){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto expre = assignment_expression();
		if(expre == NULL){
			index = idx;
			return nullptr;
		}
		if(index >= Token.size()||Token[index].content != ")"){
			index = idx;
			return nullptr;
		}
		int iii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node *node;
		node = new AST_node;
		node->type = "primary_expresstion";
		if((expre->ptr_vec.size() == 3&&expre->ptr_vec[1]->ptr_vec.size()>0)||expre->type == "primary_expresstion"){
			return expre;
		}
		node->ptr_vec.push_back(creatnew("(",ii));
		node->ptr_vec.push_back(expre);
		node->ptr_vec.push_back(creatnew(")",iii));
		return node;
	}
	else{
		index = idx;
		return nullptr;
	}
}

AST_node* CParser::postfix_expression(){
	int idx = index;
	auto pe = primary_expresstion();
	if(pe == NULL){
		index = idx;
		return nullptr;
	}
	idx = index;
	AST_node* node;
	node = new AST_node;
	node->type = "postfix_expression";
	node->ptr_vec.push_back(pe);
	while(1){
		idx = index;
		auto temp = subpostfix_expression();
		if(temp.size() == 0){
			index = idx;
			break;
		}
		for(int i = 0;i < temp.size();i++){
			node->ptr_vec.push_back(temp[i]);
		}
	}
	if(node->ptr_vec.size()>1)
		return node;
	else 
		return node->ptr_vec[0];
}

vector<AST_node*> CParser::subpostfix_expression(){
	vector<AST_node*> result;
	int idx = index;
	if(index >= Token.size()){
		index = idx;
		return result;
	}
	if(Token[index].content == "++"||Token[index].content == "--"){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		result.push_back(creatnew(Token[index-1].content,ii));
		return result;
	}
	else if(Token[index].content == "."||Token[index].content == "->"){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		if(index >= Token.size()){
			index = idx;
			return result;
		}
		if(Token[index].type != "identifier"){
			index = idx;
			return result;
		}
		index++;
		errortokenP = max(index,errortokenP);
		result.push_back(creatnew(Token[index-2].content,ii));
		result.push_back(creatnew(Token[index-1].content,index-1));
		return result;
	}
	else if(Token[index].content == "["||Token[index].content == "<:"){
		string_t ttt = Token[index].content;
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto ae = assignment_expression();
		if(ae == NULL){
			index = idx;
			return result;
		}
		if(index >= Token.size()){
			index = idx;
			return result;
		}
		if(Token[index].content != "]"&&Token[index].content != ":>"){
			index = idx;
			return result;
		}
		index++;
		errortokenP = max(index,errortokenP);
		result.push_back(creatnew(ttt,ii));
		result.push_back(ae);
		result.push_back(creatnew(Token[index-1].content,index-1));
		return result;
	}
	else if(Token[index].content == "("){
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto ael = argument_expression_list();
		if(index >= Token.size()){
			index = idx;
			return result;
		}
		if(Token[index].content != ")"){
			index = idx;
			return result;
		}
		int iii = index;
		index++;
		errortokenP = max(index,errortokenP);
		result.push_back(creatnew("(",ii));
		if(ael != NULL)
			result.push_back(ael);
		result.push_back(creatnew(")",iii));
		return result;
	}
	else{
		index = idx;
		return result;
	}

}

AST_node* CParser::argument_expression_list(){
	int idx = index;
	auto ae = assignment_expression();
	if(ae == NULL){
		index = idx;
		return nullptr;
	}
	AST_node *node;
	node = new AST_node;
	node->type = "argument_expression_list";
	node->ptr_vec.push_back(ae);
	idx = index;
	while(1){
		idx = index;
		if(index >= Token.size()||Token[index].content != ","){
			index = idx;
			break;
		}
		int ii = index;
		index++;
		errortokenP = max(index,errortokenP);
		ae = assignment_expression();
		if(ae == NULL){
			index = idx;
			break;
		}
		node->ptr_vec.push_back(creatnew(",",ii));
		node->ptr_vec.push_back(ae);
	}
	return node;
}	

AST_node* CParser::unary_expression(){
	int idx = index;
	auto pe = postfix_expression();
	if(pe != NULL){
		AST_node* node;
		node = new AST_node;
		node->type = "unary_expression";
		node->ptr_vec.push_back(pe);
		return node;
	}
	index = idx;
	auto uo = unary_operator();
	if(uo != NULL){
		auto ue = unary_expression();
		if(ue == NULL){
			index = idx;
			return nullptr;
		}
		AST_node* node;
		node = new AST_node;
		node->type = "unary_expression";
		node->ptr_vec.push_back(uo);
		node->ptr_vec.push_back(ue);
		return node;
	}
	if(index >= Token.size()||Token[index].content != "sizeof"){
		index = idx;
		return nullptr;
	}
	int ii = index;
	index++;
	errortokenP = max(index,errortokenP);
	if(index >= Token.size()){
		index = idx;
		return nullptr;
	}
	if(Token[index].content == "("){
		int iii = index;
		index++;
		errortokenP = max(index,errortokenP);
		auto tn = type_name();
		if(tn == NULL){
			index = idx+1;
			auto ue = unary_expression();
			if(ue == NULL){
				index = idx;
				return nullptr;
			}
			AST_node* node;
			node = new AST_node;
			node->type = "unary_expression";
			node->ptr_vec.push_back(creatnew("sizeof",ii));
			node->ptr_vec.push_back(ue);
			return node;
		}
		if(index >= Token.size()){
			index = idx+1;
			auto ue = unary_expression();
			if(ue == NULL){
				index = idx;
				return nullptr;
			}
			AST_node* node;
			node = new AST_node;
			node->type = "unary_expression";
			node->ptr_vec.push_back(creatnew("sizeof",ii));
			node->ptr_vec.push_back(ue);
			return node;
		}
		if(Token[index].content != ")"){
			index = idx+1;
			auto ue = unary_expression();
			if(ue == NULL){
				index = idx;
				return nullptr;
			}
			AST_node* node;
			node = new AST_node;
			node->type = "unary_expression";
			node->ptr_vec.push_back(creatnew("sizeof",ii));
			node->ptr_vec.push_back(ue);
			return node;
		}
		int iiii = index;
		index++;
		errortokenP = max(index,errortokenP);
		AST_node* node;
		node = new AST_node;
		node->type = "unary_expression";
		node->ptr_vec.push_back(creatnew("sizeof",ii));
		node->ptr_vec.push_back(creatnew("(",iii));
		node->ptr_vec.push_back(tn);
		node->ptr_vec.push_back(creatnew(")",iiii));
		return node;

	} else{
		auto ue = unary_expression();
		if(ue == NULL){
			index = idx;
			return nullptr;
		}
		AST_node* node;
		node = new AST_node;
		node->type = "unary_expression";
		node->ptr_vec.push_back(creatnew("sizeof",ii));
		node->ptr_vec.push_back(ue);
		return node;
	}

}

AST_node* CParser::unary_operator(){
	int idx = index;
	if(index >= Token.size()||(Token[index].content != "&"
		&&Token[index].content != "*"&&Token[index].content != "-"
		&&Token[index].content != "!")){
		return nullptr;
	}
	index++;
	errortokenP = max(index,errortokenP);
	return creatnew(Token[index-1].content,index-1); 
}

AST_node* CParser::type_name(){
    int idx = index;
    auto sql= specifier_qualifier_list();
    if(sql == NULL){
        index = idx;
        return nullptr;
    }
    AST_node* node;
    node = new AST_node;
    node->type = "type_name";
    node->ptr_vec.push_back(sql);
    auto ad = abstract_declarator();
    if(ad != nullptr){
        node->ptr_vec.push_back(ad);
    }
    return node;
}
