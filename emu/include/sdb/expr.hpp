#pragma once

#include <common.hpp>
#include <regex.h>
#include <exec.hpp>

#define TOKENS_MAX 128

enum {
    TK_NOTYPE = 256, TK_EQ, TK_NUM,
    TK_HEX, TK_REG, TK_NQ, TK_AND,
    TK_DEREF, TK_NEG, TK_OR, TK_NOT
};

struct Rule {
    const char *regex;
    int token_type;
};

constexpr Rule rules[] = {
    {" +", TK_NOTYPE},        // spaces
    {"\\+", '+'},             // plus
    {"\\-", '-'},             // minus or neg
    {"\\*", '*'},             // times or deref, judged by the previous token
    {"\\/", '/'},             // divide
    {"\\(", '('},             // left
    {"\\)", ')'},             // right
    {"==", TK_EQ},            // equal
    {"!=", TK_NQ},            // not equal
    {"&&", TK_AND},           // logic and
    {"\\|\\|", TK_OR},        // logic or 
    {"!", TK_NOT},            // logic not
    {"\\$[0-9a-z\\$]+", TK_REG},// register
    {"0x[0-9a-f]+", TK_HEX}, // hex
    {"[0-9]+", TK_NUM}       // number
};

struct Token {
    int type;
    std::string str;
};

class ExprHandler {
    SimExecutor *executor;
    std::vector<regex_t> re;
    std::vector<Token> tokens;
    int nr_token;
private:
    bool makeTokens(const char *expr);
    word_t eval(int p, int q, bool *success);
public:
    ExprHandler();
    void setExecutor(SimExecutor *executor);
    word_t expr(const char *expr, bool *success);
};

extern ExprHandler exprHandler;