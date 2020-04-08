#include "stdafx.h"
#include "Lexer.h"
#include "stdio.h"
#include <codecvt>
#include <iostream>


CLexer::CLexer(string_t str): str(str)
{
    length = str.length();
    //assert(length > 0);
}

CLexer::~CLexer()
{
}

#define DEFINE_LEXER_GETTER(t) \
LEX_T(t) CLexer::get_##t() \
{ \
    return bags._##t; \
}
DEFINE_LEXER_GETTER(char)
DEFINE_LEXER_GETTER(uchar)
DEFINE_LEXER_GETTER(short)
DEFINE_LEXER_GETTER(ushort)
DEFINE_LEXER_GETTER(int)
DEFINE_LEXER_GETTER(uint)
DEFINE_LEXER_GETTER(long)
DEFINE_LEXER_GETTER(ulong)
DEFINE_LEXER_GETTER(float)
DEFINE_LEXER_GETTER(double)
DEFINE_LEXER_GETTER(operator)
DEFINE_LEXER_GETTER(keyword)
DEFINE_LEXER_GETTER(identifier)
DEFINE_LEXER_GETTER(string)
DEFINE_LEXER_GETTER(comment)
DEFINE_LEXER_GETTER(space)
DEFINE_LEXER_GETTER(newline)

#undef DEFINE_LEXER_GETTER

bool match_pred(smatch_t::value_type sm)
{
    return sm.matched;
}

lexer_t CLexer::next()
{
    auto c = local();
    if (c == -1)
        return l_end;
    type = l_error;
    if (isalpha(c) || c == '_') // 变量名或关键字
    {
        type = next_alpha();
    }
    else if (isdigit(c)) // 数字
    {
        type = next_digit();
    }
    else if (isspace(c)) // 空白字符
    {
        type = next_space();
    }
    else if (c == '\'') // 字符
    {
        type = next_char();
    }
    else if (c == '\"') // 字符串
    {
        type = next_string();
    }
    else if (c == '/') // 注释
    {
        auto c2 = local(1);
        if (c2 == '/' || c2 == '*') // 注释
        {
            type = next_comment();
        }
        else // 操作符
        {
            type = next_operator();
        }
    }
    else
    {
        type = next_operator();
    }
    return type;
}

lexer_t CLexer::get_type() const
{
    return type;
}

int CLexer::get_line() const
{
    return line;
}

int CLexer::get_column() const
{
    return column;
}

int CLexer::get_last_line() const
{
    return last_line;
}

int CLexer::get_last_column() const
{
    return last_column;
}

string_t CLexer::current() const
{
    switch (type)
    {
    case l_space:
    case l_newline:
    case l_comment:
        return str.substr(last_index, index - last_index);
    case l_operator:
        return str.substr(last_index, index - last_index);
    default:
        break;
    }
    return str.substr(last_index, index - last_index);
}

void CLexer::move(int idx, int inc, bool newline)
{
    last_index = index;
    last_line = line;
    last_column = column;
    if (newline)
    {
        column = 1;
        line += inc;
    }
    else
    {
        if (inc < 0)
            column += idx;
        else
            column += inc;
    }
    index += idx;
}


lexer_t CLexer::next_digit()
{
    if (std::regex_search(str.cbegin() + index, str.cend(), sm, r_hex))
    {
        if (sm[1].matched)
        {
            auto s = sm[1].str();
            bags._uint = std::strtol(s.c_str(), NULL, 16);
            move(sm[0].length());
            return l_uint;
        }
    }
    else if (std::regex_search(str.cbegin() + index, str.cend(), sm, r_digit))
    {
        auto s = sm[1].str();
        auto type = l_error;
        if (sm[4].matched)
        {
            if (!sm[3].matched)
            {
                switch (sm[4].str()[0])
                {
                case 'F': // 100F
                case 'f': // 100f
                    type = l_float;
                    bags._float = LEX_T(float)(std::atof(s.c_str()));
                    break;
                case 'D': // 100D
                case 'd': // 100d
                    type = l_double;
                    bags._double = LEX_T(double)(std::atof(s.c_str()));
                    break;
                case 'I': // 100I
                case 'i': // 100i
                    type = l_int;
                    bags._int = LEX_T(int)(std::atoi(s.c_str()));
                    break;
                case 'L': // 100L
                case 'l': // 100l
                    type = l_long;
                    bags._long = LEX_T(long)(std::atol(s.c_str()));
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (sm[4].str()[0])
                {
                case 'I': // 100UI 100uI
                case 'i': // 100Ui 100ui
                    type = l_uint;
                    bags._uint = LEX_T(uint)(std::atof(s.c_str()));
                    break;
                case 'L': // 100UL 100uL
                case 'l': // 100Ul 100ul
                    type = l_ulong;
                    bags._ulong = LEX_T(ulong)(std::atof(s.c_str()));
                    break;
                default:
                    break;
                }
            }
        }
        else if (sm[5].matched) // 0x12345678
        {
            type = l_uint;
            bags._uint = std::atoi(sm[5].str().c_str());
        }
        else
        {
            if (sm[2].matched) // double <- contains dot '.'
            {
                type = l_double;
                bags._double = std::atof(s.c_str());
            }
            else // int <- default
            {
                type = l_int;
                bags._int = std::atoi(s.c_str());
            }
        }
        move(sm[0].length());
        return type;
    }
    //assert(!"digit not match");
    return l_error;
}

lexer_t CLexer::next_alpha()
{
    
    string_t temp = "";
    int idx = index;
    temp += str[idx];
    if(idx + 1 < length){
        idx++;
    } else{
        bags._identifier = temp;
        move(temp.length());
        return l_identifier;
    }
    int flag = 1;
    while(isalpha(str[idx])||isdigit(str[idx])||str[idx]=='_'){
        temp += str[idx];
        if(idx + 1 < length){
            idx++;
        } else{
            flag = 0;
            break;
        }
    }
    if(temp.size()<=17&&std::regex_search(temp.cbegin(), temp.cend(), sm, r_keyword)){
        auto b = sm.begin() + 1;
        auto i = std::distance(b, std::find_if(b, sm.end(), match_pred));
        bags._keyword = (keyword_t)(i + 1); 
        move(temp.length());
        return l_keyword;
    } else{
        bags._identifier = temp.c_str();
        move(temp.length());
        return l_identifier;
    }
    /*
    if (std::regex_search(str.cbegin() + index, str.cend(), sm, r_alpha))
    {
        auto s = sm[0].str();
        if (std::regex_search(s.cbegin(), s.cend(), sm, r_keyword))
        {
            auto b = sm.begin() + 1;
            auto i = std::distance(b, std::find_if(b, sm.end(), match_pred));
            bags._keyword = (keyword_t)(i + 1); 
            move(s.length());
            return l_keyword;
        }
        else
        {
            bags._identifier = s.c_str();
            move(s.length());
            return l_identifier;
        }
    }*/
    //assert(!"alpha not match");
    return l_error;
}

lexer_t CLexer::next_space()
{
    if (std::regex_search(str.cbegin() + index, str.cend(), sm, r_space))
    {
        auto ms = sm[0].str();
        auto ml = ms.length();
        if (ms[0] == ' ' || ms[0] == '\t') // space or tab
        {
            bags._space = ml;
            if(ms[0] == ' '){
               move(ml);
            }
            else{
                move(ml,0,false);
                move(0,1,false);
            }
            return l_space;
        }
        else if (ms[0] == '\r') // \r\n
        {
            bags._newline = ml / 2;
            move(ml, bags._newline, true);
            return l_newline;
        }
        else if (ms[0] == '\n') // \n
        {
            bags._newline = ml;
            move(ml, bags._newline, true);
            return l_newline;
        }
    }
    //assert(!"space not match");
    return l_error;
}

lexer_t CLexer::next_char()
{
    if (std::regex_search(str.cbegin() + index, str.cend(), sm, r_char))
    {
        if (sm[1].matched) // like 'a'
        {
            bags._char = sm[1].str()[0];
            move(sm[0].length());
            if (!isprint(bags._char))
                return l_error;
            return l_int;
        }
        else if (sm[2].matched)
        {
            auto type = l_int;
            switch (sm[2].str()[0]) // like \r, \n, ...
            {
            case 'b':
                bags._char = '\b';
                break;
            case 'f':
                bags._char = '\f';
                break;
            case 'n':
                bags._char = '\n';
                break;
            case 'r':
                bags._char = '\r';
                break;
            case 't':
                bags._char = '\t';
                break;
            case 'v':
                bags._char = '\v';
                break;
            case '\'':
                bags._char = '\'';
                break;
            case '\"':
                bags._char = '\"';
                break;
            case '\\':
                bags._char = '\\';
                break;
            case 'a':
                bags._char = '\a';
                break;
            case '\?':
                bags._char = '\?';
                break;
            default:
                type = l_error;
                break;
            }
            move(sm[0].length());
            return type;
        }
        else if (sm[3].matched) // like '\0111'
        {
            auto oct = std::strtol(sm[3].str().c_str(), NULL, 8);
            bags._char = char(oct);
            move(sm[0].length());
            return l_int;
        }
        else if (sm[4].matched) // like '\8'
        {
            auto n = std::atoi(sm[4].str().c_str());
            bags._char = char(n);
            move(sm[0].length());
            return l_int;
        }
        else if (sm[5].matched) // like '\xff'
        {
            auto hex = std::strtol(sm[3].str().c_str(), NULL, 16);
            bags._char = char(hex);
            move(sm[0].length());
            return l_int;
        }
    }
    //assert(!"char not match");
    return l_error;
}

lexer_t CLexer::next_string()
{
    auto idx = index + 1;
    bags._string.clear();
    int minus = 0;
    string_t nonasc = "";
    for (;;)
    {
        if(str[idx] == '\\'){
            if(idx +1 < length){
                if(str[idx+1] == '\"'||str[idx+1] == 'b'||str[idx+1] == '\\'||str[idx+1] == '\''||str[idx+1] == 'f'||str[idx+1] == 'n'||str[idx+1] == 'r'||str[idx+1] == 'n'||str[idx+1] == 'v'||str[idx+1] == 't'||str[idx+1] == 'a'||str[idx+1] == '?'){
                    if(idx +1 < length){
                        // to do
                        bags._string += str[idx];
                        bags._string += str[idx+1];
                        if(idx +2 < length){
                            idx = idx + 2;
                        } else{
                            return l_error;
                        }
                    }
                    else{
                        return l_error;
                    }
                }
                else{
                    //to do
                    return l_error;
                }
            }
            else {
                return l_error;
            }
        }   else if(str[idx] == '\"'){
            //to do
            bags._string += str[idx];
            idx++;
            //std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cn;
            //auto sz = cn.from_bytes(nonasc).size();
            move(idx - index,idx - index,false);
            return l_string;
        }   else if(str[idx] == '\n'||str[idx] == '\r'){
    
            return l_error;
        }  
        else {
            //to do
            if(idx +1 < length){
               bags._string += str[idx];
               if((str[idx] < 0 || str[idx] >= 128)){
                   minus++;
                   nonasc += str[idx];
               }
               idx++; 
            }
            else
            {
                //to do
                return l_error;
            }
        }
        /*if (std::regex_search(str.cbegin() + idx, str.cend(), sm, r_string))
        {
            idx += sm[0].length();
            if (sm[1].matched) // like 'a'
            {
                auto c = sm[1].str()[0];
                if (c == '\"') // match end '"'
                {
                    move(idx - index);
                    return l_string;
                }
                bags._string += c;
                if (!isprint(c))
                    return l_error;
            }
            else if (sm[2].matched) // like \r, \n, ...
            {
                //auto type = l_char;
                switch (sm[2].str()[0])
                {
                case 'b':
                    bags._string += '\b';
                    break;
                case 'f':
                    bags._string += '\f';
                    break;
                case 'n':
                    bags._string += '\n';
                    break;
                case 'r':
                    bags._string += '\r';
                    break;
                case 't':
                    bags._string += '\t';
                    break;
                case 'v':
                    bags._string += '\v';
                    break;
                case '\'':
                    bags._string += '\'';
                    break;
                case '\"':
                    bags._string += '\"';
                    break;
                case '\\':
                    bags._string += '\\';
                    break;
                default:
                    type = l_error;
                    break;
                }
            }
            else if (sm[3].matched) // like '\0111'
            {
                auto oct = std::strtol(sm[3].str().c_str(), NULL, 8);
                bags._string += char(oct);
            }
            else if (sm[4].matched) // like '\8'
            {
                auto n = std::atoi(sm[4].str().c_str());
                bags._string += char(n);
            }
            else if (sm[5].matched) // like '\xff'
            {
                auto hex = std::strtol(sm[3].str().c_str(), NULL, 16);
                bags._string += char(hex);
            }
            else break;
        }
        else{
            return l_error;
        }*/

    }
    //assert(!"string not match");
    return l_error;
}

lexer_t CLexer::next_comment()
{
    int idx = index + 1;//move to second character
    bags._comment += '/';
    if(str[idx] != '*'){
        while(1){
            if(str[idx] == '\n'||str[idx] == '\r'){
                if(str[idx] == '\r'){
                    if(idx + 1 < length&&str[idx+1] == '\n'){
                        idx++;
                    }
                }
                move(idx - index,false);
                return l_comment;
            } 
            else{
                if(idx + 1 < length){
                    bags._comment += str[idx];
                    idx++;
                } else{
                    move(idx - index+1,false);
                    return l_comment;
                }
            }
        }
    }
    else{
        int colomn_bias = 2;
        int minus = 0;
        int changeline = 0;
        string_t nonasc = "";
        bags._comment += str[idx];
        idx++;
        while(1){
            if(str[idx] == '\n'||str[idx] == '\r'){
                bags._comment += str[idx];
                nonasc = "";
                changeline++;
                colomn_bias = 0;
                minus = 0;
                //idx++;
                if(idx + 1 < length){
                    idx++;
                    if(str[idx-1] == '\r'&&str[idx] == '\n'){
                        bags._comment += str[idx];
                        idx++;
                    }
                } else{
                    return l_error;
                }
            } else if(str[idx] == '*'){
                bags._comment += str[idx];
                colomn_bias++;
                if(idx + 1 < length){
                    if(str[idx+1] == 47){
                        idx++;
                        colomn_bias++;
                        bags._comment += str[idx];
                        if(changeline)
                        move(0,changeline,true);
                        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cn;
                        auto sz = cn.from_bytes(nonasc).size();
                        move(idx - index+1,colomn_bias,false);
                        return l_comment;
                    } else{
                            idx++;
                    } 
                } else {
                    return l_error;
                }
            } else {
                bags._comment += str[idx];
                if(str[idx] < 0||str[idx] >= 128){
                    minus++;
                    nonasc+=str[idx];
                }
                if(idx + 1 < length){
                    idx++;
                    colomn_bias++;
                } else {
                    return l_error;
                }
            }
        }
    }
    /*
    if (std::regex_search(str.cbegin() + index, str.cend(), sm, r_comment))
    {
        auto ms = sm[0].str();
        auto ml = ms.length();
        if (sm[1].matched) // comment like '// ...'
        {
            bags._comment = sm[1].str();
            move(ml);
            return l_comment;
        }
        else if (sm[2].matched) // comment like
        {
            bags._comment = sm[2].str();			
			if (std::count(bags._comment.begin(), bags._comment.end(), '\n') == 0)
				move(ml); // check new line
			else {
				move(ml, std::count(bags._comment.begin(), bags._comment.end(), '\n'), true); // check new line
				move(0, sm[2].str().length() - sm[2].str().find_last_of('\n') + 1, false);
			}			
            return l_comment;
        }
    }
    //assert(!"comment not match");
    */
    return l_error;
}

lexer_t CLexer::next_operator()
{   
    for (auto i = 3; i >= 0; i--)
    {
        if (index + i >= length)
            continue;
        if (std::regex_search(str.cbegin() + index, str.cbegin() + index + i + 1, sm, r_operator[i]))
        {
            auto s = sm[0].str();
            auto b = sm.begin() + 1;
            auto j = std::distance(b, std::find_if(b, sm.end(), match_pred));
            j += lexer_operator_start_idx(i + 1) - 1;
            bags._operator = (operator_t)(j + 1);
            move(s.length());
            return l_operator;
        }
    }

    // ignore error
     //assert(!"operator not match");
    move(1);
    return l_error;
}

int CLexer::local()
{
    if (index < length)
        return str[index];
    return -1;
}

int CLexer::local(int offset)
{
    if (index + offset < length)
        return str[index + offset];
    return -1;
}
