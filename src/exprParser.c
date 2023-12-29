/* ReSolve v0.11.09h 2023/12/29 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* exprParser.c is part of ReSolve
   ReSolve is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ReSolve is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ReSolve. If not, see <http://www.gnu.org/licenses/>.          */

/* exprParser.c recursive descent parser recognizing variables */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define DELIMITER 1
#define VARIABLE  2
#define NUMBER    3

#include "comType.h"
#include "exprParser.h" /* pubblic variables and functions */

// public variables:
/* put values in exprVarsParser[]: a=[0], b=[1], ..., z=[25],
then call evalExprParser() passing the expression containing letters A-Z
return the result. Supported: numbers, variables, operators +,-,*,/,%,^,(,) */
double exprVarsParser[26] = { /* 26 user variables, A-Z */
                              0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 0.0
                            };

// public functions:
double evalExprParser(char* expressionPtr);

// private variables:
static char* prog; /* points to the expression to be analyzed */
static char token[80];
static char tokType;

// private functions:
void serror(int error);
int isdelim(char c);
void getToken(void);
void putBack(void);
double findVar(char* s);
void atom(double* answer);
void putBack(void);
void evalExp6(double* answer);
void evalExp5(double* answer); // evalExp6();
void evalExp4(double* answer);
void evalExp3(double* answer); // evalExp4();
void evalExp2(double* answer);
void evalExp1(double* result);
void evalExp0(double* answer); // evalExp2();

/* Display a syntax error */
void serror(int error) {
   static char* e[]= {
         "Formula: Syntax Error",
         "Formula: Unbalanced Parentheses",
         "Formula: No Expression Present",
         "Formula: Division by Zero",
         "Formula: Null pointer"
   };
   printf("\n%s\n", e[error]);
   exit(ERROR);
} // serror()

/* Return true if c is a delimiter */
int isdelim(char c) {
   if (strchr (" +-/*%^=()", c) || c==9 || c=='\r' || c==0)
      return 1; // true
   return 0; // false
} // isdelim()

/* Return the next token */
void getToken(void) {
   char* temp;
   tokType = 0;
   temp = token;
   *temp = '\0';
   if (!*prog) return; /* at end of expression */
   while (isspace((u08)*prog)) ++prog; /* skip over white space */
   if (strchr("+-*/%^=()", *prog)) {
      tokType = DELIMITER;
      /* advance to next char */
      *temp++ = *prog++;
   }
   else if (isalpha((u08)*prog)) {
      while (!isdelim(*prog)) *temp++ = *prog++;
      tokType = VARIABLE;
   }
   else if (isdigit((u08)*prog)) {
      while (!isdelim(*prog)) *temp++ = *prog++;
      tokType = NUMBER;
   }
   *temp = '\0';
} // getToken()

/* Return the value of a variable */
double findVar(char* s) {
   if (!isalpha((u08)*s)) {
      serror(1);
      return 0.0;
   }
   return exprVarsParser[toupper((u08)*token)-'A'];
} // findVar()

/* Get the value of a number or a variable */
void atom(double* answer) {
   switch (tokType) {
      case VARIABLE:
         *answer = findVar(token);
         getToken();
         return;
      case NUMBER:
         *answer = atof(token);
         getToken();
         return;
      default:
         serror (0);
   }
} // atom()

/* Return a token to the input stream */
void putBack(void) {
   char* t;
   t = token;
   for (; *t; t++) prog--;
} // putBack()

/* Process a parenthesized expression */
void evalExp6(double* answer) {
   if (*token == '(') {
      getToken();
      evalExp2(answer);
      if (*token != ')')
         serror (1);
      getToken();
   }
   else atom(answer);
} // evalExp6()

/* Evaluate an unary + or - */
void evalExp5(double* answer) {
   char op;
   op = 0;
   if ( ((tokType == DELIMITER) && *token=='+') || (*token == '-') ) {
      op = *token;
      getToken();
   }
   evalExp6(answer);
   if (op == '-') *answer = -(*answer);
} // evalExp5()

/* Process an exponent */
void evalExp4(double* answer) {
   double temp, ex;
   int t;
   evalExp5(answer);
   if (*token == '^') {
      getToken();
      evalExp4(&temp);
      ex = *answer;
      if (temp==0.0) {
         *answer = 1.0;
         return;
      }
      for (t=(int)temp-1; t>0; --t) *answer = (*answer) * (double)ex;
          /* ^^^^^ Visual C++ require an explicit cast */
   }
} // evalExp4()

/* Multiply or divide two factors */
void evalExp3(double* answer) {
   char op;
   double temp;
   evalExp4(answer);
   while ((op = *token) == '*' || op == '/' || op == '%') {
      getToken();
      evalExp4(&temp);
      switch (op) {
         case '*':
            *answer = *answer * temp;
            break;
         case '/':
            if (temp == 0.0) {
               serror(3); /* division by zero */
               *answer = 0.0;
            } else *answer = *answer / temp;
            break;
         case '%':
            if (temp == 0.0) {
               serror(3); /* division by zero */
               *answer = 0.0;
            } else *answer = (int) *answer % (int) temp;
            break;
      }
   }
} // evalExp3()

/* Add or subtract two terms */
void evalExp2(double* answer) {
   char op;
   double temp;
   evalExp3(answer);
   while ((op = *token) == '+' || op == '-') {
      getToken();
      evalExp3(&temp);
      switch (op) {
         case '-':
            *answer = *answer - temp;
            break;
         case '+':
            *answer = *answer + temp;
            break;
      }
   }
} // evalExp2()

/* Process an assignment */
void evalExp1(double* answer) {
   int  slot;
   char tempTokType;
   char tempToken[80];
   if (tokType == VARIABLE) {
      /* save old token */
      strcpy(tempToken, token);
      tempTokType = tokType;
      /* compute the index of the variable */
      slot = toupper((u08)*token) - 'A';
      getToken();
      if (*token != '=') {
         putBack(); /* return current token */
         /* restore old token - not assignment */
         strcpy(token, tempToken);
         tokType = tempTokType;
      }
      else {
         getToken(); /* get next part of exp */
         evalExp2(answer);
         exprVarsParser[slot] = *answer;
         return;
      }
   }
   evalExp2(answer);
} // evalExp1()

/* Parser entry point */
void evalExp0(double* answer) {
   getToken();
   if (!*token) {
      serror(2);
      return;
   }
   evalExp1(answer);
   if (*token) serror(0); /* last token must be null */
} // evalExp0()

/* expression parser public interface */
double evalExprParser(char* expressionPtr) {
   double val;
   if (expressionPtr != NULL) prog = expressionPtr;
   else { /* protect getToken from 'prog' pointer to NULL */
      serror(4);
   }
   evalExp0(&val);
   return val;
} // evalExprParser()
