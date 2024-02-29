/*TODO chunk 3:
  2. Parse statements
  3. Build the Abstract Syntax Tree*/
/*TODO Helper Functions:
  */
//////////////////////////
// Include statements
//////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//////////////////////////
// Type Definitions
//////////////////////////
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

typedef enum {
    NODE_BINARY_EXPR,
    NODE_GROUPING_EXPR,
    NODE_LITERAL_EXPR,
    NODE_UNARY_EXPR,
    // Add other node types here...
} NodeType;

typedef struct {
    NodeType type;
    int line; // optional, for error reporting
} Node;

typedef enum {
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    // ... other types of expressions
} ExprType;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_STRING,
    // Add more types as needed
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        char* string;
        // Add more fields for different types
    } as;
} Value;

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

// Forward declaration of Expr
typedef struct Expr Expr;

typedef struct {
    Node base;
    Token operator;
    Expr* operand;
} UnaryExpr;

typedef struct {
    Expr* left;
    Expr* right;
    Token operator;
} BinaryExpr;

typedef struct {
    Node base;
    Expr* expression;
} GroupingExpr;

struct Expr {
    ExprType type;
    union {
        LiteralExpr literal;
        UnaryExpr unary;
        BinaryExpr binary;
        GroupingExpr grouping;
        // ... other specific expression structures
    } as;
};

typedef enum {
    STMT_EXPRESSION,
    STMT_PRINT,
    STMT_VAR,
    STMT_BLOCK
} StmtType;

typedef struct Stmt Stmt;

typedef struct {
    Expr* expression;
} PrintStmt;

typedef struct {
    Token name;
    Expr* initializer;
} VarStmt;

typedef struct {
    Stmt** statements;
    int count;
} BlockStmt;

struct Stmt {
    StmtType type;
    union {
        Expr* expression; // For expression statements
        PrintStmt print;
        VarStmt var;
        BlockStmt block;
    } as;
};

typedef struct ExprVisitor ExprVisitor;

typedef struct {
    Node base; // Inherited base structure
    Token operator;
    Node* left;
    Node* right;
    void (*accept)(struct Expr*, ExprVisitor*);
} BinaryExprNode;

typedef struct Visitor {
    void (*visitLiteralExpr)(LiteralExpr* expr);
    void (*visitUnaryExpr)(UnaryExpr* expr);
    void (*visitBinaryExpr)(BinaryExprNode* expr);
    void (*visitGroupingExpr)(GroupingExpr* expr);
} Visitor;

typedef void (*VisitLiteralExpr)(LiteralExpr* expr);
typedef void (*VisitUnaryExpr)(UnaryExpr* expr);
typedef void (*VisitBinaryExpr)(BinaryExpr* expr);
typedef void (*VisitGroupingExpr)(GroupingExpr* expr);

struct ExprVisitor{
    VisitLiteralExpr visitLiteralExpr;
    VisitUnaryExpr visitUnaryExpr;
    VisitBinaryExpr visitBinaryExpr;
    VisitGroupingExpr visitGroupingExpr;
};

typedef struct {
    Stmt base;  // Inherit from Stmt
    Expr* expression;
} ExpressionStmt;

typedef struct {
    Stmt base;  // Inherit from Stmt
    Expr* expression;
} PrintStmt;

typedef struct EnvEntry {
    char* name;
    Value value;
    struct EnvEntry* next;
} EnvEntry;

typedef struct {
    EnvEntry* head;
} Environment;

typedef struct {
    Node base;
    Token name;  // Token representing the variable name
} VariableExpr;

typedef struct {
    Node base;
    Token name;
    Expr* value;
} AssignExpr;

//////////////////////////
// Macros and global variables
//////////////////////////
bool hadError = false;
Token previous;
Scanner scanner;
const Value NIL_VALUE = { .type = VAL_NIL };

//////////////////////////
// Function Declarations
//////////////////////////
void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

char advance() {
    scanner.current++;
    return scanner.current[-1];
}

bool isAtEnd() {
    return *scanner.current == '\0';
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

char peek() {
    return *scanner.current;
}

bool check(TokenType type) {
    if (isAtEnd()) return false;
    return peek() == type;
}

void errorAtCurrent(const char* message) {
    if (hadError) return; // Prevent multiple errors for the same issue
    hadError = true;

    fprintf(stderr, "[line %d] Error at '%.*s': %s\n", 
            previous.line, previous.length, previous.start, message);
}

char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

Value createNumberValue(double number) {
    Value value;
    value.type = VAL_NUMBER;
    value.as.number = number;
    return value;
}

bool isNumber(Value value) {
    return value.type == VAL_NUMBER;
}

double asNumber(Value value) {
    return value.as.number;
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(value.as.boolean ? "true" : "false");
            break;
        case VAL_NUMBER:
            printf("%g", value.as.number);
            break;
        case VAL_STRING:
            printf("%s", value.as.string);
            break;
        // Handle other types
    }
}

void initEnvironment(Environment* env) {
    env->head = NULL;
}

void setVariable(Environment* env, Token name, Value value) {
    // Traverse the environment to find the variable and update its value
    // If the variable doesn't exist, handle the error (e.g., undefined variable)
}

EnvEntry* newEnvEntry(const char* name, Value value) {
    EnvEntry* entry = (EnvEntry*)malloc(sizeof(EnvEntry));
    entry->name = strdup(name); // Duplicate the string
    entry->value = value;
    entry->next = NULL;
    return entry;
}

void defineVariable(Environment* env, const char* name, Value value) {
    EnvEntry* entry = newEnvEntry(name, value);
    entry->next = env->head;
    env->head = entry;
}

Value* getVariable(Environment* env, const char* name) {
    for (EnvEntry* entry = env->head; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) {
            return &entry->value;
        }
    }
    return NULL; // Variable not found
}

Value getVariableValue(Environment* env, Token name) {
    Value* value = getVariable(env, name.start);
    if (value == NULL) {
        // Handle undefined variable error
    }
    return *value;
}

Expr* parseVariable() {
    Token name = previousToken();
    VariableExpr* expr = (VariableExpr*)malloc(sizeof(VariableExpr));
    expr->base.type = EXPR_VARIABLE;
    expr->name = name;
    return (Expr*)expr;
}

Expr* parseAssignment() {
    Expr* expr = parseEquality();

    if (match(TOKEN_EQUAL)) {
        Token equals = previousToken();
        Expr* value = parseAssignment();

        if (expr->type == EXPR_VARIABLE) {
            Token name = ((VariableExpr*)expr)->name;
            return newAssignExpr(name, value);
        }

        // Handle invalid assignment target
    }

    return expr;
}

Value evaluate(Expr* expr, Environment* env) {
    switch (expr->type) {
        case EXPR_BINARY:
            return evaluateBinaryExpr(&expr->as.binary);
        case EXPR_VARIABLE: {
            VariableExpr* varExpr = (VariableExpr*)expr;
            return getVariableValue(env, varExpr->name);
        }
        case EXPR_ASSIGN: {
            AssignExpr* assignExpr = (AssignExpr*)expr;
            Value value = evaluate(assignExpr->value, env);
            setVariable(env, assignExpr->name, value);
            return value;
        }
        // Handle other expressions
    }
}

void executeVarStmt(Environment* env, VarStmt* stmt) {
    Value value = NIL_VALUE; // Default to nil if no initializer
    if (stmt->initializer != NULL) {
        value = evaluate(stmt->initializer);
    }
    defineVariable(env, stmt->name.start, value);
}

void executeStatement(Environment* env, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_VAR:
            executeVarStmt(env, (VarStmt*)stmt);
            break;
        // ... other cases ...
    }
}

Value evaluateBinaryExpr(BinaryExpr* expr) {
    Value left = evaluate(expr->left);
    Value right = evaluate(expr->right);

    // Assuming both are numbers for simplicity; add type checking and error handling
    if (expr->operator.type == TOKEN_PLUS) {
        return createNumberValue(asNumber(left) + asNumber(right));
    }
    // Handle other operators
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

Token advanceToken() {
    if (!isAtEnd()) {
        previous = makeToken(scanner.current);
        advance();
    }
    return previous;
}

Token consumeToken(TokenType type, const char* message) {
    if (check(type)) {
        return advanceToken();
    }

    errorAtCurrent(message);
    previous = makeToken(type);
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

TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        // ... other keywords
        default: return TOKEN_IDENTIFIER;
    }
}

Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    consumeToken(identifierType());
    return previous;
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

    Token token = makeToken(...);
    previous = token;
    return token;
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

void errorAtCurrent(const char* message) {
    if (hadError) return; // Prevent multiple errors for the same issue
    hadError = true;

    fprintf(stderr, "[line %d] Error at '%.*s': %s\n", 
            previous.line, previous.length, previous.start, message);
}

Token consume(TokenType type, const char* message) {
    if (check(type)) {
        advance();
        return;
    }

    errorAtCurrent(message);
    return errorToken(message);
}

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

BinaryExprNode* newBinaryExprNode(Token operator, Node* left, Node* right) {
    BinaryExprNode* node = malloc(sizeof(BinaryExprNode));
    node->base.type = NODE_BINARY_EXPR;
    node->operator = operator;
    node->left = left;
    node->right = right;
    node->accept = acceptBinaryExpr;
    return node;
}

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
        if (!string) {
            // Memory allocation failed, handle error
            return NULL;
        }
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

    // Error handling for unexpected tokens
    // errorAtCurrent("Expect expression.");
    // return NULL;
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

Stmt* parsePrintStatement() {
    Expr* value = parseExpression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    PrintStmt printStmt = {value};
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_PRINT;
    stmt->as.print = printStmt;
    return stmt;
}

Stmt* parseVarDeclaration() {
    Token name = consume(TOKEN_IDENTIFIER, "Expect variable name.");

    Expr* initializer = NULL;
    if (match(TOKEN_EQUAL)) {
        initializer = parseExpression();
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    VarStmt varStmt = {name, initializer};
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_VAR;
    stmt->as.var = varStmt;
    return stmt;
}

Stmt* parseExpressionStatement() {
    Expr* expr = parseExpression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_EXPRESSION;
    stmt->as.expression = expr;
    return stmt;
}

Stmt* parseStatement() {
    if (match(TOKEN_PRINT)) {
        return parsePrintStatement();
    } else if (match(TOKEN_VAR)) {
        return parseVarDeclaration();
    } else {
        return parseExpressionStatement();
    }
}

void interpret(Expr* expr) {
    switch (expr->type) {
        case EXPR_LITERAL:
            acceptLiteralExpr(&expr->as.literal, &printVisitor);
            break;
    }
}

void interpretExpr(Expr* expr) {
    switch (expr->type) {
        case EXPR_BINARY:
            // Handle binary expression
            break;
    }
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

void freeExpr(Expr* expr) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_LITERAL:
            if (expr->as.literal.valueType == LITERAL_TYPE_STRING) {
                free((char*)expr->as.literal.value.string); // Free the string
            }
            break;
        case EXPR_UNARY:
            freeExpr(expr->as.unary.operand); // Recursively free operand
            break;
        case EXPR_BINARY:
            freeExpr(expr->as.binary.left);  // Recursively free left
            freeExpr(expr->as.binary.right); // Recursively free right
            break;
    }

    free(expr); // Free the expression itself
}

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
    expr->expression->accept(expr->expression, &printVisitor);
}

ExprVisitor printVisitor = {
    printLiteralExpr,
    printUnaryExpr,
    printBinaryExpr,
    printGroupingExpr
};

void executeStatement(Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            evaluate(((ExpressionStmt*)stmt)->expression);
            break;
        case STMT_PRINT:
            Value val = evaluate(((PrintStmt*)stmt)->expression);
            printValue(val);
            break;
        // Handle other statement types
    }
}

////////////////
// No idea where to organize below this line:

bool match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

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

Token previousToken() {
    return previous;
}
/*
TODO:
-Global Variables
    -Implement support for global variables with variable declaration statements
    -Extend the syntax tree to include a node for variable declarations
    -Add parsing logic for variable declarations
-Variable Expressions
    -Handle variable expressions in the syntax tree
    -Modify the parser to recognize and process variable names as expressions
-Environment Handling
    -Implement an environment to store variable bindings
-Interpreting variable declarations and access
    -Implement logic in the interpreter to handle variable declarations and access
    -Add functinoality to define new variables and retrieve their values from the environment
-Assignment
    -Add syntax and parsing for assignment expressions
    -Update the interpreter to handle variables assignment*/