/* ReSolve V0.09.09h 2023/03/12 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* exprParser.h is part of ReSolve
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

/* exprParser.h interfaces to recursive descent parser recognizing variables*/

#ifndef _INCexprParserh
#define _INCexprParserh

// public variables:
/* put values in exprVarsParser[]: a=[0], b=[1], ..., z=[25],
then call evalExprParser() passing the expression containing letters A-Z
return the result. Supported: numbers, variables, operators +,-,*,/,%,^,(,) */
extern double exprVarsParser[26]; /* 26 input user variables, case insensitive A-Z */

// public functions:
double evalExprParser(char* expressionPtr); /* calculate an expression */

#endif /* _INCexprParserh */
