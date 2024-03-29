/* ReSolve v0.11.09h 2023/12/29 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* reSolveLib.h is part of ReSolve
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

/* reSolveLib.h This is the main lib source include */

#ifndef _INCreSolveLibh
#define _INCreSolveLibh

#define _GNU_SOURCE     /* vasprintf() */
#include <stdio.h>      /* printf() */
#include <string.h>     /* strcpy(), strlen(), strcspn() */
#include <stdint.h>     /* uintptr_t */
#include <stdlib.h>     /* qsort(), atof(), exit(), malloc() */
#include <stdarg.h>     /* va_list, va_start(), va_end() */
#include <limits.h>     /* PATH_MAX,NAME_MAX */
#include <math.h>       /* pow(), fabs() */
#include <ctype.h>      /* isdigit() */
#include <time.h>       /* clock(), CLOCKS_PER_SEC */
#include <unistd.h>     /* fsync() */
#include "comType.h"    /* common type */
#include "fileIo.h"     /* file Input/Output */
#include "exprParser.h" /* expression parser interface */

#define AppName       "ReSolve"
#define SourceVersion "0.11.09h beta"
#define CopyrightYear "2023"
#define SourceDate    CopyrightYear"/12/29"
#define ReSolveVer    SourceVersion" "SourceDate
#define Author        "Valerio Messina"
#define WebLink       "github.com/efa/ReSolve"
#define WebLabel      WebLink

#ifdef __MSVCRT__       /* CrossCompile to MinGw target */
#define fsync _commit   /* msvcrt.dll miss fsync, that is present on unix */
#endif                  /* on Win32 try if work using _commit */
#ifdef _WIN32           // Win have '_fullpath'
   #define realpath(N,R) _fullpath((R),(N),PATH_MAX)
#endif

#define Series 96 /* Exx: Series E12, E24, E48, E96 or E192. Use 0 for custom list */
#define Decades 7 /* number of decades of interest, normally 6 or 7 */
#define NumR1 153 /* Default user list quantity */
#define NumR2  77 /* Default user list2 quantity */
#if (Series>0) /* standard Exx series */
    #define NumR (u32)Series*Decades /* number of existant values of inputs */
#else /* custom list */
    #define NumR NumR1
#endif
#define MaxRp 1 /* max number of resistances supported per position: as now 1 or 2 */
#if (MaxRp==1)
    #define NumV NumR /* number of possible values x each position */
#endif
#if (MaxRp==2)    /* if we have 2 resistors per position ... */
    #define NumV NumR+NumR*NumR+NumR /* number of possible values x each position */
#endif
#define MaxRc 2 /* number of resistances (variables) in the circuit: 2 */
#if (MaxRc==2)   /* as now 2 is the only supported number */
    #define TotV (((u32)NumV)*((u32)NumV)) /* number of values to try */
#endif
/* default value for formula: reversed high partitor (LM317) */
#define ExpressionDefault "1.25*(1+b/a)                              "
/* Formula+spaces:        "        " */
#define TargetDefault     9 /* default value for desired */
#define MaxValue 50E9 // 50 G(Ohm) should be greather than any practical value
#define Epsilon 1.0E-14 // used to compare double/float numbers
#define MICRO "μ"//"u" // used when SI prefix are requested

#define NumberResDefault  20  /* default number of results to print */
#define NumberResMax     512000U /* the limit depends on compiler */

#define PRINTBATCH   3
#define PRINTF       4
#define PRINTDEBUG   5
#define PRINTVERBOSE 6
#define DbgLv PRINTF

extern char expr[];    /* default value for formula: reversed high partitor (LM317) */
extern double target;  /* searched value */
extern u08 Eserie;     /* Ex: Series E12, E24, E48, E96 or E192. Use 0 for custom list */
extern u08 decades;    /* number of decades of interest, normally 6 or 7 */
extern u16 numR1;      /* number of values in first user list */
extern u16 maxRp;      /* max number of resistances supported per position: as now 1 or 2 */
extern u16 maxRc;      /* number of resistances (variables) in the circuit: 2 */
extern u32 numV;       /* number of input possible values (all configurations) */
extern u64 totV;       /* number of results values to try */
extern u32 numBestRes; /* number of best results to show */
//extern u16 listNumber; // user list quantity: numR1 OR numR1+numR2
extern u16 numR;       // number of values from both lists: numR1 OR numR1+numR2
extern double* userR;  // declare vector pointer, will be a vector of double userR[numR1]
extern char userRdesc[]; // description print: reserve space for 65 chars
extern u32 numS; // number of single values, like plv+1
extern u32 pfv;  // position of first single value
extern u32 plv;  // position of last single value
extern u32 pfs;  // position of first serie
extern u32 pls;  // position of last serie
extern u32 pfp;  // position of first parallel
extern u32 plp;  // position of last parallel
extern u32 numF; // save calculated numV for memory free

extern u08 lists; // 1 normal, 2 use userR as low precision & userR2 as hi prec
extern double* userR2; // declare vector pointer, will be a vector of double userR2[numR2]
extern float userRtol;    // userR percent tolerance: 0.1, 1, 2, 5, 10, 20, 40
extern float userR2tol;   // userR2 percent tolerance: 0.1, 1, 2, 5, 10, 20, 40
extern char userR2desc[]; // description print: reserve space for 65 chars
extern u16 numR2;   // number of values in second user list
extern u08 bestTol; // 0 normal, 1 use userR2 as 1/10 tolerance than userR
extern float tolRatio; // userR2 to userR tolerance

extern char Vdesc[][17]; // "UserListX", "EXXXserie", "Series of", "Parallel "
struct rValuesTy { double* rp; /* will be a vector of values with [maxRp] elements */
                   double  r;    /* resultant value, single, series & parallel */
                   u08     descIdx; /* description Vdesc[] index how is built (single, series or parallel) */
                 }; /* struct declaration */
extern struct rValuesTy* rValues; /* pointer to rValues[numV] for single, series & parallel rValues[numV] */
struct resultsTy { u16   pos[MaxRc]; /* positions in rValues of each resistance, [maxRc=2] elements */
                   float delta;     /* distance from desiderata */
                 }; /* struct declaration */
extern struct resultsTy* results; // pointer to results[totV] for results. Ex.[(12*7)^2]

extern u16 valTy, resTy; // sizeof struct
extern u32 rValueSize; // sizeof vector of struct: rValues[numV]
extern u64 resultSize; // sizeof vector of struct: results[totV]
extern u64 allocatedB; // sizeof total allocated memory in Bytes
extern u32 first; // first result to show

extern size_t resultLowSize; // size of mem low vectors resultsLow[numBestRes]
extern struct resultsTy* resultsLowPtr; // low mem results[numBestRes], all kind solutions
extern struct resultsTy* results4LowPtr; // low mem results[numBestRes], 4R solutions
extern struct resultsTy* results3LowPtr; // low mem results[numBestRes], 3R solutions
extern struct resultsTy* results2LowPtr; // low mem results[numBestRes], 2R solutions

extern u08 format; // 0 scientific notation, 1 engineering notation, 2 SI prefix
extern bool mem; // 0 use old memory hungry strategy, 1 use new mem low strategy
extern bool gui;  // when 1, gprintf() update the GUI
extern bool winGuiLoop; // Win loop gtk_events_pending/gtk_main_iteration to update GUI
extern int (*guiUpdateOutPtr)(char*,int); // function pointer to guiUpdateOut()
extern bit stop; // used by GUI to ask stop computations

// public library functions:
void chDirBin(char* argList); // change current working directory to binary path
int gprintf (int gui, const char* format, ...); // printf() or update GUI
char* siMem(u64 sizeB); // convert an u64 to string using SI prefix, do not require libmath
char* engStr(double num, int significant, bool sign, bool siPref); // engineering notation or SI prefix, require libmath
int isNumber(char* strPtr, bool dotComma); // return 1 for numbers. When dotComma=1 accept dot and comma
int fillConfigVars(); // load and check users config file
int exprCheck(); // check expression syntax
void showHead();
void showHelp();
int updateEserie(char* EseriePtr); // update u08 Eserie from char* EseriePtr
int updateRdesc(bit force); // update Rdesc
void showQty();   // show compute quantities
void showInPos(); // show input value positions
int globalInit();  // basic initialization
int memInpCalc();  // memory size calculation for input values
int memResCalc();  // memory size calculation for results
int memInpAlloc(); // memory allocation for input values
int memResAlloc(); // memory allocation for results
int showConf(); // show config set
void showEserie();  // show all Eserie resistor values
int calcRvalues();  // calc values and series or parallel, lists=1
int calcR2values(); // calc values R1%+R0.1% & R1%//R0.1%, lists=2
void showRvalues(); // show all input resistor values
int calcFm0values(); // calculate all results using 'maxRc' when mem=0
int calcFm1values(); // calculate all results using 'maxRc' when mem=1
int structQuickSort(struct resultsTy results[], s32 totNumber);/* QuickSort for vector of structs of type resultsTy, using field 'abs(delta)' */
int doCalc(); // fill inputs, calcs, sort solutions
int showVal(u32 numBestRes); // show Solutions
int freeMem(); // free memory

#endif /* _INCh */
