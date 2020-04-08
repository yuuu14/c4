#ifndef __SA_H
#define __SA_H
#include <vector>
#include "types.h"
#include "Parser.h"

using namespace std;

struct func {
    string_t name;
    string_t ret_type;
    int ret_ptr_level;
    vector<string_t> para_name_list;
    vector<string_t> para_type_list;
    vector<int> para_ptr_level;
    vector<string_t> label_list;
};

struct Struct { 
    string_t name;
    vector<string_t> para_name_list;
    vector<string_t> para_type_list;
};

struct ext_dec {
    string_t name;
    string_t type;
    int ptr_level;
    int dec_pos;
    struct func Func;
    struct Struct Struct;
};

struct dec {
    string_t name;
    string_t type;
    int ptr_level;
    struct Struct Struct;
};

struct dec_list {
    vector<struct dec> Dec;
};

class SA {
public:
    AST_node* ast_tree;
    int index;
    int err_idx;

    vector<struct ext_dec> ExtDecList;
    vector<struct dec_list> DecListVec;
    void set_ast_tree(AST_node* node) { ast_tree = node; err_idx = 10e10; }
    void semantic_analysis();
    void del_paren(AST_node* node);
private:
    int min(int a, int b) { return a < b ? a : b;}
    void check_ext_dec(AST_node* node);
    void check_comp_stat(AST_node* node, int ext_idx); // ext_idx is the index of the external declaration the compound statement belongs to
    void check_exp(AST_node* node);
    void check_dec(AST_node* node);
    struct Struct struct_construct(AST_node* node);
    void check_label(AST_node* node);
    
};



#endif