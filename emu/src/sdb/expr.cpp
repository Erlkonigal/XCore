#include "common.hpp"
#include <sdb/expr.hpp>
#include <ic.hpp>

ExprHandler exprHandler;

ExprHandler::ExprHandler() {
    re.resize(ARRAY_SIZE(rules));
    tokens.resize(TOKENS_MAX);
    nr_token = 0;

    for (size_t i = 0; i < ARRAY_SIZE(rules); i++) {
        int ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            std::string err;
            regerror(ret, &re[i], &err[0], err.size());
            Error("Regex Compilation Error: %s, %s", err.c_str(), rules[i].regex);
            assert(0);
        }
    }
}

void ExprHandler::setExecutor(SimExecutor *executor) {
    this->executor = executor;
}

bool ExprHandler::makeTokens(const char *expr) {
    int position = 0;
    int i;
    regmatch_t pmatch;
  
    nr_token = 0;

    while (expr[position] != '\0') {
      /* Try all rules one by one. */
      for (i = 0; i < ARRAY_SIZE(rules); i ++) {
        if (regexec(&re[i], expr + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
          char *substr_start = (char *)expr + position;
          int substr_len = pmatch.rm_eo;
  
          Debug("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
              i, rules[i].regex, position, substr_len, substr_len, substr_start);
  
          position += substr_len;

          switch (rules[i].token_type) {
            case TK_NOTYPE: break;
            case '+':
            case '-':
            case '*':
            case '/':
            case '(':
            case ')':
            case TK_EQ:
            case TK_NQ:
            case TK_OR:
            case TK_AND:
            case TK_NOT: {
                Token token;
                token.type = rules[i].token_type;
                token.str = "";
                tokens[nr_token++] = token;
                break;
            }
            case TK_REG:
            case TK_HEX:
            case TK_NUM: {
                int fix_size = (substr_len < 32 ? substr_len : 31);
                Token token;
                token.type = rules[i].token_type;
                token.str = std::string(substr_start, fix_size);
                tokens[nr_token++] = token;
                break;
            }
            default: assert(0);//todo
          }
  
          break;
        }
      }
  
      if (i == ARRAY_SIZE(rules)) {
        Warn("no match at position %d\n%s\n%*.s^", position, expr, position, "");
        return false;
      }
    }
  
    return true;
}

word_t ExprHandler::eval(int p, int q, bool *success) {
    auto checkParentheses = [&](int p, int q) {
        int stack = 0;
        for(int i = p; i <= q; i++) {
          if(tokens[i].type == '(') stack++;
          else if (tokens[i].type == ')') stack--;
          
          if(stack == 0 && i != q) return false;
        }
        return stack == 0 ? true : false; 
    };

    auto getPriority = [&](int pos) {
        if(pos == TOKENS_MAX) return TOKENS_MAX;
        switch(tokens[pos].type) {
            case TK_OR:
            case TK_AND: return 0;
            case TK_NQ:
            case TK_EQ: return 1;
            case '+':
            case '-': return 2;
            case '*':
            case '/': return 3;
            case TK_DEREF:
            case TK_NOT:
            case TK_NEG: return 4;
            default: 
                assert(0);
                return -1;
        }
    };

    auto checkMainPos = [&](int p, int q) {
        int stack = 0;
        int pos = TOKENS_MAX, pos_depth = TOKENS_MAX; //初始化主运算符的位置与深度
        for (int i = p; i <= q; i++) {
            if (tokens[i].type == '(') stack++;
            else if (tokens[i].type == ')') stack--;
            else if (tokens[i].type == TK_NUM\
                    || tokens[i].type == TK_HEX\
                    || tokens[i].type == TK_REG) continue;
            else if ((stack < pos_depth) || 
                    (stack == pos_depth && \
                    getPriority(i) <= getPriority(pos))) { 
                pos = i;
                pos_depth = stack;
            }
        }
        return pos;
    };

    if (p > q) {
        Warn("error at left=%d,right=%d", p, q);
        *success = false;
        return 0;
    }
    else if (p == q) {
        word_t tok = 0;
        switch (tokens[p].type) {
            case (TK_NUM): sscanf(&tokens[p].str[0], "%d", &tok); break;
            case (TK_HEX): sscanf(&tokens[p].str[0], "%x", &tok); break;
            case (TK_REG): 
                tok = executor->getReg(&tokens[p].str[0] + 1, success); 
                break;
            default: {
                Warn("error at left=%d,right=%d", p, q);
                *success = false;
                return 0;
            }
        }
        return tok;
    } 
    else if (checkParentheses(p, q) == true) {
        return eval(p + 1, q - 1, success);
    }
    else {
        int op = checkMainPos(p, q);
        if (getPriority(op) >= 4) {//一元运算符
          word_t val = eval(op+1, q, success);
          switch (tokens[op].type) {
            case (TK_NOT): return !val;
            case (TK_NEG): return -val;
            case (TK_DEREF): {
              word_t addr = val;
              word_t data;
              ic.read(addr, &data);
              return data;
            }
            default: 
              Warn("num error");
              *success = false;
              return 0;
          }
        }
        else {//二元运算符
          word_t vall = eval(p, op-1, success);
          word_t valr = eval(op+1, q, success);
    
          switch (tokens[op].type) {
            case ('+'): return vall + valr;
            case ('-'): return vall - valr;
            case ('*'): return vall * valr;
            case ('/'): {
              if (valr == 0) {
                Warn("sth. is divided by 0");
                *success = false;
                return 0;
              }
              else return vall / valr;
            }
            case (TK_EQ): {
              return vall == valr ? 1 : 0;
            }
            case (TK_NQ): {
              return vall != valr ? 1 : 0;
            }
            case (TK_AND): {
              return (vall != 0 && valr != 0) ? 1 : 0;
            }
            case (TK_OR): {
              return (vall != 0 || valr != 0) ? 1 : 0;
            }
            default: 
              assert(0);
          }
        }
      }
}

word_t ExprHandler::expr(const char *expr, bool *success) {
  if (!makeTokens(expr)) {
    *success = false;
    return 0;
  }

  for (int i = 0; i < nr_token; i++) {
    bool pre = (i == 0 || (tokens[i-1].type != TK_NUM &&\
                            tokens[i-1].type != TK_HEX &&\
                            tokens[i-1].type != TK_REG &&\
                            tokens[i-1].type != ')')); //前一个type不能是数值类型或者')'
    switch (tokens[i].type) {
      case ('*'): if (pre) tokens[i].type = TK_DEREF; break;
      case ('-'): if (pre) tokens[i].type = TK_NEG; break;
      default: continue;
    }
  }
  return eval(0, nr_token - 1, success);
}
