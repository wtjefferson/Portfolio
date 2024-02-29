
/*TODO chunk 3:
  1. Complete the parser
  2. Parse statements
  3. Build the Abstract Syntax Tree*/
/*TODO Helper Functions:
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool hadError = false;

typedef enum {
    // Single-character tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // Literals
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_ERROR,

    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

Token previous;

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

char advance() {
    scanner.current++;
    return scanner.current[-1];
}

void consumeToken(TokenType type) {
    previous = makeToken(type);
}

Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

typedef enum {
    NODE_BINARY_EXPR,
    NODE_GROUPING_EXPR,
    NODE_LITERAL_EXPR,
    NODE_UNARY_EXPR,
    // Add other node types here...
} NodeType;

typedef struct {
    NodeType type;
    int line; // For error reporting
} Node;

typedef struct {
    Node base; // Inherited base structure
    Token operator;
    Node* left;
    Node* right;
    void (*accept)(struct Expr*, ExprVisitor*);
} BinaryExprNode;

BinaryExprNode* newBinaryExprNode(Token operator, Node* left, Node* right) {
    BinaryExprNode* node = malloc(sizeof(BinaryExprNode));
    node->base.type = NODE_BINARY_EXPR;
    node->operator = operator;
    node->left = left;
    node->right = right;
    node->accept = acceptBinaryExpr;
    return node;
}

typedef struct Visitor {
    void (*visitLiteralExpr)(LiteralExpr* expr);
    void (*visitUnaryExpr)(UnaryExpr* expr);
    void (*visitBinaryExpr)(BinaryExprNode* expr);
    void (*visitGroupingExpr)(GroupingExpr* expr);
} Visitor;

typedef enum {
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_GROUPING,
} ExprType;

typedef union {
    double number;
    const char* string;
    bool boolean;
    char nil;
} LiteralValue;

typedef enum {
    LITERAL_TYPE_NUMBER,
    LITERAL_TYPE_STRING,
    LITERAL_TYPE_BOOLEAN,
    LITERAL_TYPE_NIL
} LiteralType;

typedef struct {
    Node base;
    LiteralValue value;
    LiteralType valueType;
} LiteralExpr;

typedef struct {
    Node base;
    Token operator;
    Expr* operand;
} UnaryExpr;

typedef struct {
    Node base;
    Expr* expression;
} GroupingExpr;

typedef struct {
    Expr* left;
    Expr* right;
    Token operator;
} BinaryExpr;

typedef struct Expr {
    ExprType type;
    union {
        LiteralExpr literal;
        UnaryExpr unary;
        BinaryExpr binary;
        GroupingExpr grouping;
    } as;
} Expr;

LiteralExpr* newLiteralExpr(LiteralValue value, LiteralType valueType) {
    LiteralExpr* expr = (LiteralExpr*)malloc(sizeof(LiteralExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
    }
    expr->base.type = NODE_LITERAL_EXPR;
    expr->value = value;
    expr->valueType = valueType;
    return expr;
}

UnaryExpr* newUnaryExpr(Token operator, Expr* operand) {
    UnaryExpr* expr = (UnaryExpr*)malloc(sizeof(UnaryExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
    }
    expr->base.type = NODE_UNARY_EXPR;
    expr->operator = operator;
    expr->operand = operand;
    return expr;
}

Expr* newBinaryExpr(Expr* left, Expr* right, Token operator) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->as.binary.left = left;
    expr->as.binary.right = right;
    expr->as.binary.operator = operator;
    return expr;
}

GroupingExpr* newGroupingExpr(Expr* expression) {
    GroupingExpr* expr = (GroupingExpr*)malloc(sizeof(GroupingExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
    }
    expr->base.type = NODE_GROUPING_EXPR;
    expr->expression = expression;
    return expr;
}

typedef void (*VisitLiteralExpr)(LiteralExpr* expr);
typedef void (*VisitUnaryExpr)(UnaryExpr* expr);
typedef void (*VisitBinaryExpr)(BinaryExpr* expr);
typedef void (*VisitGroupingExpr)(GroupingExpr* expr);

void acceptLiteralExpr(LiteralExpr* expr, ExprVisitor* visitor) {
    if (visitor->visitLiteralExpr != NULL){
        visitor->visitLiteralExpr(expr);
    }
}

void acceptUnaryExpr(UnaryExpr* expr, ExprVisitor* visitor) {
    if (visitor->visitUnaryExpr != NULL){
        visitor->visitUnaryExpr(expr);
    }
}

void acceptBinaryExpr(BinaryExpr* expr, ExprVisitor* visitor) {
    if (visitor->visitBinaryExpr != NULL) {
        visitor->visitBinaryExpr(expr);
    }
}

void acceptGroupingExpr(GroupingExpr* expr, ExprVisitor* visitor) {
    if (visitor->visitGroupingExpr != NULL){
        visitor->visitGroupingExpr(expr);
    }
}

typedef struct {
    VisitLiteralExpr visitLiteralExpr;
    VisitUnaryExpr visitUnaryExpr;
    VisitBinaryExpr visitBinaryExpr;
    VisitGroupingExpr visitGroupingExpr;
} ExprVisitor;

void printBinaryExpr(BinaryExpr* expr) {
    printf("BinaryExpr: Operator=%c\n", expr->operator);
    expr->left->accept(expr->left, &printVisitor);
    expr->right->accept(expr->right, &printVisitor);
}

void printLiteralValue(LiteralExpr* expr) {
    if (!expr) return; // Safety check

    switch(expr->valueType) {
        case LITERAL_TYPE_NUMBER:
            printf("%f", expr->value.number);
            break;
        case LITERAL_TYPE_STRING:
            printf("\"%s\"", expr->value.string);
            break;
        case LITERAL_TYPE_BOOLEAN:
            printf(expr->value.boolean ? "true" : "false");
            break;
    }
}

void printLiteralExpr(LiteralExpr* expr) {
    if (!expr) return; // Safety check

    printLiteralValue(expr);
    printf("\n");
}

void printUnaryExpr(UnaryExpr* expr) {
    printf("Unary Expression: Operator=%c\n", expr->operator);
    expr->operand->accept(expr->operand, &printVisitor);
}

void printGroupingExpr(GroupingExpr* expr){
    printf("Grouping Expression:\n");
    expr->expression->accept (expr->expression, &printVisitor);
}


ExprVisitor printVisitor = {
    printLiteralExpr,
    printUnaryExpr,
    printBinaryExpr,
    printGroupingExpr
};

char* extractString(const char* start, int length) {
    // Allocate memory for the new string (excluding quotes)
    char* str = malloc(length - 1);
    if (str == NULL) {
        // Handle allocation failure
        return NULL;
    }

    // Copy the characters (excluding the surrounding quotes)
    strncpy(str, start + 1, length - 2);
    str[length - 2] = '\0'; // Null-terminate the string

    return str;
}

void errorAtCurrent(const char* message) {
    if (hadError) return; // Prevent multiple errors for the same issue
    hadError = true;

    fprintf(stderr, "[line %d] Error at '%.*s': %s\n", 
            previous.line, previous.length, previous.start, message);
}

Expr* parsePrimary() {
    if (match(TOKEN_FALSE)) {
        LiteralValue value = {.boolean = false};
        return newLiteralExpr(value, LITERAL_TYPE_BOOLEAN);
    }
    if (match(TOKEN_TRUE)) {
        LiteralValue value = {.boolean = true};
        return newLiteralExpr(value, LITERAL_TYPE_BOOLEAN);
    }
    if (match(TOKEN_NIL)) {
        LiteralValue value;
        value.boolean = false;
        return newLiteralExpr(value, LITERAL_TYPE_NIL);
    }

    if (match(TOKEN_NUMBER)) {
        double number = strtod(previous.start, NULL);
        LiteralValue value = {.number = number};
        return newLiteralExpr(value, LITERAL_TYPE_NUMBER);
    }

    if (match(TOKEN_STRING)) {
        char* string = extractString(previous.start, previous.length);
        LiteralValue value = {.string = string};
        return newLiteralExpr(value, LITERAL_TYPE_STRING);
    }

    if (match(TOKEN_LEFT_PAREN)) {
        Expr* expr = parseExpression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
        return newGroupingExpr(expr);
    }
    // Handle unexpected tokens
    errorAtCurrent("Expect expression.");
    return NULL; // Indicate an eror occurred
}


Expr* parseUnary() {
    if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
        Token operator = previousToken();
        Expr* right = parseUnary();
        return newUnaryExpr(operator, right);
    }

    return parsePrimary();
}

Expr* parseMultiplication() {
    Expr* expr = parseUnary();

    while (match(TOKEN_STAR) || match(TOKEN_SLASH)) {
        Token operator = previousToken();
        Expr* right = parseUnary();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parseAddition() {
    Expr* expr = parseMultiplication();

    while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) {
        Token operator = previousToken();
        Expr* right = parseMultiplication();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parseComparison() {
    Expr* expr = parseAddition();

    while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) ||
           match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)) {
        Token operator = previousToken();
        Expr* right = parseAddition();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parseEquality() {
    Expr* expr = parseComparison();

    while (match(TOKEN_EQUAL_EQUAL) || match(TOKEN_BANG_EQUAL)) {
        Token operator = previousToken();
        Expr* right = parseComparison();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parse() {
    hadError = false; // Reset error flag
    Expr* expression = parseExpression();
    if (hadError) {
        // Handle the error
        return NULL;
    }
    return expression;
}

Expr* parseExpression() {
    // Start with the lowest precedence function
    return parseEquality();
}


void interpret(Expr* expr) {
    // Example: using the print visitor
    switch (expr->type) {
        case EXPR_LITERAL:
            acceptLiteralExpr(&expr->as.literal, &printVisitor);
            break;
        // ... other cases
    }
}

void interpretExpr(Expr* expr) {
    switch (expr->type) {
        case EXPR_BINARY:
            // Handle binary expression
            break;
        // ... other cases
    }
}

char peek() {
    return *scanner.current;
}

char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;

    scanner.current++;
    return true;
}

Token number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    consumeToken(TOKEN_NUMBER);
    return previous;
}

Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    consumeToken(identifierType());
    return previous;
}

TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        // ... other keywords
        default: return TOKEN_IDENTIFIER;
    }
}

TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

bool isAtEnd() {
    return *scanner.current == '\0';
}

void skipWhitespace() {
    while (1) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token previousToken;
bool hadError = false;

bool match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token previousToken() {
    return previous;
}

bool check(TokenType type) {
    if (isAtEnd()) return false;
    return peek() == type;
}

void consume(TokenType type, const char* message) {
    if (check(type)) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

void errorAtCurrent(const char* message) {
    errorAt(&scanner.current, message);
}

void errorAt(Token* token, const char* message) {
    if (hadError) return;
    hadError = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
}

Token scanToken() {
    skipWhitespace();

    scanner.start = scanner.current;
    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    // Add logic to handle different characters
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);

    }

    return errorToken("Unexpected character.");
}


void runFile(const char* path) {
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);

    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';

    fclose(file);

    // Process the buffer containing the file contents
    // run(buffer);

    free(buffer);
}

void runPrompt() {
    char line[1024];

    while (1) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        // Process the input line
        // run(line);
    }
}

void error(int line, const char* message) {
    fprintf(stderr, "[line %d] Error: %s\n", line, message);
    // Handle the error, possibly by setting a flag to indicate an error state
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        printf("Usage: [interpreter] [script]\n");
        return 1;
    }

    if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}
