#ifndef _COMPILER_H_
#define _COMPILER_H_

/*
 * compiler.h
 */

#define TRUE 1
#define FALSE 0

enum StatementType
{
    NOOP_STMT = 1000,
    PRINT_STMT,
    ASSIGN_STMT,
    IF_STMT,
    GOTO_STMT
};

#define KEYWORDS    8
#define RESERVED    28
#define VAR         1
#define IF          2
#define WHILE       3
#define SWITCH      4
#define CASE        5
#define DEFAULT     6
#define PRINT       7
#define ARRAY       8
#define PLUS        9
#define MINUS       10
#define DIV         11
#define MULT        12
#define EQUAL       13
#define COLON       14
#define COMMA       15
#define SEMICOLON   16
#define LBRAC       17
#define RBRAC       18
#define LPAREN      19
#define RPAREN      20
#define LBRACE      21
#define RBRACE      22
#define NOTEQUAL    23
#define GREATER     24
#define LESS        25
#define ID          26
#define NUM         27
#define ERROR       28

// This implementation does not allow tokens
// that are more than 200 characters long
#define MAX_TOKEN_LENGTH 200

// The following global variables are defined in compiler.c:
extern char token[MAX_TOKEN_LENGTH];
extern int  ttype;

//---------------------------------------------------------
// Data structures:

struct ValueNode
{
    char* name;
    int   value;
};

struct GotoStatement
{
    struct StatementNode* target;
};

struct AssignmentStatement
{
    struct ValueNode* left_hand_side;

    struct ValueNode* operand1;
    struct ValueNode* operand2;

    /*
     * If op == 0 then only operand1 is meaningful.
     * Otherwise op has to be one of the following values:
     * PLUS, MINUS, MULT, DIV
     * and both operands are meaningful
     */
    int op;
};

struct PrintStatement
{
    struct ValueNode* id;
};

struct IfStatement
{
    struct ValueNode* condition_operand1;
    struct ValueNode* condition_operand2;

    /*
     * condition_op should be one of the following values:
     * GREATER, LESS, NOTEQUAL
     */
    int condition_op;

    struct StatementNode* true_branch;
    struct StatementNode* false_branch;
};

struct StatementNode
{
    enum StatementType type;

    union
    {
        struct AssignmentStatement* assign_stmt;
        struct PrintStatement* print_stmt;
        struct IfStatement* if_stmt;
        struct GotoStatement* goto_stmt;
    };

    struct StatementNode* next; // next statement in the list or NULL
};

//---------------------------------------------------------
// Functions that are provided:

void debug(const char* format, ...);

int  getToken();
void ungetToken();

//---------------------------------------------------------
// Functions that you should write:

struct StatementNode* parse_generate_intermediate_representation();

/*
  NOTE:

  You need to write a function with the above signature. This function
  is supposed to parse the input program and generate an intermediate
  representation for it. The output of this function is passed to the
  execute_program function in main().

  Write your code in a separate file and include this header file (compiler.h)
  in your code as described below.

  A) If you are coding in C,

     Include compiler.h in your code like this:

     #include "compiler.h"

     Compile using the following command:

     gcc compiler.c yourcode.c

  B) If you are coding in C++,

     Include compiler.h in your code like this:

     extern "C" {
         #include "compiler.h"
     }

     Use the following commands to compile your code:

     gcc -c compiler.c
     g++ yourcode.cpp compiler.o

*/

#endif /* _COMPILER_H_ */
