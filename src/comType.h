/* ReSolve v0.10.09h 2023/04/05 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* comType.h is part of ReSolve
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

/* comType.h interface to reference common type and data. */

#ifndef _INCcomTypeh
#define _INCcomTypeh

#include "stdbool.h"

#define OK            0
#define ERROR        -1
#define WARN         -2
#define DEBUG        -3

#define PRINTOFF      0
#define PRINTERROR    1
#define PRINTWARN     2
#define PRINTBATCH    3
#define PRINTF        4
#define PRINTDEBUG    5
#define PRINTVERBOSE  6
#define PRINTALL      7

typedef                   bool bit;
typedef   signed          char s08;
typedef unsigned          char u08;
typedef   signed short     int s16;
typedef unsigned short     int u16;
typedef   signed           int s32;
typedef unsigned           int u32;
typedef   signed long long int s64;
typedef unsigned long long int u64;

typedef signed int errOk;

#endif /* _INCcomTypeh */
