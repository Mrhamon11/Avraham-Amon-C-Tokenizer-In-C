#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Type{
    INTEGER_CONSTANT,
    BAD_OCTAL,
    BAD_HEX,
    BAD_SUFFIX_TOKEN,
    FLOAT_CONSTANT,
    CHAR_CONSTANT,
    STRING_CONSTANT,
    AUTO_KEYWORD,
    DOUBLE_KEYWORD,
    INT_KEYWORD,
    STRUCT_KEYWORD,
    CONST_KEYWORD,
    FLOAT_KEYWORD,
    SHORT_KEYWORD,
    UNSIGNED_KEYWORD,
    BREAK_KEYWORD,
    ELSE_KEYWORD,
    LONG_KEYWORD,
    SWITCH_KEYWORD,
    CONTINUE_KEYWORD,
    FOR_KEYWORD,
    SIGNED_KEYWORD,
    VOID_KEYWORD,
    CASE_KEYWORD,
    ENUM_KEYWORD,
    REGISTER_KEYWORD,
    RESTRICT_KEYWORD,
    TYPEDEF_KEYWORD,
    DEFAULT_KEYWORD,
    GOTO_KEYWORD,
    SIZEOF_KEYWORD,
    VOLATILE_KEYWORD,
    CHAR_KEYWORD,
    EXTERN_KEYWORD,
    RETURN_KEYWORD,
    UNION_KEYWORD,
    DO_KEYWORD,
    IF_KEYWORD,
    INLINE_KEYWORD,
    STATIC_KEYWORD,
    WHILE_KEYWORD,
    IDENTIFIER,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_PARENT,
    CLOSE_PARENT,
    OPEN_BRACE,
    CLOSE_BRACE,
    ADD_OP,
    SUB_OP,
    MULT_OP,
    DIV_OP,
    MOD_OP,
    INCREMENT,
    DECREMENT,
    PERIOD,
    VARIADIC,
    ARROW,
    LOG_NOT,
    BIT_NOT,
    BIT_LEFT,
    BIT_RIGHT,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    EQUAL,
    NOT_EQUAL,
    BIT_AND,
    BIT_XOR,
    BIT_OR,
    LOG_AND,
    LOG_OR,
    ASSIGN,
    PLUS_EQUALS,
    MINUS_EQUALS,
    MULT_EQUALS,
    DIV_EQUALS,
    MOD_EQUALS,
    ASSIGN_LEFT,
    ASSIGN_RIGHT,
    ASSIGN_AND,
    ASSIGN_OR,
    ASSIGN_XOR,
    COMMA,
    COLON,
    SEMI_COLON,
    POUND,
    TERNARY,
    LEFT_ANGLE_COLON,
    RIGHT_ANGLE_COLON,
    LEFT_ANGLE_PERCENT,
    RIGHT_ANGLE_PERCENT,
    PERCENT_COLON,
    PERCENT_COLON_PERCENT_COLON,
    BLOCK_COMMENT,
    LINE_COMMENT,
    BAD_COMMENT,
    BAD_PUNCTUATOR,
    END,
    BAD_FLOAT,
    BAD_STRING,
    BAD_CHAR,
    NULL_TYPE
};

typedef enum Type Type;

char *strEnums[] = {
        "INTEGER_CONSTANT",
        "BAD_OCTAL",
        "BAD_HEX",
        "BAD_SUFFIX_TOKEN",
        "FLOAT_CONSTANT",
        "CHAR_CONSTANT",
        "STRING_CONSTANT",
        "AUTO_KEYWORD",
        "DOUBLE_KEYWORD",
        "INT_KEYWORD",
        "STRUCT_KEYWORD",
        "CONST_KEYWORD",
        "FLOAT_KEYWORD",
        "SHORT_KEYWORD",
        "UNSIGNED_KEYWORD",
        "BREAK_KEYWORD",
        "ELSE_KEYWORD",
        "LONG_KEYWORD",
        "SWITCH_KEYWORD",
        "CONTINUE_KEYWORD",
        "FOR_KEYWORD",
        "SIGNED_KEYWORD",
        "VOID_KEYWORD",
        "CASE_KEYWORD",
        "ENUM_KEYWORD",
        "REGISTER_KEYWORD",
        "RESTRICT_KEYWORD",
        "TYPEDEF_KEYWORD",
        "DEFAULT_KEYWORD",
        "GOTO_KEYWORD",
        "SIZEOF_KEYWORD",
        "VOLATILE_KEYWORD",
        "CHAR_KEYWORD",
        "EXTERN_KEYWORD",
        "RETURN_KEYWORD",
        "UNION_KEYWORD",
        "DO_KEYWORD",
        "IF_KEYWORD",
        "INLINE_KEYWORD",
        "STATIC_KEYWORD",
        "WHILE_KEYWORD",
        "IDENTIFIER",
        "OPEN_BRACKET",
        "CLOSE_BRACKET",
        "OPEN_PARENT",
        "CLOSE_PARENT",
        "OPEN_BRACE",
        "CLOSE_BRACE",
        "ADD_OP",
        "SUB_OP",
        "MULT_OP",
        "DIV_OP",
        "MOD_OP",
        "INCREMENT",
        "DECREMENT",
        "PERIOD",
        "VARIADIC",
        "ARROW",
        "LOG_NOT",
        "BIT_NOT",
        "BIT_LEFT",
        "BIT_RIGHT",
        "LESS_THAN",
        "GREATER_THAN",
        "LESS_THAN_EQUAL",
        "GREATER_THAN_EQUAL",
        "EQUAL",
        "NOT_EQUAL",
        "BIT_AND",
        "BIT_XOR",
        "BIT_OR",
        "LOG_AND",
        "LOG_OR",
        "ASSIGN",
        "PLUS_EQUALS",
        "MINUS_EQUALS",
        "MULT_EQUALS",
        "DIV_EQUALS",
        "MOD_EQUALS",
        "ASSIGN_LEFT",
        "ASSIGN_RIGHT",
        "ASSIGN_AND",
        "ASSIGN_OR",
        "ASSIGN_XOR",
        "COMMA",
        "COLON",
        "SEMI_COLON",
        "POUND",
        "TERNARY",
        "LEFT_ANGLE_COLON",
        "RIGHT_ANGLE_COLON",
        "LEFT_ANGLE_PERCENT",
        "RIGHT_ANGLE_PERCENT",
        "PERCENT_COLON",
        "PERCENT_COLON_PERCENT_COLON",
        "BLOCK_COMMENT",
        "LINE_COMMENT",
        "BAD_COMMENT",
        "BAD_PUNCTUATOR",
        "END",
        "BAD_FLOAT",
        "BAD_STRING",
        "BAD_CHAR"
};

struct Token{
    char * lexeme;
    Type type;
};

typedef struct Token Token;

Token* tokenInit(char* lex, Type t){
    Token* token = malloc(sizeof(Token));
    token->lexeme = lex;
    token->type = t;
    return token;
}

char* getLexeme(Token* token){
    return token->lexeme;
}

Type getType(Token* token){
    return token->type;
}


char* tokenToString(Token* token){
    char* string = (char *) malloc(sizeof(char) * 200);
    strcpy(string, "Token: ");
    strcat(string, strEnums[getType(token)]);
    strcat(string, ", Lexeme: ");
    strcat(string, getLexeme(token));
    strcat(string, "\n");
    return string;
}

struct Stack{
    int top;
    char lex[10000];
};

typedef struct Stack Stack;

Stack* stackInit(){
    Stack* s = malloc(sizeof(Stack));
    s->top = -1;
    return s;
}

void push(Stack* stack, char c){
    int i = ++stack->top;
    stack->lex[i] = c;
}

char pop(Stack* stack){
    if(stack->top == -1){
        return -1;
    }
    char c = stack->lex[stack->top];
    stack->lex[stack->top] = '0';
    stack->top--;
    return c;
}

int isEmpty(Stack* stack){
    if(stack->top == -1){
        return 1;
    }
    return 0;
}

