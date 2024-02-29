#include <stdio.h>      // For input/output functions
#include <stdlib.h>     // For dynamic memory allocation and general utilities
#include <string.h>     // For string manipulation functions
#include <ctype.h>      // For character type functions, like isalpha, isdigit
#include <stdbool.h>    // For using the 'bool' type

//====================
// Constants and macros
//====================
#define BUFFER_SIZE 1024
#define READ_BUFFER_SIZE 1024
#define MAX_FUNCTIONS 100
#define MAX_MAP_SIZE 100
#define MAX_SYMBOLS 100

//====================
// Data structures
//====================
// Token structures
typedef enum {
    TOKEN_INT, 
    TOKEN_FLOAT, 
    TOKEN_PLUS, 
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SPIRAL,
    TOKEN_DOT,
    TOKEN_INSERT,
    TOKEN_LAT,
    TOKEN_SPIRALOUT,
    TOKEN_REFLECTION,
    TOKEN_STRING,
} TokenType;

typedef struct {
    TokenType type;
    int line; // Line number where token was found
    int column; // Column number where the token starts
    union {
        int intValue;
        float floatValue;
        char* stringValue;
    };
} Token;

// AST structures
typedef enum {
    NODE_INT_LITERAL,
    NODE_FLOAT_LITERAL,
    NODE_BINARY_OP,
    NODE_FUNCTION_CALL,
    NODE_FUNCTION_DEFINITION,
    NODE_IDENTIFIER,
    NODE_LAT_DECL,
    NODE_ULAT_DECL,
    NODE_MAP_OPERATION,
    NODE_MAP_DELETION,
    NODE_COMPOUND_STATEMENT,
    NODE_SPIRAL,
    NODE_MAP_REFLECTION,
} NodeType;

// Forward declaration
typedef struct ASTNode ASTNode;

// Map operations
typedef struct {
    NodeType type;
    char* mapName;
    ASTNode* key;
    ASTNode* value;
    TokenType operation;
} MapOperationsNode;

// Compound statement
typedef struct {
    NodeType type;
    ASTNode** statements;
    int statementCount;
} CompoundStatementNode;

// spiral
typedef struct {
    NodeType type;
    int iterations;
    int currentIteration;
    struct ASTNode* body;
} SpiralNode;

// Primary AST Node struct
struct ASTNode {
    NodeType type;
    union {
        int intValue;
        float floatValue;
        char* identifierValue;
        int evaluatedValue;
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            TokenType op; // Operator type, e.g., TOKEN_PLUS
        } binaryOp;
        struct {
            char* functionName;
            struct ASTNode** arguments;
            struct ASTNode* body;
            int argCount;
        } functionCall;
        CompoundStatementNode* compound;
        SpiralNode* spiral;
        MapOperationsNode mapOp;
        char* mapName;
    };
};

// lat and ulat
typedef struct {
    NodeType type;
    // Additional fields for maps
    struct KeyValuePairs* pairs;
    int size;
} MapNode;

typedef struct {
    char* key;
    ASTNode* value;
} KeyValuePair;

// Function definitions
typedef struct {
    char* name;
    ASTNode* body;
} FunctionDef;

// lat and ulate associated structs
typedef struct {
    char* key;
    ASTNode* value;
} MapEntry;

typedef struct {
    MapEntry entries[MAX_MAP_SIZE];
    int size;
} Map;

typedef struct {
    char* name;
    Map* map;
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int size;
} SymbolTable;

//====================
// Global Variables
//====================
Token currentToken;
Token nextToken;
bool nextTokenAvailable = false;
char* currentPosition;
FILE *inputFile; // File stream for input
int currentLine = 1;
int currentColumn = 1;
FunctionDef functionTable[MAX_FUNCTIONS];
int functionCount = 0;
SymbolTable globalSymbolTable = {0};

//====================
// Helper Functions
//====================
// Function to create a map
Map* createNewMap() {
    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) {
        fprintf(stderr, "Memory allocation failed\n");
    }
    map->size = 0;
    return map;
}

// Function to store a map
void storeMap(const char* mapName, Map* map) {
    if (globalSymbolTable.size >= MAX_SYMBOLS) {
        fprintf(stderr, "Symbol table overflow\n");
        exit(EXIT_FAILURE);
    }

    // Check for existing map with the same name
    for (int i = 0; i < globalSymbolTable.size; i++) {
        if (strcmp(globalSymbolTable.symbols[i].name, mapName) == 0) {
            fprintf(stderr, "Map with name '%s' already exists\n", mapName);
            exit(EXIT_FAILURE);
        }
    }

    // Store the new map
    int index = globalSymbolTable.size++;
    globalSymbolTable.symbols[index].name = strdup(mapName);
    globalSymbolTable.symbols[index].map = map;
}

// Function to get the next character from the input
char getNextChar() {
    if (*currentPosition == '\0') {
        return EOF; // Return EOF if end of buffer
    }

    char c = *currentPosition;
    currentPosition++; // Move to the next character

    if (c == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
    return c;
}

// Error Reporting Function
void reportError(const char* errorMessage, int line, int column) {
    fprintf(stderr, "Error at Line %d, Column %d: %s\n", line, column, errorMessage);
    exit(EXIT_FAILURE);
}

// Function to return the next token from the input
Token getNextToken() {
    if (nextTokenAvailable) {
        nextTokenAvailable = false;
        return nextToken;
    }
    Token token;
    char currentChar = getNextChar();

    // Set the token's line and column
    token.line = currentLine;
    token.column = currentColumn - 1;

    // Skipping whitespace
    while (isspace(currentChar) || currentChar == ';') {
        if (currentChar == ';') {
            // Skip the entire line (until newline)
            while (currentChar != '\n' && currentChar != EOF) {
                currentChar = getNextChar();
            }
        }
        if (currentChar == EOF) {
            token.type = TOKEN_EOF;
            return token;
        }
        currentChar = getNextChar();
    }

    // Handling unexpected characters
    if (!isalnum(currentChar) && currentChar != '+' && currentChar != '-' && currentChar != EOF) {
        reportError("Unexpected character", token.line, token.column);
    }
    // Handling EOF (End of File or End of Input)
    if (currentChar == EOF) {
        token.type = TOKEN_EOF;
        return token;
    }

    // Handling numbers (integers and floats)
    if (isdigit(currentChar)) {
        char numBuffer[64];  // Buffer to store the number string
        int i = 0;
        bool isFloat = false;

        do {
            numBuffer[i++] = currentChar;
            currentChar = getNextChar();
            if (currentChar == '.') {
                isFloat = true;
            }
        } while (isdigit(currentChar) || (currentChar == '.' && !isFloat));

        numBuffer[i] = '\0';  // Null-terminate the string

        if (isFloat) {
            token.type = TOKEN_FLOAT;
            token.floatValue = atof(numBuffer);
        } else {
            token.type = TOKEN_INT;
            token.intValue = atoi(numBuffer);
        }

        return token;
    }

    // Handling math operators
    switch (currentChar) {
        case '+':
            token.type = TOKEN_PLUS;
            return token;
        case '-':
            token.type = TOKEN_MINUS;
            return token;
        case '*':
            token.type = TOKEN_MULTIPLY;
            return token;
        case '/':
            token.type = TOKEN_DIVIDE;
    }

    // Handling identifiers (example: variable names, function names)
    if (isalpha(currentChar)) {
        char strBuffer[BUFFER_SIZE];
        int i = 0;

        do {
            strBuffer[i++] = currentChar;
            if (i >= BUFFER_SIZE) {
                reportError("Identifier too long", currentLine, currentColumn);
            }
            currentChar = getNextChar();
        } while (isalnum(currentChar) || currentChar == '_');

        strBuffer[i] = '\0'; // Null-terminate the string

        // Check for specific keywords
        if (strcmp(strBuffer, "lat") == 0) {
            token.type = TOKEN_LAT;
        } else {
            // Store the string value in the token for general identifiers
            token.type = TOKEN_IDENTIFIER;
            token.stringValue = strdup(strBuffer);
        }

        return token;
    }

    // Handling string literals for map keys
    if (currentChar == '"') {
        char strBuffer[BUFFER_SIZE];
        int i = 0;
        currentChar = getNextChar(); // Skip the opening quote

        while (currentChar != '"' && currentChar != EOF) {
            if (i >= BUFFER_SIZE - 1) {
                reportError("String literal too long", currentLine, currentColumn);
            }
            strBuffer[i++] = currentChar;
            currentChar = getNextChar();
        }
    

        if (currentChar != '"') {
            reportError("Unterminated string literal", currentLine, currentColumn);
        }

        strBuffer[i] = '\0'; // Null-terminate the string
        token.type = TOKEN_STRING; // Define TOKEN_STRING in your TokenType enum
        token.stringValue = strdup(strBuffer);
        return token;

    }

    // If no token is recognized, return an error or a special token type
    reportError("Unrecognized token", currentLine, currentColumn);
}

// Peek next token without consuming
Token peekNextToken() {
    if (!nextTokenAvailable) {
        nextToken = getNextToken();
        nextTokenAvailable = true;
    }
    return nextToken;
}

//====================
// Parser Functions
//====================
// Handles what token to expect
Token expectToken(TokenType expectedType) {
    Token token = getNextToken();
    if (token.type != expectedType) {
        reportError("Expected different token type", token.line, token.column);
    }
    return token;
}

// Create an identifier node
ASTNode* createIdentifierNode(char* identifier) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    node->type = NODE_IDENTIFIER;
    node->identifierValue = strdup(identifier); // Store the identifier

    return node;
}

// Parse a parameter list
ASTNode** parseParameterList() {
    ASTNode** parameters = NULL;
    int paramCount = 0;
    Token token = getNextToken();

    while (token.type != TOKEN_RPAREN) {
        if (token.type != TOKEN_IDENTIFIER) {
            reportError("Expected parameter identifier", token.line, token.column);
        }

        // Create a new parameter node
        ASTNode* paramNode = createIdentifierNode(token.stringValue);
        
        // Add to parameters array
        parameters = (ASTNode**)realloc(parameters, sizeof(ASTNode) * (paramCount + 1));
        parameters[paramCount++] = paramNode;

        token = getNextToken();
    }

    // Mark the end of the parameters array
    parameters = (ASTNode**)realloc(parameters, sizeof(ASTNode*) * (paramCount + 1));
    parameters[paramCount] = NULL;

    return parameters;
}

// Create a function definition node
ASTNode* createFunctionDefNode(char* functionName, ASTNode** parameters, ASTNode* body) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    node->type = NODE_FUNCTION_DEFINITION;
    node->functionCall.functionName = strdup(functionName); // Copy the function name
    node->functionCall.arguments = parameters;
    node->functionCall.body = body;

    return node;
}

ASTNode* createMapDeclNode(TokenType mapType, char* mapName) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    // ... Error checking and memory allocation ...

    node->type = (mapType == TOKEN_LAT) ? NODE_LAT_DECL : NODE_ULAT_DECL;
    node->mapName = strdup(mapName);

    return node;
}

// Parse lat/ulat (maps) declarations
ASTNode* parseMapDeclaration(TokenType mapType) {
    // Expecting map keyword (lat or ulat)
    expectToken(mapType);

    // Expecting map name (identifier)
    Token nameToken = expectToken(TOKEN_IDENTIFIER);
    char* mapName = nameToken.stringValue;

    // Construct map declaration node
    ASTNode* mapDeclNode = createMapDeclNode(mapType, mapName);

    return mapDeclNode;
}

// Parse an expression
ASTNode* parseExpression() {
    Token token = getNextToken();
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));

    if (!node) {
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    switch (token.type) {
        case TOKEN_INT:
            node->type = NODE_INT_LITERAL;
            node->intValue = token.intValue;
            break;
        case TOKEN_PLUS:
            node->type = NODE_BINARY_OP;
            node->binaryOp.left = parseExpression(); // Recursive parsing for left operand
            node->binaryOp.right = parseExpression(); // Recursive parsing for right operand
            node->binaryOp.op = TOKEN_PLUS;
            break;
        //
        default:
            free(node);
            reportError("Unexpected token in expression", token.line, token.column);
    }

    return node;
}

ASTNode* createMapOpNode(char* mapName, ASTNode* key, ASTNode* value, TokenType opType) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));

    node->type = NODE_MAP_OPERATION;
    node->mapOp.mapName = strdup(mapName);
    node->mapOp.key = key;
    node->mapOp.value = value;
    node->mapOp.operation = opType;

    return node;
}

ASTNode* createMapDelNode(char* mapName) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));

    node->type = NODE_MAP_DELETION;
    node->mapOp.mapName = strdup(mapName);
    node->mapOp.key = NULL;
    node->mapOp.value = NULL;

    return node;
}

ASTNode* createMapReflectionNode(char* mapName) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    node->type = NODE_MAP_REFLECTION;
    node->mapName = strdup(mapName);

    return node;
}

// Parse lat/ulat (maps) operations
ASTNode* parseMapOperation() {
    // Assuming the current token is the map name (identifier)
    char* mapName = currentToken.stringValue;

    // Expecting '.insert'
    expectToken(TOKEN_DOT);
    expectToken(TOKEN_INSERT);

    // Capture the operation token
    Token operationToken = getNextToken();

    if (operationToken.type == TOKEN_INSERT) {
        // Parse key and value for insert operation
        ASTNode* keyNode = parseExpression();
        ASTNode* valueNode = parseExpression();
        return createMapOpNode(mapName, keyNode, valueNode, operationToken.type);
    } else if (operationToken.type == TOKEN_SPIRALOUT) {
        return createMapDelNode(mapName);
    } else if (operationToken.type == TOKEN_REFLECTION) {
        return createMapReflectionNode(mapName);
    }else {
        reportError("Expected map operation, like '.insert' or '.spiralOut'", operationToken.line, operationToken.column);
    }
}

ASTNode* createCompoundStatementNode(ASTNode** statements, int statementCount) {
    CompoundStatementNode* compoundNode = (CompoundStatementNode*)malloc(sizeof(CompoundStatementNode));
    if (!compoundNode) {
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    compoundNode->type = NODE_COMPOUND_STATEMENT;
    compoundNode->statements = statements;
    compoundNode->statementCount = statementCount;

    // Wrapping in ASTNode
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        free(compoundNode); // Free previously allocated memory
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    node->type = NODE_COMPOUND_STATEMENT;
    node->compound = compoundNode;

    return node;
}

ASTNode* parseBody() {
    ASTNode** bodyNodes = NULL;
    int nodeCount = 0;
    Token token = peekNextToken(); // Function to look at the next token without consuming it

    while (token.type != TOKEN_RPAREN && token.type != TOKEN_EOF) {
        // Parse an individual statement or expression
        ASTNode* stmt = parseExpression();

        // Add to body nodes array
        bodyNodes = (ASTNode**)realloc(bodyNodes, sizeof(ASTNode*) * (nodeCount + 1));
        bodyNodes[nodeCount++] = stmt;

        token = peekNextToken();
    }

    // Mark the end of the body nodes array
    bodyNodes = (ASTNode**)realloc(bodyNodes, sizeof(ASTNode*) * (nodeCount + 1));
    bodyNodes[nodeCount] = NULL;

    // Create a compound statement node if needed or return the array directly
    return createCompoundStatementNode(bodyNodes, nodeCount);
}

ASTNode* createSpiralNode(int iterations, ASTNode* body) {
    SpiralNode* spiralNode = (SpiralNode*)malloc(sizeof(SpiralNode));
    if (!spiralNode) {
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    spiralNode->type = NODE_SPIRAL;
    spiralNode->iterations = iterations;
    spiralNode->currentIteration = 0;
    spiralNode->body = body;

    // Wrapping in ASTNode
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        free(spiralNode); // Free previously allocated memory
        reportError("Memory allocation failed", currentLine, currentColumn);
    }

    node->type = NODE_SPIRAL;
    node->spiral = spiralNode;

    return node;
}

// Parse spirals (loops)
ASTNode* parseSpiral() {
    // Expecting spiral keyword
    expectToken(TOKEN_SPIRAL);

    // Parse the iteration count
    Token countToken = expectToken(TOKEN_INT);
    int iterations = countToken.intValue;

    // Parse the body of the loop
    ASTNode* body = parseBody();

    // Construct the spiral AST node
    ASTNode* spiralNode = createSpiralNode(iterations, body);

    return spiralNode;
}

// Add functions to global lookup table
void addFunctionToTable(const char* name, ASTNode* body) {
    if (functionCount >= MAX_FUNCTIONS) {
        fprintf(stderr, "Function table overflow\n");
        return;
    }
    functionTable[functionCount].name = strdup(name);
    functionTable[functionCount].body = body;
    functionCount++;
}

// Find a function in the global lookup table
ASTNode* lookupFunction(const char* functionName) {
    for (int i = 0; i < functionCount; ++i) {
        if (strcmp(functionTable[i].name, functionName) == 0) {
            return functionTable[i].body;
        }
    }
    return NULL; // Function not found
}

// Parse a parabol (function)
ASTNode* parseParabol() {
    Token token = getNextToken();

    // Expecting an opening parenthesis
    expectToken(TOKEN_LPAREN);
    if (token.type != TOKEN_LPAREN) {
        reportError("Expected '(' after 'parabol'", token.line, token.column);
    }

    // Expecting function name (identifier)
    token = getNextToken();
    if (token.type != TOKEN_IDENTIFIER) {
        reportError("Expected function name after '('", token.line, token.column);
    }
    char* functionName = token.stringValue; // Store the function name

    // Parse parameters
    ASTNode** parameters = parseParameterList();

    // Parse the function body
    ASTNode* body = parseExpression();

    // Construct the function definition AST node
    ASTNode* functionDef = createFunctionDefNode(functionName, parameters, body);

    // Expecting a closing parenthesis at the end
    token = getNextToken();
    if (token.type != TOKEN_RPAREN) {
        reportError("Expected ')' at end of function definition", token.line, token.column);
    }

    return functionDef;
}

//========================
// Helper Void Functions
//========================
void processToken(Token token) {
    // Example processing based on token type
    switch (token.type) {
        case TOKEN_INT:
            // Handle integer token
            printf("Integer: %d\n", token.intValue);
            break;
        case TOKEN_PLUS:
            // Handle plus token
            printf("Plus operator\n");
            break;
        // ... other cases ...
        default:
            printf("Unrecognized token\n");
            break;
    }
}

void processBuffer(char* buffer) {
    currentPosition = buffer; // Set the currentPosition to the start of the buffer

    while (*currentPosition != '\0') { // Continue until end of buffer
        Token token = getNextToken();

        // Process the token
        // Depending on your design, you might add tokens to a list, or directly parse them, etc.
        processToken(token);

        if (token.type == TOKEN_EOF) {
            break; // If EOF token is encountered, stop processing
        }
    }
}

// Read source code from an input file
void readSourceCode(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[READ_BUFFER_SIZE];
    while (fgets(buffer, READ_BUFFER_SIZE, file) != NULL) {
        // Process the buffer - pass it to your lexer
        processBuffer(buffer);
    }

    fclose(file);
}

void executeAST(ASTNode* node); // Forward declaration

void printMap(const char* mapName) {
    // Find the map in the global symbol table
    for (int i = 0; i < globalSymbolTable.size; ++i) {
        if (strcmp(globalSymbolTable.symbols[i].name, mapName) == 0) {
            Map* map = globalSymbolTable.symbols[i].map;
            if (map) {
                // Iterate through the map and print each key-value pair
                for (int j = 0; j < map->size; ++j) {
                    printf("Key: %s, Value: ", map->entries[j].key);
                    // If the value is a simple type, print directly
                    if (map->entries[j].value->type == NODE_INT_LITERAL) {
                        printf("%d\n", map->entries[j].value->intValue);
                    } else {
            
                    }
                }
            } else {
                printf("Map '%s' is empty or does not exist\n", mapName);
            }
            return;
        }
    }
    printf("Map '%s' not found\n", mapName);
}

void executeMapOperation(ASTNode* node) {
    if (!node || node->type != NODE_MAP_OPERATION) {
        fprintf(stderr, "Invalid node for map operation\n");
        return;
    }

    char* mapName = node->mapOp.mapName;
    TokenType operationType = node->mapOp.operation;

    // Find the map in the global symbol table
    Map* map = NULL;
    for (int i = 0; i < globalSymbolTable.size; ++i) {
        if (strcmp(globalSymbolTable.symbols[i].name, mapName) == 0) {
            map = globalSymbolTable.symbols[i].map;
            break;
        }
    }
    if (!map) {
        fprintf(stderr, "Map '%s' not found\n", mapName);
        return;
    }

    switch (operationType) {
        case TOKEN_INSERT: {
            // Insert operation
            char* key = node->mapOp.key->identifierValue;

            // Execute the value node
            executeAST(node->mapOp.value);

            // Insert from value node
            int value = 0;
            if (node->mapOp.value->type == NODE_INT_LITERAL) {
                value = node->mapOp.value->intValue;
            } else {
                fprintf(stderr, "Value for map insert is not an integer literal\n");
                return;
            }

            // Check for existing key
            for (int i = 0; i < map->size; ++i) {
                if (strcmp(map->entries[i].key, key) == 0) {
                    fprintf(stderr, "Key '%s' already exists in map '%s'\n", key, mapName);
                    return;
                }
            }

            // Insert key-value pair
            if (map->size < MAX_MAP_SIZE) {
                map->entries[map->size].key = strdup(key);
                map->entries[map->size].value = node->mapOp.value; // Assuming value is an integer
                map->size++;
            } else {
                fprintf(stderr, "Map '%s' is full\n", mapName);
            }
            break;
        }
        case TOKEN_SPIRALOUT: {
            // Delete (spiral out) operation
            // Simply reset the size to 0
            map->size = 0;
            break;
        }
        case TOKEN_REFLECTION: {
            // Reflection operation
            printMap(mapName);
            break;
        }
        default:
            fprintf(stderr, "Unhandled map operation\n");
            break;
    }
}

// AST Execution Function
void executeAST(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case NODE_INT_LITERAL:
            // Handle integer literal
            node->evaluatedValue = node->intValue;
            printf("Integer: %d\n", node->intValue);
            break;

        case NODE_BINARY_OP:
            // Recursively execute the left and right operands
            int leftValue = 0, rightValue = 0;

            // Ensure that the operands are evaluated and typecasted correctly
            if (node->binaryOp.left && node->binaryOp.left->type == NODE_INT_LITERAL) {
                leftValue = node->binaryOp.left->intValue;
            } else {
                executeAST(node->binaryOp.left);
            }

            if (node->binaryOp.right && node->binaryOp.right->type == NODE_INT_LITERAL) {
                rightValue = node->binaryOp.right->intValue;
            } else {
                executeAST(node->binaryOp.right);
            }

            // Apply the operator
            switch (node->binaryOp.op) {
                case TOKEN_PLUS:
                    node->evaluatedValue = leftValue + rightValue;
                    printf("%d\n", leftValue + rightValue);
                    break;
                case TOKEN_MINUS:
                    node->evaluatedValue = leftValue - rightValue;
                    printf("%d\n", leftValue - rightValue);
                    break;
                case TOKEN_MULTIPLY:
                    node->evaluatedValue = leftValue * rightValue;
                    printf("%d\n", leftValue * rightValue);
                    break;
                case TOKEN_DIVIDE:
                    if (rightValue == 0) {
                        fprintf(stderr, "Division by zero\n");
                    } else {
                        node->evaluatedValue = leftValue / rightValue;
                        printf("%d\n", leftValue / rightValue);
                    }
                    break;
                default:
                    fprintf(stderr, "Unhandled binary operator\n");
            }
            break;
        case NODE_FUNCTION_DEFINITION:
            // Execute the body of the function
            if (node->functionCall.body) {
                executeAST(node->functionCall.body);
            }
            break;
        
        case NODE_MAP_REFLECTION:
            if (node->mapName) {
                printMap(node->mapName);
            }
            break;
        
        case NODE_COMPOUND_STATEMENT:
            // Execute each statement in the compound statement
            for (int i = 0; i < node->compound->statementCount; ++i) {
                executeAST(node->compound->statements[i]);
            }
            break;
        
        case NODE_FUNCTION_CALL:
            // Handle function call
            // Retrieve the function name
            char* functionName = node->functionCall.functionName;

            // Evaluate arguments
            int* argumentValues = (int*)malloc(sizeof(int) * node->functionCall.argCount);
            for (int i = 0; i < node->functionCall.argCount; ++i) {
                executeAST(node->functionCall.arguments[i]);
                argumentValues[i] = node->functionCall.arguments[i]->evaluatedValue;
            }
            ASTNode* functionBody = lookupFunction(functionName);
            if (functionBody) {
                executeAST(functionBody);
            } else {
                fprintf(stderr, "Function not found\n");
            }

            // Free the allocated memory for arguments
            free(argumentValues);
            break;

        case NODE_LAT_DECL:
        case NODE_ULAT_DECL:
            Map* newMap = createNewMap();
            storeMap(node->mapName, newMap);
            break;

        case NODE_MAP_OPERATION:
            // Handle map operations (insert, delete, reflection)
            executeMapOperation(node);
            break;

        case NODE_SPIRAL:
            // Handle spiral (loop)
            for (int i = 0; i < node->spiral->iterations; ++i) {
                executeAST(node->spiral->body);
            }
            break;

        default:
            fprintf(stderr, "Unhandled node type in executeAST\n");
            break;
    }
}

//=============
// Main
//=============
int main(int argc, char** argv) {
    // Initialization and starting point of your interpreter
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Lexer and Parser Logic
    readSourceCode(argv[1]);
    fclose(inputFile);
    return EXIT_SUCCESS;
}

//===================
// Example Programs
//===================

/*
// Tokenize from input into a map
; easy.txt
lat tokens;

; Insert some tokens into the map 'tokens'
tokens.insert("intToken", 123);
tokens.insert("floatToken", 45.67);
tokens.insert("plusToken", "+");
tokens.insert("minusToken", "-");

; Print the content of the map 'tokens'
tokens.reflection;



// Insert Fibonacci numbers into a map
; test.txt
lat fib;

; Initialize the first two elements of the Fibonacci sequence
fib.insert("black", 0); ; 0th Fibonacci number
fib.insert("then", 1); ; 1st Fibonacci number

parabol (lateralus) (
    spiral (7) (
        ; Use the currentIteration of the spiral as the iterator
        int a = 0;
        int b = 1;
        if (currentIteration > 1) {
            a = fib[previousKey]; ; Retrieve the previous Fibonacci number
            b = fib[currentKey]; ; Retrieve the one before the previous Fibonacci number
        }

        ; Determine the current key based on the iteration
        char* currentKey = (currentIteration == 0) ? "black" :
                           (currentIteration == 1) ? "then" :
                           (currentIteration == 2) ? "white" :
                           (currentIteration == 3) ? "are" :
                           (currentIteration == 4) ? "all" :
                           (currentIteration == 5) ? "I" :
                           "see";

        ; Determine the previous key for the next iteration
        char* previousKey = currentKey;

        ; Insert the new Fibonacci number with the corresponding string key
        fib.insert(currentKey, a + b);
    )
    fib.reflection; ; Print all elements in the map
)

lateralus();

fib.spiralOut; ; Delete the entire map
*/