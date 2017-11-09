#include <stdio.h>
#include <ctype.h>
#include "DataStructs.h"
#define numKeywords 34
#define numPuncuators 24
#define numHexValues 12
#define tokenLength 10000

//Global Variables
FILE *fp;
char token[tokenLength];
char potentialKw[tokenLength];
Stack* stack;
char* keywords[numKeywords];
Type keywordTokens[numKeywords];
char punctuators[numPuncuators];
char hexSet[numHexValues];

//Function Declarations
Token* nextToken();
Token* checkConstantNumbers(char token[], char c, int i);
Token* checkConstantString(char token[], char c, int i);
Token* checkConstantChar(char token[], char c, int i);
Token* checkBlockComment(char token[]);
Token* checkLineComment(char token[]);
Token* checkIdentifier(char token[]);
Token* checkKeyword(char token[]);
Token* checkPunctuators(char token[]);
char getChar();
void ungetChar(char c);
void skipWhiteSpaces();
void initKeywords();
void initPuncSet();
void initHexSet();
_Bool isIntSuffix(char c);
_Bool isFloatSuffix(char c);
Type getTokenTypeFromString(char* string);
_Bool hexSetContains(char c);
_Bool punctuatorsContains(char c);
char *trimwhitespace(char *str);

int main(){

    fp = fopen("../CFile.c", "r");
    if(fp != NULL) {
        stack = stackInit();
        initKeywords();
        initPuncSet();
        initHexSet();

        Token* tk = nextToken();
        while (tk->type != END) {
            printf(tokenToString(tk));
            memset(token, 0, sizeof(token));
            memset(potentialKw, 0, sizeof(potentialKw));
            tk = nextToken();
        }
        printf(tokenToString(tk));

        fclose(fp);
    }
    return 0;
}

/**
 * Returns the next token in the C file.
 * @return the next token in the C file
 */
Token* nextToken(){
    char c;
    int i = 0;
    Token* tk;

    skipWhiteSpaces();

    //if at end of input stream
    c = getChar();
    if((int) c == 0){
        return tokenInit("EOF", END);
    }

    token[i++] = c;

    //check for integer or float
    tk = checkConstantNumbers(token, c, i);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for string constant
    tk = checkConstantString(token, c, i);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for char constant
    tk = checkConstantChar(token, c, i);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for  block comment
    tk = checkBlockComment(token);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for line comment
    tk = checkLineComment(token);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for keyword
    tk = checkKeyword(token);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for punctuators
    tk = checkPunctuators(token);
    if(tk->type != NULL_TYPE){
        return tk;
    }

    //check for identifier
    tk = checkIdentifier(token);
    if(tk->type != NULL_TYPE) {
        return tk;
    }

    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a number constant.
 * @param token the token char array which that will contain the token.
 * @param c the current character being looked at.
 * @param i the index used for token char array.
 * @return a number token depending on the type, or null if not number constant
 */
Token* checkConstantNumbers(char token[], char c, int i){
    int state;
    if(isdigit(c) || c == '.'){
        if(c == '.'){
            state = 4;
            c = getChar();
            char d = getChar();
            _Bool variadic = c == '.' && d == '.';
            if(variadic){
                return tokenInit("...", VARIADIC);
            }
            else if(!isdigit(c)){
                ungetChar(d);
                ungetChar(c);
                return tokenInit(".", PERIOD);
            }
            ungetChar(d);
            ungetChar(c);
        }
        else {
            char d = getChar();
            if(c == '0' && isdigit(d)){
                state = 9;
            }
            else {
                state = 2;
            }
            ungetChar(d);
        }

        c = getChar();
        while(state > 0){
            switch (state) {
                case 2:
                    if (isdigit(c))
                        state = 2;
                    else if (c == 'e' || c == 'E')
                        state = 5;
                    else if (c == '.')
                        state = 3;
                    else if (isIntSuffix(c)) {
                        state = 12;
                    }
                    else
                        state = -2;
                    break;
                case 3:
                    if (isdigit(c))
                        state = 8;
                    else
                        state = -3;
                    break;
                case 4:
                    if (isdigit(c))
                        state = 3;
                    else
                        state = -4;
                    break;
                case 5:
                    if (isdigit(c))
                        state = 6;
                    else if (c == '-' || c == '+')
                        state = 7;
                    else
                        state = -5;
                    break;
                case 6:
                    if (isdigit(c))
                        state = 6;
                    else if (isFloatSuffix(c)){
                        state = 19;
                    }
                    else
                        state = -6;
                    break;
                case 7:
                    if(isdigit(c))
                        state = 6;
                    else
                        state = -7;
                    break;
                case 8:
                    if(isdigit(c))
                        state = 8;
                    else if(c == 'e' || c == 'E')
                        state = 5;
                    else if(isFloatSuffix(c)) {
                        state = 19;
                    }
                    else
                        state = -8;
                    break;
                case 9:
                    if(c == 'x' || c == 'X'){
                        state = 11;
                    }
                    else{
                        state = 10;
                    }
                    break;

                    int num;
                case 10:
                    num = c - '0';
                    if(isdigit(c) && num < 7)
                        state = 10;
                    else if(isIntSuffix(c)){
                        state = 12;
                    }
                    else
                        state = -10;
                    break;
                case 11:
                    if(isdigit(c) || hexSetContains(c))
                        state = 11;
                    else if(isIntSuffix(c)){
                        state = 12;
                    }
                    else
                        state = -11;
                    break;
                case 12:
                    if(c == 'U')
                        state = 13;
                    else if(c == 'u')
                        state = 14;
                    else if(c == 'L')
                        state = 15;
                    else if(c == 'l')
                        state = 16;
                    else if(punctuatorsContains(c))
                        state = -2;
                    else
                        state = -12;
                    break;
                case 13:
                    if(c == 'L')
                        state = 15;
                    else if(punctuatorsContains(c))
                        state = -2;
                    else
                        state = -12;
                    break;
                case 14:
                    if(c == 'l')
                        state = 16;
                    else if(punctuatorsContains(c))
                        state = -2;
                    else
                        state = -12;
                    break;
                case 15:
                    if(c == 'L')
                        state = 17;
                    else if(punctuatorsContains(c))
                        state = -2;
                    else
                        state = -12;
                    break;
                case 16:
                    if(c == 'l')
                        state = 18;
                    else if(punctuatorsContains(c))
                        state = -2;
                    else
                        state = -12;
                    break;
                case 17:
                    state = -2;
                    break;
                case 18:
                    if(punctuatorsContains(c))
                        state = -2;
                    else
                        state = -12;
                    break;
                case 19:
                    if(punctuatorsContains(c)){
                        state = -6;
                    }
                    else
                        state = -12;
                    break;
                case 20:
                    state = -6;
                    break;
                default:
                    state = -1;
            }
            if (state > 0) {
                token[i++] = c;
                token[i] = '\0';
                c = getChar();
            }
        }
        ungetChar(c);
        switch(-state){
            case 2:
                return tokenInit(token, INTEGER_CONSTANT);
                //CLion IDE says that "1." format in case -3 turns number to zero. Online gcc compilers just make
                //it an integer, usually a very small negative. I give it the lexeme 0 with INTEGER_CONSTANT as token.
            case 3:
                return tokenInit("0", INTEGER_CONSTANT);
            case 4:
                return tokenInit(token, PERIOD);
            case 6:
                return tokenInit(token, FLOAT_CONSTANT);
            case 8:
                return tokenInit(token, FLOAT_CONSTANT);
            case 10:
                return tokenInit(token, BAD_OCTAL);
            case 11:
                return tokenInit(token, BAD_HEX);
            case 12:
                return tokenInit(token, BAD_SUFFIX_TOKEN);
            default:
                return tokenInit(token, BAD_FLOAT);
        }
    }
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a string constant.
 * @param token the token char array which that will contain the token.
 * @param c the current character being looked at.
 * @param i the index used for token char array.
 * @return a string token, or null if not string constant
 */
Token* checkConstantString(char token[], char c, int i){
    if(c == '"'){
        _Bool unbalancedQuote = 0;
        while((c = getChar()) != '"' ||
              (token[i - 1] == '\\' && token[i - 2] != '\\')){
            if(c =='"' || c == 0){
                unbalancedQuote = 1;
            }
            else{
                unbalancedQuote = 0;
            }
            if((int) c == 0 )
                break;
            token[i++] = c;
        }
        token[i] = c;

        if(c == '"'){
            unbalancedQuote = 0;
        }

        if(unbalancedQuote)
            return tokenInit(token, BAD_STRING);

        return tokenInit(token, STRING_CONSTANT);
    }
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a character constant.
 * @param token the token char array which that will contain the token.
 * @param c the current character being looked at.
 * @param i the index used for token char array.
 * @return a character token, or null if not character constant
 */
Token* checkConstantChar(char token[], char c, int i){
    if(c == '\''){
        _Bool unbalancedSingleQuote = 0;
        while((c = getChar()) != '\'' ||
              (token[i - 1] == '\\' && token[i - 2] != '\\')){
            if(c =='\'' || c == 0){
                unbalancedSingleQuote = 1;
            }
            else{
                unbalancedSingleQuote = 0;
            }
            if((int) c == 0 )
                break;
            token[i++] = c;
        }
        token[i] = c;

        if(c == '\''){
            unbalancedSingleQuote = 1;
        }

        if(unbalancedSingleQuote)
            return tokenInit(token, BAD_CHAR);

        return tokenInit(token, CHAR_CONSTANT);
    }
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a block comment. If so, the lexeme will only be '/*'
 * @param token the token char array which that will contain the token.
 * @return a block comment token, or null if not block comment
 */
Token* checkBlockComment(char token[]){
    char c = getChar();
    if(token[0] == '/' && c == '*') {
        token[1] = '*';
        char first = getChar();
        char second = getChar();
        while (first != '*' && second != '/') {
            if((int) first == 0){
                return tokenInit(token, BAD_COMMENT);
            }
            first = second;
            second = getChar();
        }
        return tokenInit(token, BLOCK_COMMENT);
    }
    ungetChar(c);
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a line comment. If so, the lexeme will only be '//'
 * @param token the token char array which that will contain the token
 * @return a line comment token, or null if not block comment
 */
Token* checkLineComment(char token[]){
    char c = getChar();
    if(token[0] == '/' && c =='/'){
        token[1] = '/';
        while((c = getChar()) != '\n' && c != 0){
            //Nothing to do, parse until we hit the next line.
        }
        return tokenInit(token, LINE_COMMENT);
    }
    ungetChar(c);
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is an identifier.
 * @param token the token char array which that will contain the token
 * @return an identifier token, or null if not identifier
 */
Token* checkIdentifier(char token[]){
    char c = token[0];
    int i = 1;
    if(isalpha(c) || c == '_' || c == '$'){
        c = getChar();
        while(isalpha(c) || isdigit(c) || c == '_' || c == '$'){
            token[i++] = c;
            c = getChar();
        }
        ungetChar(c);

        return tokenInit(token, IDENTIFIER);
    }
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a keyword.
 * @param token the token char array which that will contain the token
 * @return a keyword token, or null if not identifier
 */
Token* checkKeyword(char token[]){
    int i = 1;
    if(isspace(token[0])){
        token[0] = 0;
        i = 0;
    }

    potentialKw[0] = token[0];

    char c = getChar();

    while(!isspace(c) && c != 0){
        potentialKw[i++] = c;
        c = getChar();
    }
    if(c == ' '){
        ungetChar(c);
    }

    char newString[i];
    strncpy(newString, potentialKw, i);

    Type type = getTokenTypeFromString(newString);

    if(type != NULL_TYPE){
        return tokenInit(potentialKw, type);
    }
    else{
        //> and not >= because we already have the first character in the token[].
        //when the next function is called, the first character should be the one
        //looked at in the token[], and then the stack.
        int j;
        for(j = i - 1; j > 0; j--){
            ungetChar(potentialKw[j]);
        }
    }
    return tokenInit("", NULL_TYPE);
}

/**
 * Determines if the next token is a punctuator.
 * @param token the token char array which that will contain the token
 * @return a punctuator token, or null if not identifier
 */
Token* checkPunctuators(char token[]){
    char c = token[0];
    char first;
    char second;
    switch(c){
        case '[':
            return tokenInit(token, OPEN_BRACKET);
        case ']':
            return tokenInit(token, CLOSE_BRACKET);
        case '(':
            return tokenInit(token, OPEN_PARENT);
        case ')':
            return tokenInit(token, CLOSE_PARENT);
        case '{':
            return tokenInit(token, OPEN_BRACE);
        case '}':
            return tokenInit(token, CLOSE_BRACKET);
        case '.': //case covered in numbers method. Gets periods and variadics. checks for .. and returns bad token.
            first = getChar();
            if(first == '.'){
                token[1] = first;
                return tokenInit(token, BAD_PUNCTUATOR);
            }
            else{
                ungetChar(first);
                return tokenInit("", NULL_TYPE);
            }
        case '-':
            first = getChar();
            if(first == '>') {
                token[1] = first;
                return tokenInit(token, ARROW);
            }
            else if(first == '-'){
                token[1] = first;
                return tokenInit(token, DECREMENT);
            }
            else if(first == '='){
                token[1] = first;
                return tokenInit(token, MINUS_EQUALS);
            }
            else{
                ungetChar(first);
                return tokenInit(token, SUB_OP);
            }
        case '+':
            first = getChar();
            if(first == '+'){
                token[1] = first;
                return tokenInit(token, INCREMENT);
            }
            else if(first == '='){
                token[1] = first;
                return tokenInit(token, PLUS_EQUALS);
            }
            else{
                ungetChar(first);
                return tokenInit(token, ADD_OP);
            }
        case '*':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return tokenInit(token, MULT_EQUALS);
            }
            else{
                ungetChar(first);
                return tokenInit(token, MULT_OP);
            }
        case '/':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return tokenInit(token, DIV_EQUALS);
            }
            else{
                ungetChar(first);
                return tokenInit(token, DIV_OP);
            }
        case '%':
            first = getChar();
            second = getChar();
            char third = getChar();
            if(first == ':' && second == '%' && third == ':'){
                token[1] = first;
                token[2] = second;
                token[3] = third;
                return tokenInit(token, PERCENT_COLON_PERCENT_COLON);
            }
            else if(first == '='){
                token[1] = first;
                ungetChar(third);
                ungetChar(second);
                return tokenInit(token, MOD_EQUALS);
            }
            else if(first == ':'){
                token[1] = first;
                ungetChar(third);
                ungetChar(second);
                return tokenInit(token, PERCENT_COLON);
            }
            else if(first == '>'){
                token[1] = first;
                return tokenInit(token, RIGHT_ANGLE_PERCENT);
            }
            else{
                ungetChar(third);
                ungetChar(second);
                ungetChar(first);
                return tokenInit(token, MOD_OP);
            }
        case '&':
            first = getChar();
            if(first == '&'){
                token[1] = first;
                return tokenInit(token, LOG_AND);
            }
            else if(first == '='){
                token[1] = first;
                return tokenInit(token, ASSIGN_AND);
            }
            else{
                ungetChar(first);
                return tokenInit(token, BIT_AND);
            }
        case '~':
            return tokenInit(token, BIT_NOT);
        case '!':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return tokenInit(token, NOT_EQUAL);
            }
            else{
                ungetChar(first);
                return tokenInit(token, LOG_NOT);
            }
        case '^':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return tokenInit(token, ASSIGN_XOR);
            }
            else{
                ungetChar(first);
                return tokenInit(token, BIT_XOR);
            }
        case '|':
            first = getChar();
            if(first == '|'){
                token[1] = first;
                return tokenInit(token, LOG_OR);
            }
            else if(first == '='){
                token[1] = first;
                return tokenInit(token, ASSIGN_OR);
            }
            else{
                ungetChar(first);
                return tokenInit(token, BIT_OR);
            }
        case '<':
            first = getChar();
            second = getChar();
            if(first == '<' && second == '='){
                token[1] = first;
                token[2] = second;
                return tokenInit(token, ASSIGN_LEFT);
            }
            else if(first == '<'){
                token[1] = first;
                ungetChar(second);
                return tokenInit(token, BIT_LEFT);
            }
            else if(first == '='){
                token[1] = first;
                ungetChar(second);
                return tokenInit(token, LESS_THAN_EQUAL);
            }
            else if(first == ':'){
                token[1] = first;
                ungetChar(second);
                return tokenInit(token, LEFT_ANGLE_COLON);
            }
            else if(first == '%'){
                token[1] = first;
                ungetChar(second);
                return tokenInit(token, LEFT_ANGLE_PERCENT);
            }
            else{
                ungetChar(second);
                ungetChar(first);
                return tokenInit(token, LESS_THAN);
            }
        case '>':
            first = getChar();
            second = getChar();
            if(first == '>' && second == '='){
                token[1] = first;
                token[2] = second;
                return tokenInit(token, ASSIGN_RIGHT);
            }
            else if(first == '>'){
                token[1] = first;
                ungetChar(second);
                return tokenInit(token, BIT_RIGHT);
            }
            else if(first == '='){
                token[1] = first;
                ungetChar(second);
                return tokenInit(token, GREATER_THAN_EQUAL);
            }
            else{
                ungetChar(second);
                ungetChar(first);
                return tokenInit(token, GREATER_THAN);
            }
        case '=':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return tokenInit(token, EQUAL);
            }
            else{
                ungetChar(first);
                return tokenInit(token, ASSIGN);
            }
        case '?':
            return tokenInit(token, TERNARY);
        case ':':
            first = getChar();
            if(first == '>'){
                token[1] = first;
                return tokenInit(token, RIGHT_ANGLE_COLON);
            }
            else{
                ungetChar(first);
                return tokenInit(token, COLON);
            }
        case ';':
            return tokenInit(token, SEMI_COLON);
        case ',':
            return tokenInit(token, COMMA);
        case '#':
            return tokenInit(token, POUND);
        default:
            return tokenInit("", NULL_TYPE);
    }
}

/**
 * Gets the next char from the input stream, or the top char on the stack if we saved any.
 * @return the next char from the input stream, or the top char on the stack if we saved any.
 */
char getChar(){
    if(isEmpty(stack)){
        int code = fgetc(fp);
        if(code != -1){
            return (char) code;
        }
    }
    else{
        return pop(stack);
    }
    return 0;
}

/**
 * Ungets a character from the input stream by storing in stack. If getChar is called, char from stack
 * will be popped before the next character is read from input stream.
 * @param c the char to save
 */
void ungetChar(char c){
    push(stack, c);
}

/**
 * Reads through white spaces on input stream to get to next token.
 */
void skipWhiteSpaces(){
    _Bool whiteSpace = 1;
    char ch = getChar();
    int c = (int) ch;
    while(whiteSpace && c > 0){
        if(isspace(c) || c == '\r'){
            c = getChar();
        } else {
            whiteSpace = 0;
            ungetChar((char) c);
        }
    }
}

/**
 * Creates the mapping for keywords to the their respective Tokens beteween two arrays.
 */
void initKeywords(){
    int i = 0;
    keywordTokens[i] = AUTO_KEYWORD;    keywords[i++] = "auto";
    keywordTokens[i] = BREAK_KEYWORD;    keywords[i++] = "break";
    keywordTokens[i] = CASE_KEYWORD;    keywords[i++] = "case";
    keywordTokens[i] = CHAR_KEYWORD;    keywords[i++] = "char";
    keywordTokens[i] = CONST_KEYWORD;    keywords[i++] = "const";
    keywordTokens[i] = CONTINUE_KEYWORD;    keywords[i++] = "continue";
    keywordTokens[i] = DEFAULT_KEYWORD;    keywords[i++] = "default";
    keywordTokens[i] = DO_KEYWORD;    keywords[i++] = "do";
    keywordTokens[i] = DOUBLE_KEYWORD;    keywords[i++] = "double";
    keywordTokens[i] = ELSE_KEYWORD;   keywords[i++] = "else";
    keywordTokens[i] = ENUM_KEYWORD;    keywords[i++] = "enum";
    keywordTokens[i] = EXTERN_KEYWORD;    keywords[i++] = "extern";
    keywordTokens[i] = FLOAT_KEYWORD;    keywords[i++] = "float";
    keywordTokens[i] = FOR_KEYWORD;    keywords[i++] = "for";
    keywordTokens[i] = GOTO_KEYWORD;    keywords[i++] = "goto";
    keywordTokens[i] = IF_KEYWORD;    keywords[i++] = "if";
    keywordTokens[i] = INLINE_KEYWORD;    keywords[i++] = "inline";
    keywordTokens[i] = INT_KEYWORD;    keywords[i++] = "int";
    keywordTokens[i] = LONG_KEYWORD;    keywords[i++] = "long";
    keywordTokens[i] = REGISTER_KEYWORD;    keywords[i++] = "register";
    keywordTokens[i] = RESTRICT_KEYWORD;    keywords[i++] = "restrict";
    keywordTokens[i] = RETURN_KEYWORD;    keywords[i++] = "return";
    keywordTokens[i] =  SHORT_KEYWORD;   keywords[i++] = "short";
    keywordTokens[i] = SIGNED_KEYWORD;    keywords[i++] = "signed";
    keywordTokens[i] = SIZEOF_KEYWORD;    keywords[i++] = "sizeof";
    keywordTokens[i] = STATIC_KEYWORD;    keywords[i++] = "static";
    keywordTokens[i] = STRUCT_KEYWORD;    keywords[i++] = "struct";
    keywordTokens[i] = SWITCH_KEYWORD;    keywords[i++] = "switch";
    keywordTokens[i] = TYPEDEF_KEYWORD;    keywords[i++] = "typedef";
    keywordTokens[i] = UNION_KEYWORD;    keywords[i++] = "union";
    keywordTokens[i] = UNSIGNED_KEYWORD;    keywords[i++] = "unsigned";
    keywordTokens[i] = VOID_KEYWORD;    keywords[i++] = "void";
    keywordTokens[i] = VOLATILE_KEYWORD;    keywords[i++] = "volatile";
    keywordTokens[i] = WHILE_KEYWORD;    keywords[i] = "while";
}

/**
 * Creates a list of punctuators for use in program.
 */
void initPuncSet(){
    int i = 0;
    punctuators[i++] = '[';
    punctuators[i++] = ']';
    punctuators[i++] = '(';
    punctuators[i++] = ')';
    punctuators[i++] = '{';
    punctuators[i++] = '}';
    punctuators[i++] = '-';
    punctuators[i++] = '+';
    punctuators[i++] = '&';
    punctuators[i++] = '*';
    punctuators[i++] = '~';
    punctuators[i++] = '!';
    punctuators[i++] = '|';
    punctuators[i++] = '/';
    punctuators[i++] = '%';
    punctuators[i++] = '<';
    punctuators[i++] = '>';
    punctuators[i++] = '=';
    punctuators[i++] = '^';
    punctuators[i++] = '|';
    punctuators[i++] = '?';
    punctuators[i++] = ':';
    punctuators[i++] = ';';
    punctuators[i] = ',';
}

/**
 * Creates a set of all alphabetic characters allowed in hex numbers.
 */
void initHexSet(){
    int i = 0;
    hexSet[i++] = 'A';
    hexSet[i++] = 'B';
    hexSet[i++] = 'C';
    hexSet[i++] = 'D';
    hexSet[i++] = 'E';
    hexSet[i++] = 'F';
    hexSet[i++] = 'a';
    hexSet[i++] = 'b';
    hexSet[i++] = 'c';
    hexSet[i++] = 'd';
    hexSet[i++] = 'e';
    hexSet[i] = 'f';
}

/**
 * Checks if the character is an integer suffix.
 * @param c the character to be checked
 * @return true if the char is an int suffix, false otherwise.
 */
_Bool isIntSuffix(char c){
    return c == 'l' || c == 'L' || c == 'u' || c == 'U';
}

/**
 * Checks if the character is an floating point suffix.
 * @param c the character to be checked
 * @return true if the char is an floating point suffix, false otherwise.
 */
_Bool isFloatSuffix(char c){
    return c == 'f' || c == 'F' || c == 'l' || c == 'L';
}

Type getTokenTypeFromString(char* str){
    str = trimwhitespace(str);
    int i;
    for(i = 0; i < numKeywords; i++){
        if(strcmp(keywords[i], str) == 0){
            return keywordTokens[i];
        }
    }
    return NULL_TYPE;
}

_Bool hexSetContains(char c){
    int i;
    for(i = 0; i < numHexValues; i++){
        if(c == hexSet[i]){
            return 1;
        }
    }
    return 0;
}

_Bool punctuatorsContains(char c){
    int i;
    for(i = 0; i < numPuncuators; i++){
        if(c == punctuators[i]){
            return 1;
        }
    }
    return 0;
}

char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)){
        str++;
    }

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end+1) = 0;

    return str;
}