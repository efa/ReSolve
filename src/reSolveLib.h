/* ReSolve v0.09.09h 2023/03/22 solve math expressions using discrete values*/
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

//#define _XOPEN_SOURCE 600
#define _GNU_SOURCE     /* vasprintf() */
#include <stdio.h>      /* printf() */
#include <string.h>     /* strcpy(), strlen(), strcspn() */
#include <stdint.h>     /* uintptr_t */
#include <stdlib.h>     /* qsort(), atof(), exit() */
#include <stdarg.h>     /* va_list, va_start(), va_end() */
#include <math.h>       /* pow(), fabs() */
#include <ctype.h>      /* isdigit() */
#include <malloc.h>     /* malloc() */
#include <time.h>       /* clock(), CLOCKS_PER_SEC */
#include <unistd.h>     /* fsync() */
#include "comType.h"    /* common type */
#include "fileIo.h"     /* file Input/Output */
#include "exprParser.h" /* expression parser interface */

#define SourceVersion "0.09.09h beta"
#define SourceDate    "2023/03/22"

#ifdef __MSVCRT__       /* CrossCompile to MinGw target */
#define fsync _commit   /* msvcrt.dll miss fsync, that is present on unix */
#endif                  /* on Win32 try if work using _commit */

/* Up to V0.07.07c before compile, to configure available values, need to set:
    1) 'Series' with xx (96 for E96), or 0 for custom list
    2) Decades 6 or 7 for Exx, ignored with custom list
    3) ListNumber with values quantity in case of custom list
    4) MaxRp 1 or 2 (also test series and parallel in each position)
    5) in custom lists keep only one variable 'baseRdesc[65]' removing numbers after R
    6) in custom lists keep only one variable 'baseR[24]' removing numbers after R
    Optional:
    7) MaxRc as now fixed to 2 variables (letter a,b)
    8) NumberResMax can be kept to 512000 that is compatible with all compilers
*/

#define Series 192 /* Exx: Series E12, E24, (E48), E96 or E192. Use 0 for custom list */
#define Decades 7 /* number of decades of interest, normally 6 or 7 */
#if (Series>0) /* standard Exx series */
    #define NumR    (u32)Series*Decades /* number of existant values of resistance */
    #define ListNumber 0 // not used
#else /* custom list */
    #define ListNumber 57 /* insert here custom list quantity */
    #define NumR   (u32)ListNumber /* number of existant values of resistance */
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
#define DesiredDefault     9 /* default value for desired */
#define MaxValue 50E9 // 50 G(Ohm) should be greather than any practical value
#define Epsilon 1.0E-14 // used to compare double/float numbers

#define NumberResDefault  20  /* default number of results to print */
#define NumberResMax     512000 /* the limit depends on compiler */

#define PRINTBATCH   3
#define PRINTF       4
#define PRINTDEBUG   5
#define PRINTVERBOSE 6
#define DbgLv PRINTF

extern char expr[];    /* default value for formula: reversed high partitor (LM317) */
extern double desired; /* searched value */
extern u08 Eseries;    /* Exx: Series E12, E24, (E48), E96 or E192. Use 0 for custom list */
extern u08 decades;    /* number of decades of interest, normally 6 or 7 */
extern u32 numR;       /* number of existant values of resistance */
extern u16 maxRp;      /* max number of resistances supported per position: as now 1 or 2 */
extern u16 maxRc;      /* number of resistances (variables) in the circuit: 2 */
extern u32 numV;       /* number of input possible values (x each position) */
extern u64 totV;       /* number of results values to try */
extern u16 numBestRes; /* number of best results to show */
extern u16 listNumber; // custom list quantity
extern double* baseR;  // declare vector pointer, will be a vector of double baseR[listNumber]
extern char baseRdesc[]; // description: reserve space for 65 chars
struct rValuesTy { double* rp; /* will be a vector of values with [maxRp] elements */
                   double  r;        /* resultant value, single, series & parallel */
                   char    desc[25]; /* description of how is built (single, series or parallel) */
                 }; /* struct declaration */
extern struct rValuesTy* rValues; /* pointer to rValues[numV] for single, series & parallel rValues[numV] */
struct resultsTy { u16   pos[MaxRc]; /* positions in rValues of each resistance, [maxRc] elements */
                   float delta;     /* distance from desiderata */
                 }; /* struct declaration */
extern struct resultsTy* results; /* pointer to results[totV] for results: [(12*7)^2] results[totV] */
extern u16 valTy, resTy; // sizeof struct
extern u32 rValueSize; // sizeof vector of struct: rValues[numV]
extern u64 resultSize; // sizeof vector of struct: results[totV]
extern u64 allocatedB; // sizeof allocated memory in Bytes
extern u32 first; // first result to show
extern size_t resultLowSize; // size of mem low vectors resultsLow[numBestRes]
extern struct resultsTy* resultsLowPtr; // low mem results[numBestRes], all kind solutions
extern struct resultsTy* results4LowPtr; // low mem results[numBestRes], 4R solutions
extern struct resultsTy* results3LowPtr; // low mem results[numBestRes], 3R solutions
extern struct resultsTy* results2LowPtr; // low mem results[numBestRes], 2R solutions
extern bool algo; // 0 use old memory hungry strategy, 1 use new mem low strategy
extern bool gui;  // when 1, gprintf() update the GUI
extern bool winGuiLoop; // Win loop gtk_events_pending/gtk_main_iteration to update GUI
extern int (*guiUpdateOutPtr)(char*,int); // function pointer to guiUpdateOut()

// public library functions:
int fillConfigVars(void); // load and check users config file
void showHelp(u64 allocatedB);
void showHead(void);
int calcRvalues(void); /* in series or parallel, support MaxRp=2 only */
int calcFvalues(void); /* calculate all formula results using 'maxRc' resistances */
int structQuickSort(struct resultsTy results[], s32 totNumber);/* QuickSort for vector of structs of type resultsTy, using field 'abs(delta)' */
int showVal(u32 first); // solutions with up to 4 resistors
int showVal4(u32 numBestRes); // Solutions with 4 resistors
int showVal3(u32 numBestRes); // Solutions with 3 resistors
int showVal2(u32 numBestRes); // Solutions with 2 resistors
int updateRdesc(); // update Rdesc
int baseInit(); // basic initialization: load config from file
int memValCalc(); // memory size calculation for input values
int memCalc();  // memory size calculation for results
int memValAlloc(); // memory allocation for input values
int memAlloc(); // memory allocation for results
int showConf(); // show config set
int doCalc(); // fill inputs, calcs, sort solutions
int freeMem(); // free memory
int gprintf (int gui, const char* format, ...); // printf() or update GUI
char* siMem(u64 sizeB); // convert an u64 to string using SI prefix

int memLowCalc(); // low memory size calculation
int memLowAlloc(); // allocate low mem for results
int doMemLowCalc(); // fill inputs, low mem calcs+sort solutions
int showValMemLow(u32 numBestRes, struct resultsTy* resultsLowPtr); // Solutions

#endif /* _INCh */
