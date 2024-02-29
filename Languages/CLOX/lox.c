//////////////////////////
// Include Statements
//////////////////////////
#include <stdio.h> // Standard input-output header
#include <stdlib.h> // Standard library header
#include <stdbool.h> // Boolean type header
#include <string.h> // String handling header
#include <setjmp.h> // Non-Local Jumps

//////////////////////////
// Macros and Constants
//////////////////////////
#define MAX_SIZE 100
#define HASHMAP_INITIAL_CAPACITY 8
const int MAX_ITERATIONS = 50;
// Forward declarations
typedef struct Expr Expr;
typedef struct LiteralExpr LiteralExpr;
typedef struct List List;
typedef struct ExprVisitor ExprVisitor;
typedef struct Stmt Stmt;
jmp_buf returnPoint;
// Hash Function
unsigned int hashFunction(const char* key) {
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = 31 * hash + key[i];
    }
    return hash;
}

////////////////////////////
// Basic Types and Enums
////////////////////////////
typedef enum { // Token Types
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

typedef enum { // Node types
    NODE_BINARY_EXPR,
    NODE_GROUPING_EXPR,
    NODE_LITERAL_EXPR,
    NODE_UNARY_EXPR,
    // Add other node types here...
} NodeType;

typedef enum { // Expression types
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    EXPR_LOGICAL,
    EXPR_SUPER,
    EXPR_THIS,
    EXPR_FUNCTIONCALL,
    // ... other types of expressions
} ExprType;

typedef enum { // Literal types
    LITERAL_TYPE_NUMBER,
    LITERAL_TYPE_STRING,
    LITERAL_TYPE_BOOLEAN,
    LITERAL_TYPE_NIL
} LiteralType;

typedef enum { // Statement types
    STMT_EXPRESSION,
    STMT_PRINT,
    STMT_VAR,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
} StmtType;

typedef enum { // Value types
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_INSTANCE,
    VAL_CLASS,
    VAL_FUNCTION,
    VAL_BOUND_METHOD,
    VAL_VAR,
    // Add more types as needed
} ValueType;

// Token and scanner
typedef struct { // Token struct
    TokenType type;
    const char* start;
    int length;
    int line;
    char* lexeme;
} Token;

typedef struct { // Scanner struct
    const char* start;
    const char* current;
    int line;
} Scanner;

//////////////////////////////////
// Abstract Syntax Tree Elements
//////////////////////////////////
typedef struct { // AST Node
    NodeType type;
    int line; // optional, for error reporting
} Node;

//Expression Elements
typedef struct UnaryExpr { // UnaryExpr struct
    Node base;
    Token operator;
    Expr* operand;
} UnaryExpr;

typedef struct { // BinaryExpr struct
    Expr* left;
    Expr* right;
    Token operator;
} BinaryExpr;

typedef struct { // GroupingExpr struct
    Node base;
    Expr* expression;
} GroupingExpr;

typedef union { // LiteralValue struct
    double number;
    const char* string;
    bool boolean;
    char nil;
} LiteralValue;

typedef struct LiteralExpr{ // LiteralExpr struct
    Node base;
    LiteralValue value;
    LiteralType valueType;
} LiteralExpr;

typedef struct { // Variable Expression Node
    Node base;
    Token name;  // Token representing the variable name
} VariableExpr;

typedef struct { // AssignExpr struct
    Node base;
    Token name;
    Expr* value;
} AssignExpr;

typedef struct { // LogicalExpr struct
    Node base;
    Expr* left;
    Expr* right;
    TokenType operator;
} LogicalExpr;

typedef struct { // FunctionCallExpr struct
    Node base;
    Expr* callee;  // The expression that produces the function
    List* arguments;  // A list of expressions representing arguments
} FunctionCallExpr;

// SuperExpr struct
typedef struct SuperExpr { 
    Node base;       // Inherit from a base Node structure
    Token keyword;   // The 'super' keyword token
    Token method;    // The method name token
    char* name;
} SuperExpr;

typedef struct ThisExpr { // ThisExpr struct
    void* referencedObject;  // A pointer to a referenced object or expression
} ThisExpr;

// Expr struct
struct Expr {
    ExprType type;
    union {
        LiteralExpr literal;
        UnaryExpr unary;
        BinaryExpr binary;
        GroupingExpr grouping;
        VariableExpr variable;
        AssignExpr assign;
        LogicalExpr logical;
        SuperExpr super;
        ThisExpr this;
        FunctionCallExpr functionCall;
        // ... other specific expression structures
    } as;
};

//////////////////////
//Statement Elements
//////////////////////
typedef struct Stmt Stmt; 

typedef struct { // BlockStmt struct
    Stmt** statements;
    int count;
} BlockStmt;

typedef struct { // PrintStmt struct
    Stmt* base;  // Inherit from Stmt
    Expr* expression;
} PrintStmt;

typedef struct { 
    Token name;
    Expr* initializer;
} VarStmt;

typedef struct { // IfStmt struct
    Stmt* base;     // Inherit from Stmt
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;
} IfStmt;

typedef struct { // WhileStmt struct
    Stmt* base;  // Inherit from Stmt
    Expr* condition;
    Stmt* body;
} WhileStmt;

typedef struct { // FunctionDeclStmt struct
    Node base;
    Token name;  // Function name
    List* params;  // List of parameters
    BlockStmt* body;  // Function body (a series of statements)
} FunctionDeclStmt;

typedef struct { // ReturnStmt struct
    Node base;
    Expr* value;  // Expression for the return value
} ReturnStmt;

// ExpressionStmt struct
typedef struct { // Expression Statement Node
    Stmt* base;  // Inherit from Stmt
    Expr* expression;
} ExpressionStmt;

struct Stmt { // Stmt struct
    StmtType type;
    union {
        int count;
        Expr* expression; // For expression statements
        PrintStmt print;
        VarStmt var;
        BlockStmt block;
        IfStmt ifStmt;
        WhileStmt whileStmt;
    } as;
};

typedef struct LoxFunction LoxFunction;
typedef struct LoxClass LoxClass;
typedef struct LoxInstance LoxInstance;
typedef struct LoxBoundMethod LoxBoundMethod;

typedef struct { // Value struct
    ValueType type;
    union {
        bool boolean;
        double number;
        char* string;
        LoxInstance* loxInstance;
        LoxClass* loxClass;
        LoxFunction* loxFunction;
        LoxBoundMethod* loxBoundMethod;
        void* variable;
        // Add more fields for different types
    } as;
} Value;

//////////////////////////////////
//Data Structures and Utilitities
//////////////////////////////////
// List
typedef struct ListItem { // ListItem struct
    void** item;
    const char* lexeme;
} ListItem;

// List struct
typedef struct List { // List node for storing parameters
    void** items;    // Dynamic array of pointers
    ListItem* lexemes;
    int capacity;    // Allocated capacity
    int count;       // Number of items
} List;

// Hashmap
typedef struct HashmapEntry { // HashmapEntry struct
    char* key;
    Value value;
    struct HashmapEntry* next;
} HashmapEntry;

typedef struct Hashmap { // Hashmap struct
    HashmapEntry** entries;
    int capacity;
    int count;
    int size;
} Hashmap;

// Scope
typedef struct Scope { // Scope struct
    Hashmap* variables;  // HashMap to store variable state
    struct Scope* enclosing;  // Pointer to the enclosing (outer) scope
} Scope;

typedef struct { //Scopestack struct
    Scope* current;  // Current (top) scope
    // Possibly other fields for tracking state
} ScopeStack;

// Environment
typedef struct EnvEntry { // EnvEntry struct
    char* name;
    Value value;
    struct EnvEntry* next;
} EnvEntry;

typedef struct Environment { // Environment struct
    EnvEntry* head;
    Hashmap* variables;
    struct Environment* enclosing;
} Environment;

////////////////////////////////////////
// Function and Method Representations
////////////////////////////////////////
// Function
typedef struct { // Function struct
    List* params;     // List of parameter names (Tokens)
    BlockStmt* body;  // Function body (block statement)
    Environment* closure;  // Closure environment, if needed
} Function;

typedef enum { // FunctionContext struct
    CONTEXT_NONE,
    CONTEXT_CLASS
} FunctionContext;

typedef struct LoxFunction{ // LoxFunction struct
    Function* declaration;   // AST node representing the function declaration
    Environment* closure;    // Captured environment for closures
    bool isInitializer;      // Flag for class initializers
    char* name;
} LoxFunction;

// Method
typedef struct { // Method struct
    Token name;  // Method name
    Function* function;  // The function representing the method
} Method;

/////////////////////////////////////
// Class and Object Representations
/////////////////////////////////////
// Class
typedef struct ClassDecl { // ClassDecl struct
    Token name;      // Class name
    Expr* superclass;  // Optional superclass
    List* methods;   // List of methods
} ClassDecl;

typedef struct LoxClass { // LoxClass struct
    char* name;       // Name of the class
    Hashmap methods;  // Hashmap to store methods
    Expr* superclass;
} LoxClass;

typedef struct LoxInstance{ // LoxInstance struct
    LoxClass* klass;   // Reference to the class
    Hashmap fields;    // Hashmap for instance fields
    LoxClass* methods;
} LoxInstance;

typedef struct LoxBoundMethod{ // LoxBoundMethod struct
    LoxInstance* instance;   // The instance to which the method is bound
    LoxFunction* method;     // The method being bound
} LoxBoundMethod;

// Resolver
typedef struct { // Resolver struct
    Scope* currentScope;
    int currentScopeDepth;
    int scopes[8];
    FunctionContext currentContext;
    bool insideClass;
    bool insideSubclass;
} Resolver;

///////////////////////////////////
// Visitor Pattern and Evaluation
///////////////////////////////////
// Visitor nodes
typedef struct { // BinaryExprNode struct (node)
    Node base; // Inherited base structure
    Token operator;
    Node* left;
    Node* right;
    void (*accept)(struct Expr*, ExprVisitor*);
} BinaryExprNode;

typedef struct Visitor { // Visitor struct
    Environment* environment;
    void (*visitLiteralExpr)(LiteralExpr* expr);
    void (*visitUnaryExpr)(UnaryExpr* expr);
    void (*visitBinaryExpr)(BinaryExprNode* expr);
    void (*visitGroupingExpr)(GroupingExpr* expr);
} Visitor;

// Nodes for visiting different nodes
typedef void (*VisitLiteralExpr)(LiteralExpr* expr);
typedef void (*VisitUnaryExpr)(UnaryExpr* expr);
typedef void (*VisitBinaryExpr)(BinaryExpr* expr);
typedef void (*VisitGroupingExpr)(GroupingExpr* expr);

struct ExprVisitor{ // ExprVisitor struct
    VisitLiteralExpr visitLiteralExpr;
    VisitUnaryExpr visitUnaryExpr;
    VisitBinaryExpr visitBinaryExpr;
    VisitGroupingExpr visitGroupingExpr;
};

/////////////////////
// Global Variables
/////////////////////
bool hadError = false; // Error flag
Scanner scanner; // Scanner initialization
const Value NIL_VALUE = { .type = VAL_NIL }; // Damage control
Token previous; // The actual previous token
Token previousToken() { // Function to return the previous token
    return previous;
}

////////////////////////
// Function Prototypes
////////////////////////
// Resolver functions
void resolveStatement(Scope* currentScope, Stmt* stmt);
void resolveExpression(Scope* currentScope, Expr* expr);
// Functions for handling functions
Value callFunction(Function* function, List* arguments, Environment* globalEnv);
Value getVariableValue(Environment* env, Token name);
struct Expr* parseEquality();
// Print functions
ExprVisitor printVisitor;
void printfExpr(Expr* expr);
// Evaluate functions
Value evaluateBinaryExpr(BinaryExpr* expr, Environment* env);
Value evaluateLogicalExpr(LogicalExpr* expr, Environment* env);
Value evaluateFunctionCallExpr(FunctionCallExpr* expr, Environment* env);
Value evaluate(Expr* expr, Environment* env);

////////////////////////////
// Function Implementations
////////////////////////////
void printValue(Value value) { // Print the value of a Value
    switch (value.type) {
        case VAL_BOOL:
            printf(value.as.boolean ? "true" : "false");
            break;
        case VAL_NIL:
            printf("nil");
            break;
        case VAL_NUMBER:
            printf("%g", value.as.number);
            break;
        case VAL_STRING:
            printf("%s", value.as.string);
            break;
        case VAL_INSTANCE:
            printf("Instance of %s", value.as.loxInstance->klass->name);
            break;
        case VAL_CLASS:
            printf("Class %s", value.as.loxClass->name);
            break;
        case VAL_FUNCTION:
            printf("Function %s", value.as.loxFunction->name);
            break;
        case VAL_BOUND_METHOD:
            printf("Bound method %s", value.as.loxBoundMethod->method->name);
            break;
        case VAL_VAR:
            printf("Variable");
            break;
        // Handle other types
    }
}

char* tokenToString(const Token* token) { // Convert a token to a string
    char* str = malloc(token->length + 1);
    strncpy(str, token->start, token->length);
    str[token->length] = '\0';
    return str;
}

// Error reporters
void reportError(const char* message) {
    int line = previous.line;
    printf("[Line %d] Error: %s\n", line, message);
}

void errorAtCurrent(const char* message) { // One of the error reporting functions
    if (hadError) return; // Prevent multiple errors for the same issue
    hadError = true;

    fprintf(stderr, "[line %d] Error at '%.*s': %s\n", 
            previous.line, previous.length, previous.start, message);
}

void error(int line, const char* message) {
    fprintf(stderr, "[line %d] Error: %s\n", line, message);
    // Handle the error, possibly by setting a flag to indicate an error state
}

bool isVariableDeclared(Scope* currentScope, const char* name);

bool resolveVariable(Scope* currentScope, const char* name) {
    while (currentScope != NULL) {
        if (isVariableDeclared(currentScope, name)) {
            return true;  // Variable found in the current or an enclosing scope
        }
        currentScope = currentScope->enclosing;
    }
    return false;  // Variable not found
}

void resolveVariableWithErrorHandling(Scope* currentScope, const char* name, int line) {
    if (!resolveVariable(currentScope, name)) {
        reportError("Variable not declared in this scope.");
    }
}

// List functions
void initList(List* list) { // Initializes a list
    list->items = NULL;
    list->capacity = 0;
    list->count = 0;
}

void listAppend(List* list, void* item) { // Appends a list
    if (list->count == list->capacity) {
        int newCapacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->items = realloc(list->items, sizeof(void*) * newCapacity);
        list->capacity = newCapacity;
    }
    list->items[list->count] = item;
    list->count++;
}

// Free functions for managing memory
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
        // ... other cases
    }

    free(expr); // Free the expression itself
}

void freeEnvironment(Environment* env) {
    // Free all variables in the environment
    for (int i = 0; i < env->variables->capacity; i++) {
        HashmapEntry* entry = env->variables->entries[i];
        while (entry != NULL) {
            HashmapEntry* next = entry->next;
            free(entry->key);
            freeValue(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(env->variables->entries);
    free(env->variables);
    free(env);

    // If the environment has an enclosing environment, consider whether it should also be freed
    if (env->enclosing != NULL) {
        freeEnvironment(env->enclosing);
    }
}

void freeValue(Value value) {
    switch (value.type) {
        case VAL_STRING:
            free(value.as.string);
            break;
    }
}

void freeList(List* list) { // Frees a list
    free(list->items);
}

void freeLoxFunction(LoxFunction* function) { // Free a LOX function
    free(function);
}

void freeHashmap(Hashmap* hashmap) { // Free the hashmap
    // Check if the hashmap is NULL
    if (hashmap == NULL) return;

    // Iterate over the hashmap
    for (int i = 0; i < hashmap->capacity; i++) {
        HashmapEntry* entry = hashmap->entries[i];
        while (entry != NULL) {
            HashmapEntry* next = entry->next;
            free(entry->key); // Free the key
            free(entry); // Free the entry itself

            // Free the entry itself
            free(entry);

            entry = next;
        }
    }

    // Free the entries array and the hashmap structure itself
    free(hashmap->entries);
    free(hashmap);
}

void freeLoxInstance(LoxInstance* instance) {
    freeHashmap(&instance->fields);
    free(instance);
}

void freeLoxClass(LoxClass* klass) {
    free(klass->name);
    freeHashmap(&klass->methods);
    free(klass);
}

void freeThisExpr(ThisExpr* thisExpr) {
    if (thisExpr == NULL) return;
    // Free the referenced object
    free(thisExpr->referencedObject);

    // Finally, free the ThisExpr struct itself
    free(thisExpr);
}

// Scanning and parsing functions
void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

char advance() { // Advance the scanner
    scanner.current++;
    return scanner.current[-1];
}

bool isAtEnd() { // Checks for the EOF
    return *scanner.current == '\0';
}

bool isDigit(char c) { // Checks if a char is a digit
    return c >= '0' && c <= '9';
}

bool isNumber(Value value) { // Same as isDigit, but in the scope of Value
    return value.type == VAL_NUMBER;
}

bool isAlpha(char c) { // Checks if a char is a letter
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isTruthy(Value value) { // Checks if a value is a boolean
    switch (value.type) {
        case VAL_BOOL: return value.as.boolean;
        case VAL_NIL: return false;
        default: return true;
    }
}

char peek() { // Returns a reference to the token's type
    return *scanner.current;
}

bool check(TokenType type) { // Check a token's type
    if (isAtEnd()) return false;
    return peek() == type;
}

bool match(TokenType type) { // Checks if token types are the same
    if (check(type)) { 
        advance();
        return true;
    }
    return false;
}

char peekNext() { // Check the next token for EOF
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

// LOX function/instance functions
LoxFunction* newLoxFunction(Function* declaration, Environment* closure, bool isInitializer) { // Creates a new LOX function
    LoxFunction* function = malloc(sizeof(LoxFunction));
    function->declaration = declaration;
    function->closure = closure;
    function->isInitializer = isInitializer;
    return function;
}

void initHashmap(Hashmap* hashmap) { // Initializes the hashmap
    hashmap->entries = malloc(sizeof(HashmapEntry*) * HASHMAP_INITIAL_CAPACITY);
    if (hashmap->entries == NULL) {
        // Handle allocation failure
        reportError("Memory allocation failed");
        return;
    }
    for (int i = 0; i < HASHMAP_INITIAL_CAPACITY; i++) {
        hashmap->entries[i] = NULL;
    }
    hashmap->capacity = HASHMAP_INITIAL_CAPACITY;
    hashmap->size = 0;
}

void hashmapPut(Hashmap* hashmap, const char* key, Value value) { // Inserts elements into a hashmap
    // Calculate hash for the key and find the correct bucket
    unsigned int bucket = hashFunction(key) % hashmap->capacity;
    HashmapEntry* entry = hashmap->entries[bucket];

    // Handle collisions and insert the new key-value pair
    HashmapEntry* newEntry = malloc(sizeof(HashmapEntry));
    newEntry->key = strdup(key);  // Duplicate the key
    newEntry->value = value;      // Assign the value
    newEntry->next = entry;       // Insert at the beginning of the bucket
    hashmap->entries[bucket] = newEntry;
}

LoxFunction* hashmapGet(Hashmap* hashmap, const char* key) { // Look up a function in the hashmap
    unsigned int bucket = hashFunction(key) % hashmap->capacity;
    HashmapEntry* entry = hashmap->entries[bucket];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value.as.loxFunction;
        }
        entry = entry->next;
    }

    return NULL; // Key not found
}

LoxInstance* newLoxInstance(LoxClass* klass) { // Creates a new LOX instance
    LoxInstance* instance = malloc(sizeof(LoxInstance));
    instance->klass = klass;
    initHashmap(&instance->fields); 
    return instance;
}

void loxClassAddMethod(LoxClass* klass, const char* methodName, LoxFunction* method) { // Adds a method to a LOX class
    Value methodValue;
    methodValue.type = VAL_FUNCTION;  // Set the type to your function type
    methodValue.as.loxFunction = method;  // Store the LoxFunction pointer

    hashmapPut(&klass->methods, methodName, methodValue);
}

// Create functions for value types
Value createNilValue() {
    Value value;
    value.type = VAL_NIL;
    
    return value;
}

Value createNumberValue(double number) {
    Value value;
    value.type = VAL_NUMBER;
    value.as.number = number;
    return value;
}

Value createBoolValue(bool boolean) {
    Value value;
    value.type = VAL_BOOL;
    value.as.boolean = boolean;
    return value;
}

double asNumber(Value value) { // Returns a Value as a double
    return value.as.number;
}

// Get functions for everything that's not a hashmap
Value environmentGet(Environment* env, const char* name) {
    // Traverse the linked list of environment entries
    EnvEntry* current = env->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }

    // Handle the case where the variable is not found
    Value notFound;
    notFound.type = VAL_NIL;
    return notFound;
}

LoxClass* getSuperclass(Environment* env, const char* className) {
    Value classValue = environmentGet(env, className);
    if (classValue.type != VAL_CLASS) {
        // Handle error: Not a class
        reportError("Not a class");
        return NULL;
    }
    return classValue.as.loxClass->superclass;
}

LoxInstance* getThisInstance(Environment* env) {
    Value thisValue = environmentGet(env, "this");
    if (thisValue.type != VAL_INSTANCE) {
        // Handle error: 'this' is not an instance
        reportError("'this' is not an instance");
        return NULL;
    }
    return thisValue.as.loxInstance;
}

// Functions for implementing and binding methods
LoxBoundMethod* newLoxBoundMethod(LoxInstance* instance, LoxFunction* method) {
    LoxBoundMethod* boundMethod = malloc(sizeof(LoxBoundMethod));
    if (boundMethod == NULL) {
        // Handle allocation failure
        reportError("Memory allocation failed");
        return NULL;
    }
    boundMethod->instance = instance;
    boundMethod->method = method;
    return boundMethod;
}

Value bindMethod(LoxInstance* instance, LoxFunction* method) {
    LoxBoundMethod* boundMethod = newLoxBoundMethod(instance, method);

    Value boundMethodValue;
    boundMethodValue.type = VAL_BOUND_METHOD;  // Set the type to bound method type
    boundMethodValue.as.loxBoundMethod = boundMethod;  // Store the LoxBoundMethod pointer

    // Store it in the hash map
    hashmapPut(&instance->methods, method->name, boundMethodValue);

    // Maybe return something here to indicate success?
}

LoxFunction* loxClassGetMethod(LoxClass* klass, const char* methodName) {
    LoxFunction* method = hashmapGet(&klass->methods, methodName);
    if (method != NULL) {
        return method;
    }

    if (klass->superclass != NULL) {
        return loxClassGetMethod(klass->superclass, methodName);
    }

    return NULL;
}

// Evaluation functions
Value evaluateBinaryExpr(BinaryExpr* expr, Environment* env) {
    Value left = evaluate(expr->left, env);
    Value right = evaluate(expr->right, env);

    // Ensure both operands are numbers. Add error handling if necessary.
    if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
        // Handle error: operands must be numbers
        return createNilValue();  // Assuming createNilValue() creates a Value representing 'nil'
    }

    switch (expr->operator.type) {
        case TOKEN_PLUS:
            return createNumberValue(left.as.number + right.as.number);

        case TOKEN_MINUS:
            return createNumberValue(left.as.number - right.as.number);

        case TOKEN_STAR:
            return createNumberValue(left.as.number * right.as.number);

        case TOKEN_SLASH:
            // Handle division by zero if necessary
            return createNumberValue(left.as.number / right.as.number);

        case TOKEN_GREATER:
            return createBoolValue(left.as.number > right.as.number);

        case TOKEN_GREATER_EQUAL:
            return createBoolValue(left.as.number >= right.as.number);

        case TOKEN_LESS:
            return createBoolValue(left.as.number < right.as.number);

        case TOKEN_LESS_EQUAL:
            return createBoolValue(left.as.number <= right.as.number);

        case TOKEN_BANG_EQUAL:
            return createBoolValue(left.as.number != right.as.number);

        case TOKEN_EQUAL_EQUAL:
            return createBoolValue(left.as.number == right.as.number);

        // Add cases for other binary operators
    }

    // Return NIL if the operator is not supported
    return createNilValue();
}

Value evaluateLogicalExpr(LogicalExpr* expr, Environment* env) {
    Value left = evaluate(expr->left, env);

    if (expr->operator == TOKEN_OR) { 
        if (isTruthy(left)) return left;
    } else if (expr->operator == TOKEN_AND) { 
        if (!isTruthy(left)) return left;
    }

    return evaluate(expr->right, env);
}

Value evaluateFunctionCallExpr(FunctionCallExpr* expr, Environment* env) {
    Value callee = evaluate(expr->callee, env);
    List args;
    initList(&args);

    for (int i = 0; i < expr->arguments->count; i++) {
        Value* argValue = malloc(sizeof(Value));
        *argValue = evaluate(expr->arguments->items[i], env);
        listAppend(&args, argValue); // Append pointer to Value
    }

    Value result = callFunction(callee.as.loxFunction, &args, env);

    // Clean up argument values
    for (int i = 0; i < args.count; i++) {
        free(args.items[i]);
    }
    freeList(&args);

    return result;
}

// Main evaluate function
Value evaluate(Expr* expr, Environment* env) {
    switch (expr->type) {
        case EXPR_LITERAL:
            printLiteralExpr(expr);
            return;
        case EXPR_UNARY:
            printUnaryExpr(expr);
            return;
        case EXPR_BINARY:
            return evaluateBinaryExpr(&expr->as.binary, env);
        case EXPR_GROUPING:
            printGroupingExpr(expr);
            return;
        case EXPR_VARIABLE: {
            VariableExpr* varExpr = (VariableExpr*)expr;
            return getVariableValue(env, varExpr->name);
        }
        case EXPR_ASSIGN: {
            AssignExpr* assignExpr = (AssignExpr*)expr;
            Value value = evaluate(assignExpr->value, env);
            return value;
        }
        case EXPR_LOGICAL:
            return evaluateLogicalExpr((LogicalExpr*)expr, env);
        case EXPR_SUPER:
            return;
        case EXPR_THIS:
            return;
        case EXPR_FUNCTIONCALL:
            return evaluateFunctionCallExpr(expr, env);
        default:
            // Handle unknown or unsupported expression types
            reportError("Unknown expression type");
            break;
    }
}

// Lox class functions
LoxClass* asClass(Value value) {
    if (value.type != VAL_CLASS) {
        // Handle error: Value is not a class
        reportError("Value is not a class");
        return NULL;
    }
    return value.as.loxClass;
}

LoxClass* newLoxClass(const char* name) {
    LoxClass* klass = malloc(sizeof(LoxClass));
    klass->name = strdup(name);
    initHashmap(&klass->methods);
    return klass;
}

void defineVariable(Environment* env, const char* name, Value value) {
    hashmapPut(&env->variables, name, value);
}

void executeClassDecl(ClassDecl* classDecl, Environment* env) {
    Value superclassValue = NIL_VALUE;
    if (classDecl->superclass != NULL) {
        superclassValue = evaluate(classDecl->superclass, env);
        if (superclassValue.type != VAL_CLASS) {
            // Handle error: Superclass must be a class
            reportError("Superclass must be a class");
        }
    }
    // Convert Token to string
    char* className = tokenToString(&classDecl->name);

    // Create the class object
    LoxClass* klass = newLoxClass(className);

    // Convert LoxClass* to Value
    Value klassValue;
    klassValue.type = VAL_CLASS;
    klassValue.as.loxClass = klass;

    // Add methods to the class...
    defineVariable(env, className, klassValue);

    free(className);
}

// Execution functions
Value executeSuperExpr(SuperExpr* expr, Environment* env) {
    // Convert the keyword and method tokens to strings
    char* keywordStr = tokenToString(&expr->keyword);
    char* methodStr = tokenToString(&expr->method);

    // Get the superclass and the instance ('this')
    LoxClass* superclass = getSuperclass(env, keywordStr);
    LoxInstance* instance = getThisInstance(env);

    LoxFunction* method = loxClassGetMethod(superclass, methodStr);
    if (method == NULL) {
        errorAtCurrent("Expected a method");
    }

    Value result = bindMethod(instance, method);

    // Free the temporary strings
    free(keywordStr);
    free(methodStr);

    return result;
}

void executeVarStmt(Environment* env, VarStmt* stmt) {
    Value value = NIL_VALUE; // Default to nil if no initializer
    if (stmt->initializer != NULL) {
        value = evaluate(stmt->initializer, env);
    }
    defineVariable(env, stmt->name.start, value);
}

void executeStatement(Environment* env, Stmt* stmt);

void executeIfStmt(Environment* env, IfStmt* stmt) {
    Value conditionValue = evaluate(stmt->condition, env);
    if (isTruthy(conditionValue)) {
        executeStatement(env, stmt->thenBranch); 
    } else if (stmt->elseBranch != NULL) {
        executeStatement(env, stmt->elseBranch);
    }
}

void executeWhileStmt(Environment* env, WhileStmt* stmt) {
    while (isTruthy(evaluate(stmt->condition, env))) {
        executeStatement(env, stmt->body);
    }
}

void executeStatement(Environment* env, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            evaluate(((ExpressionStmt*)stmt)->expression, env);
            break;
        case STMT_PRINT:
            {
                Value val = evaluate(((PrintStmt*)stmt)->expression, env);
                printValue(val);
            }
            break;
        case STMT_VAR:
            executeVarStmt(env, (VarStmt*)stmt);
            break;
        case STMT_BLOCK:
            break;
        case STMT_WHILE:
            executeWhileStmt(env, (WhileStmt*)stmt);
            break;
        case STMT_IF:
            executeIfStmt(env, (IfStmt*)stmt);
            break;
        // Handle other statement types
        default:
            reportError("Unknown statement type");
            break;
    }
}

void initEnvironment(Environment* env) { // Initialize the environment
    env->head = NULL;
}

// Makes calls to much earlier functions, so it stays here
Value callFunction(Function* function, List* arguments, Environment* globalEnv) {
    // Create a new environment for the function scope
    Environment localEnv;
    initEnvironment(&localEnv);
    localEnv.enclosing = function->closure ? function->closure : globalEnv;

    // Bind each argument to the corresponding parameter
    for (int i = 0; i < function->params->count; i++) {
        if (i < arguments->count) {
            Token* paramToken = (Token*)function->params->items[i];
            Value argValue = *(Value*)arguments->items[i];
            defineVariable(&localEnv, paramToken->lexeme, argValue);
        }
    }

    // Execute the function body
    Value returnValue = NIL_VALUE;
    for (int i = 0; i < function->body->count; i++) {
        executeStatement(function->body->statements[i], &localEnv);
        // Handle return value if there's a return statement
        return returnValue;
    }

    // Clean up the local environment
    freeEnvironment(&localEnv); 

    return returnValue;
}

LoxFunction* loxClassFindMethod(LoxClass* klass, const char* methodName) { // Searches the hashmap for a method
    return hashmapGet(&klass->methods, methodName);
}


Value instantiateClass(LoxClass* klass, List* arguments) { // Returns a new instance
    LoxInstance* instance = newLoxInstance(klass);

    // Find the initializer, if it exists
    LoxFunction* initializer = loxClassFindMethod(klass, "init"); 
    if (initializer != NULL) {
        callFunction(initializer, arguments, instance);
    }

    // Return the new instance
    Value instanceVal;
    instanceVal.type = VAL_INSTANCE;
    instanceVal.as.loxInstance = instance;
    return instanceVal;
}


////////////////////////////////
// Parsing and AST Construction
////////////////////////////////
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

void acceptVariableExpr(VariableExpr* expr, Visitor* visitor) {
    // Retrieve the value of the variable from the environment
    Value value = getVariableValue(visitor->environment, expr->name);
    printValue(value);
}

void acceptAssignExpr(AssignExpr* expr, Visitor* visitor) {
    // Evaluate the value to be assigned
    Value value = evaluate(expr->value, visitor->environment);
    printValue(value);
}

void acceptLogicalExpr(LogicalExpr* expr, Visitor* visitor) {
    // Evaluate the left operand
    Value left = evaluate(expr->left, visitor->environment);
    // Depending on the operator, evaluate the right operand and apply the logic
    if (expr->operator == TOKEN_AND) {
        if (isTruthy(left)) {
            Value right = evaluate(expr->right, visitor->environment);
            printValue(right);
        } else {
            printValue(left);
        }
    }
    if (expr->operator == TOKEN_OR) {
        if (!isTruthy(left)) {
            Value right = evaluate(expr->right, visitor->environment);
            printValue(right);
        } else {
            printValue(left);
            }
    }
}

void acceptSuperExpr(SuperExpr* expr, Visitor* visitor) {
    // Retrieve the superclass
    LoxClass* superclass = getSuperclass(visitor->environment, expr->name);
    // Retrieve the method from the superclass
    //LoxBoundMethod* method = expr->method;
    // Do something with the method, like executing it or printing information
    //printMethod(method);
}

Expr* newBinaryExpr(Expr* left, Expr* right, Token operator) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->as.binary.left = left;
    expr->as.binary.right = right;
    expr->as.binary.operator = operator;
    return expr;
}

SuperExpr* newSuperExpr(Token keyword, Token method) {
    SuperExpr* expr = malloc(sizeof(SuperExpr));
    if (!expr) {
        // Handle memory allocation failure
        reportError("Memory allocation failed");
    }
    expr->base.type = EXPR_SUPER;
    expr->keyword = keyword;
    expr->method = method;
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

LiteralExpr* newLiteralExpr(LiteralValue value, LiteralType valueType) {
    LiteralExpr* expr = (LiteralExpr*)malloc(sizeof(LiteralExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
        reportError("Memory allocation failed");
    }
    expr->base.type = NODE_LITERAL_EXPR;
    expr->value = value;
    expr->valueType = valueType;
    return expr;
}

GroupingExpr* newGroupingExpr(Expr* expression) {
    GroupingExpr* expr = (GroupingExpr*)malloc(sizeof(GroupingExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
        reportError("Memory allocation failed");
    }
    expr->base.type = NODE_GROUPING_EXPR;
    expr->expression = expression;
    return expr;
}

// Begins the parse function flow
Expr* parseExpression() {
    // Start with the lowest precedence function
    return parseEquality();
}

Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

Token advanceToken() {
    if (!isAtEnd()) {
        TokenType type;
        previous = makeToken(type); // FIXME
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
        consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after expression."); // IMLPLICIT
        return newGroupingExpr(expr); 
    }
    // Handle unexpected tokens
    errorAtCurrent("Expect expression.");
    return NULL; // Indicate an eror occurred
}

UnaryExpr* newUnaryExpr(Token operator, Expr* operand) {
    UnaryExpr* expr = (UnaryExpr*)malloc(sizeof(UnaryExpr));
    if (expr == NULL) {
        // Handle memory allocation failure
        reportError("Memory allocation failed");
    }
    expr->base.type = NODE_UNARY_EXPR;
    expr->operator = operator;
    expr->operand = operand;
    return expr;
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

Expr* parseLogical() {
    Expr* expr = parseEquality();

    while (match(TOKEN_AND) || match(TOKEN_OR)) { 
        Token operator = previousToken(); // The logical operator token
        Expr* right = parseEquality();
        LogicalExpr* logical = (LogicalExpr*)malloc(sizeof(LogicalExpr));
        if (!logical) {
            // Handle memory allocation failure
            reportError("Memory allocation failed");
            return NULL;
        }
        logical->base.type = EXPR_LOGICAL;
        logical->operator = operator.type;
        logical->left = expr;
        logical->right = right;
        expr = (Expr*)logical;
    }

    return expr;
}

AssignExpr* newAssignExpr(Token name, Expr* value) {
    AssignExpr* expr = malloc(sizeof(AssignExpr));
    if (expr == NULL) {
        // Handle allocation failure
        reportError("Memory allocation failed");
        return NULL;
    }
    expr->name = name;  // Assuming Token is a value type; if it's a pointer, duplicate it
    expr->value = value;
    return expr;
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
        reportError("Invalid assignment target");
        return NULL;
    }

    return expr;
}

Expr* parseSuperExpression() {
    Token keyword = previousToken();  // The 'super' token
    consumeToken(TOKEN_DOT, "Expect '.' after 'super'."); 
    Token method = consumeToken(TOKEN_IDENTIFIER, "Expect superclass method name.");
    return newSuperExpr(keyword, method);
}

Stmt* parseVarDeclaration() {
    Token name = consumeToken(TOKEN_IDENTIFIER, "Expect variable name."); 

    Expr* initializer = NULL;
    if (match(TOKEN_EQUAL)) { 
        initializer = parseExpression();
    }

    consumeToken(TOKEN_SEMICOLON, "Expect ';' after variable declaration."); 
    VarStmt varStmt = {name, initializer};
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_VAR;
    stmt->as.var = varStmt;
    return stmt;
}

Stmt* parseExpressionStatement() {
    Expr* expr = parseExpression();
    consumeToken(TOKEN_SEMICOLON, "Expect ';' after expression."); 
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_EXPRESSION;
    stmt->as.expression = expr;
    return stmt;
}

Stmt* parsePrintStatement() {
    Expr* value = parseExpression();
    consumeToken(TOKEN_SEMICOLON, "Expect ';' after value."); 
    PrintStmt printStmt = {value};
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_PRINT;
    stmt->as.print = printStmt;
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

Stmt* parseIfStatement() {
    consumeToken(TOKEN_LEFT_PAREN, "Expect '(' after 'if'."); 
    Expr* condition = parseExpression();
    consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after if condition."); 

    Stmt* thenBranch = parseStatement();
    Stmt* elseBranch = NULL;
    if (match(TOKEN_ELSE)) { 
        elseBranch = parseStatement();
    }

    IfStmt* stmt = (IfStmt*)malloc(sizeof(IfStmt));
    if (!stmt) {
        // Handle memory allocation failure
    }
    stmt->base->type = STMT_IF; // FIXME
    stmt->condition = condition;
    stmt->thenBranch = thenBranch;
    stmt->elseBranch = elseBranch;

    return (Stmt*)stmt;
}

Stmt* parseWhileStatement() {
    consumeToken(TOKEN_LEFT_PAREN, "Expect '(' after 'while'."); 
    Expr* condition = parseExpression();
    consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after condition."); 
    Stmt* body = parseStatement();

    WhileStmt* stmt = (WhileStmt*)malloc(sizeof(WhileStmt));
    if (!stmt) {
        // Handle memory allocation failure
    }
    stmt->base->type = STMT_WHILE; // FIXME
    stmt->condition = condition;
    stmt->body = body;

    return (Stmt*)stmt;
}

Expr* parseVariable() {
    Token name = previousToken();
    VariableExpr* expr = (VariableExpr*)malloc(sizeof(VariableExpr));
    expr->base.type = EXPR_VARIABLE;
    expr->name = name;
    return (Expr*)expr;
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
/////////////////////
// Runtime Control
/////////////////////
void run(const char* source) {
    // Parse the source code to produce an AST
    Expr* expression = parse(source);

    // Check for parsing errors
    if (expression == NULL) {
        // Handle parsing error
        return;
    }

    // Interpret the AST
    interpret(expression);    
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
    run(buffer);

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
        run(line);
    }
}

// Printers
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
    printExpr(expr->operand); // Assuming printExpr is a function that prints any Expr type
}

void printBinaryExpr(BinaryExpr* expr) {
    printf("Binary Expression: Operator=%c\n", expr->operator);
    printExpr(expr->left);
    printExpr(expr->right);
}

void printGroupingExpr(GroupingExpr* expr){
    printf("Grouping Expression:\n");
    printExpr(expr->expression);
}

ExprVisitor printVisitor = {
    printLiteralExpr,
    printUnaryExpr,
    printBinaryExpr,
    printGroupingExpr
};

void printExpr(Expr* expr) {
    switch (expr->type) {
        case EXPR_UNARY:
            printUnaryExpr((UnaryExpr*)expr);
            break;
        case EXPR_BINARY:
            printBinaryExpr((BinaryExpr*)expr);
            break;
        case EXPR_GROUPING:
            printGroupingExpr((GroupingExpr*)expr);
            break;
        default:
            printf("Unknown Expression Type\n");
    }
}

void interpret(Expr* expr) {
    switch (expr->type) {
        case EXPR_LITERAL:
            acceptLiteralExpr(&expr->as.literal, &printVisitor);
            break;
        case EXPR_UNARY:
            acceptUnaryExpr(&expr->as.unary, &printVisitor);
            break;
        case EXPR_BINARY:
            acceptBinaryExpr(&expr->as.binary, &printVisitor);
            break;
        case EXPR_GROUPING:
            acceptGroupingExpr(&expr->as.grouping, &printVisitor);
            break;
        case EXPR_VARIABLE:
            acceptVariableExpr(&expr->as.variable, &printVisitor);
            break;
        case EXPR_ASSIGN:
            acceptAssignExpr(&expr->as.assign, &printVisitor);
            break;
        case EXPR_LOGICAL:
            acceptLogicalExpr(&expr->as.logical, &printVisitor);
            break;
        case EXPR_SUPER:
            acceptSuperExpr(&expr->as.super, &printVisitor);
            break;
        default:
            fprintf(stderr, "Unknown expression type.\n");
    }
}

// Variable evaluation
Value* getVariable(Environment* env, const char* name) {
    for (EnvEntry* entry = env->head; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) {
            return &entry->value;
        }
    }
    reportError("Variable not found");
    return NULL; // Variable not found
}

Value getVariableValue(Environment* env, Token name) {
    Value* value = getVariable(env, name.start);
    if (value == NULL) {
        // Handle undefined variable error
        reportError("Undefined variable");
    }
    return *value;
}

////////////////////////////////////
// Environment and Scope Handling
////////////////////////////////////
EnvEntry* newEnvEntry(const char* name, Value value) {
    EnvEntry* entry = (EnvEntry*)malloc(sizeof(EnvEntry));
    entry->name = strdup(name); // Duplicate the string
    entry->value = value;
    entry->next = NULL;
    return entry;
}

void pushScope(Scope** currentScope) {
    Scope* newScope = malloc(sizeof(Scope));
    initHashmap(&newScope->variables);
    newScope->enclosing = *currentScope;
    *currentScope = newScope;
}

void popScope(Scope** currentScope) {
    Scope* oldScope = *currentScope;
    *currentScope = oldScope->enclosing;
    freeHashmap(&oldScope->variables);
    free(oldScope);
}

void declareVariable(Scope* currentScope, const char* name) {
    // Add the variable to the current scope with a state indicating it's declared but not defined
    Value temp;
    hashmapPut(&currentScope->variables, name, temp);
}

bool isVariableDeclared(Scope* currentScope, const char* name) {
    bool isDeclared;
    if (hashmapGet(&currentScope->variables, name)) {
        return isDeclared;
    }
    return false;
}

// Resolver logic
void resolveThisExpr(Resolver* resolver, ThisExpr* expr) {
    if (resolver->currentContext != CONTEXT_CLASS) {
        reportError("Can't use 'this' outside of a class method.");
    }
    freeThisExpr(expr);
    // Resolve 'this' expression
}

void resolveStatement(Scope* currentScope, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_VAR: {
            VarStmt* varStmt = (VarStmt*)stmt;
            declareVariable(currentScope, varStmt->name.lexeme);
            if (varStmt->initializer != NULL) {
                resolveExpression(currentScope, varStmt->initializer);
            }
            Value temp;
            defineVariable(currentScope, varStmt->name.lexeme, temp);
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* blockStmt = (BlockStmt*)stmt;
            pushScope(&currentScope);
            for (int i = 0; i < blockStmt->count; i++) {
                resolveStatement(currentScope, blockStmt->statements[i]);
            }
            popScope(&currentScope);
            break;
        }
        case STMT_EXPRESSION: {
            ExpressionStmt* expressionStmt = (ExpressionStmt*)stmt;
            resolveExpression(currentScope, expressionStmt->expression);
            break;
        }
        case STMT_PRINT: {
            PrintStmt* printStmt = (PrintStmt*)stmt;
            resolveExpression(currentScope, printStmt->expression);
            break;
        }
        case STMT_IF: {
            IfStmt* ifStmt = (IfStmt*)stmt;
            resolveExpression(currentScope, ifStmt->condition);
            resolveStatement(currentScope, ifStmt->thenBranch);
            if (ifStmt->elseBranch != NULL) {
                resolveStatement(currentScope, ifStmt->elseBranch);
            }
            break;
        }
        case STMT_WHILE: {
            WhileStmt* whileStmt = (WhileStmt*)stmt;
            resolveExpression(currentScope, whileStmt->condition);
            resolveStatement(currentScope, whileStmt->body);
            break;
        }
        // Add other cases
    }
}

void resolveExpression(Scope* currentScope, Expr* expr) {
    switch (expr->type) {
        case EXPR_VARIABLE: {
            VariableExpr* varExpr = (VariableExpr*)expr;
            if (!isVariableDeclared(currentScope, varExpr->name.lexeme)) {
                reportError("Undefined variable.");
            }
            resolveVariable(currentScope, varExpr->name.lexeme);
            break;
        }
        case EXPR_ASSIGN: {
            AssignExpr* assignExpr = (AssignExpr*)expr;
            resolveExpression(currentScope, assignExpr->value);
            resolveVariable(currentScope, assignExpr->name.lexeme);
            break;
        }
        case EXPR_BINARY: {
            BinaryExpr* binaryExpr = (BinaryExpr*)expr;
            resolveExpression(currentScope, binaryExpr->left);
            resolveExpression(currentScope, binaryExpr->right);
            break;
        }
        case EXPR_UNARY: {
            UnaryExpr* unaryExpr = (UnaryExpr*)expr;
            resolveExpression(currentScope, unaryExpr->operand);
            break;
        }

        case EXPR_GROUPING: {
            GroupingExpr* groupingExpr = (GroupingExpr*)expr;
            resolveExpression(currentScope, groupingExpr->expression);
            break;
        }

        case EXPR_LOGICAL: {
            LogicalExpr* logicalExpr = (LogicalExpr*)expr;
            resolveExpression(currentScope, logicalExpr->left);
            resolveExpression(currentScope, logicalExpr->right);
            break;
        }

        case EXPR_SUPER: {
            SuperExpr* superExpr = (SuperExpr*)expr;
            break;
        }

        case EXPR_THIS: {
            ThisExpr* thisExpr = (ThisExpr*)expr;
            break;
        }

        case EXPR_FUNCTIONCALL: {
            FunctionCallExpr* functionCallExpr = (FunctionCallExpr*)expr;
            resolveExpression(currentScope, functionCallExpr->callee);
            for (int i = 0; i < functionCallExpr->arguments->count; i++) {
                resolveExpression(currentScope, functionCallExpr->arguments->items[i]);
            }
            break;
        }
        // Add other cases
    }
}

void analyzeSemantics(Stmt** statements, Scope* globalScope) {
    Scope* currentScope = globalScope;
    for (int i = 0; statements[i] != NULL; i++) {
        resolveStatement(currentScope, statements[i]);
    }
}

void resolveFunctionDecl(Scope* currentScope, FunctionDeclStmt* stmt) {
    Value temp;
    declareVariable(currentScope, stmt->name.lexeme);
    defineVariable(currentScope, stmt->name.lexeme, temp);

    pushScope(&currentScope);  // New scope for function body
    for (int i = 0; i < stmt->params->count; i++) {
        declareVariable(currentScope, stmt->params->lexemes[i].lexeme);
        defineVariable(currentScope, stmt->params->lexemes[i].lexeme, temp);
    }
    resolveStatement(currentScope, stmt->body);
    popScope(&currentScope);
}

bool hashmapContains(Hashmap* hashmap, const char* key) {
    unsigned int bucket = hashFunction(key) % hashmap->capacity;
    for (HashmapEntry* entry = hashmap->entries[bucket]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->key, key) == 0) {
            return true;
        }
    }
    return false;
}

void resolveLocal(Resolver* resolver, Token keyword) {
    // Iterate from the innermost scope to the outer
    for (int i = resolver->currentScopeDepth; i >= 0; i--) {
        if (hashmapContains(&resolver->scopes[i], keyword.lexeme)) {
            // Notify the interpreter about the scope level where the variable was found
            return;
        }
    }
    // If not found, it's either a global variable or an error
}


void resolveSuperExpr(Resolver* resolver, SuperExpr* expr) {
    if (!resolver->insideSubclass) {
        reportError("Can't use 'super' outside of a subclass.");
    }
    // Resolve 'super' as a local variable to ensure proper binding
    resolveLocal(resolver, expr->keyword); 
}

/////////////////////////////
// Function Call Processing
/////////////////////////////
Value makeFunctionValue(LoxFunction* function) {
    Value functionValue;
    functionValue.type = VAL_FUNCTION;
    functionValue.as.loxFunction = function;
    return functionValue;
}

void executeFunctionDeclStmt(FunctionDeclStmt* stmt, Environment* env) {
    bool isInit = false;
    Function* function = newLoxFunction(stmt, env, isInit);
    char* name = tokenToString(&stmt->name);
    defineVariable(env, name, makeFunctionValue(function));
}

Value executeReturnStmt(ReturnStmt* stmt, Environment* env) {
    Value returnValue = NIL_VALUE; // Default to nil
    if (stmt->value != NULL) {
        returnValue = evaluate(stmt->value, env);
    }
    // Use longjmp to jump back to the function call point
    double returnNum = returnValue.as.number;
    longjmp(returnPoint, returnNum); 
}

//////////////////////////
// Helper Functions
//////////////////////////
void resolveClassDecl(Resolver* resolver, ClassDecl* classDecl) {
    bool enclosingClass = resolver->insideClass;
    resolver->insideClass = true;
    resolver->insideSubclass = classDecl->superclass != NULL;

    if (classDecl->superclass != NULL) {
        resolveExpression(resolver, classDecl->superclass);
    }

    resolver->insideClass = enclosingClass;
    resolver->insideSubclass = false;
}

VariableExpr* newVariableExpr(Token token) {
    VariableExpr* expr = malloc(sizeof(VariableExpr));
    if (expr == NULL) {
        // Handle allocation failure
        reportError("Memory allocation failed");
        return NULL;
    }
    expr->name = token;
    return expr;
}

Function* parseFunction(const char* kind) {
    Function* function = malloc(sizeof(Function));
    if (function == NULL) {
        // Handle allocation failure
        return NULL;
    }

    // Assuming you have a tokenizer and a parser
    Token name = consumeToken(TOKEN_IDENTIFIER, "Expect function name.");
    consumeToken(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
    // Parse parameters...
    consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consumeToken(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
    // Parse function body...

    return function;
}

List* initNewList() {
    List* list = malloc(sizeof(List));
    if (list == NULL) {
        // Handle allocation failure
        reportError("Memory allocation failed");
        return NULL;
    }

    list->capacity = 10; // Starting capacity
    list->count = 0;
    list->items = malloc(sizeof(void*) * list->capacity);
    if (list->items == NULL) {
        // Handle allocation failure
        reportError("Memory allocation failed");
        free(list);
        return NULL;
    }

    return list;
}

ClassDecl* parseClassDeclaration() {
    consumeToken(TOKEN_CLASS, "Expect 'class' keyword."); 
    Token name = consumeToken(TOKEN_IDENTIFIER, "Expect class name."); 

    Expr* superclass = NULL;
    if (match(TOKEN_LESS)) { 
        consumeToken(TOKEN_IDENTIFIER, "Expect superclass name."); 
        superclass = newVariableExpr(previousToken());
    }

    consumeToken(TOKEN_LEFT_BRACE, "Expect '{' before class body."); 
    List* methods = initNewList();
    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) { 
        listAppend(methods, parseFunction("method"));
    }
    consumeToken(TOKEN_RIGHT_BRACE, "Expect '}' after class body."); 

    ClassDecl* classDecl = (ClassDecl*)malloc(sizeof(ClassDecl));
    classDecl->name = name;
    classDecl->superclass = superclass;
    classDecl->methods = methods;
    return classDecl;
}

/////////////////////////////////
// Error Reporting and Handling
/////////////////////////////////


//////////////////////
// Token Processing
//////////////////////
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

Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR; 
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
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

    consumeToken(identifierType(),"");
    return previous;
}

Token number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    consumeToken(TOKEN_NUMBER, "Expected a number"); 
    return previous;
}

Token scanToken() {
    skipWhitespace();

    scanner.start = scanner.current;
    if (isAtEnd()) return makeToken(TOKEN_EOF); 

    char c = advance();

    // Add logic to handle different characters
    // For example:
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN); 
        // Add cases for other single characters
        // ...
    }

    Token token;
    previous = token;
    return token;
}

//////////////////////////
// Main Function
//////////////////////////
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