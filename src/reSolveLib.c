/* ReSolve v0.09.09h 2023/03/22 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* reSolveLib.c is part of ReSolve
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

/* reSolveLib.c This is the main lib source code */

#include "reSolveLib.h"    /* main source include */

u08 dbgLev = DbgLv;
u08 dbgLv  = DbgLv;

char expr[LineLen] = ExpressionDefault;/* default value for formula: reversed high partitor (LM317) */
double desired;        /* searched value */
u08 Eseries = Series;  /* Exx: Series E12, E24, (E48), E96 or E192. Use 0 for custom list */
u08 decades = Decades; /* number of decades of interest, normally 6 or 7 */
u32 numR = NumR;   /* number of existant values of resistance */
u16 maxRp = MaxRp; /* max number of resistances supported per position: as now 1 or 2 */
u16 maxRc = MaxRc; /* number of resistances (variables) in the circuit: 2 */
u32 numV = NumV;   /* number of input possible values (x each position) */
u64 totV = TotV;   /* number of results values to try */
u16 numBestRes = NumberResDefault; /* number of best results to show */

char Exx[24]=""; // description for single result when Rp=2

char   baseR1desc[65] = "standard series E1"; // max 64 chars
double baseR1[1] = { 1.0 }; // E1

char   baseR3desc[65] = "standard series E3 @40% tolerance"; // max 64 chars
double baseR3[3] = { 1.0, 2.2, 4.7 }; // E3 @40% tolerance

char   baseR6desc[65] = "standard series E6 @20% tolerance"; // max 64 chars
double baseR6[6] = { 1.0, 1.5, 2.2, 3.3, 4.7, 6.8 }; // E6 @20% tolerance

char   baseR12desc[65] = "standard series E12 @10% tolerance"; // max 64 chars
double baseR12[12] = { 1.0, 1.2, 1.5, 1.8, 2.2, 2.7,   // E12 @10% tolerance, 100ppm. 200pm below 10 Ohm. MIL-PRF55342H (1 to 22 MOhm)
                        3.3, 3.9, 4.7, 5.6, 6.8, 8.2 };

char   baseR24desc[65] = "standard series E24 @5% (and 2%) tolerance"; // max 64 chars
double baseR24[24] = { 1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0,   // E24 @5% (and 2%) tolerance, 100ppm. 200pm below 10 Ohm. MIL-PRF55342H (1 to 22 MOhm)
                       2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3,   // M55342M06/VishayRCWPM (0705) down to 1
                       4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1 }; // M55342M06/VishayEH    50ppm (20-150k, 10-650k depend on package)

char   baseR48desc[65] = "standard series E48 @2% tolerance"; // max 64 chars
double baseR48[48] = { 1.00, 1.05, 1.10, 1.15, 1.21, 1.27, 1.33, // E48 @2% tolerance
                       1.40, 1.47, 1.54, 1.62, 1.69, 1.78, 1.87,
                       1.96, 2.05, 2.15, 2.26, 2.37, 2.49, 2.61,
                       2.74, 2.87, 3.01, 3.16, 3.32, 3.48, 3.65,
                       3.83, 4.02, 4.22, 4.42, 4.64, 4.87, 5.11,
                       5.36, 5.62, 5.90, 6.19, 6.49, 6.81, 7.15,
                       7.50, 7.87, 8.25, 8.66, 9.09, 9.53 };

char   baseR96desc[65] = "standard series E96 @1% tolerance"; // max 64 chars
double baseR96[96] = { 1.00, 1.02, 1.05, 1.07, 1.10, 1.13, // E96 @1% tolerance, 100ppm. MIL-PRF55342H (1 to 22 MOhm). 0.1% 15ppm ESCC4001-029/VishayTNPS
                       1.15, 1.18, 1.21, 1.24, 1.27, 1.30, // M55342H06 down to 1 Ohm, VishayRCWPM (0705) down to 10 or 5.62 Ohm (24 values 1st decade)
                       1.33, 1.37, 1.40, 1.43, 1.47, 1.50, // M55342H06 down to 1 Ohm, VishayEH    (0705) down to 20 Ohm(50ppm) or 49.9 Ohm(25ppm)
                       1.54, 1.58, 1.62, 1.65, 1.69, 1.74, // ESCC4001-029/VishayTNPS (0.1% tolerance, 15ppm, 10 to 422 kOhm @0805, to 221 kOhm @0603)
                       1.78, 1.82, 1.87, 1.91, 1.96, 2.00,
                       2.05, 2.10, 2.15, 2.21, 2.26, 2.32, // M55342M06/VishayRCWPM (0705) up to 22 MOhm (33 values last decade)
                       2.37, 2.43, 2.49, 2.55, 2.61, 2.67,
                       2.74, 2.80, 2.87, 2.94, 3.01, 3.09,
                       3.16, 3.24, 3.32, 3.40, 3.48, 3.57,
                       3.65, 3.74, 3.83, 3.92, 4.02, 4.12,
                       4.22, 4.32, 4.42, 4.53, 4.64, 4.75, // ESCC4001-029/VishayTNPS 0.1% 15ppm, up to 422 kOhm 0805 or 221 kOhm 0602
                       4.87, 4.99, 5.11, 5.23, 5.36, 5.49,
                       5.62, 5.76, 5.90, 6.04, 6.19, 6.34,
                       6.49, 6.65, 6.81, 6.98, 7.15, 7.32,
                       7.50, 7.68, 7.87, 8.06, 8.25, 8.45,
                       8.66, 8.87, 9.09, 9.31, 9.53, 9.76 };

char   baseR192desc[65] = "standard series E192 @0.5% (and 0.1%) tolerance"; // max 64 chars
double baseR192[192] = { 1.00, 1.01, 1.02, 1.04, 1.05, 1.06, 1.07, 1.09, // E192 @0.5 (and 0.1%) tolerance. MIL-PRF55342H (1 to 22 MOhm)
                         1.10, 1.11, 1.13, 1.14, 1.15, 1.17, 1.18, 1.20, // M55342M06 down to 1, VishayRCWPM (100ppm) down to (small:0505)10 Ohm or (0603up)5.62 Ohm
                         1.21, 1.23, 1.24, 1.26, 1.27, 1.29, 1.30, 1.32, // M55342M06 down to 1, VishayEH    down to 20 Ohm(50ppm) or 49.9 Ohm(25ppm)
                         1.33, 1.35, 1.37, 1.38, 1.40, 1.42, 1.43, 1.45,
                         1.47, 1.49, 1.50, 1.52, 1.54, 1.56, 1.58, 1.60,
                         1.62, 1.64, 1.65, 1.67, 1.69, 1.72, 1.74, 1.76,
                         1.78, 1.80, 1.82, 1.84, 1.87, 1.89, 1.91, 1.93,
                         1.96, 1.98, 2.00, 2.03, 2.05, 2.08, 2.10, 2.13,
                         2.15, 2.18, 2.21, 2.23, 2.26, 2.29, 2.32, 2.34, // M55342M06/VishayRCWPM (0705) up to 22 MOhm
                         2.37, 2.40, 2.43, 2.46, 2.49, 2.52, 2.55, 2.58,
                         2.61, 2.64, 2.67, 2.71, 2.74, 2.77, 2.80, 2.84,
                         2.87, 2.91, 2.94, 2.98, 3.01, 3.05, 3.09, 3.12,
                         3.16, 3.20, 3.24, 3.28, 3.32, 3.36, 3.40, 3.44,
                         3.48, 3.52, 3.57, 3.61, 3.65, 3.70, 3.74, 3.79,
                         3.83, 3.88, 3.92, 3.97, 4.02, 4.07, 4.12, 4.17,
                         4.22, 4.27, 4.32, 4.37, 4.42, 4.48, 4.53, 4.59,
                         4.64, 4.70, 4.75, 4.81, 4.87, 4.93, 4.99, 5.05,
                         5.11, 5.17, 5.23, 5.30, 5.36, 5.42, 5.49, 5.56,
                         5.62, 5.69, 5.76, 5.83, 5.90, 5.97, 6.04, 6.12,
                         6.19, 6.26, 6.34, 6.42, 6.49, 6.57, 6.65, 6.73,
                         6.81, 6.90, 6.98, 7.06, 7.15, 7.23, 7.32, 7.41,
                         7.50, 7.59, 7.68, 7.77, 7.87, 7.96, 8.06, 8.16,
                         8.25, 8.35, 8.45, 8.56, 8.66, 8.76, 8.87, 8.98,
                         9.09, 9.20, 9.31, 9.42, 9.53, 9.65, 9.76, 9.88 };

u16 listNumber = ListNumber; // custom list quantity

char    baseRdesc[65]=""; // description: reserve space for 65 chars
double* baseR; // declare vector pointer, will be a vector of double baseR[listNumber]

//static struct rValuesTy rValues[numV]; /* allocate memory for single, series & parallel */
struct rValuesTy* rValues; /* pointer to memory for single, series & parallel rValues[numV] */

//static struct resultsTy results[totV]; /* allocate memory for results: [(12*7)^2] */
struct resultsTy* results; /* pointer to memory for results: [(12*7)^2] results[totV] */

u16 valTy, resTy; // 
u32 rValueSize; // sizeof vector of struct: rValues[numV]
u64 resultSize; // sizeof vector of struct: results[totV]
u64 allocatedB; // sizeof allocated memory in Bytes
u32 first; // 
size_t sizeLow=0; // size of low mem vectors

int gui;   // when not 0 gprintf() update the GUI
int (*guiUpdateOutPtr)(char*,int); // function pointer to guiUpdateOut()
int winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

// gprintf(): selectable printf OR asprintf(autoalloc sprintf)
// when first parameter is 0 is like printf()
// when first parameter is not 0 is like asprintf() AND call
// functionPtr(strPtr) to update the GUI, autofree allocated string ptr
// Call with:
// gprintf(int, "asprintf str:'%s' is %d\n", text, num);
int gprintf (int gui, const char* format, ...) {
   int len;
   va_list ap;
   va_start(ap, format);
   if (gui==0) { // output to stdout like printf()
      len = vprintf(format, ap);
   } else { // output to auto allocated string like asprintf()
      char* stringPtr;
      len = vasprintf (&stringPtr, format, ap);
      // then update the GUI
      if (guiUpdateOutPtr!=NULL) (*guiUpdateOutPtr)(stringPtr, len);
      free(stringPtr);
   }
   va_end(ap);
   return len;
} // gprintf ()

char* siMem(u64 sizeB) { // convert an u64 to string using SI prefix
   int len=0;
   char* stringPtr;
   if      (sizeB>1E12) len = asprintf (&stringPtr, "%.1f TB", (float)sizeB/1E12);
   else if (sizeB>1E9)  len = asprintf (&stringPtr, "%.1f GB", (float)sizeB/1E9);
   else if (sizeB>1E6)  len = asprintf (&stringPtr, "%.1f MB", (float)sizeB/1E6);
   else if (sizeB>1E3)  len = asprintf (&stringPtr, "%.1f kB", (float)sizeB/1E3);
   else                 len = asprintf (&stringPtr, "%.0f B ", (float)sizeB);
   if (len==0) return NULL;
   return stringPtr; // remember caller to free ptr
} // siMem(u64 sizeB)

u64 powll(u32 base, u32 exp) { /* like pow() in math.h but return u64 */
    register u32 c;
    register u64 res = 1;
    for (c=0; c<exp; c++) {
       res = res * base;
    }
    return (res);
} // u64 powll(u64 base, u64 exp)

void showHead(void) {
   printf ("ReSolve v%s %s by Valerio Messina users.iol.it/efa\n", SourceVersion, SourceDate);
   printf ("\n");
}

void showHelp(u64 allocatedB) {
   showHead ();
   /*printf ("Syntax: reSolve desiredFloatValue [numberOfResults]\n");*/
   printf ("Syntax: reSolve -h|--help\n");
   printf ("Syntax: reSolve [desiredFloatValue ['function(a,b)' [numberOfResults]]]\n");
   printf ("\n");
   printf ("if (desiredFloatValue==NULL) desiredFloatValue=%d\n", DesiredDefault);
   printf ("if (function==NULL)          function=%s\n", ExpressionDefault);
   printf ("if (numberOfResults==NULL)   numberOfResults=%u\n", NumberResDefault);
   printf ("functions support: numbers, variables a-b, operators +,-,*,/,%%,^,(,) any depth\n");
   printf ("Note: use '.' as decimal separators for costants and desiredFloatValue\n");
   printf ("\n");
   printf ("Current configurations read from 'reSolveConf.txt':\n");
   printf ("Desired Value:%11.5f\n", desired);
   printf ("Solve Formula: %s\n", expr);
   printf ("Values list: '%s'", baseRdesc);
   if (Eseries>0) printf (" with %u decades", decades);
   printf ("\n");
   printf ("numR:%u, numV:%u, totV:%llu\n", numR, numV, totV);
   printf ("%u possible values with max:%u resistors per position\n", numV, maxRp);
   //printf ("WARN: may allocate about %lu MB of RAM !\n", 220*powlmy(2, decades-5));
   char* stringPtr;
   stringPtr=siMem(allocatedB);
   printf ("Will allocate about %s of total RAM for %llu solutions\n", stringPtr, totV);
   if (allocatedB>200E6) printf ("WARN: may allocate more than 200 MB RAM !\n");
   printf ("Show the best:%u results\n", numBestRes);
   printf ("\n");
} // showHelp(float allocatedMB)

int fillConfigVars(void) { // load and check users config file
   char fileConf[]="reSolveConf.txt";
   char* bufferPtr;
   off_t bufSize;
   char paramName[LineLen];
   char paramValue[LineLen];
   int  ret;
   char* endPtr;
   bufSize=readFile (fileConf, &bufferPtr);
   if (bufSize==ERROR) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot read file:%s\n", __FUNCTION__, fileConf);
      return (ERROR);
   }
   if (bufferPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: returned NULL pointer\n", __FUNCTION__);
      return (ERROR);
   }
   //printf ("bufSize=%lld\n", (s64)bufSize);
   //printf ("bufferPtr:'\n%s\n':bufferPtr\n", bufferPtr);
   //printf ("\n");

   // reading user configured parameters ...
   strcpy (paramName, "expr");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTWARN) printf ("WARN %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
   } else {
      //printf ("expr='%s'\n", paramValue);
      int len=strlen(paramValue);
      if (len>=LineLen) {
         printf ("Parameter:'%s' len:%u, max supported len:%u\n", paramName, len, LineLen);
         return (ERROR);
      }
      endPtr = strcpy (expr, paramValue);
      if (endPtr==NULL) {
         if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
         return (ERROR);
      }
   }
   if (dbgLev>=PRINTDEBUG) printf ("expr='%s'\n", expr);

   strcpy (paramName, "desired");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTWARN) printf ("WARN %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
   } else {
      //printf ("desired='%s'\n", paramValue);
      desired = strtod (paramValue, &endPtr);
      if (endPtr==paramValue) {
         if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in desired='%s'\n", __FUNCTION__, paramValue);
         return (ERROR);
      }
   }
   if (dbgLev>=PRINTDEBUG) printf ("desired=%g\n", desired);

   strcpy (paramName, "Eseries");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("Eseries='%s'\n", paramValue);
   Eseries = strtol (paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in Eseries='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("Eseries=%u\n", Eseries);

   strcpy (paramName, "decades");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("decades='%s'\n", paramValue);
   decades = strtol (paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in decades='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("decades=%u\n", decades);

   strcpy (paramName, "listNumber");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("listNumber='%s'\n", paramValue);
   listNumber = strtol (paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in listNumber='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("listNumber=%u\n", listNumber);

   strcpy (paramName, "baseR");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("paramValue='%s'\n", paramValue);
   /*for (u16 p=0; p<16; p++) {
      if (dbgLev>=PRINTDEBUG) printf ("@:%p paramValue[%u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
   }*/
   if (dbgLev>=PRINTDEBUG) printf ("paramValue[19-24]='%s'\n", &paramValue[19]);
   u16 size;
   sscanf (&paramValue[19], "0x%04hx", &size); // extract size of baseR
   if (dbgLev>=PRINTDEBUG) printf ("size:0x%04x=%u\n", size, size);
   if (dbgLev>=PRINTDEBUG) printf ("array size:%u=0x%x\n", size, size);
   if (listNumber!=size) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: listNumber:%u <> baseR[size]:%u in config file\n", __FUNCTION__, listNumber, size);
      return (ERROR);
   }
   paramValue[18]='\0'; // clear comma/size overlapping with string terminator
   for (int p=0; p<=24; p++) {
      if (dbgLev>=PRINTDEBUG) printf ("@:%p paramValue[%02u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
   }
   if (dbgLev>=PRINTDEBUG) printf ("@:%p paramValue[25]=0x%02x='%c'\n", &paramValue[25], paramValue[25], paramValue[25]);
   uintptr_t hex;
   sscanf (paramValue, "0x%16zx", &hex); // extract address of baseR
   if (dbgLev>=PRINTDEBUG) printf ("hex:%zu=0x%16zx\n", hex, hex);
   double* vectorPtr;
   vectorPtr=(void*)(size_t)hex;
   //double baseR[size]; // allocate space for custom values
   //double* baseR; // declare vector pointer
   baseR=malloc(size*sizeof(double)); // allocate space for custom values
   if (!baseR) {
      printf ("Dynamic allocation of:%zu Bytes failed:0x%8p\n", size*sizeof(double), baseR);
      return (ERROR);
   }
   for (u16 p=0; p<size; p++) {
      if (dbgLev>=PRINTDEBUG) printf ("vectorPtr[%u]:%g\n", p, vectorPtr[p]);
      baseR[p]=vectorPtr[p]; // fill vector of custom values
   }
   // now can use global baseR[size] for custom values
   free (vectorPtr);
   if (dbgLev>=PRINTDEBUG) printf ("baseR[%u]:", size);
   for (u16 p=0; p<size; p++) {
      if (dbgLev>=PRINTDEBUG) printf ("%g, ", baseR[p]);
   }
   if (dbgLev>=PRINTDEBUG) printf ("\n");

   strcpy (paramName, "baseRdesc");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("baseRdesc='%s'\n", paramValue);
   endPtr = strcpy (baseRdesc, paramValue);
   if (endPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("baseRdesc='%s'\n", baseRdesc);

   strcpy (paramName, "numberOfResults");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("numberOfResults='%s'\n", paramValue);
   numBestRes = strtol (paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("numberOfResults='%s'\n", baseRdesc);

   strcpy (paramName, "maxRp");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("maxRp='%s'\n", paramValue);
   maxRp = strtol (paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in maxRp='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("maxRp=%u\n", maxRp);

   strcpy (paramName, "maxRc");
   ret=parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return (ERROR);
   }
   //printf ("maxRc='%s'\n", paramValue);
   maxRc = strtol (paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in maxRc='%s'\n", __FUNCTION__, paramValue);
      return (ERROR);
   }
   if (dbgLev>=PRINTDEBUG) printf ("maxRc=%u\n", maxRc);

   //dbgLev=PRINTF;
   //printf ("Freeing buffer ...\n");
   free (bufferPtr);

   // check supported user request
   //Eseries=192; // possible values are: 1, 3, 6, 12, 24, 48, 96, 192. Use 0 for custom list
   switch (Eseries) {
   case 0: case 1: case 3: case 6: case 12: case 24: case 48: case 96: case 192:
      if (dbgLev>=PRINTDEBUG) printf ("supported Eseries\n");
      break;
   default:
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: unsupported Eseries:%u\n", __FUNCTION__, Eseries);
      return (ERROR);
   }
   //decades=7; // ignored when Eseries=0
   if (decades==0 || decades>7) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: unsupported decades:%u\n", __FUNCTION__, decades);
      return (ERROR);
   }
   if (listNumber==0 || listNumber>1344) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: unsupported listNumber:%u\n", __FUNCTION__, listNumber);
      return (ERROR);
   }
   if (maxRp==0 || maxRp>2) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: unsupported maxRp:%u\n", __FUNCTION__, maxRp);
      return (ERROR);
   }
   if (maxRc<2 || maxRc>2) { // as now only two variables are supported
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: unsupported maxRc:%u\n", __FUNCTION__, maxRc);
      return (ERROR);
   }
   if (baseRdesc[0]=='\0') {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: empty string 'baseRdesc'\n", __FUNCTION__);
      return (ERROR);
   }
   for (u16 p=0; p<size; p++) {
      //if (dbgLev>=PRINTF) printf ("%g, ", baseR[p]);
      if (baseR[0]==0) {
         if (dbgLev>=PRINTERROR) printf ("ERROR %s: value 0 in 'baseR' not allowed\n", __FUNCTION__);
         return (ERROR);
      }
   }
   return  (OK);
} // int fillConfigVars()

int updateRdesc() { // update Rdesc
   switch (Eseries) {
   case (0):
      strcpy (baseRdesc, "custom list of ");
      char str[6];
      sprintf(str, "%u", listNumber);
      strcat (baseRdesc, str);
      strcat (baseRdesc, " values");
      break;
   case (1): // standard Exx series resistors
      strcpy (baseRdesc, baseR1desc);
      break;
   case (3):
      strcpy (baseRdesc, baseR3desc);
      break;
   case (6):
      strcpy (baseRdesc, baseR6desc);
      break;
   case (12):
      strcpy (baseRdesc, baseR12desc);
      break;
   case (24):
      strcpy (baseRdesc, baseR24desc);
      break;
   case (48):
      strcpy (baseRdesc, baseR48desc);
      break;
   case (96):
      strcpy (baseRdesc, baseR96desc);
      break;
   case (192):
      strcpy (baseRdesc, baseR192desc);
      break;
   default:
      printf ("Unsupported Series:%u. Supported are 1, 3, 6, 12, 24, 48, 96 and 192\n", Eseries);
      return 1;
   }
   return 0;
} // int updateRdesc()

int baseInit() { // basic initialization: load config from file
   int ret;

   // Use of dynamic memory allocation
   // 1 - load configuration file and params
   ret = fillConfigVars (); // load and check users config file
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: fillConfigVars returned not OK\n", __FUNCTION__);
      return (ERROR);
   }
   updateRdesc();

   return 0;
} // int baseInit()

int memCalc() { // memory size calculation
   // 2 - read and set user request
   if (Eseries>0) {
      numR=(u32)Eseries*decades; /* number of existant values of resistances */
      listNumber=0; // not used with Eseries
   } else { /* custom list */
      // listNumber=57; /* insert here custom list quantity */
      numR=(u32)listNumber; /* number of existant values of resistances */
   }
   strcpy (Exx, "E");
   char series[4];
   sprintf (series, "%u", Eseries);
   strcat (Exx, series);
   strcat (Exx, " series"); // 11 chars
   //maxRp=1; /* max number of resistances supported per position: as now 1 or 2 */

   // 3 - calculate the needed memory
   if (maxRp==1) {
      numV=numR; /* possible values x each position */
   }
   if (maxRp==2) {
      numV=numR+numR*numR+numR; /* number of possible values x each position */
   }
   //maxRc=2; /* number of resistances (variables) in the circuit: 2 */
   if (maxRc==2) {   /* as now 2 is the only supported value */
      totV=((u64)numV)*((u64)numV); /* number of values to try */
      // compile&doNotStart with 3'882'988'894 max. Start with 917'797'375 max.
      // compile&doNotStart with   268'402'689 max. Start with 59'954'049 max.
      // RunWell                                          with 59'954'049 max.
   }
   valTy = sizeof(struct rValuesTy);
   rValueSize=numV*valTy; // expected no more than '1344x48=64512'
   if (dbgLev>=PRINTDEBUG) printf ("numV:%u X valTy:%u = rValueSize:%u\n", numV, valTy, rValueSize);

   resTy = sizeof(struct resultsTy);
   resultSize=(u64)totV*resTy;
   if (dbgLev>=PRINTDEBUG) printf ("totV:%llu X resTy:%u = resultSize:%llu\n", totV, resTy, resultSize);
   //printf ("Will allocate about %f MB of RAM for val\n", rValueSize/1048576.0);
   //printf ("size of struct resultsTy:%u\n", resTy);
   //printf ("Will allocate about %f MB of RAM for res\n", resultSize/1048576.0);
   allocatedB = rValueSize+resultSize;
   //printf("will allocateMB:'%.1f'\n", allocatedB);

   return 0;
} // int memCalc()

int memLowCalc() { // low memory size calculation
   size_t structsize;
   structsize=sizeof(struct resultsTy);
   if (maxRp==1) {
      sizeLow=numBestRes*structsize;
   }
   if (maxRp==2) {
      sizeLow=4*numBestRes*structsize;
   }
   //printf("will allocate low mem: structsize:%zu numBestRes:%u size:%zu\n", structsize, numBestRes, sizeLow);
   return 0;
} // memLowCalc()

int memValAlloc() { // memory allocation for input values
   // 6 - allocate the memory asking to the OS a malloc()
   /* allocated memory for all R: [12*7]+([12*7]*[12*7])+[12*7] : */
   //struct rValuesTy rValues[numV]; /* single, series & parallel */
   //struct rValuesTy* rValues; /* pointer to memory for single, series & parallel */
   char* stringPtr;
   stringPtr=siMem(rValueSize);
   //printf ("allocat:'%s'\n", stringPtr);
   gprintf (gui, "Allocating about %s of RAM for inputs ...\n", stringPtr);
   free(stringPtr);
   rValues=malloc(rValueSize); // numV*sizeof(struct rValuesTy)
   // allocation fail with 2'930'000'000 on a 4 GB RAM, 8 GB swap, 32 bit OS
   if (!rValues) {
      printf ("Dynamic allocation of:%u Bytes failed:0x%8p\n", rValueSize, rValues);
      return (ERROR);
   }
   // 7 - create the structure's vector inside the allocated memory
   for (u32 e=0; e<numV; e++) {
      rValues[e].rp = (double*) malloc (maxRp*sizeof (double));
      if (!rValues[e].rp) {
         printf ("Dynamic allocation of:%zu Bytes failed:0x%8p\n", maxRp*sizeof (double), rValues[e].rp);
         return (ERROR);
      }
   }
   // now can use global rValues[].rp[], rValues[].r for input values
   return 0;
}

int memAlloc() { // memory allocation for results
   char* stringPtr;
   stringPtr=siMem(resultSize);
   gprintf (gui, "Allocating about %s of RAM for results ...\n", stringPtr);
   free(stringPtr);
   //printf ("size_t size:%u Bytes ptr:%lu\n", sizeof (size_t), (u32) pow(2, 8*sizeof (size_t)));
   if (resultSize > pow(2, 8*sizeof (size_t))) {
      printf ("ERROR: This machine cannot handle allocation of:%llu Bytes\n", resultSize);
      return (ERROR);
   }
   results=malloc(resultSize); // (u64)totV*sizeof(struct resultsTy)
   if (dbgLev>=PRINTDEBUG) printf ("allocated %llu Bytes at:0x%8p\n", resultSize, results);
   if (!results) {
      printf ("Dynamic allocation of:%llu Bytes failed:0x%8p\n", resultSize, results);
      return (ERROR);
   }
   // now can use global results[].pos[], results[].delta for results
   if (dbgLev>=PRINTDEBUG) printf ("Done. Dynamically allocated 'results' at:%p\n", results);
   return 0;
} // int memAlloc()

int memLowAlloc() { // allocate low mem for results
   size_t size, structsize;
   structsize=sizeof(struct resultsTy);
   size=numBestRes*structsize;
   if (maxRp==1) {
      printf("Allocating low mem: structsize:%zu numBestRes:%u size:%zu for results ...\n", structsize, numBestRes, size);
      results2LowPtr=malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
      if (results2LowPtr==NULL) {
         printf("malloc error, quit\n");
         return 1;
      }
      printf("Allocated low mem: structsize:%zu numBestRes:%u size:%zu for results ...\n", structsize, numBestRes, size);
   }
   if (maxRp==2) {
      printf("Allocating low mem: structsize:%zu numBestRes:%u size:%zu for results ...\n", structsize, numBestRes, 4*size);
      resultsLowPtr=malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
      if (resultsLowPtr==NULL) {
         printf("malloc error, quit\n");
         return 1;
      }
      results4LowPtr=malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
      if (results4LowPtr==NULL) {
         printf("malloc error, quit\n");
         return 1;
      }
      results3LowPtr=malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
      if (results3LowPtr==NULL) {
         printf("malloc error, quit\n");
         return 1;
      }
      results2LowPtr=malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
      if (results2LowPtr==NULL) {
         printf("malloc error, quit\n");
         return 1;
      }
      printf("Allocated low mem: structsize:%zu numBestRes:%u size:%zu for results ...\n", structsize, numBestRes, 4*size);
   }
   return 0;
} // memLowAlloc()

int showConf() { // show config set
   gprintf (gui, "Target Value : %0.3f\n", desired);
   gprintf (gui, "Solve Formula: %s\n", expr);
   exprVarsParser[0]=10.0; exprVarsParser[1]=1.0;
   evalExprParser (expr); /* formula syntax test */
   exprVarsParser[0]=0.0; exprVarsParser[1]=0.0;
   //printf ("sizeof (rValuesTy):%u\n", valTy);
   //printf ("sizeof (rValues[%lu]):%lu\n", numV, val);
   //printf ("sizeof (resultsTy):%u\n", resTy);
   //printf ("sizeof (results[%lu]):%lu\n", totV, res);
   if (Eseries>0) {
      gprintf (gui, "Resistors Series:E%u, Decades:%u\n", Eseries, decades);
      //printf ("Allocated about %lu MB of RAM\n", 220*powlmy(2, decades-5));
   } else {
      gprintf (gui, "Resistors custom list of %u values\n", numR);
      /*maxRc = 2;
      maxRp = 1; // as now with custom list of values, we support only 1 resistor
      numR = listNumber;
      numV = listNumber;
      totV = numV * numV;*/
   }
   char* stringPtr;
   stringPtr=siMem(allocatedB);
   gprintf (gui, "Allocated about %s of total RAM\n", stringPtr);
   free(stringPtr);

   return 0;
} // showConf()

/* using one decade values and 'decades', calculate all standard Exx series values */
// 'baseRxx[Eseries/listNumber]' ==> rValues[pos].r, rValues[pos].rp[p], rValues[pos].desc
s16 calcEseries(void) { // or fill with the custom list of values when Eseries=0
   u08 decade;
   u08 rBase;
   s16 pos = 0; // expected max 7*192=1344
   register double val;
   u08 p;
   if (Eseries>0) { // standard Exx series resistors
      for (decade=0; decade<decades; decade++) {
         for (rBase=0; rBase<Eseries; rBase++) {
            pos = rBase + decade*Eseries;
            switch (Eseries) {
            case (1):
               /*printf ("rBase:%u baseR1[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR1[rBase], decade, pos);*/
               val = baseR1[rBase] * (double)powll (10, decade);
               break;
            case (3):
               /*printf ("rBase:%u baseR3[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR3[rBase], decade, pos);*/
               val = baseR3[rBase] * (double)powll (10, decade);
               break;
            case (6):
               /*printf ("rBase:%u baseR6[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR6[rBase], decade, pos);*/
               val = baseR6[rBase] * (double)powll (10, decade);
               break;
            case (12):
               /*printf ("rBase:%u baseR12[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR12[rBase], decade, pos);*/
               val = baseR12[rBase] * (double)powll (10, decade);
               break;
            case (24):
               /*printf ("rBase:%u baseR24[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR24[rBase], decade, pos);*/
               val = baseR24[rBase] * (double)powll (10, decade);
               break;
            case (48):
               /*printf ("rBase:%u baseR48[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR48[rBase], decade, pos);*/
               val = baseR48[rBase] * (double)powll (10, decade);
               break;
            case (96):
               /*printf ("rBase:%u baseR96[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR96[rBase], decade, pos);*/
               val = baseR96[rBase] * (double)powll (10, decade);
               break;
            case (192):
               /*printf ("rBase:%u baseR192[rBase]:%lu decade:%u pos:%ld\n", rBase, baseR192[rBase], decade, pos);*/
               val = baseR192[rBase] * (double)powll (10, decade);
               break;
            default:
               printf ("Unsupported Series:%u. Supported are 1, 3, 6, 12, 24, 48, 96 and 192\n", Eseries);
               return (ERROR);
            }
            rValues[pos].r = val;
            for (p=0; p<maxRp; p++) {
               rValues[pos].rp[p] = val;
            }
            //strcpy (rValues[pos].desc, "Exx series"); // 11 chars
            strcpy (rValues[pos].desc, Exx); // description
            if (dbgLv>=PRINTDEBUG) printf ("rValue:%g\n", val);
         }
      }
   } else { // custom list of values
      for (pos=0; pos<listNumber; pos++) { // populate with custom values
         val = baseR[pos];
         rValues[pos].r = val;
         for (p=0; p<maxRp; p++) {
            rValues[pos].rp[p] = val;
         }
         strcpy (rValues[pos].desc, "Custom list"); // 11 chars
         if (dbgLv>=PRINTVERBOSE) printf ("rValue:%g\n", val);
      }
   }
   if (dbgLv>=PRINTDEBUG) printf ("calcEseries pos:%d\n", pos);
   return (pos);
} // s16 calcEseries(void)

/* calculate all possible resistive values using 'MaxRp' resistances */
// 'baseRxx[Eseries/listNumber]' ==> rValues[pos].r, rValues[pos].rp[p], rValues[pos].desc
int calcRvalues(void) { /* when MaxRp=2 also in series and parallel */
   s32 pos;
   register double val;
   u16 rp1, rp2;
   u08 p;
   /* at first calculate the single values (Eseries) or fill custom values */
   pos = calcEseries (); // calculate all standard Exx series values
   if (pos==ERROR) {
      printf ("calcEseries returned ERROR\n");
      return (ERROR);
   }
   //printf ("numR:%u numV:%u totV:%llu numBestRes:%u\n", numR, numV, totV, numBestRes);
   if (dbgLv>=PRINTDEBUG) printf ("pos:%d\n", pos);
   if (maxRp==2) { // as now support only maxRp=2 resistances per position
      /* now calculate all series values with 2 resistances per position */
      pos = numR;
      for (rp1=0; rp1<numR; rp1++) {
         for (rp2=0; rp2<numR; rp2++) {
            if (rp1<rp2) continue; /* skip triangular matrix */
            val = rValues[rp1].r + rValues[rp2].r;
            rValues[pos].r = val;
            rValues[pos].rp[0] = rValues[rp1].r;
            for (p=1; p<maxRp; p++) {
               rValues[pos].rp[p] = rValues[rp2].r;
            }
            strcpy (rValues[pos].desc, "Series   of"); // 11 chars
/*            printf ("r1:%g + r2:%g = ", rValues[rp1].r, rValues[rp2].r);
            printf ("rValue:%g\n", val);*/
            pos++;
            //printf ("here rp1:%u rp2:%u pos:%u\n", rp1, rp2, pos);
         }
      }
      if (dbgLv>=PRINTDEBUG) printf ("calcRvalues pos:%d\n", pos);
      /* now calculate all parallel values with 2 resistances per position */
      for (rp1=0; rp1<numR; rp1++) {
         for (rp2=0; rp2<numR; rp2++) {
            if (rp1<rp2) continue; /* skip triangular matrix */
            val = rValues[rp1].r * rValues[rp2].r / (rValues[rp1].r + rValues[rp2].r);
            rValues[pos].r = val;
            rValues[pos].rp[0] = rValues[rp1].r;
            for (p=1; p<maxRp; p++) {
               rValues[pos].rp[p] = rValues[rp2].r;
            }
            strcpy (rValues[pos].desc, "Parallel of"); // 11 chars
/*            printf ("r1:%g // r2:%g = ", rValues[rp1].r, rValues[rp2].r);
            printf ("rValue:%g\n", val);*/
            pos++;
         }
      }
   }
   if (maxRp>2) {
      printf ("Unsupported maxRp:%u. Supported maxRp 1 or 2\n", maxRp);
      return (ERROR);
   }
   if (dbgLv>=PRINTDEBUG) printf ("calcRvalues pos:%d end\n", pos);
   //firstSingle  =0;
   //lastSingle   =numR-1;
   //firstSeries  =numR;
   //lastSeries   =numR+(numR*numR+numR)/2-1;
   //firstParallel=numR+(numR*numR+numR)/2;
   //lastParallel =numV-1;
   //rValues[   lastSingle:firstSingle ] are single (Exx or custom)
   //rValues[  firstSeries:lastSeries  ] are series
   //rValues[firstParallel:lastParallel] are parallel
   return (OK);
} // int calcRvalues(void)

/* calculate all formula results using 'maxRc' resistances */
int calcFvalues(void) {
   u32 rc1, rc2;
   /*float res[maxRc];*/ /* single case, normally 2 resistances */
   register u64 per; // percentage progress
   register double val;
   register double delta = 0;
   register u32 pos = 0;
   if (dbgLv>=PRINTDEBUG) printf ("numV:%u, totV:%llu\n", numV, totV);
   if (dbgLv>=PRINTDEBUG) printf ("expr:'%s'\n", expr);
   if (dbgLv>=PRINTF) gprintf (gui, "sol="); // here print something for user feedback (take time)
   for (rc1=0; rc1<numV; rc1++) {
      exprVarsParser[0] = rValues[rc1].r; /* estract single case to try */
      if (dbgLv>=PRINTDEBUG) printf ("exprVarsParser[0]:%g\n", exprVarsParser[0]);
      for (rc2=0; rc2<numV; rc2++) {
         exprVarsParser[1] = rValues[rc2].r; /* estract single case to try */
         if (dbgLv>=PRINTDEBUG) printf ("exprVarsParser[1]:%g\n", exprVarsParser[1]);
         val = evalExprParser (expr);
         delta = val - desired;
         /*printf ("rc1:%u rc2:%u pos:%lu val:%g delta:%g\n", rc1, rc2, pos, val, delta);*/
         results[pos].pos[0] = rc1;
         results[pos].pos[1] = rc2;
         results[pos].delta  = delta;
         pos++;
      }
      if (rc1%1000==0) { // here print something for user feedback (take time)
         //if (dbgLv>=PRINTF) gprintf (gui, "%u,", rc1*numV);
         per=(u64)rc1*numV*100/totV;
         if (dbgLv>=PRINTF) gprintf (gui, "%d%%,", per);
         fflush (NULL); // user space stdout flush
         fsync (1);   // kernel space stdout flush
      }
   }
   //if (dbgLv>=PRINTF) gprintf (gui, "%u\n", rc1*numV-1); // here print something for user feedback (take time)
   if (dbgLv>=PRINTF) gprintf (gui, "100%\n"); // here print something for user feedback (take time)
   return (OK);
} // int calcFvalues(void) {

struct resultsTy* resultsLowPtr; // low mem results[numBestRes], all kind solutions
struct resultsTy* results4LowPtr; // low mem results[numBestRes], 4R solutions
struct resultsTy* results3LowPtr; // low mem results[numBestRes], 3R solutions
struct resultsTy* results2LowPtr; // low mem results[numBestRes], 2R solutions
int w=0; // worst solution index
int w4=0; // worst solution index
int w3=0; // worst solution index
int w2=0; // worst solution index
double deltaWorst = MaxValue; // 50 GOhm
double delta4Worst = MaxValue; // 50 GOhm
double delta3Worst = MaxValue; // 50 GOhm
double delta2Worst = MaxValue; // 50 GOhm

// find worst (higher delta) low mem solution
int findWorst(struct resultsTy* resultsNLowPtr, int* wNPtr, double* deltaNWorstPtr) {
   double max=0;
   double delta;
   for (int s=0; s<numBestRes; s++) {
      delta=fabs(resultsNLowPtr[s].delta);
      //printf("s:%d delta:%f\n", s, delta);
      if (delta > max) {
         //printf("old max:%f at:%d, new max:%f at:%d\n", max, *wNPtr, delta, s);
         *wNPtr=s;
         max=delta;
      }
   }
   *deltaNWorstPtr=max;
   //printf("worst:%f max:%f at:%d\n", *deltaNWorstPtr, max, *wNPtr);
   return 0;
} // findWorst()

bool is_double_le(double a, double b, double epsilon) {
    return a < b + epsilon;
}

/* calculate best formula results using 'maxRc' resistances */
int calcLowMemFvalues(void) {
   u32 rc1, rc2;
   register u64 per; // percentage progress
   register double val;
   register double delta = 0;
   if (dbgLv>=PRINTDEBUG) printf ("numV:%u, totV:%llu\n", numV, totV);
   if (dbgLv>=PRINTDEBUG) printf ("expr:'%s'\n", expr);
   for (int s=0; s<numBestRes; s++) {
      results2LowPtr[s].delta=MaxValue; // 50 GOhm
   }
   if (maxRp==2) { // 
      for (int s=0; s<numBestRes; s++) {
         resultsLowPtr[s].delta=MaxValue; // 50 GOhm
      }
      for (int s=0; s<numBestRes; s++) {
         results4LowPtr[s].delta=MaxValue; // 50 GOhm
      }
      for (int s=0; s<numBestRes; s++) {
         results3LowPtr[s].delta=MaxValue; // 50 GOhm
      }
   }
   if (dbgLv>=PRINTF) gprintf (gui, "sol="); // here print something for user feedback (take time)
//printf ("\n");
//printf ("numR:%u numV:%u totV:%llu numBestRes:%u\n", numR, numV, totV, numBestRes);
//printf ("rValues[numV:%u] results[totV:%llu] results4LowPtr[numBestRes:%u]\n", numV, totV, numBestRes);
//printf ("rValues[numR                     = %u:                         0] '%s'\n", numR-1, Exx);
//printf ("rValues[numR+(numR*numR+numR)/2-1=%u:numR                   = %u] '%s'\n", numR+(numR*numR+numR)/2-1, numR, rValues[numR].desc);
//printf ("rValues[numV-1                   =%u:numR+(numR*numR+numR)/2=%u] '%s'\n", numV-1, numR+(numR*numR+numR)/2, rValues[numV-1].desc);
//printf ("\n");
//firstSeries  =numR;
//lastSeries   =numR+(numR*numR+numR)/2-1;
//firstParallel=numR+(numR*numR+numR)/2;
//lastParallel =numV-1;
   for (rc1=0; rc1<numV; rc1++) {
      exprVarsParser[0] = rValues[rc1].r; /* estract single case to try */
      if (dbgLv>=PRINTDEBUG) printf ("exprVarsParser[0]:%g\n", exprVarsParser[0]);
      for (rc2=0; rc2<numV; rc2++) {
         exprVarsParser[1] = rValues[rc2].r; /* estract single case to try */
         if (dbgLv>=PRINTDEBUG) printf ("exprVarsParser[1]:%g\n", exprVarsParser[1]);
         val = evalExprParser (expr);
         delta = val - desired;
         /*printf ("rc1:%u rc2:%u pos:%lu val:%g delta:%g\n", rc1, rc2, pos, val, delta);*/
         if (maxRp==1) { // 
//            if (fabs(delta) <= delta2Worst) {
//            if (delta2Worst > fabs(delta)) {
//            if ( definitelyGreaterThan(delta2Worst, fabs(delta), Epsilon )) {
            if (is_double_le(fabs(delta), delta2Worst, Epsilon)) {
               //printf ("new better result:%f delta:%.4f oldDeltaMin:%.4f\n", val, delta, deltaWorst);
               //printf("rc1:%03u rc2:%03u old delta:%f fill at:%d with new delta:%f\n", rc1, rc2, deltaWorst, w, fabs(delta));
               results2LowPtr[w2].pos[0] = rc1;   // always insert in worst pos
               results2LowPtr[w2].pos[1] = rc2;   // always insert in worst pos
               results2LowPtr[w2].delta  = delta; // always insert in worst pos
               findWorst(results2LowPtr, &w2, &delta2Worst); // find worst (higher delta) solution
            }
         }
         if (maxRp==2) { // 
            if (is_double_le(fabs(delta), deltaWorst, Epsilon)) {
               //printf ("new better result:%f delta:%.4f oldDeltaMin:%.4f\n", val, delta, deltaWorst);
               //printf("rc1:%03u rc2:%03u old delta:%f fill at:%d with new delta:%f\n", rc1, rc2, deltaWorst, w, fabs(delta));
               resultsLowPtr[w].pos[0] = rc1;   // always insert in worst pos
               resultsLowPtr[w].pos[1] = rc2;   // always insert in worst pos
               resultsLowPtr[w].delta  = delta; // always insert in worst pos
               findWorst(resultsLowPtr, &w, &deltaWorst); // find worst (higher delta) solution
            }
            int cmp0, cmp1;
            if (is_double_le(fabs(delta), delta4Worst, Epsilon)) {
               //printf ("new better result:%.1f delta:%.4f oldDeltaMin:%.1f\n", val, delta, delta4Worst);
               //printf("rc1:%03u rc2:%03u old delta:%.1f fill at:%d with new delta:%.1f\n", rc1, rc2, delta4Worst, w4, fabs(delta));
               if (rc1<numR) cmp0 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp0 = 1;
               if (rc2<numR) cmp1 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp1 = 1;
               if (cmp0==0 || cmp1==0) goto n3; // skip when one is 0 (so skip when are less than 4 resistances, do when both are series/parallel)
               //printf("pos:%d cmp0:%d cmp1:%d w4:%d\n", pos, cmp0, cmp1, w4);
               results4LowPtr[w4].pos[0] = rc1;   // always insert in worst pos
               results4LowPtr[w4].pos[1] = rc2;   // always insert in worst pos
               results4LowPtr[w4].delta  = delta; // always insert in worst pos
               findWorst(results4LowPtr, &w4, &delta4Worst); // find worst (higher delta) solution
            }
            n3:
            if (is_double_le(fabs(delta), delta3Worst, Epsilon)) {
               if (rc1<numR) cmp0 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp0 = 1;
               if (rc2<numR) cmp1 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp1 = 1;
               if (cmp0==cmp1) goto n2; // skip when both 0 or both 1 (both Exx/custom or both series/parallel, so do when are 3 resistances)
               results3LowPtr[w3].pos[0] = rc1;   // always insert in worst pos
               results3LowPtr[w3].pos[1] = rc2;   // always insert in worst pos
               results3LowPtr[w3].delta  = delta; // always insert in worst pos
               findWorst(results3LowPtr, &w3, &delta3Worst); // find worst (higher delta) solution
            }
            n2:
            if (is_double_le(fabs(delta), delta2Worst, Epsilon)) {
               if (rc1<numR) cmp0 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp0 = 1;
               if (rc2<numR) cmp1 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp1 = 1;
               if (cmp0!=0 || cmp1!=0) break; // skip when one is 1 (so skip when are more than 2 resistances, do when both are Exx/custom)
               results2LowPtr[w2].pos[0] = rc1;   // always insert in worst pos
               results2LowPtr[w2].pos[1] = rc2;   // always insert in worst pos
               results2LowPtr[w2].delta  = delta; // always insert in worst pos
               findWorst(results2LowPtr, &w2, &delta2Worst); // find worst (higher delta) solution
            }
         } // if (maxRp==2)
      } // inner for ()
      if (rc1%1000==0) { // here print something for user feedback (take time)
         //if (dbgLv>=PRINTF) gprintf (gui, "%u,", rc1*numV);
         per=(u64)rc1*numV*100/totV;
         if (dbgLv>=PRINTF) gprintf (gui, "%d%%,", per);
         fflush (NULL); // user space stdout flush
         fsync (1);   // kernel space stdout flush
      }
   } // external for ()
   //if (dbgLv>=PRINTF) gprintf (gui, "%u\n", rc1*numV-1); // here print something for user feedback (take time)
   if (dbgLv>=PRINTF) gprintf (gui, "100%\n"); // here print something for user feedback (take time)
   return (OK);
} // int calcLowMemFvalues(void) {

s64 tot; // used for debug link between 'structQuickSort()' & 'qsStruct()'

/* QuickSort for vector of structs of type resultsTy. */
/* Sorting criteria: field 'abs(delta)' */
void qsStruct(struct resultsTy results[], s32 left, s32 right) {
   register s32 l, r, m; /* position left, right and median */
   register u64 per; // percentage progress
   float  mp; /* will contain data in center position */
   struct resultsTy temp;            /* temp struct for swap */
   static u64 qs;
   if (left==0 && right==tot-1) qs=0; // so next run of GUI start from 0
   if (qs%20000000==0) {
      //if (dbgLv>=PRINTF) gprintf (1, "qs:%llu tot:%lld ", qs, tot);
      per=qs*100/tot;
      if (dbgLv>=PRINTF) gprintf (gui, "%llu%%,", per);
      fflush (NULL); // user space stdout flush
      fsync (1);   // kernel space stdout flush
   }
   qs++;
   if (dbgLv>=PRINTDEBUG) {
      for (s32 c=0; c<tot; c++) {
         printf ("s[%2d]=%11G\n", c, results[c].delta);
      }
   }
   if (dbgLv>=PRINTDEBUG) printf ("left:%9d right:%9d\n", left, right);
   l = left; r = right;
   m = (left+right)/2; /* center position rounded down */
   mp = fabs(results[m].delta); /* take center element absolute delta */
   if (dbgLv>=PRINTDEBUG) printf ("meanPos(left+right)/2:%9d meanValue_delta[meanPos]:%11G\n", m, mp);
   do { /* at least once, until is true (l<=r) */
      if (dbgLv>=PRINTDEBUG) printf ("l:%9d r:%9d\n", l, r);
      /* find first element at left <= of center element */
      while ( ((mp-fabs(results[l].delta))<0) && (l<right) ) l++; /* test, if true exec */
      /* find first element at right > of center element */
      while ( ((mp-fabs(results[r].delta))>0) && (r>left ) ) r--; /* test, if true exec */
      if (dbgLv>=PRINTDEBUG) printf ("limits: value[l=%9d]=%11G value[r=%9d]=%11G\n", l, results[l].delta, r, results[r].delta);
      if (l <= r) { /* if not crossed, swap */
         if (dbgLv>=PRINTDEBUG) printf ("swap: l:%9d r:%9d\n", l, r);
         temp = results[l];
         results[l] = results[r];
         results[r] = temp;
         l++; r--;
      }
      if (dbgLv>=PRINTDEBUG) {
         for (s32 c=0; c<tot; c++) {
            printf (" [%2d]=%11G\n", c, results[c].delta);
         }
      }
   } while (l <= r);
   if (dbgLv>=PRINTVERBOSE) {
      for (s32 c=left; c<right; c++) {
         printf ("value[%9d]=%11G\n", c, results[c].delta);
      }
   }
   if (left < r) {
      if (dbgLv>=PRINTDEBUG) printf ("analisys: value[left=%9d]=%11G value[r=%9d]=%11G\n", left, results[left].delta, r, results[r].delta);
      if (dbgLv>=PRINTDEBUG) printf ("\n");
      qsStruct (results, left , r);
   }
   if (l < right) {
      if (dbgLv>=PRINTDEBUG) printf ("analisys: value[l=%9d]=%11G value[right=%9d]=%11G\n", l, results[l].delta, right, results[right].delta);
      if (dbgLv>=PRINTDEBUG) printf ("\n");
      qsStruct (results, l, right);
   }
   if (dbgLv>=PRINTDEBUG) printf ("\n");
} // void qsStruct(struct resultsTy results[], s32 left, s32 right)

/* QuickSort for vector of structs of type resultsTy, using field 'abs(delta)' */
/* put results[0] the greatest(worst), results[totNumber-1] the smallest(best) */
int structQuickSort(struct resultsTy results[], s32 totNumber) {
   if (totNumber==0) return (ERROR);
   tot = totNumber;
   if (dbgLv>=PRINTF) {
      gprintf (gui, "qs="); // here print something for user feedback (take time)
      fflush (NULL); // user space stdout flush
      fsync (1);   // kernel space stdout flush
   }
   qsStruct (results, 0UL, totNumber-1);
   //if (dbgLv>=PRINTF) gprintf (gui, "%d\n", totNumber-1);
   if (dbgLv>=PRINTF) gprintf (gui, "100%%\n");
   if (dbgLv>=PRINTDEBUG) {
      for (s64 c=0; c<tot; c++) {
         printf ("f[%2lld]=%11G\n", c, results[c].delta);
      }
   }
   return (OK);
} // int structQuickSort(struct resultsTy results[], s32 totNumber)

/* print last 'numBestRes=totV-first' results */
int showVal(u32 first) { // solutions with up to 4 resistors
   u32 pos;
   int cmp0, cmp1;
   for (pos=first; pos<totV; pos++) { // 'totV-1' is the best solution
      /*printf ("pos:%9lu", pos);*/
      cmp0 = (strcmp (rValues[results[pos].pos[0]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      cmp1 = (strcmp (rValues[results[pos].pos[1]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      gprintf (gui, "a:%11G b:%11G", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      gprintf (gui, "   val:%11G   delta:%11.4G", desired+results[pos].delta, results[pos].delta);
      gprintf (gui, " e%%:%6.3G\n", results[pos].delta/desired*100);
      if (maxRp==2) { // maybe a series or parallel value
         if (cmp0==0) { // single resistors
            gprintf (gui, "a:%-12s:%8g             ", rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "a:%-12s:%8g & %8g  " , rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0], rValues[results[pos].pos[0]].rp[1]);
         }
         if (cmp1==0) { // single resistors
            gprintf (gui, "b:%-12s:%8g", rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "b:%-12s:%8g & %8g ", rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0], rValues[results[pos].pos[1]].rp[1]);
         }
         gprintf (gui, "\n");
      }
   }
   return (OK);
} // int showVal(u32 first)

/* print best 'numBestRes' results */
int showVal4(u32 numBestRes) { // Solutions with 4 resistors
   s32 pos, best;
   u32 count=0;
   int cmp0, cmp1;
   u32 res[NumberResMax];
   //printf ("numBestRes:%ld NumberResMax:%d\n", numBestRes, NumberResMax);
   if (numBestRes>NumberResMax) numBestRes=NumberResMax;
   //printf ("numBestRes:%ld NumberResMax:%d\n", numBestRes, NumberResMax);
   for (pos=totV-1; pos>=0; pos--) { // [totV] is the best solution
      /*printf ("pos:%9lu", pos);*/
      if (count>=numBestRes) break; // printed 'numBestRes' results
      cmp0 = (strcmp (rValues[results[pos].pos[0]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      cmp1 = (strcmp (rValues[results[pos].pos[1]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (cmp0==0 || cmp1==0) continue; // skip when one is 0 (so skip when are less than 4 resistances, do when both are series/parallel)
      res[count]=pos;
      //printf ("4 added pos:%ld\n", pos);
      count++;
   }
   for (best=count-1; best>=0; best--) { // [0] is the best solution
      pos=res[best];
      cmp0 = (strcmp (rValues[results[pos].pos[0]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      cmp1 = (strcmp (rValues[results[pos].pos[1]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      gprintf (gui, "a:%11G b:%11G", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      gprintf (gui, "   val:%11G   delta:%11.4G", desired+results[pos].delta, results[pos].delta);
      gprintf (gui, " e%%:%6.3G\n", results[pos].delta/desired*100);
      if (maxRp==2) { // maybe a series or parallel value
         if (cmp0==0) { // single resistors
            gprintf (gui, "a:%-12s:%8g             ", rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "a:%-12s:%8g & %8g  "  , rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0], rValues[results[pos].pos[0]].rp[1]);
         }
         if (cmp1==0) { // single resistors
            gprintf (gui, "b:%-12s:%8g", rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "b:%-12s:%8g & %8g ", rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0], rValues[results[pos].pos[1]].rp[1]);
         }
         gprintf (gui, "\n");
      }
   }
   return (OK);
} // int showVal4(u32 numBestRes)

/* print best 'numBestRes' results */
int showVal3(u32 numBestRes) { // Solutions with 3 resistors
   s32 pos, best;
   u32 count=0;
   int cmp0, cmp1;
   u32 res[NumberResMax];
   if (numBestRes>NumberResMax) numBestRes=NumberResMax;
   for (pos=totV-1; pos>=0; pos--) { // [totV] is the best solution
      if (count>=numBestRes) break; // printed 'numBestRes' results
      cmp0 = (strcmp (rValues[results[pos].pos[0]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      cmp1 = (strcmp (rValues[results[pos].pos[1]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (cmp0==cmp1) continue; // skip when both 0 or both 1 (both Arb/Exx or both series/parallel, so do when are 3 resistances)
      res[count]=pos;
      //printf ("3 added pos:%ld\n", pos);
      count++;
   }
   for (best=count-1; best>=0; best--) { // [0] is the best solution
      pos=res[best];
      cmp0 = (strcmp (rValues[results[pos].pos[0]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      cmp1 = (strcmp (rValues[results[pos].pos[1]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      gprintf (gui, "a:%11G b:%11G", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      gprintf (gui, "   val:%11G   delta:%11.4G", desired+results[pos].delta, results[pos].delta);
      gprintf (gui, " e%%:%6.3G\n", results[pos].delta/desired*100);
      if (maxRp==2) {
         if (cmp0==0) { // single resistors
            gprintf (gui, "a:%-12s:%8g             ", rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "a:%-12s:%8g & %8g  " , rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0], rValues[results[pos].pos[0]].rp[1]);
         }
         if (cmp1==0) { // single resistors
            gprintf (gui, "b:%-12s:%8g", rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "b:%-12s:%8g & %8g ", rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0], rValues[results[pos].pos[1]].rp[1]);
         }
         gprintf (gui, "\n");
      }
   }
   return (OK);
} // int showVal3(u32 numBestRes)

/* print best 'numBestRes' results */
int showVal2(u32 numBestRes) { // Solutions with 2 resistors
   s32 pos, best;
   u32 count=0;
   int cmp0, cmp1;
   u32 res[NumberResMax];
   if (numBestRes>NumberResMax) numBestRes=NumberResMax;
   for (pos=totV-1; pos>=0; pos--) { // [totV] is the best solution
      if (count>=numBestRes) break; // printed 'numBestRes' results
      cmp0 = (strcmp (rValues[results[pos].pos[0]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      cmp1 = (strcmp (rValues[results[pos].pos[1]].desc, "Custom list")!=0) &&
             (strcmp (rValues[results[pos].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (cmp0!=0 || cmp1!=0) continue; // skip when one is 1 (so skip when are more than 2 resistances, do when both are Exx/custom)
      res[count]=pos;
      //printf ("2 added pos:%ld\n", pos);
      count++;
   }
   for (best=count-1; best>=0; best--) { // [0] is the best solution
      pos=res[best];
      //printf("pos:%d results[pos].pos[0]:%u, results[pos].pos[1]:%u\n", pos, results[pos].pos[0], results[pos].pos[1]);
      gprintf (gui, "a:%11G b:%11G", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      gprintf (gui, "   val:%11G   delta:%11.4G", desired+results[pos].delta, results[pos].delta);
      gprintf (gui, " e%%:%6.3G\n", results[pos].delta/desired*100);
      if (maxRp==2) { // this is just a repetition of 'a' and 'b' as are Custom list
         gprintf (gui, "a:%-12s:%8g             ", rValues[results[pos].pos[0]].desc, rValues[results[pos].pos[0]].rp[0]);
         gprintf (gui, "b:%-12s:%8g"       , rValues[results[pos].pos[1]].desc, rValues[results[pos].pos[1]].rp[0]);
         gprintf (gui, "\n");
      }
   }
   return (OK);
} // int showVal2(u32 numBestRes)

/* print best 'numBestRes' LowMem results */
int showValLowMem(u32 numBestRes, struct resultsTy* resultsLowPtr) { // Solutions
   for (int s=0; s<numBestRes; s++) {
      //printf("s:%02d results[].pos[0]:%03u results[].pos[1]:%03u results[].delta:%.5f\n", s, resultsLowPtr[s].pos[0], resultsLowPtr[s].pos[1], resultsLowPtr[s].delta);
      gprintf(gui, "a:%11G b:%11G", rValues[resultsLowPtr[s].pos[0]].r, rValues[resultsLowPtr[s].pos[1]].r);
      gprintf(gui, "   val:%11G   delta:%11.4G", desired+resultsLowPtr[s].delta, resultsLowPtr[s].delta);
      gprintf(gui, " e%%:%6.3G\n", resultsLowPtr[s].delta/desired*100);
      if (maxRp==2) { // 
         int cmp0, cmp1;
         cmp0 = (strcmp (rValues[resultsLowPtr[s].pos[0]].desc, "Custom list")!=0) &&
                (strcmp (rValues[resultsLowPtr[s].pos[0]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
         cmp1 = (strcmp (rValues[resultsLowPtr[s].pos[1]].desc, "Custom list")!=0) &&
                (strcmp (rValues[resultsLowPtr[s].pos[1]].desc, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
         if (cmp0==0) { // single resistors
            gprintf (gui, "a:%-12s:%8g             ", rValues[resultsLowPtr[s].pos[0]].desc, rValues[resultsLowPtr[s].pos[0]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "a:%-12s:%8g & %8g  " , rValues[resultsLowPtr[s].pos[0]].desc, rValues[resultsLowPtr[s].pos[0]].rp[0], rValues[resultsLowPtr[s].pos[0]].rp[1]);
         }
         if (cmp1==0) { // single resistors
            gprintf (gui, "b:%-12s:%8g"       , rValues[resultsLowPtr[s].pos[1]].desc, rValues[resultsLowPtr[s].pos[1]].rp[0]);
         } else { // 2 resistors in series or parallel
            gprintf (gui, "b:%-12s:%8g & %8g ", rValues[resultsLowPtr[s].pos[1]].desc, rValues[resultsLowPtr[s].pos[1]].rp[0], rValues[resultsLowPtr[s].pos[1]].rp[1]);
         }
         gprintf (gui, "\n");
      }
   }
   return (OK);
} // int showValLowMem(u32 numBestRes, struct resultsTy* resultsLowPtr)

int doCalc() { // fill inputs, calcs, sort solutions
   int ret;
   winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // 8 - fill the input vectors with needed data
   if (maxRp>1) {
      gprintf (gui, "Calculating all:%u values with max:%u resistors per position ...\n", numV, maxRp);
   } else {
      gprintf (gui, "Populating all:%u values with max:%u resistors per position ...\n", numV, maxRp);
   }
   ret = calcRvalues (); /* populate with all possible resistance values */
   if (ret==ERROR) {
      printf ("calcRvalues returned ERROR\n");
      free (results);
      return (ERROR);
   }

   // 9 - calculus of solutions
   gprintf (gui, "Calculating all:%llu solutions with a formula of:%u resistors ...\n", totV, maxRc);
   ret = calcFvalues (); /* calculate all results and delta */
   if (ret==ERROR) {
      printf ("calcFvalues returned ERROR\n");
      free (results);
      return (ERROR);
   }
   if (numBestRes>totV) numBestRes=totV;
   first = totV-numBestRes; /* 0 worse, 'first' 1st printed, 'totV' best, numBestRes # of best print */

   // 10 - sorting of solutions
   gprintf (gui, "Sorting all:%llu found solutions ...\n", totV);
   structQuickSort (results, totV); /* QuickSort on all results */

   return 0;
} // int doCalc()

int doLowMemCalc() { // fill inputs, low mem calcs+sort solutions
   int ret;

   // 8 - fill the input vectors with needed data
   if (maxRp>1) {
      gprintf (gui, "Calculating new:%u values with max:%u resistors per position ...\n", numV, maxRp);
   } else {
      gprintf (gui, "Populating new:%u values with max:%u resistors per position ...\n", numV, maxRp);
   }
   ret = calcRvalues (); /* populate with all possible resistance values */
   if (ret==ERROR) {
      printf ("calcRvalues returned ERROR\n");
      free (results);
      return (ERROR);
   }

   // 9 - calculus of solutions
   gprintf (gui, "Calculating new:%u/%llu solutions with a formula of:%u resistors ...\n", numBestRes, totV, maxRc);
   ret = calcLowMemFvalues (); /* calculate best results and delta */
   if (ret==ERROR) {
      printf ("calcLowMemFvalues returned ERROR\n");
      free (results);
      return (ERROR);
   }
   if (numBestRes>totV) numBestRes=totV;
   first = totV-numBestRes; /* 0 worse, 'first' 1st printed, 'totV' best, numBestRes # of best print */

   // 10 - sorting of solutions
   gprintf (gui, "Sorting new:%llu found solutions ...\n", numBestRes);
   structQuickSort(results2LowPtr, numBestRes); /* QuickSort on new results */
   if (maxRp==2) { // 
      structQuickSort(resultsLowPtr, numBestRes); /* QuickSort on new results */
      structQuickSort(results4LowPtr, numBestRes); /* QuickSort on new results */
      structQuickSort(results3LowPtr, numBestRes); /* QuickSort on new results */
   }

   return 0;
} // int doLowMemCalc()

int freeMem() { // free memory
   // 12 - freeing dynamic allocated memory ...
   for (u32 e=0; e<numV; e++) {
      if (rValues[e].rp) free (rValues[e].rp);
   }
   if (results) free (results);
   if (maxRp==1) return (OK);
   if (rValues) free (rValues); // only with parallel/series
   return 0;
} // int freeMem()
