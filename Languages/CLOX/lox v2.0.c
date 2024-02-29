//////////////////////////
// Header Comments
//////////////////////////
// Incomplete Functions:


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

//////////////////////////////////////////
// Type Definitions and Global Variables
//////////////////////////////////////////

////////////////////////////
// Basic Types and Enums
////////////////////////////
typedef enum { // Token Types
    // Single-character tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, // Token enumeration for lexer/parser
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE, // Token enumeration for lexer/parser
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS, // Token enumeration for lexer/parser
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR, // Token enumeration for lexer/parser

    // One or two character tokens
    TOKEN_BANG, TOKEN_BANG_EQUAL, // Token enumeration for lexer/parser
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, // Token enumeration for lexer/parser
    TOKEN_GREATER, TOKEN_GREATER_EQUAL, // Token enumeration for lexer/parser
    TOKEN_LESS, TOKEN_LESS_EQUAL, // Token enumeration for lexer/parser

    // Literals
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, // Token enumeration for lexer/parser

    // Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, // Token enumeration for lexer/parser
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR, // Token enumeration for lexer/parser
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, // Token enumeration for lexer/parser
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_ERROR, // Token enumeration for lexer/parser

    TOKEN_EOF // Token enumeration for lexer/parser
} TokenType;

typedef enum { // Node types
    NODE_BINARY_EXPR,
    NODE_GROUPING_EXPR,
    NODE_LITERAL_EXPR,
    NODE_UNARY_EXPR,
    // Add other node types here...
} NodeType;

typedef enum { // Defining a new type
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    EXPR_LOGICAL,
    EXPR_SUPER,
    // ... other types of expressions
} ExprType;

typedef enum { // Defining a new type
    LITERAL_TYPE_NUMBER,
    LITERAL_TYPE_STRING,
    LITERAL_TYPE_BOOLEAN,
    LITERAL_TYPE_NIL
} LiteralType;

typedef enum { // Defining a new type
    STMT_EXPRESSION,
    STMT_PRINT,
    STMT_VAR,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
} StmtType;

typedef enum { // Defining a new type
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_INSTANCE,
    VAL_CLASS,
    VAL_FUNCTION,
    // Add more types as needed
} ValueType;

///////////////////////////////
//Tokens and Lexical Elements
///////////////////////////////
typedef struct { // Token
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

typedef struct { // Defining a new type
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

    ////////////////////////
    //Expression Elements
    ////////////////////////
typedef struct { // Defining a new type
    Node base;
    Token operator;
    Expr* operand;
} UnaryExpr;

typedef struct { // Defining a new type
    Expr* left;
    Expr* right;
    Token operator;
} BinaryExpr;

typedef struct { // Defining a new type
    Node base;
    Expr* expression;
} GroupingExpr;

typedef union { // Defining a new type
    double number;
    const char* string;
    bool boolean;
    char nil;
} LiteralValue;

typedef struct LiteralExpr{ // Defining a new type
    Node base;
    LiteralValue value;
    LiteralType valueType;
} LiteralExpr;

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

typedef struct { // Variable Expression Node
    Node base;
    Token name;  // Token representing the variable name
} VariableExpr;

typedef struct { // Defining a new type
    Node base;
    Token name;
    Expr* value;
} AssignExpr;

typedef struct { // Defining a new type
    Node base;
    Expr* left;
    Expr* right;
    TokenType operator;
} LogicalExpr;

typedef struct { // Defining a new type
    Node base;
    Expr* callee;  // The expression that produces the function
    List* arguments;  // A list of expressions representing arguments
} FunctionCallExpr;

typedef struct { // Defining a new type
    Node base;       // Inherit from a base Node structure
    Token keyword;   // The 'super' keyword token
    Token method;    // The method name token
} SuperExpr;

    //////////////////////
    //Statement Elements
    //////////////////////
typedef struct Stmt Stmt; // Defining a new type

typedef struct { // Defining a new type
    Stmt** statements;
    int count;
} BlockStmt;

typedef struct { // Defining a new type
    Stmt* base;  // Inherit from Stmt
    Expr* expression;
} PrintStmt;

typedef struct { // Defining a new type
    Token name;
    Expr* initializer;
} VarStmt;

typedef struct { // Defining a new type
    Stmt* base;     // Inherit from Stmt
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;
} IfStmt;

typedef struct { // Defining a new type
    Stmt* base;  // Inherit from Stmt
    Expr* condition;
    Stmt* body;
} WhileStmt;

typedef struct { // Defining a new type
    Node base;
    Token name;  // Function name
    List* params;  // List of parameters
    BlockStmt* body;  // Function body (a series of statements)
} FunctionDeclStmt;

typedef struct { // Defining a new type
    Node base;
    Expr* value;  // Expression for the return value
} ReturnStmt;

typedef struct { // Expression Statement Node
    Stmt* base;  // Inherit from Stmt
    Expr* expression;
} ExpressionStmt;

struct Stmt {
    StmtType type;
    union {
        Expr* expression; // For expression statements
        PrintStmt print;
        VarStmt var;
        BlockStmt block;
    } as;
};

// Value Representations
typedef struct { // Defining a new type
    ValueType type;
    union {
        bool boolean;
        double number;
        char* string;
        // Add more fields for different types
    } as;
} Value;

//////////////////////////////////
//Data Structures and Utilitities
//////////////////////////////////
//List, Hashmap, EnvEntry, Environment, Scope, ScopeStack
typedef struct List { // List node for storing parameters
    void** items;    // Dynamic array of pointers
    int capacity;    // Allocated capacity
    int count;       // Number of items
} List;

typedef struct { // Defining a new type
    char* key;
    bool value;
} HashmapEntry;

typedef struct { // Defining a new type
    HashmapEntry* entries;
    int capacity;
    int count;
} Hashmap;

typedef struct Scope { // Defining a new type
    Hashmap* variables;  // HashMap to store variable state
    struct Scope* enclosing;  // Pointer to the enclosing (outer) scope
} Scope;

typedef struct { // Defining a new type
    Scope* current;  // Current (top) scope
    // Possibly other fields for tracking state
} ScopeStack;

typedef struct EnvEntry { // Defining a new type
    char* name;
    Value value;
    struct EnvEntry* next;
} EnvEntry;

typedef struct { // Defining a new type
    EnvEntry* head;
} Environment;

////////////////////////////////////////
// Function and Method Representations
////////////////////////////////////////
typedef struct { // Defining a new type
    List* params;     // List of parameter names (Tokens)
    BlockStmt* body;  // Function body (block statement)
    Environment* closure;  // Closure environment, if needed
} Function;

typedef enum { // Defining a new type
    CONTEXT_NONE,
    CONTEXT_CLASS
} FunctionContext;

typedef struct { // Defining a new type
    Function* declaration;   // AST node representing the function declaration
    Environment* closure;    // Captured environment for closures
    bool isInitializer;      // Flag for class initializers
} LoxFunction;

// Represents a method in a class
typedef struct { // Defining a new type
    Token name;  // Method name
    Function* function;  // The function representing the method
} Method;

/////////////////////////////////////
// Class and Object Representations
/////////////////////////////////////
// Represents a class declaration
typedef struct { // Defining a new type
    Token name;      // Class name
    Expr* superclass;  // Optional superclass
    List* methods;   // List of methods
} ClassDecl;

typedef struct LoxClass { // Defining a new type
    char* name;       // Name of the class
    Hashmap methods;  // Hashmap to store methods
} LoxClass;

typedef struct { // Defining a new type
    LoxClass* klass;   // Reference to the class
    Hashmap fields;    // Hashmap for instance fields
} LoxInstance;

typedef struct {
    LoxInstance* instance;   // The instance to which the method is bound
    LoxFunction* method;     // The method being bound
} LoxBoundMethod;

typedef struct { // Defining a new type
    Scope* currentScope;
    FunctionContext currentContext;
    bool insideClass;
    bool insideSubclass;
} Resolver;

///////////////////////////////////
// Visitor Pattern and Evaluation
///////////////////////////////////

typedef struct { // Binary Expression Node
    Node base; // Inherited base structure
    Token operator;
    Node* left;
    Node* right;
    void (*accept)(struct Expr*, ExprVisitor*);
} BinaryExprNode;

typedef struct Visitor { // Visitor Node
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

struct ExprVisitor{ // Expression Visitor Node
    VisitLiteralExpr visitLiteralExpr;
    VisitUnaryExpr visitUnaryExpr;
    VisitBinaryExpr visitBinaryExpr;
    VisitGroupingExpr visitGroupingExpr;
};

/////////////////////
// Global Variables
/////////////////////
bool hadError = false;
Token previous;
Scanner scanner;
const Value NIL_VALUE = { .type = VAL_NIL };
Token previousToken() {
    return previous;
}

////////////////////////
// Function Prototypes
////////////////////////
void resolveStatement(Scope* currentScope, Stmt* stmt);
void resolveExpression(Scope* currentScope, Expr* expr);

////////////////////////////
// Function Implementations
////////////////////////////
void initList(List* list) {
    list->items = NULL;
    list->capacity = 0;
    list->count = 0;
}

void freeList(List* list) {
    free(list->items);
}

void listAppend(List* list, void* item) {
    if (list->count == list->capacity) {
        int newCapacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->items = realloc(list->items, sizeof(void*) * newCapacity);
        list->capacity = newCapacity;
    }
    list->items[list->count] = item;
    list->count++;
}

void freeLoxFunction(LoxFunction* function) {
    // Free the function resources. Be careful with the closure as it might be shared.
    free(function);
}

void freeHashmap(Hashmap* hashmap) {
    // Check if the hashmap is NULL
    if (hashmap == NULL) return;

    // Iterate over the hashmap
    for (int i = 0; i < hashmap->capacity; i++) {
        HashmapEntry* entry = hashmap->entries[i];
        while (entry != NULL) {
            HashmapEntry* next = entry->next;

            // Free the key and value here.
            // This depends on how they were allocated.
            // For example, if they are dynamically allocated strings:
            free(entry->key);
            free(entry->value);  // Adjust this if value needs different handling

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

void reportError(int line, const char* message) {
    // Implementation depends on how you handle errors in your interpreter
    printf("[Line %d] Error: %s\n", line, message);
}

void resolveThisExpr(Resolver* resolver, ThisExpr* expr) {
    if (resolver->currentContext != CONTEXT_CLASS) {
        reportError("Can't use 'this' outside of a class method.");
    }
    // Resolve 'this' expression
}

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

bool isTruthy(Value value) {
    switch (value.type) {
        case VAL_BOOL: return value.as.boolean;
        case VAL_NIL: return false;
        default: return true;
    }
}

char peek() {
    return *scanner.current;
}

bool check(TokenType type) {
    if (isAtEnd()) return false;
    return peek() == type;
}

bool match(TokenType type) {
    if (check(type)) { // IMPLICIT
        advance();
        return true;
    }
    return false;
}

LoxFunction* newLoxFunction(Function* declaration, Environment* closure, bool isInitializer) {
    LoxFunction* function = malloc(sizeof(LoxFunction));
    function->declaration = declaration;
    function->closure = closure;
    function->isInitializer = isInitializer;
    return function;
}

void initHashmap(Hashmap* hashmap) {
    hashmap->entries = malloc(sizeof(HashmapEntry*) * HASHMAP_INITIAL_CAPACITY);
    if (hashmap->entries == NULL) {
        // Handle allocation failure
        return;
    }
    for (int i = 0; i < HASHMAP_INITIAL_CAPACITY; i++) {
        hashmap->entries[i] = NULL;
    }
    hashmap->capacity = HASHMAP_INITIAL_CAPACITY;
    hashmap->size = 0;
}

void hashmapPut(Hashmap* hashmap, const char* key, Value value) {
    // Calculate hash for the key and find the correct bucket
    unsigned int bucket = hashFunction(key) % hashmap->capacity;
    HashmapEntry* entry = hashmap->entries[bucket];

    // Handle collisions and insert the new key-value pair
    // This is a simplistic approach; modify according to your collision handling strategy
    HashmapEntry* newEntry = malloc(sizeof(HashmapEntry));
    newEntry->key = strdup(key);  // Duplicate the key
    newEntry->value = value;      // Assign the value
    newEntry->next = entry;       // Insert at the beginning of the bucket
    hashmap->entries[bucket] = newEntry;
}

LoxInstance* newLoxInstance(LoxClass* klass) {
    LoxInstance* instance = malloc(sizeof(LoxInstance));
    instance->klass = klass;
    initHashmap(&instance->fields); // IMPLICIT
    return instance;
}

void loxClassAddMethod(LoxClass* klass, const char* methodName, LoxFunction* method) {
    hashmapPut(&klass->methods, methodName, method); // IMPLICIT
}

void freeLoxClass(LoxClass* klass) {
    free(klass->name);
    freeHashmap(&klass->methods);
    free(klass);
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

Value createNumberValue(double number) {
    Value value;
    value.type = VAL_NUMBER;
    value.as.number = number;
    return value;
}

double asNumber(Value value) {
    return value.as.number;
}

LoxFunction* hashmapGet(Hashmap* hashmap, const char* key) {
    unsigned int bucket = hashFunction(key) % hashmap->capacity;
    HashmapEntry* entry = hashmap->entries[bucket];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value.as.function;  // Assuming the value is a LoxFunction
        }
        entry = entry->next;
    }

    return NULL; // Key not found
}

Value environmentGet(Environment* env, const char* name) {
    // Search for the variable in the current environment scope
    HashmapEntry* entry = hashmapGet(env->variables, name);
    if (entry != NULL) {
        return entry->value;
    }

    // If not found in the current scope, check the enclosing environment
    if (env->enclosing != NULL) {
        return environmentGet(env->enclosing, name);
    }

    // Handle the case where the variable is not found
    // You might want to return a special 'undefined' value or handle an error
    Value notFound;
    notFound.type = VAL_NIL;
    return notFound;
}

LoxClass* getSuperclass(Environment* env, const char* className) {
    Value classValue = environmentGet(env, className);
    if (classValue.type != VAL_CLASS) {
        // Handle error: Not a class
        return NULL;
    }
    return classValue.as.class->superclass;
}

LoxInstance* getThisInstance(Environment* env) {
    Value thisValue = environmentGet(env, "this");
    if (thisValue.type != VAL_INSTANCE) {
        // Handle error: 'this' is not an instance
        return NULL;
    }
    return thisValue.as.instance;
}

LoxBoundMethod* newLoxBoundMethod(LoxInstance* instance, LoxFunction* method) {
    LoxBoundMethod* boundMethod = malloc(sizeof(LoxBoundMethod));
    if (boundMethod == NULL) {
        // Handle allocation failure
        return NULL;
    }
    boundMethod->instance = instance;
    boundMethod->method = method;
    return boundMethod;
}

void bindMethod(LoxInstance* instance, LoxFunction* method) {
    // Assuming LoxFunction can be turned into a bound method
    LoxBoundMethod* boundMethod = newLoxBoundMethod(instance, method);
    // Add the bound method to the instance's methods
    hashmapPut(&instance->methods, method->name, boundMethod);
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

Value executeSuperExpr(SuperExpr* expr, Environment* env) {
    // Get the superclass and the instance ('this')
    LoxClass* superclass = getSuperclass(env, expr->keyword); // IMPLICIT
    LoxInstance* instance = getThisInstance(env); // IMPLICIT

    LoxFunction* method = loxClassGetMethod(superclass, expr->method.lexeme); // IMPLICIT
    if (method == NULL) {
        // Handle error: Method not found
    }

    return bindMethod(instance, method); // IMPLICIT
}

// Forward declaration of evaluate functions
Value evaluateBinaryExpr(BinaryExpr* expr);
Value evaluateLogicalExpr(LogicalExpr* expr, Environment* env);
Value evaluateFunctionCallExpr(FunctionCallExpr* expr, Environment* env);
Value evaluate(Expr* expr, Environment* env);

Value evaluateBinaryExpr(BinaryExpr* expr) {
    Value left = evaluate(expr->left);
    Value right = evaluate(expr->right);

    // Assuming both are numbers for simplicity; add type checking and error handling
    if (expr->operator.type == TOKEN_PLUS) { // Token enumeration for lexer/parser
        return createNumberValue(asNumber(left) + asNumber(right));
    }
    // Handle other operators
}

Value evaluateLogicalExpr(LogicalExpr* expr, Environment* env) {
    Value left = evaluate(expr->left, env);

    if (expr->operator == TOKEN_OR) { // Token enumeration for lexer/parser
        if (isTruthy(left)) return left;
    } else if (expr->operator == TOKEN_AND) { // Token enumeration for lexer/parser
        if (!isTruthy(left)) return left;
    }

    return evaluate(expr->right, env);
}

// Forward declarations
// callFunction
Value callFunction(struct Function* function, struct List* arguments, struct Environment* globalEnv);
// getVariableValue
Value getVariableValue(struct Environment* env, struct Token name);
// parseEquality
struct Expr* parseEquality();

Value evaluateFunctionCallExpr(FunctionCallExpr* expr, Environment* env) {
    Value callee = evaluate(expr->callee, env);
    List args;
    initList(&args);

    for (int i = 0; i < expr->arguments->count; i++) {
        listAppend(&args, evaluate(expr->arguments->items[i], env));
    }

    Value result = callFunction(callee.as.function, &args);
    freeList(&args);
    return result;
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
            return value;
        }
        case EXPR_LOGICAL:
            return evaluateLogicalExpr((LogicalExpr*)expr, env);
        // Add cases for other expression types here
        default:
            // Handle unknown or unsupported expression types
            break;
    }
}

LoxClass* asClass(Value value) {
    if (value.type != VAL_CLASS) {
        // Handle error: Value is not a class
        return NULL;
    }
    return value.as.class;
}

LoxClass* newLoxClass(const char* name) {
    LoxClass* klass = malloc(sizeof(LoxClass));
    klass->name = strdup(name);
    initHashmap(&klass->methods);
    return klass;
}

void defineVariable(Scope* currentScope, const char* name) {
    // Update the variable state in the current scope to indicate it's fully defined
    hashmapPut(&currentScope->variables, name, true);
}

void executeClassDecl(ClassDecl* classDecl, Environment* env) {
    Value superclassValue = NIL_VALUE;
    if (classDecl->superclass != NULL) {
        superclassValue = evaluate(classDecl->superclass, env);
        if (superclassValue.type != VAL_CLASS) {
            // Handle error: Superclass must be a class
        }
    }

    // Create the class object
    LoxClass* klass = newLoxClass(classDecl->name.lexeme, asClass(superclassValue)); // IMPLICIT
    // Add methods to the class...
    defineVariable(env, classDecl->name.lexeme, klass);
}

void executeVarStmt(Environment* env, VarStmt* stmt) {
    Value value = NIL_VALUE; // Default to nil if no initializer
    if (stmt->initializer != NULL) {
        value = evaluate(stmt->initializer);
    }
    defineVariable(env, stmt->name.start, value);
}

void executeStatement(struct Environment* env, struct Stmt* stmt);

void executeIfStmt(Environment* env, IfStmt* stmt) {
    Value conditionValue = evaluate(stmt->condition, env);
    if (isTruthy(conditionValue)) {
        executeStatement(env, stmt->thenBranch); // IMPLICIT
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
            evaluate(((ExpressionStmt*)stmt)->expression); // IMPLICIT
            break;
        case STMT_PRINT:
            {
                Value val = evaluate(((PrintStmt*)stmt)->expression);
                printValue(val);
            }
            break;
        case STMT_VAR:
            executeVarStmt(env, (VarStmt*)stmt);
            break;
        case STMT_WHILE:
            executeWhileStmt(env, (WhileStmt*)stmt);
            break;
        case STMT_IF:
            executeIfStmt(env, (IfStmt*)stmt);
            break;
        // Handle other statement types
        default:
            // Handle unknown statement types or raise an error
            break;
    }
}

void initEnvironment(Environment* env) {
    env->head = NULL;
}

void freeValue(Value value) {
    switch (value.type) {
        case VAL_STRING:
            free(value.as.string);
            break;
    }
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

Value callFunction(Function* function, List* arguments, Environment* globalEnv) {
    // Create a new environment for the function scope
    Environment localEnv;
    initEnvironment(&localEnv);
    localEnv.enclosing = function->closure ? function->closure : globalEnv;

    // Bind each argument to the corresponding parameter
    for (int i = 0; i < function->params->count; i++) {
        if (i < arguments->count) {
            defineVariable(&localEnv, function->params->items[i]->lexeme, arguments->items[i]);
        }
    }

    // Execute the function body
    Value returnValue = NIL_VALUE;
    for (int i = 0; i < function->body->statements->count; i++) {
        executeStatement(function->body->statements->items[i], &localEnv);
        // Handle return value if there's a return statement
        // You might need a mechanism to detect return statements and break out of the loop
    }

    // Clean up the local environment
    freeEnvironment(&localEnv); // IMPLICIT

    return returnValue;
}

LoxFunction* loxClassFindMethod(LoxClass* klass, const char* methodName) {
    // Assuming klass has a hashmap or similar structure to store methods
    // The details of this function depend on how methods are stored in LoxClass
    return hashmapGet(&klass->methods, methodName);
}


Value instantiateClass(LoxClass* klass, List* arguments) {
    LoxInstance* instance = newLoxInstance(klass);

    // Find the initializer, if it exists
    LoxFunction* initializer = loxClassFindMethod(klass, "init"); // IMPLICIT
    if (initializer != NULL) {
        callFunction(initializer, arguments, instance);
    }

    // Return the new instance
    Value instanceVal;
    instanceVal.type = VAL_INSTANCE;
    instanceVal.as.instance = instance; // union <anonymous> has no member named 'instance' FIXME
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

void errorAtCurrent(const char* message) {
    if (hadError) return; // Prevent multiple errors for the same issue
    hadError = true;

    fprintf(stderr, "[line %d] Error at '%.*s': %s\n", 
            previous.line, previous.length, previous.start, message);
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
    }
    expr->base.type = NODE_GROUPING_EXPR;
    expr->expression = expression;
    return expr;
}

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

Expr* parsePrimary() {
    if (match(TOKEN_FALSE)) { // Token enumeration for lexer/parser
        LiteralValue value = {.boolean = false};
        return newLiteralExpr(value, LITERAL_TYPE_BOOLEAN);
    }
    if (match(TOKEN_TRUE)) { // Token enumeration for lexer/parser
        LiteralValue value = {.boolean = true};
        return newLiteralExpr(value, LITERAL_TYPE_BOOLEAN);
    }
    if (match(TOKEN_NIL)) { // Token enumeration for lexer/parser
        LiteralValue value;
        value.boolean = false;
        return newLiteralExpr(value, LITERAL_TYPE_NIL);
    }

    if (match(TOKEN_NUMBER)) { // Token enumeration for lexer/parser
        double number = strtod(previous.start, NULL);
        LiteralValue value = {.number = number};
        return newLiteralExpr(value, LITERAL_TYPE_NUMBER);
    }

    if (match(TOKEN_STRING)) { // Token enumeration for lexer/parser
        char* string = extractString(previous.start, previous.length);
        if (!string) {
            // Memory allocation failed, handle error
            return NULL;
        }
        LiteralValue value = {.string = string};
        return newLiteralExpr(value, LITERAL_TYPE_STRING);
    }

    if (match(TOKEN_LEFT_PAREN)) { // Token enumeration for lexer/parser
        Expr* expr = parseExpression();
        consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after expression."); // IMLPLICIT
        return newGroupingExpr(expr); // IMPLICIT
    }
    // Handle unexpected tokens
    errorAtCurrent("Expect expression.");
    return NULL; // Indicate an eror occurred

    // Error handling for unexpected tokens
    // Example:
    // errorAtCurrent("Expect expression.");
    // return NULL;
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

Expr* parseUnary() {
    if (match(TOKEN_BANG) || match(TOKEN_MINUS)) { // Token enumeration for lexer/parser
        Token operator = previousToken();
        Expr* right = parseUnary();
        return newUnaryExpr(operator, right);
    }

    return parsePrimary();
}

Expr* parseMultiplication() {
    Expr* expr = parseUnary();

    while (match(TOKEN_STAR) || match(TOKEN_SLASH)) { // Token enumeration for lexer/parser
        Token operator = previousToken();
        Expr* right = parseUnary();
        expr = newBinaryExpr(expr, right, operator); // IMPLICIT
    }

    return expr;
}

Expr* parseAddition() {
    Expr* expr = parseMultiplication();

    while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) { // Token enumeration for lexer/parser
        Token operator = previousToken();
        Expr* right = parseMultiplication();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parseComparison() {
    Expr* expr = parseAddition();

    while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) || // Token enumeration for lexer/parser
           match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)) { // Token enumeration for lexer/parser
        Token operator = previousToken();
        Expr* right = parseAddition();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parseEquality() {
    Expr* expr = parseComparison();

    while (match(TOKEN_EQUAL_EQUAL) || match(TOKEN_BANG_EQUAL)) { // Token enumeration for lexer/parser
        Token operator = previousToken();
        Expr* right = parseComparison();
        expr = newBinaryExpr(expr, right, operator);
    }

    return expr;
}

Expr* parseLogical() {
    Expr* expr = parseEquality();

    while (match(TOKEN_AND) || match(TOKEN_OR)) { // Token enumeration for lexer/parser
        Token operator = previousToken(); // The logical operator token
        Expr* right = parseEquality();
        LogicalExpr* logical = (LogicalExpr*)malloc(sizeof(LogicalExpr));
        if (!logical) {
            // Handle memory allocation failure
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
        return NULL;
    }
    expr->name = name;  // Assuming Token is a value type; if it's a pointer, duplicate it
    expr->value = value;
    return expr;
}

Expr* parseAssignment() {
    Expr* expr = parseEquality();

    if (match(TOKEN_EQUAL)) { // Token enumeration for lexer/parser
        Token equals = previousToken();
        Expr* value = parseAssignment();

        if (expr->type == EXPR_VARIABLE) {
            Token name = ((VariableExpr*)expr)->name;
            return newAssignExpr(name, value); // IMPLICIT
        }

        // Handle invalid assignment target
    }

    return expr;
}

Expr* parseSuperExpression() {
    Token keyword = previousToken();  // The 'super' token
    consumeToken(TOKEN_DOT, "Expect '.' after 'super'."); // Token enumeration for lexer/parser
    Token method = consumeToken(TOKEN_IDENTIFIER, "Expect superclass method name.");
    return newSuperExpr(keyword, method);
}

Stmt* parseVarDeclaration() {
    Token name = consumeToken(TOKEN_IDENTIFIER, "Expect variable name."); // Token enumeration for lexer/parser

    Expr* initializer = NULL;
    if (match(TOKEN_EQUAL)) { // Token enumeration for lexer/parser
        initializer = parseExpression();
    }

    consumeToken(TOKEN_SEMICOLON, "Expect ';' after variable declaration."); // Token enumeration for lexer/parser
    VarStmt varStmt = {name, initializer};
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_VAR;
    stmt->as.var = varStmt;
    return stmt;
}

Stmt* parseExpressionStatement() {
    Expr* expr = parseExpression();
    consumeToken(TOKEN_SEMICOLON, "Expect ';' after expression."); // Token enumeration for lexer/parser
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_EXPRESSION;
    stmt->as.expression = expr;
    return stmt;
}

Stmt* parsePrintStatement() {
    Expr* value = parseExpression();
    consumeToken(TOKEN_SEMICOLON, "Expect ';' after value."); // Token enumeration for lexer/parser
    PrintStmt printStmt = {value};
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_PRINT;
    stmt->as.print = printStmt;
    return stmt;
}

Stmt* parseStatement() {
    if (match(TOKEN_PRINT)) { // Token enumeration for lexer/parser
        return parsePrintStatement();
    } else if (match(TOKEN_VAR)) { // Token enumeration for lexer/parser
        return parseVarDeclaration();
    } else {
        return parseExpressionStatement();
    }
}

Stmt* parseIfStatement() {
    consumeToken(TOKEN_LEFT_PAREN, "Expect '(' after 'if'."); // Token enumeration for lexer/parser
    Expr* condition = parseExpression();
    consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after if condition."); // Token enumeration for lexer/parser

    Stmt* thenBranch = parseStatement();
    Stmt* elseBranch = NULL;
    if (match(TOKEN_ELSE)) { // Token enumeration for lexer/parser
        elseBranch = parseStatement();
    }

    IfStmt* stmt = (IfStmt*)malloc(sizeof(IfStmt));
    if (!stmt) {
        // Handle memory allocation failure
    }
    stmt->base.type = STMT_IF;
    stmt->condition = condition;
    stmt->thenBranch = thenBranch;
    stmt->elseBranch = elseBranch;

    return (Stmt*)stmt;
}

Stmt* parseWhileStatement() {
    consumeToken(TOKEN_LEFT_PAREN, "Expect '(' after 'while'."); // Token enumeration for lexer/parser
    Expr* condition = parseExpression();
    consumeToken(TOKEN_RIGHT_PAREN, "Expect ')' after condition."); // Token enumeration for lexer/parser
    Stmt* body = parseStatement();

    WhileStmt* stmt = (WhileStmt*)malloc(sizeof(WhileStmt));
    if (!stmt) {
        // Handle memory allocation failure
    }
    stmt->base.type = STMT_WHILE;
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

    ///////////////////////
    // AST Node Creators
    ///////////////////////

    /////////////////////////////////////
    // Runtime Execution and Evaluation
    /////////////////////////////////////
    // Runtime Control
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

    //Interpreter Runtime
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

    // Evaluation
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

void defineVariable(Environment* env, const char* name, Value value) {
    EnvEntry* entry = newEnvEntry(name, value);
    entry->next = env->head;
    env->head = entry;
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
    hashmapPut(&currentScope->variables, name, false);
}

bool isVariableDeclared(Scope* currentScope, const char* name) {
    bool isDeclared;
    if (hashmapGet(&currentScope->variables, name, &isDeclared)) { // IMPLICIT
        return isDeclared;
    }
    return false;
}

bool resolveVariable(Scope* currentScope, const char* name) {
    while (currentScope != NULL) {
        if (isVariableDeclared(currentScope, name)) {
            return true;  // Variable found in the current or an enclosing scope
        }
        currentScope = currentScope->enclosing;
    }
    return false;  // Variable not found
}

void resolveStatement(Scope* currentScope, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_VAR: {
            VarStmt* varStmt = (VarStmt*)stmt;
            declareVariable(currentScope, varStmt->name.lexeme);
            if (varStmt->initializer != NULL) {
                resolveExpression(currentScope, varStmt->initializer);
            }
            defineVariable(currentScope, varStmt->name.lexeme);
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* blockStmt = (BlockStmt*)stmt;
            pushScope(&currentScope);
            for (int i = 0; i < blockStmt->statements->count; i++) {
                resolveStatement(currentScope, blockStmt->statements->items[i]);
            }
            popScope(&currentScope);
            break;
        }
        // Add cases for other statement types like STMT_IF, STMT_WHILE, etc.
        // ...
    }
}

void resolveExpression(Scope* currentScope, Expr* expr) {
    switch (expr->type) {
        case EXPR_VARIABLE: {
            VariableExpr* varExpr = (VariableExpr*)expr;
            if (!isVariableDeclared(currentScope, varExpr->name.lexeme)) {
                reportError(varExpr->name.line, "Undefined variable.");
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
        // Add cases for other expression types like EXPR_CALL, EXPR_LOGICAL, etc.
        // ...
    }
}

void analyzeSemantics(Stmt** statements, Scope* globalScope) {
    Scope* currentScope = globalScope;
    for (int i = 0; statements[i] != NULL; i++) {
        resolveStatement(currentScope, statements[i]);
    }
}

void resolveFunctionDecl(Scope* currentScope, FunctionDeclStmt* stmt) {
    declareVariable(currentScope, stmt->name.lexeme);
    defineVariable(currentScope, stmt->name.lexeme);

    pushScope(&currentScope);  // New scope for function body
    for (int i = 0; i < stmt->params->count; i++) {
        declareVariable(currentScope, stmt->params->items[i].lexeme);
        defineVariable(currentScope, stmt->params->items[i].lexeme);
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
    resolveLocal(resolver, expr->keyword); // IMPLICIT
}

    /////////////////////////////
    // Function Call Processing
    /////////////////////////////
Value makeFunctionValue(LoxFunction* function) {
    Value functionValue;
    functionValue.type = VAL_FUNCTION;
    functionValue.as.function = function;
    return functionValue;
}

void executeFunctionDeclStmt(FunctionDeclStmt* stmt, Environment* env) {
    bool isInit = false;
    Function* function = newLoxFunction(stmt, env, isInit); // IMPLICIT
    defineVariable(env, stmt->name, makeFunctionValue(function)); // IMPLICIT (makeFunctionValue)
}

Value executeReturnStmt(ReturnStmt* stmt, Environment* env) {
    Value returnValue = NIL_VALUE; // Default to nil
    if (stmt->value != NULL) {
        returnValue = evaluate(stmt->value, env);
    }
    // Use longjmp to jump back to the function call point
    longjmp(returnPoint, returnValue); // IMPLICIT
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
        // Additional logic to ensure the superclass is valid
    }

    // Resolve methods...

    resolver->insideClass = enclosingClass;
    resolver->insideSubclass = false;
}

VariableExpr* newVariableExpr(Token token) {
    VariableExpr* expr = malloc(sizeof(VariableExpr));
    if (expr == NULL) {
        // Handle allocation failure
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
        return NULL;
    }

    list->capacity = 10; // Starting capacity, can be any reasonable number
    list->count = 0;
    list->items = malloc(sizeof(void*) * list->capacity);
    if (list->items == NULL) {
        // Handle allocation failure
        free(list);
        return NULL;
    }

    return list;
}

ClassDecl* parseClassDeclaration() {
    consumeToken(TOKEN_CLASS, "Expect 'class' keyword."); // Token enumeration for lexer/parser
    Token name = consumeToken(TOKEN_IDENTIFIER, "Expect class name."); // Token enumeration for lexer/parser

    Expr* superclass = NULL;
    if (match(TOKEN_LESS)) { // Token enumeration for lexer/parser
        consumeToken(TOKEN_IDENTIFIER, "Expect superclass name."); // Token enumeration for lexer/parser
        superclass = newVariableExpr(previousToken()); // IMPLICIT (newVariableExpr)
    }

    consumeToken(TOKEN_LEFT_BRACE, "Expect '{' before class body."); // Token enumeration for lexer/parser
    List* methods = initNewList();
    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) { // Token enumeration for lexer/parser
        listAppend(methods, parseFunction("method")); // IMPLICIT (parseFunction)
    }
    consumeToken(TOKEN_RIGHT_BRACE, "Expect '}' after class body."); // Token enumeration for lexer/parser

    ClassDecl* classDecl = (ClassDecl*)malloc(sizeof(ClassDecl));
    classDecl->name = name;
    classDecl->superclass = superclass;
    classDecl->methods = methods;
    return classDecl;
}

void loxClassAddMethod(LoxClass* klass, const char* methodName, LoxFunction* method) {
    hashmapPut(&klass->methods, methodName, method);
}

    /////////////////////////////
    // Lexical Analysis Helpers
    /////////////////////////////
char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

bool isNumber(Value value) {
    return value.type == VAL_NUMBER;
}

    /////////////////////////////////
    // Error Reporting and Handling
    /////////////////////////////////
void error(int line, const char* message) {
    fprintf(stderr, "[line %d] Error: %s\n", line, message);
    // Handle the error, possibly by setting a flag to indicate an error state
}

void resolveVariableWithErrorHandling(Scope* currentScope, const char* name, int line) {
    if (!resolveVariable(currentScope, name)) {
        reportError(line, "Variable not declared in this scope.");
    }
}

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
    token.type = TOKEN_ERROR; // Token enumeration for lexer/parser
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

    return TOKEN_IDENTIFIER; // Token enumeration for lexer/parser
}

TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND); // Token enumeration for lexer/parser
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS); // Token enumeration for lexer/parser
        // ... other keywords
        default: return TOKEN_IDENTIFIER; // Token enumeration for lexer/parser
    }
}

Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    consumeToken(identifierType());
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

    consumeToken(TOKEN_NUMBER, "Expected a number"); // Token enumeration for lexer/parser
    return previous;
}

Token scanToken() {
    skipWhitespace();

    scanner.start = scanner.current;
    if (isAtEnd()) return makeToken(TOKEN_EOF); // Token enumeration for lexer/parser

    char c = advance();

    // Add logic to handle different characters
    // For example:
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN); // Token enumeration for lexer/parser
        // Add cases for other single characters
        // ...
    }

    Token token = makeToken(...);
    previous = token;
    return token;
}

    //////////////////////////////////////
    // Expression Printing and Debugging
    //////////////////////////////////////
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

void printBinaryExpr(BinaryExpr* expr) {
    printf("BinaryExpr: Operator=%c\n", expr->operator);
    expr->left->accept(expr->left, &printVisitor);
    expr->right->accept(expr->right, &printVisitor);
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

    // Memory Management
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

//////////////////////////
// Comments for Sections
//////////////////////////

//////////////////////////
// Test Functions
//////////////////////////

///////////////////////////////////////
// Appendices or Additional Resources
///////////////////////////////////////