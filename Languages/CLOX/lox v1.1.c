// Dynamic typing:
// Not static typing
// Vars can store values of any type
// A single var can store vars of different types at different times
// Illegal operations are detected and reported at runtime (number/string)

// Automatic memory management:
// Tracing garbage collection

// Data Types:
// Booleans- true; false;
// Numbers- 1234; 12.23; ints and floats basically
// Strings- "I am a string"; ""; "123";
// Nil

// Expressions:
// Arithmatic- add + me; subtract - me; multiply * me; divide / me; -negateMe;
    // + works on strings, but any other pass other than a number yields an error
    // ternary operator: condition ? thenArm : elseArm;
// Comparision and equality- <, <=, >, >=, ==
// Logicial operators- !, and, or
// Precedence and grouping- same as C

// Statements: print breakfast

// Variables: var imAVariable = "here is my value"; var iAmNil; var breakfast = "bagels";

// Control flow: if, while, else, for

// Functions: fun printSum(a, b) { print a + b;} printSum(a, b);
// First class in LOX (real values that you can get a reference to)
// You can declare local functions inside another function

// Classes:

// Begin with a class

// Left off on pg. 38
// 38-56
// 63-75

/*TODO chunk 2:
  5. Define functions for handling expressions
  6. Building the syntax tree
  7. Test and optimize*/
/*TODO Helper Functions:
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
} LiteralValue;

typedef enum {
    LITERAL_TYPE_NUMBER,
    LITERAL_TYPE_STRING,
    LITERAL_TYPE_BOOLEAN
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

LiteralExpr* newLiteralExpr(LiteralValue value) {
    LiteralExpr* expr = (LiteralExpr*)malloc(sizeof(LiteralExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
    }
    expr->base.type = NODE_LITERAL_EXPR;
    expr->value = value;
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

    return makeToken(TOKEN_NUMBER);
}

Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    return makeToken(identifierType());
}

TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
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
        // Add cases for other single characters
        // ...
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
