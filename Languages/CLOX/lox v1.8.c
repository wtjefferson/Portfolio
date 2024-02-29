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
    EXPR_LOGICAL,
    EXPR_SUPER,
} ExprType;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_INSTANCE,
    VAL_CLASS,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        char* string;
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
    } as;
};

typedef enum {
    STMT_EXPRESSION,
    STMT_PRINT,
    STMT_VAR,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
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

typedef struct {
    Stmt base;     // Inherit from Stmt
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;
} IfStmt;

typedef struct {
    Node base;
    Expr* left;
    Expr* right;
    TokenType operator;
} LogicalExpr;

typedef struct {
    Stmt base;  // Inherit from Stmt
    Expr* condition;
    Stmt* body;
} WhileStmt;

typedef struct {
    void** items;    // Dynamic array of pointers
    int capacity;    // Allocated capacity
    int count;       // Number of items
} List;

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

typedef struct {
    Node base;
    Expr* callee;  // The expression that produces the function
    List* arguments;  // A list of expressions representing arguments
} FunctionCallExpr;

typedef struct {
    Node base;
    Token name;  // Function name
    List* params;  // List of parameters
    BlockStmt* body;  // Function body (a series of statements)
} FunctionDeclStmt;

typedef struct {
    Node base;
    Expr* value;  // Expression for the return value
} ReturnStmt;

typedef struct {
    List* params;     // List of parameter names (Tokens)
    BlockStmt* body;  // Function body (block statement)
    Environment* closure;  // Closure environment, if needed
} Function;

#define HASHMAP_INITIAL_CAPACITY 8

typedef struct {
    char* key;
    bool value;
} HashmapEntry;

typedef struct {
    HashmapEntry* entries;
    int capacity;
    int count;
} Hashmap;

typedef struct Scope {
    Hashmap* variables;  // HashMap to store variable state
    struct Scope* enclosing;  // Pointer to the enclosing (outer) scope
} Scope;

typedef struct {
    Scope* current;  // Current (top) scope
    // Possibly other fields for tracking state
} ScopeStack;

// Represents a method in a class
typedef struct {
    Token name;  // Method name
    Function* function;  // The function representing the method
} Method;

// Represents a class declaration
typedef struct {
    Token name;  // Class name
    List* methods;  // List of methods
} ClassDecl;

typedef struct LoxClass {
    char* name;       // Name of the class
    Hashmap methods;  // Hashmap to store methods
} LoxClass;

typedef struct {
    Function* declaration;   // AST node representing the function declaration
    Environment* closure;    // Captured environment for closures
    bool isInitializer;      // Flag for class initializers
} LoxFunction;

LoxFunction* newLoxFunction(Function* declaration, Environment* closure, bool isInitializer) {
    LoxFunction* function = malloc(sizeof(LoxFunction));
    function->declaration = declaration;
    function->closure = closure;
    function->isInitializer = isInitializer;
    return function;
}

void freeLoxFunction(LoxFunction* function) {
    // Free the function resources. Be careful with the closure as it might be shared.
    free(function);
}

typedef struct {
    LoxClass* klass;   // Reference to the class
    Hashmap fields;    // Hashmap for instance fields
} LoxInstance;

LoxInstance* newLoxInstance(LoxClass* klass) {
    LoxInstance* instance = malloc(sizeof(LoxInstance));
    instance->klass = klass;
    initHashmap(&instance->fields);
    return instance;
}

void freeLoxInstance(LoxInstance* instance) {
    freeHashmap(&instance->fields);
    free(instance);
}

typedef enum {
    CONTEXT_NONE,
    CONTEXT_CLASS
} FunctionContext;

typedef struct {
    Scope* currentScope;
    FunctionContext currentContext;
    bool insideClass;
    bool insideSubclass;
} Resolver;

void resolveThisExpr(Resolver* resolver, ThisExpr* expr) {
    if (resolver->currentContext != CONTEXT_CLASS) {
        reportError("Can't use 'this' outside of a class method.");
    }
    // Resolve 'this' expression
}

typedef struct {
    Token name;      // Class name
    Expr* superclass;  // Optional superclass
    List* methods;   // List of methods
} ClassDecl;

typedef struct {
    Node base;       // Inherit from a base Node structure
    Token keyword;   // The 'super' keyword token
    Token method;    // The method name token
} SuperExpr;

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

ClassDecl* parseClassDeclaration() {
    consume(TOKEN_CLASS, "Expect 'class' keyword.");
    Token name = consume(TOKEN_IDENTIFIER, "Expect class name.");

    Expr* superclass = NULL;
    if (match(TOKEN_LESS)) {
        consume(TOKEN_IDENTIFIER, "Expect superclass name.");
        superclass = newVariableExpr(previousToken());
    }

    consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
    List* methods = initList();
    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        listAppend(methods, parseFunction("method"));
    }
    consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");

    ClassDecl* classDecl = (ClassDecl*)malloc(sizeof(ClassDecl));
    classDecl->name = name;
    classDecl->superclass = superclass;
    classDecl->methods = methods;
    return classDecl;
}

LoxClass* newLoxClass(const char* name) {
    LoxClass* klass = malloc(sizeof(LoxClass));
    klass->name = strdup(name);
    initHashmap(&klass->methods);
    return klass;
}

void loxClassAddMethod(LoxClass* klass, const char* methodName, LoxFunction* method) {
    hashmapPut(&klass->methods, methodName, method);
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

void loxClassAddMethod(LoxClass* klass, const char* methodName, LoxFunction* method) {
    hashmapPut(&klass->methods, methodName, method);
}

void freeLoxClass(LoxClass* klass) {
    free(klass->name);
    freeHashmap(&klass->methods);
    free(klass);
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
    LoxClass* klass = newLoxClass(classDecl->name.lexeme, asClass(superclassValue));
    // Add methods to the class...
    defineVariable(env, classDecl->name.lexeme, klass);
}

Value instantiateClass(LoxClass* klass, List* arguments) {
    LoxInstance* instance = newLoxInstance(klass);

    LoxFunction* initializer = loxClassGetMethod(instance, "init");
    if (initializer != NULL) {
        callFunction(initializer, arguments, instance);
    }

    return makeInstanceValue(instance);
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

void defineVariable(Scope* currentScope, const char* name) {
    // Update the variable state in the current scope to indicate it's fully defined
    hashmapPut(&currentScope->variables, name, true);
}

bool isVariableDeclared(Scope* currentScope, const char* name) {
    bool isDeclared;
    if (hashmapGet(&currentScope->variables, name, &isDeclared)) {
        return isDeclared;
    }
    return false;
}

void analyzeSemantics(Stmt** statements, Scope* currentScope) {
    // Example: Loop through statements and analyze each one
    for (int i = 0; statements[i] != NULL; i++) {
        analyzeStatement(statements[i], currentScope);
        // Implement analyzeStatement based on statement type
    }
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

void resolveStatement(Scope* currentScope, Stmt* stmt);
void resolveExpression(Scope* currentScope, Expr* expr);

void analyzeSemantics(Stmt** statements, Scope* globalScope) {
    Scope* currentScope = globalScope;
    for (int i = 0; statements[i] != NULL; i++) {
        resolveStatement(currentScope, statements[i]);
    }
}

void resolveSuperExpr(Resolver* resolver, SuperExpr* expr) {
    if (!resolver->insideSubclass) {
        reportError("Can't use 'super' outside of a subclass.");
    }
    // Resolve 'super' as a local variable to ensure proper binding
    resolveLocal(resolver, expr->keyword);
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

void reportError(int line, const char* message) {
    // Implementation depends on how you handle errors in your interpreter
    printf("[Line %d] Error: %s\n", line, message);
}

void resolveVariableWithErrorHandling(Scope* currentScope, const char* name, int line) {
    if (!resolveVariable(currentScope, name)) {
        reportError(line, "Variable not declared in this scope.");
    }
}

Expr* parseSuperExpression() {
    Token keyword = previousToken();  // The 'super' token
    consume(TOKEN_DOT, "Expect '.' after 'super'.");
    Token method = consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
    return newSuperExpr(keyword, method);
}

Value executeSuperExpr(SuperExpr* expr, Environment* env) {
    // Get the superclass and the instance ('this')
    LoxClass* superclass = getSuperclass(env, expr->keyword);
    LoxInstance* instance = getThisInstance(env);

    LoxFunction* method = loxClassGetMethod(superclass, expr->method.lexeme);
    if (method == NULL) {
        // Handle error: Method not found
    }

    return bindMethod(instance, method);
}

Stmt* parseIfStatement() {
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = parseExpression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after if condition.");

    Stmt* thenBranch = parseStatement();
    Stmt* elseBranch = NULL;
    if (match(TOKEN_ELSE)) {
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

Expr* parseLogical() {
    Expr* expr = parseEquality();

    while (match(TOKEN_AND) || match(TOKEN_OR)) {
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

Value instantiateClass(LoxClass* klass, List* arguments) {
    LoxInstance* instance = newLoxInstance(klass);

    LoxFunction* initializer = loxClassGetMethod(instance, "init");
    if (initializer != NULL) {
        callFunction(initializer, arguments, instance);
    }

    return makeInstanceValue(instance);
}

Value instantiateClass(LoxClass* klass, List* arguments) {
    LoxInstance* instance = newLoxInstance(klass);

    // Find the initializer, if it exists
    LoxFunction* initializer = loxClassFindMethod(klass, "init");
    if (initializer != NULL) {
        callFunction(initializer, arguments, instance);  // Implement callFunction accordingly
    }

    // Return the new instance
    Value instanceVal;
    instanceVal.type = VAL_INSTANCE;
    instanceVal.as.instance = instance;
    return instanceVal;
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

Value evaluate(Expr* expr, Environment* env) {
    switch (expr->type) {
        case EXPR_LOGICAL:
            return evaluateLogicalExpr((LogicalExpr*)expr, env);
    }
}

void executeIfStmt(Environment* env, IfStmt* stmt) {
    Value conditionValue = evaluate(stmt->condition, env);
    if (isTruthy(conditionValue)) {
        executeStatement(env, stmt->thenBranch);
    } else if (stmt->elseBranch != NULL) {
        executeStatement(env, stmt->elseBranch);
    }
}

void executeStatement(Environment* env, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_IF:
            executeIfStmt(env, (IfStmt*)stmt);
            break;
    }
}

bool isTruthy(Value value) {
    switch (value.type) {
        case VAL_BOOL: return value.as.boolean;
        case VAL_NIL: return false;
        default: return true;
    }
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
    }
}

Value evaluateBinaryExpr(BinaryExpr* expr) {
    Value left = evaluate(expr->left);
    Value right = evaluate(expr->right);
    if (expr->operator.type == TOKEN_PLUS) {
        return createNumberValue(asNumber(left) + asNumber(right));
    }
    // Handle other operators
}

Stmt* parseWhileStatement() {
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = parseExpression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
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

void executeWhileStmt(Environment* env, WhileStmt* stmt) {
    while (isTruthy(evaluate(stmt->condition, env))) {
        executeStatement(env, stmt->body);
    }
}

void executeStatement(Environment* env, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_WHILE:
            executeWhileStmt(env, (WhileStmt*)stmt);
            break;
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
    }

    // Clean up the local environment
    freeEnvironment(&localEnv);

    return returnValue;
}

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

void executeFunctionDeclStmt(FunctionDeclStmt* stmt, Environment* env) {
    Function* function = newFunction(stmt, env);
    defineVariable(env, stmt->name, makeFunctionValue(function));
}

Value executeReturnStmt(ReturnStmt* stmt, Environment* env) {
    Value returnValue = NIL_VALUE; // Default to nil
    if (stmt->value != NULL) {
        returnValue = evaluate(stmt->value, env);
    }
    // Use longjmp to jump back to the function call point
    longjmp(returnPoint, returnValue);
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
    // Example: using the print visitor
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
