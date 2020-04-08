#include "stdafx.h"
#include "SA.h"
#include <string>

void SA::del_paren(AST_node* node) {
    if(node->ptr_vec.size()==0) return;
    else if(node->type == "declarator"){
        vector<AST_node*> type_point;
        vector<AST_node*> Func_point;
        AST_node* id = new AST_node;
        AST_node* Pa = new AST_node;
        int flag = 0;
        if(node->ptr_vec.size() == 2){//p + dd
            for(int i = 0;i < node->ptr_vec[0]->ptr_vec.size();i++)
                type_point.push_back(node->ptr_vec[0]->ptr_vec[i]);
            auto temp = node->ptr_vec[1];//dd
            while(temp->ptr_vec.size() < 5){
                if(temp->ptr_vec.size() ==1||temp->ptr_vec.size() ==4){
                    id = temp;//dd
                    break;
                }
                else{
                    temp = temp->ptr_vec[1];
                    if(temp->ptr_vec.size() == 2){
                        for(int i = 0;i < temp->ptr_vec[0]->ptr_vec.size();i++)
                            type_point.push_back(temp->ptr_vec[0]->ptr_vec[i]);
                        temp = temp->ptr_vec[1];
                    }
                    else{
                        temp = temp->ptr_vec[0];
                    }
                }
            }
            if(id->ptr_vec.size()==0){
                flag = 1;
                Pa = temp;//dd
                while(1){
                    if(temp->ptr_vec.size() ==1||temp->ptr_vec.size() ==4){
                        id = temp;
                        break;
                    }
                    else{
                        temp = temp->ptr_vec[1];
                        if(temp->ptr_vec.size() == 2){
                            for(int i = 0;i < temp->ptr_vec[0]->ptr_vec.size();i++)
                                Func_point.push_back(temp->ptr_vec[0]->ptr_vec[i]);
                            temp = temp->ptr_vec[1];
                        }
                        else{
                            temp = temp->ptr_vec[0];
                        }
                    }
                }
            }
            node->ptr_vec.clear();
            if(type_point.size()>0){
                AST_node* point = new AST_node;
                point->type = "pointer";
                point->ptr_vec = type_point;
                //node->ptr_vec.clear();
                node->ptr_vec.push_back(point);
            }
            if(flag){
                AST_node* dec = new AST_node;
                dec->type = "declarator";
                if(Func_point.size()>0){
                    AST_node* point1 = new AST_node;
                    point1->type = "pointer";
                    point1->ptr_vec = Func_point;
                    dec->ptr_vec.push_back(point1);
                }
                dec->ptr_vec.push_back(id);
                Pa->ptr_vec[1] = dec;
                node->ptr_vec.push_back(Pa);
            }else{
                node->ptr_vec.push_back(id);
            }
            return;

        } else{//dd
            auto temp = node->ptr_vec[0];//dd
            while(temp->ptr_vec.size() < 5){
                if(temp->ptr_vec.size() ==1||temp->ptr_vec.size() ==4){
                    id = temp;
                    break;
                }
                else{
                    //temp = temp->ptr_vec[2];
                    temp = temp->ptr_vec[1];
                    if(temp->ptr_vec.size() == 2){
                        for(int i = 0;i < temp->ptr_vec[0]->ptr_vec.size();i++)
                            type_point.push_back(temp->ptr_vec[0]->ptr_vec[i]);
                        temp = temp->ptr_vec[1];
                    }
                    else{
                        temp = temp->ptr_vec[0];
                    }
                }
            }
            if(id->ptr_vec.size()==0){
                flag = 1;
                Pa = temp;
                while(1){
                    if(temp->ptr_vec.size() ==1||temp->ptr_vec.size() ==4){
                        id = temp;
                        break;
                    }
                    else{
                        temp = temp->ptr_vec[1];
                        if(temp->ptr_vec.size() == 2){
                            for(int i = 0;i < temp->ptr_vec[0]->ptr_vec.size();i++)
                                Func_point.push_back(temp->ptr_vec[0]->ptr_vec[i]);
                            temp = temp->ptr_vec[1];
                        }
                        else{
                            temp = temp->ptr_vec[0];
                        }
                    }
                }
            }
            node->ptr_vec.clear();
            if(type_point.size()>0){
                AST_node* point = new AST_node;
                point->type = "pointer";
                point->ptr_vec = type_point;
                node->ptr_vec.push_back(point);
            }
            if(flag){
                AST_node* dec = new AST_node;
                dec->type = "declarator";
                if(Func_point.size()>0){
                    AST_node* point1 = new AST_node;
                    point1->type = "pointer";
                    point1->ptr_vec = Func_point;
                    dec->ptr_vec.push_back(point1);
                }
                dec->ptr_vec.push_back(id);
                Pa->ptr_vec[1] = dec;
                node->ptr_vec.push_back(Pa);
            }else{
                node->ptr_vec.push_back(id);
            }
            return;
        }
    }
    else{
        for(int i = 0;i < node->ptr_vec.size();i++)
        del_paren(node->ptr_vec[i]);
    }
}

void SA::semantic_analysis() {
    // iterate the ast_tree 
    // new declarator always be in the last 
    int i = 0;

    for (; i < ast_tree->ptr_vec.size(); i++) // first store all external declarations
        check_ext_dec(ast_tree->ptr_vec[i]); // i stands for the number of extermal decs
    //std::cout << ExtDecList.size() << endl;
    
    /* if ExtDecList[i],type is function
       then check_comp_stat

    */
   for (i = 0; i < ExtDecList.size(); i++) {
        if (ExtDecList[i].type == "function")
            check_comp_stat(ast_tree->ptr_vec[i]->ptr_vec[0]->ptr_vec[2], i);
   }
    
}

void SA::check_ext_dec(AST_node* node) { // store in ExcDecList
    // node->type is external_declaration
    // could be function_definition or declaration

    // function are allowed to be overloaded except for main
    if (node->ptr_vec[0]->type == "function_definition") {
        struct ext_dec NewExtDec;
        struct func NewFunc;
        NewExtDec.type = "function";
        NewExtDec.ptr_level = 0;
        //std::cout << "enter func def" << endl;

        // type_specifier
        AST_node* type_node = new AST_node;
        type_node = node->ptr_vec[0]->ptr_vec[0];
        while (type_node->ptr_vec.size() > 0)
            type_node = type_node->ptr_vec[0];
        NewFunc.ret_type = type_node->type;
        //free(type_node);
        AST_node* dec_node = new AST_node;
        dec_node = node->ptr_vec[1];                
        if (dec_node->ptr_vec.size() == 2) {            
            NewFunc.ret_ptr_level = dec_node->ptr_vec.size();
            dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
        }  
        else NewExtDec.ptr_level = 0;
        dec_node = dec_node->ptr_vec[0]; // direct_declarator
        NewExtDec.name = dec_node->ptr_vec[0]->type; // id
        int id_idx = dec_node->ptr_vec[0]->token_index;
        NewFunc.name = dec_node->ptr_vec[0]->type;
        dec_node = dec_node->ptr_vec[2]; // parameter_list
        // if parameter has no name then store "\0"
        while (dec_node->ptr_vec.size() > 0) {
            type_node = dec_node->ptr_vec[0];
            while (type_node->ptr_vec.size() > 0)
                type_node = type_node->ptr_vec[0]; // type
            NewFunc.para_type_list.push_back(type_node->type);
            if (dec_node->ptr_vec[0]->ptr_vec.size() == 2) {
                type_node = dec_node->ptr_vec[0]->ptr_vec[1]; // declarator
                if (type_node->ptr_vec.size() == 2) {
                    NewFunc.para_ptr_level.push_back(type_node->ptr_vec[0]->ptr_vec.size());
                    type_node->ptr_vec.erase(type_node->ptr_vec.begin());
                }
                else NewFunc.para_ptr_level.push_back(0);
                while (type_node->ptr_vec.size() > 0)
                    type_node = type_node->ptr_vec[0]; // id
                // check whether the id name is duplicate
                for (int i = 0; i < NewFunc.para_name_list.size(); i++) {
                    if (type_node->type == NewFunc.para_name_list[i]) {
                        //TODO error
                        //std::cout << "duplicate parameter name!" << endl;
                        //free(type_node);
                        //free(dec_node);
                        err_idx = min(err_idx, type_node->token_index);
                        return;
                    }
                }
                NewFunc.para_name_list.push_back(type_node->type);
            }
            else NewFunc.para_name_list.push_back("\0");
            dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
            if (dec_node->ptr_vec.size() > 0)
                dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
        }
        // std::cout << "para-size = " << NewFunc.para_type_list.size() << endl;
        // check whether it has already been defined or declared.
        NewExtDec.Func = NewFunc;
        bool is_dul = false, is_decled = false;
        int i = 0;
        // function name cannot be duplicate with variable
        //     or function (pointer) with same parameters
        // but are allowed to be overloaded or share name with struct
        for (; i < ExtDecList.size() && !is_dul && !is_decled; i++) {
            if (ExtDecList[i].name == NewExtDec.name) {
                if (ExtDecList[i].type != "function" && ExtDecList[i].type != "function_declaration")
                    is_dul = true;
                else {
                    if (ExtDecList[i].Func.para_type_list.size() == NewFunc.para_type_list.size()) {
                        int j = 0;
                        for (; j < ExtDecList[i].Func.para_type_list.size() && \
                                        ExtDecList[i].Func.para_type_list[j] == NewFunc.para_type_list[j] && \
                                        ExtDecList[i].Func.para_ptr_level == NewFunc.para_ptr_level; j++) {}
                        if (j == ExtDecList[i].Func.para_type_list.size()) {
                            if (ExtDecList[i].type == "function")
                                is_dul = true;
                            else if (ExtDecList[i].type == "function_declaration" && ExtDecList[i].Func.ret_type == NewFunc.ret_type && \
                                                                                     ExtDecList[i].Func.ret_ptr_level == NewFunc.ret_ptr_level)
                                is_decled = true;
                        }
                    }
                }
            }
        }
        if (is_dul) {// check whether it has already been defined.
            // TODO ERROR
            //std::cout << "function definition error!" << endl;
            err_idx = min(err_idx, id_idx);
            return;
            // err_idx = min(index, i - 1);
        }
        else if (is_decled) {
            ExtDecList.push_back(NewExtDec);
            ExtDecList[i-1].dec_pos = ExtDecList.size() - 1;
            //std::cout << "function is declared " << i-1 << " " << ExtDecList[i-1].dec_pos << endl;                
        }
        else ExtDecList.push_back(NewExtDec);
        //free(type_node);
        //free(dec_node);
    }
    else if (node->ptr_vec[0]->type == "declaration") {
        auto type_node = node->ptr_vec[0]->ptr_vec[0]; // declaration_specifier
        struct ext_dec NewExtDec;
        if (node->ptr_vec[0]->ptr_vec.size() == 2 && \
            (type_node->ptr_vec[0]->ptr_vec[0]->type == "int" || \
             type_node->ptr_vec[0]->ptr_vec[0]->type == "char" || \
             type_node->ptr_vec[0]->ptr_vec[0]->type == "void"))
            return; // no need to store it;
        else NewExtDec.name = "\0";
            
        string_t type;
        while (type_node->ptr_vec[0]->type != "char" && type_node->ptr_vec[0]->type != "int" && \
               type_node->ptr_vec[0]->type != "void" && type_node->ptr_vec[0]->type != "struct_or_union_specifier") 
            type_node = type_node->ptr_vec[0];
        if (type_node->ptr_vec[0]->type == "struct_or_union_specifier") {
            if (type_node->ptr_vec[0]->ptr_vec.size() > 2) {
                struct Struct NewStr = struct_construct(type_node->ptr_vec[0]);
                //std::cout << "NewStr.name = " << NewStr.name << endl;
                // if OK then store
                if (NewStr.name != "-1") {
                    NewExtDec.type = "struct";
                    NewExtDec.ptr_level = 0;
                    NewExtDec.Struct = NewStr;
                    if (node->ptr_vec[0]->ptr_vec.size() == 2) { // no declarator
                        ExtDecList.push_back(NewExtDec);
                        return;
                    }
                }                    
                else {
                    // TODO error
                    //std::cout << "struct construction fail" << endl;
                    return;

                }

            }
            else {
                bool is_decled = false;
                // check whether it is declared
                string_t id = type_node->ptr_vec[0]->ptr_vec[1]->type;
                for (int i = 0; i < ExtDecList.size() && !is_decled; i++) {
                    if (ExtDecList[i].type == "struct" && ExtDecList[i].Struct.name == id)
                        is_decled = true;
                }
                if (is_decled)
                    NewExtDec.type = "struct+" + type_node->ptr_vec[0]->ptr_vec[1]->type; 
                else {
                    // TODO error
                    //std::cout << "struct not declared!" << endl;
                    err_idx = min(err_idx, type_node->ptr_vec[0]->ptr_vec[1]->token_index);
                    return;
                }
            }          
        }
        else type = type_node->ptr_vec[0]->type;
        //free(type_node);

        auto dec_node = node->ptr_vec[0]->ptr_vec[1]; // declarator
        int ptr_l = 0;
        // check whether it is duplicate with other variables
        if (dec_node->ptr_vec.size() == 2) {
            ptr_l = dec_node->ptr_vec[0]->ptr_vec.size();
            dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
        }
        dec_node = dec_node->ptr_vec[0]; // direct_declarator
        // if declarator->ptr_vec is ( declarator-> ptr_  id  ) then it is a function pointer
        if (dec_node->ptr_vec.size() == 6) { // it is function pointer
            struct func NewFunc;
            NewFunc.ret_ptr_level = ptr_l;
            NewFunc.name = "\0";
            NewFunc.ret_type = type;
            auto fptr_node = dec_node->ptr_vec[1]; // declarator
            NewExtDec.type = "function_pointer";
            NewExtDec.ptr_level = fptr_node->ptr_vec[0]->ptr_vec.size();
            NewExtDec.name = fptr_node->ptr_vec[1]->ptr_vec[0]->type;
            int fptr_id_idx = fptr_node->ptr_vec[1]->ptr_vec[0]->token_index;
            //free(fptr_node);
            dec_node = dec_node->ptr_vec[4]; // parameter_list

            // if parameter has no name then store "\0"
            AST_node* type_node = new AST_node;
            while (dec_node->ptr_vec.size() > 0) {
                type_node = dec_node->ptr_vec[0];
                while (type_node->ptr_vec.size() > 0)
                    type_node = type_node->ptr_vec[0]; // type
                NewFunc.para_type_list.push_back(type_node->type);
                if (dec_node->ptr_vec[0]->ptr_vec.size() == 2) {
                    type_node = dec_node->ptr_vec[0]->ptr_vec[1]; // declarator
                    if (type_node->ptr_vec.size() == 2) {
                        NewFunc.para_ptr_level.push_back(type_node->ptr_vec[0]->ptr_vec.size());
                        type_node->ptr_vec.erase(type_node->ptr_vec.begin());
                    }
                    else NewFunc.para_ptr_level.push_back(0);
                    while (type_node->ptr_vec.size() > 0)
                        type_node = type_node->ptr_vec[0]; // id
                    // check whether the id name is duplicate
                    for (int i = 0; i < NewFunc.para_name_list.size(); i++) {
                        if (type_node->type == NewFunc.para_name_list[i]) {
                            //TODO error
                            //std::cout << "duplicate parameter name!" << endl;
                            //free(type_node);
                            //free(dec_node);
                            err_idx = min(err_idx, type_node->token_index);
                            return;
                        }
                    }
                    NewFunc.para_name_list.push_back(type_node->type);
                }
                else NewFunc.para_name_list.push_back("\0");
                dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
                if (dec_node->ptr_vec.size() > 0)
                    dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
            }
            // check whether it has already been defined.
            NewExtDec.Func = NewFunc;
            bool is_dul = false;
            for (int i = 0; i < ExtDecList.size() && !is_dul; i++) {
                if (ExtDecList[i].name == NewExtDec.name)                
                    is_dul = true;
            }
            if (is_dul) {
                //TODO error
                //std::cout << "function pointer error!" << endl;
                //free(type_node);
                //free(dec_node);
                err_idx = min(err_idx, fptr_id_idx);
                return;
            }
            else ExtDecList.push_back(NewExtDec);
            //free(type_node);
            //free(dec_node);
        }
        else if (dec_node->ptr_vec.size() == 4) { // function declaration
            struct func NewFunc;

            NewExtDec.name = dec_node->ptr_vec[0]->type; // id
            int fd_id_idx = dec_node->ptr_vec[0]->token_index;
            NewExtDec.type = "function_declaration";
            NewExtDec.ptr_level = 0;
            NewFunc.name = dec_node->ptr_vec[0]->type;
            NewFunc.ret_ptr_level = ptr_l;
            NewFunc.ret_type = type;
            dec_node = dec_node->ptr_vec[2]; // parameter_list
            // if parameter has no name then store "\0"
            while (dec_node->ptr_vec.size() > 0) {
                type_node = dec_node->ptr_vec[0];
                while (type_node->ptr_vec.size() > 0)
                    type_node = type_node->ptr_vec[0]; // type
                NewFunc.para_type_list.push_back(type_node->type);
                if (dec_node->ptr_vec[0]->ptr_vec.size() == 2) {
                    type_node = dec_node->ptr_vec[0]->ptr_vec[1]; // declarator
                    if (type_node->ptr_vec.size() == 2) {
                        NewFunc.para_ptr_level.push_back(type_node->ptr_vec[0]->ptr_vec.size());
                        type_node->ptr_vec.erase(type_node->ptr_vec.begin());
                    }
                    else NewFunc.para_ptr_level.push_back(0);
                    while (type_node->ptr_vec.size() > 0)
                        type_node = type_node->ptr_vec[0]; // id
                    // check whether the id name is duplicate
                    for (int i = 0; i < NewFunc.para_name_list.size(); i++) {
                        if (type_node->type == NewFunc.para_name_list[i]) {
                            //TODO error
                            //std::cout << "duplicate parameter name!" << endl;
                            //free(type_node);
                            //free(dec_node);
                            err_idx = min(err_idx, type_node->token_index);
                            return;
                        }
                    }
                    NewFunc.para_name_list.push_back(type_node->type);
                }
                else NewFunc.para_name_list.push_back("\0");
                dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
                if (dec_node->ptr_vec.size() > 0)
                    dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
            }
            // std::cout << "para-size = " << NewFunc.para_type_list.size() << endl;
            // check whether it has already been defined.
            NewExtDec.Func = NewFunc;
            bool is_dul = false;
            int i = 0;           
            for (; i < ExtDecList.size() && !is_dul; i++) {
                if (ExtDecList[i].name == NewExtDec.name) {
                    if (ExtDecList[i].type != "function_declaration")
                        is_dul = true;
                    else {
                        if (ExtDecList[i].Func.para_type_list.size() == NewFunc.para_type_list.size()) {
                            int j = 0;
                            for (; j < ExtDecList[i].Func.para_type_list.size() && \
                                    ExtDecList[i].Func.para_type_list[j] == NewFunc.para_type_list[j] && \
                                    ExtDecList[i].Func.para_ptr_level == NewFunc.para_ptr_level; j++) {}
                            if (j == ExtDecList[i].Func.para_type_list.size())
                                is_dul = true;                            
                        }
                    }
                }
            }
            if (is_dul) {// check whether it has already been defined.
                // TODO ERROR
                //std::cout << "function declaration error!" << endl;
                err_idx = min(err_idx, fd_id_idx);
                return;
            }
            else ExtDecList.push_back(NewExtDec);
            //std::cout << NewExtDec.Func.name << " " << NewExtDec.Func.ret_type << " " << NewExtDec.Func.para_type_list[0] << endl;
            //free(type_node);
            //free(dec_node);
        }
        else {
            if (dec_node->ptr_vec.size() == 2) {
                NewExtDec.ptr_level = dec_node->ptr_vec[0]->ptr_vec.size();
                dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
            }
            while (dec_node->ptr_vec.size() > 0)
                dec_node = dec_node->ptr_vec[0];

            NewExtDec.name = dec_node->type;
            bool is_dul = false;
            for (int i = 0; i < ExtDecList.size() && !is_dul; i++) {
                if (ExtDecList[i].name == NewExtDec.name)
                    is_dul = true;
            }
            if (is_dul) {// check whether it has already been defined.
                // TODO ERROR
                //std::cout << "duplicate declaration name error!" << endl;
                // err_idx = min(index, i - 1);
                err_idx = min(err_idx, dec_node->token_index);
                return;
            }
            else ExtDecList.push_back(NewExtDec);
            //free(type_node);
            //free(dec_node);
        }                        
    }
}

void SA::check_comp_stat(AST_node* node, int ext_idx) { // store in DecList // node->type is compound_statement
    // if comp_stat is { empty } then do nothing
    if (node->ptr_vec.size() == 2) return;
    auto block_node = node->ptr_vec[1]; // block_item_list   
    struct dec_list NewDecList;
    DecListVec.push_back(NewDecList);
    // block_item could be 
    //      declaration
    //      expression
    //      statement
    //          label
    //          compound statement

    while (block_node->ptr_vec.size() > 0) {
        // declaration: int char void or struct
        if (block_node->ptr_vec[0]->ptr_vec[0]->type == "declaration") {            
            auto type_node = block_node->ptr_vec[0]->ptr_vec[0]->ptr_vec[0]; // declaration_specifier
            struct dec NewDec;
            if (block_node->ptr_vec[0]->ptr_vec[0]->ptr_vec.size() == 2 && \
                (type_node->ptr_vec[0]->ptr_vec[0]->type == "int" || \
                type_node->ptr_vec[0]->ptr_vec[0]->type == "char" || \
                type_node->ptr_vec[0]->ptr_vec[0]->type == "void"))
                return; // no need to store it;
            else NewDec.name = "\0";
                
            string_t type;
            while (type_node->ptr_vec[0]->type != "char" && type_node->ptr_vec[0]->type != "int" && \
                type_node->ptr_vec[0]->type != "void" && type_node->ptr_vec[0]->type != "struct_or_union_specifier") 
                type_node = type_node->ptr_vec[0];
            if (type_node->ptr_vec[0]->type == "struct_or_union_specifier") {
                if (type_node->ptr_vec[0]->ptr_vec.size() > 2) {
                    struct Struct NewStr = struct_construct(type_node->ptr_vec[0]);
                    //std::cout << "NewStr.name = " << NewStr.name << endl;
                    // if OK then store
                    if (NewStr.name != "-1") {
                        NewDec.type = "struct";
                        NewDec.ptr_level = 0;
                        NewDec.Struct = NewStr;
                        if (node->ptr_vec[0]->ptr_vec.size() == 2) { // no declarator                                            
                            DecListVec[DecListVec.size()-1].Dec.push_back(NewDec);
                            return;
                        }
                    }                    
                    else {
                        // TODO error
                        //std::cout << "struct construction fail" << endl;
                        return;

                    }
                }
                else {
                    bool is_decled = false;
                    // check whether it is declared
                    string_t id = type_node->ptr_vec[0]->ptr_vec[1]->type;
                    for (int i = DecListVec.size() - 1; i >= 0 && !is_decled; i--) {
                        for (int j = 0; j < DecListVec[i].Dec.size() && !is_decled; j++) {
                            if (DecListVec[i].Dec[j].type == "struct" && DecListVec[i].Dec[j].Struct.name == id)
                                is_decled = true;
                        }
                    }
                    for (int i = 0; i < ExtDecList.size() && !is_decled; i++) {
                        if (ExtDecList[i].type == "struct" && ExtDecList[i].Struct.name == id)
                            is_decled = true;
                    }
                    if (is_decled)
                        NewDec.type = "struct+" + type_node->ptr_vec[0]->ptr_vec[1]->type;                         
                    else {
                        // TODO error
                        //std::cout << "struct in function not declared!" << endl;
                        err_idx = min(err_idx, type_node->ptr_vec[0]->ptr_vec[1]->token_index);
                        return;
                    }
                }          
            }
            else NewDec.type = type_node->ptr_vec[0]->type;

            //std::cout << "enter declarator" << endl; 
            auto dec_node = block_node->ptr_vec[0]->ptr_vec[1]; // declarator
            // check whether it is duplicate with other variables
            if (dec_node->ptr_vec.size() == 2) {
                NewDec.ptr_level = dec_node->ptr_vec[0]->ptr_vec.size();
                dec_node->ptr_vec.erase(dec_node->ptr_vec.begin());
            }
            dec_node = dec_node->ptr_vec[0]; // direct_declarator        
            
                    
            while (dec_node->ptr_vec.size() > 0)
                dec_node = dec_node->ptr_vec[0];

            NewDec.name = dec_node->type;
            bool is_dul = false;            

            for (int i = 0; i < DecListVec[DecListVec.size()-1].Dec.size() && !is_dul; i++) {
                if (DecListVec[DecListVec.size()-1].Dec[i].name == NewDec.name)
                    is_dul = true;
            }
            if (is_dul) {// check whether it has already been defined.
                // TODO ERROR
                //std::cout << "duplicate declaration name in function error!" << endl;
                err_idx = min(err_idx, dec_node->token_index);
                return;
            }
            else DecListVec[DecListVec.size()-1].Dec.push_back(NewDec);            
            //free(type_node);
            //free(dec_node);    
        }
        else if (block_node->ptr_vec[0]->ptr_vec[0]->type == "statement") {
            auto stat = block_node->ptr_vec[0]->ptr_vec[0];
            if (stat->ptr_vec[0]->type == "label_statement") {
                bool is_dul = false;
                for (int i = 0; i < ExtDecList[ext_idx].Func.label_list.size() && !is_dul; i++) {
                    if (ExtDecList[ext_idx].Func.label_list[i] == stat->ptr_vec[0]->ptr_vec[0]->type)
                        is_dul = true;
                }
                if (is_dul) {
                    // TODO error
                    //std::cout << "label duplicate" << endl;
                    err_idx = min(err_idx, stat->ptr_vec[0]->ptr_vec[0]->token_index);
                    return;
                }
                else {
                    ExtDecList[ext_idx].Func.label_list.push_back(stat->ptr_vec[0]->ptr_vec[0]->type);
                    // check statement
                }
            }
        }
        
        block_node->ptr_vec.erase(block_node->ptr_vec.begin());
    }

    DecListVec.pop_back();


}

void SA::check_exp(AST_node* node) {

}

void SA::check_dec(AST_node* node) {

}

struct Struct SA::struct_construct(AST_node* node) { // node->type is struct_or_union_specifier
    struct Struct NewStr;
    AST_node* tmp = new AST_node;
    if (node->ptr_vec.size() == 4) {
        NewStr.name = "\0";
        tmp = node->ptr_vec[2];
    }        
    else {
        NewStr.name = node->ptr_vec[1]->type;
        //std::cout << "NewStr.name = " << NewStr.name << endl;
        tmp = node->ptr_vec[3]; // struct_declaration_list
    }
    while (tmp->ptr_vec.size() > 0) {
        auto type_node = tmp;
        while (type_node->ptr_vec[0]->type != "char" && type_node->ptr_vec[0]->type != "int" && \
            type_node->ptr_vec[0]->type != "void" && type_node->ptr_vec[0]->type != "struct_or_union_specifier")
            type_node = type_node->ptr_vec[0];
        if (type_node->ptr_vec[0]->type == "struct_or_union_specifier") {
            bool is_decled = false;
            // check whether it is declared
            string_t id = type_node->ptr_vec[0]->ptr_vec[1]->type;
            //std::cout << "struct id in struct = " << id << endl;
            for (int i = 0; i < ExtDecList.size() && !is_decled; i++) {
                if (ExtDecList[i].type == "struct" && ExtDecList[i].Struct.name == id)
                    is_decled = true;
            }
            if (is_decled)
                NewStr.para_type_list.push_back("struct+" + id);
            else {
                // TODO error
                //std::cout << "struct in struct not declared!" << endl;
                NewStr.name = "-1";
                err_idx = min(err_idx, type_node->ptr_vec[0]->ptr_vec[1]->token_index);
                return NewStr;
            }        
        }    
        else NewStr.para_type_list.push_back(type_node->ptr_vec[0]->type);
        //free(type_node);
        if (tmp->ptr_vec[0]->ptr_vec.size() == 3) {
            auto dec_node = tmp->ptr_vec[0]->ptr_vec[1];
            while (dec_node->ptr_vec.size() > 0) dec_node = dec_node->ptr_vec[0];
            int i = 0;
            for (; i < NewStr.para_name_list.size() && dec_node->type != NewStr.para_name_list[i]; i++) {}
            if (i < NewStr.para_name_list.size()) {
                //TODO ERROR
                //std::cout << "duplicate parameter name in struct!" << endl;
                NewStr.name = "-1";
                err_idx = min(err_idx, dec_node->token_index);
                return NewStr;
            }
            NewStr.para_name_list.push_back(dec_node->type);
            //free(dec_node);
        }
        else NewStr.para_name_list.push_back("\0");
        tmp->ptr_vec.erase(tmp->ptr_vec.begin());
    }
    //free(tmp);
    return NewStr;
}

void SA::check_label(AST_node* node) {

}

