//Justin Chen
//CSE 340

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

using namespace std;

extern "C"  {
  #include "compiler.h"
}

#define debug 0 //1 for debug 0 for off
vector<ValueNode*> total_id_list; // list of all id declarations

/*            Function declaration            */
void parse_id_section();
int id_search(char*);
StatementNode* parse_body();
StatementNode* parse_stmt_list();
StatementNode* parse_stmt();
StatementNode* parse_assign_stmt();
StatementNode* print_da_id();
StatementNode* parse_if_stmt();
StatementNode* parse_while_stmt();
StatementNode* parse_switch_stmt();
StatementNode* parse_switchBody(ValueNode*, StatementNode*);
/*        End of Function declaration         */

/*      Parsing, storing, and accessing id_list   */
int id_search(char* input) {
  char* search = input;
  int pos = -1; // can't find
  for(int i = 0; i < total_id_list.size(); i++) {
    if(strcmp(total_id_list[i]->name, search) == 0 ) {
      pos = i;  // returns the position in the id_list of the var you're searching for
      return pos;
    }
  }
  return pos;
}

void parse_id_section()  {  // parses and creates the global id list
  ttype = getToken();
  if(ttype == ID) {

    ValueNode* val = new ValueNode;
    val->name = strdup(token);
    val->value = 0;

    total_id_list.push_back(val);

    ttype = getToken();
    if(ttype == COMMA)  {
      parse_id_section();
    }
    else if(ttype == SEMICOLON) {
      return;
    }
  }
}

void print_id_list()  { // Prints the ID_list
  cout << "ID_List: \n";
  for(int i = 0; i < total_id_list.size(); i++)
  {
    cout << total_id_list[i]->name << ": " << total_id_list[i]->value << "\n";
  }
  cout << "\n";
}
/*        End of id_list methods         */

StatementNode* parse_assign_stmt()  {
  StatementNode* new_statementnode = new StatementNode;
  AssignmentStatement* new_assign_stmt = new AssignmentStatement;
  new_statementnode->type = ASSIGN_STMT;
  new_assign_stmt->op = 0;
  int position = id_search((char*)token);
  if(position != -1) { //found ID
    ValueNode* assign_id = total_id_list[position];
    new_assign_stmt->left_hand_side = assign_id;
    ttype = getToken();
    if(ttype == EQUAL)  {

      /*    sets the first operand    */
      ttype = getToken(); // either going to be primary or expr
      int position2 = id_search((char*)token);
      if(position2 != -1)  {  // the first assign node is an id
        ValueNode* assign_id2 = total_id_list[position2];
        new_assign_stmt->operand1 = assign_id2; // puts the 2nd id into operand1
      } else  { // ttype = NUM
        ValueNode* val1 = new ValueNode;
        val1->name = NULL;
        val1->value = atoi(token);
        new_assign_stmt->operand1 = val1;
      }

      /*    Determines if it is a Primay or Expr assignment    */
      ttype = getToken();

      if(ttype == SEMICOLON)  {
        new_statementnode->assign_stmt = new_assign_stmt;
        return new_statementnode; //Assignment with primary, RETURNS PRIMARY
      } else {
        new_assign_stmt->op = ttype;  //assignment with expr
      }

      /*    sets the second operand    */
      ttype = getToken(); // get second operand
      int position3 = id_search((char*)token);
      if(position3 != -1)  {  // the first assign node is an id
        ValueNode* assign_id3 = total_id_list[position3];
        new_assign_stmt->operand2 = assign_id3; // puts the 2nd id into operand1
      } else  {
        ValueNode* val2 = new ValueNode;
        val2->name = NULL;
        val2->value = atoi(token);
        new_assign_stmt->operand2 = val2;
      }
      ttype = getToken(); //SHOULD BE A SEMICOLON

      if(ttype == SEMICOLON)  { //SEMICOLON after expr
        new_statementnode->assign_stmt = new_assign_stmt;
        return new_statementnode; // Assignement with expr, RETURNS EXPR
      }
    }
  }
}

StatementNode* print_da_id() { // prints the value of the ID
  if (debug) cout << "\tPRINT: " << ttype << "\n";
  StatementNode* new_statementPrint = new StatementNode;
  PrintStatement* print_id_DUCK = new PrintStatement;
  new_statementPrint->type = PRINT_STMT;
  ttype = getToken();
  if (debug) cout << "\tID: " << ttype << "\n";
  int index = id_search((char*)token);
  if(index != -1) {
    ValueNode* foundID = new ValueNode;
    foundID = total_id_list[index];
    print_id_DUCK->id = total_id_list[index];
  }
  ttype = getToken();
  if(ttype == SEMICOLON)  {
    new_statementPrint->print_stmt = print_id_DUCK;
    return new_statementPrint;
  }
}

StatementNode* parse_if_stmt()  {
  StatementNode* if_returner = new StatementNode;
  StatementNode* if_body = new StatementNode; //if_body->true_branch & false_branch
  IfStatement* if_total = new IfStatement;

   ttype = getToken();
   if (debug) cout << "Operand1: " << ttype << "\n";
   if(ttype == ID ) {
     int idHold = id_search((char*)token); //id
     if_total->condition_operand1 = total_id_list[idHold];
   } else  { // or num
     ValueNode* numHold = new ValueNode;
     numHold->value = atoi(token);
     if_total->condition_operand1 = numHold;
   }

   ttype = getToken();
   if (debug) cout << "Op: " << ttype << "\n";
   if(ttype == GREATER || ttype == LESS || ttype == NOTEQUAL)  {
     if_total->condition_op = ttype;
   }

   ttype = getToken();
   if (debug) cout << "Operand2: " << ttype << "\n";
   if(ttype == ID) {
     int idHold2 = id_search((char*)token);  // id
     if_total->condition_operand2 = total_id_list[idHold2];
   } else  { // or num
     ValueNode* numHold2 = new ValueNode;
     numHold2->value = atoi(token);
     if_total->condition_operand2 = numHold2;
   }

    if_body = parse_body();
    if_total->true_branch = if_body;
    StatementNode* head = if_total->true_branch;

    while (head->next != NULL)  {
      head = head->next;
    }
    StatementNode* noopStuff = new StatementNode;
    noopStuff->type = NOOP_STMT;
    head->next = noopStuff;
    if_total->false_branch = new StatementNode;
    if_total->false_branch = noopStuff;

    if_returner->if_stmt = if_total;
    if_returner->type = IF_STMT;
    if_returner->next = new StatementNode;
    if_returner->next = noopStuff;

  return if_returner;
}

StatementNode* parse_while_stmt() {
  StatementNode* while_returner = new StatementNode;
  StatementNode* if_body = new StatementNode; //if_body->true_branch & false_branch
  IfStatement* if_total = new IfStatement;
  while_returner->if_stmt = if_total;

   ttype = getToken();
   if (debug) cout << "Operand1: " << ttype << "\n";
   if(ttype == ID ) {
     int idHold = id_search((char*)token); //id
     if_total->condition_operand1 = total_id_list[idHold]; //might have to make temp for this
   } else  { // or num
     ValueNode* numHold = new ValueNode;
     numHold->value = atoi(token);
     if_total->condition_operand1 = numHold; // might have to make temp for this
   }

   ttype = getToken();
   if (debug) cout << "Op: " << ttype << "\n";
   if(ttype == GREATER || ttype == LESS || ttype == NOTEQUAL)  {
     if_total->condition_op = ttype;
   }

   ttype = getToken();
   if (debug) cout << "Operand2: " << ttype << "\n";
   if(ttype == ID) {
     int idHold2 = id_search((char*)token);  // id
     if_total->condition_operand2 = total_id_list[idHold2]; //might have to make temp for this
   } else  { // or num
     ValueNode* numHold2 = new ValueNode;
     numHold2->value = atoi(token);
     if_total->condition_operand2 = numHold2; // might have to make temp for this
   }

    if_body = parse_body();
    if_total->true_branch = if_body;

    StatementNode* gtStatement = new StatementNode;
    gtStatement->type = GOTO_STMT;
    GotoStatement* goto_tar = new GotoStatement;
    gtStatement->goto_stmt = goto_tar;
    goto_tar->target = while_returner;

    StatementNode* head = if_total->true_branch;

    while (head->next != NULL)  { // traverses body to find the end
      head = head->next;
    }
    StatementNode* temp = goto_tar->target;
    head->next = temp;    //append goto to body of while

    StatementNode* noopStuff = new StatementNode;
    noopStuff->type = NOOP_STMT;
    if_total->false_branch = new StatementNode;
    if_total->false_branch = noopStuff;

    while_returner->type = IF_STMT;
    while_returner->next = new StatementNode;
    while_returner->next = noopStuff;

  return while_returner;
}

StatementNode* parse_switch_stmt()  {
  StatementNode* current_switch_stmt = new StatementNode;

  ttype = getToken(); // gets the ID following the switch
  ValueNode* switch_ID = new ValueNode;
  int pos = id_search((char*)token);

  switch_ID = total_id_list[pos]; // assigns the ID(name, val) to switch_ID

  ttype = getToken(); // skips the LBRACE ({)

    StatementNode* noop = new StatementNode;
    noop->type = NOOP_STMT;
  current_switch_stmt = parse_switchBody(switch_ID, noop);
  current_switch_stmt->next = noop;
  ttype = getToken(); // returns the RBRACE (end of switch statement })
  if(ttype == RBRACE) return current_switch_stmt;
}

StatementNode* parse_switchBody(ValueNode* input, StatementNode* noopStuff) { // PARSE THE BODY OF THE SWITCH STATEMENT
  StatementNode* switchBody = new StatementNode;  // RETURN THIS
  StatementNode* switchIfBody = new StatementNode; // into false_branch
  IfStatement* switchIfCase = new IfStatement;

  StatementNode* noodle = new StatementNode;
  noodle->type = NOOP_STMT;

  ValueNode* check = input;
  ttype = getToken(); // DEFAULT or CASE or RBRACE
  if(ttype == CASE) {
    switchBody->type = IF_STMT;
    switchBody->if_stmt = switchIfCase;
    ttype = getToken(); // # following CASE
    ValueNode* caseNum = new ValueNode;
    caseNum->value = atoi(token);
    switchIfCase->condition_operand1= check;
    switchIfCase->condition_operand2 = caseNum; // sets case num to 2nd operand
    switchIfCase->condition_op = NOTEQUAL;  // set for all switch case conditions

    ttype = getToken(); //COLON

    switchIfBody = parse_body();  // parses the body of the case starting at { ending at }
    switchIfCase->false_branch = switchIfBody;  // reverse, the NOTEQUAL == false is the statement we want to run
    StatementNode* head = switchIfCase->false_branch; // pointer to head of false_branch

    /* GoToStatement Node */
    StatementNode* Switch_gt = new StatementNode;
    Switch_gt->type = GOTO_STMT;
    GotoStatement* switch_go = new GotoStatement;
    Switch_gt->goto_stmt = switch_go;
    switch_go->target = noodle;

    switchIfCase->true_branch = new StatementNode;  // sets next of true_branch = noopStuff
    switchIfCase->true_branch = noodle;

    while (head->next != NULL)  { // goes to the end of false_branch
      head = head->next;
    }
    head->next = noopStuff;  // points the end of each false_branch to noopStuff
    switchBody->next = noodle;
    ttype = getToken();
    if(ttype == CASE || ttype == DEFAULT) {
      ungetToken(); // release the ttype
      switchBody->next->next = parse_switchBody(check, noopStuff);  // more cases or defaults
    } else  {
      ungetToken();
      switchBody->next = noopStuff;
    }
    return switchBody;

  } else if(ttype == DEFAULT) {
    ttype = getToken(); // COLON
    switchBody = parse_body();
    switchBody->next = noopStuff;
    return switchBody;
  }
}

StatementNode* parse_stmt() {
  StatementNode* current_stmt = new StatementNode;
  if(ttype == ID) { // assign
    current_stmt = parse_assign_stmt(); //assignStmtNode;

  } else if(ttype == PRINT) {
    current_stmt = print_da_id();

  } else if(ttype == IF)  {
    current_stmt = parse_if_stmt();

  } else if(ttype == WHILE) {
    current_stmt = parse_while_stmt();

  } else if(ttype == SWITCH)  {
    current_stmt = parse_switch_stmt();
  }
  return current_stmt;
}

StatementNode* parse_stmt_list()  {
  StatementNode* stmt_listData = new StatementNode;
  StatementNode* stmt_listData2 = new StatementNode;

  ttype = getToken();
  stmt_listData = parse_stmt(); //parse first statement
  ttype = getToken(); // check next line after a SEMICOLON
  if(ttype== ID || ttype == IF || ttype == WHILE || ttype == SWITCH || ttype == PRINT)  {
    ungetToken();
    stmt_listData2 = parse_stmt_list();
    if(stmt_listData->type == IF_STMT)  {
      stmt_listData->next->next = new StatementNode;
      stmt_listData->next->next = stmt_listData2;
    } else {
    stmt_listData->next = new StatementNode;
    stmt_listData->next = stmt_listData2;
    }
    return stmt_listData;
  } else {
    ungetToken(); // it's the end of the statementlist (RBRACE)
    return stmt_listData;
  }
}

StatementNode* parse_body() {
  StatementNode* bodyData = new StatementNode;
  ttype = getToken();
  if(ttype == LBRACE) bodyData = parse_stmt_list();
  ttype = getToken();
  if(ttype == RBRACE) return bodyData;
}

StatementNode* parse_generate_intermediate_representation()  {
  parse_id_section();

  StatementNode* program = new StatementNode;
  program = parse_body();

  return program;
}
