/* ReSolve v0.11.09h 2023/12/29 solve math expressions using discrete values*/
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

#include "reSolveLib.h"   /* LIB: include */

u08 dbgLev = DbgLv;
u08 dbgLv  = DbgLv;

char expr[LineLen] = ExpressionDefault;/* default value for formula: reversed high partitor (LM317) */
double target;        /* searched value */
u08 Eserie = Series;  /* Exx: Series E12, E24, E48, E96 or E192. Use 0 for custom list */
u08 decades = Decades; /* number of decades of interest, normally 6 or 7 */
u16 numR1 = NumR1; /* number of values in first user list */
u16 maxRp = MaxRp; /* max number of resistances supported per position: as now 1 or 2 */
u16 maxRc = MaxRc; /* number of resistances (variables) in the circuit: 2 */
u16 numR;          // number of values from both user lists OR Eseries*decades
//u16 listNumber = ListNumber; // user list quantity
u32 numV = NumV;   /* number of input possible values (all configurations), need u32 */
u64 totV = TotV;   /* number of results values to try, need u64 */
u32 numBestRes = NumberResDefault; /* number of best results to show */

char   baseE1desc[65] = "standard serie E1 @80% tolerance"; // max 64 chars
double baseE1[1] = { 1.0 }; // E1 IEC 60063

char   baseE3desc[65] = "standard serie E3 @40% tolerance"; // max 64 chars
double baseE3[3] = { 1.0, 2.2, 4.7 }; // E3 @40% tolerance

char   baseE6desc[65] = "standard serie E6 @20% tolerance"; // max 64 chars
double baseE6[6] = { 1.0, 1.5, 2.2, 3.3, 4.7, 6.8 }; // E6 @20% tolerance

char   baseE12desc[65] = "standard serie E12 @10% tolerance"; // max 64 chars
double baseE12[12] = { 1.0, 1.2, 1.5, 1.8, 2.2, 2.7,   // E12 @10% tolerance, 100ppm. 200pm below 10 Ohm. MIL-PRF55342H (1 to 22 MOhm)
                        3.3, 3.9, 4.7, 5.6, 6.8, 8.2 };

char   baseE24desc[65] = "standard serie E24 @5% (and 2%) tolerance"; // max 64 chars
double baseE24[24] = { 1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0,   // E24 @5% (and 2%) tolerance, 100ppm. 200pm below 10 Ohm. MIL-PRF55342H (1 to 22 MOhm)
                       2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3,   // M55342M06/VishayRCWPM (0705) down to 1
                       4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1 }; // M55342M06/VishayEH    50ppm (20-150k, 10-650k depend on package)

char   baseE48desc[65] = "standard serie E48 @2% tolerance"; // max 64 chars
double baseE48[48] = { 1.00, 1.05, 1.10, 1.15, 1.21, 1.27, 1.33, // E48 @2% tolerance
                       1.40, 1.47, 1.54, 1.62, 1.69, 1.78, 1.87,
                       1.96, 2.05, 2.15, 2.26, 2.37, 2.49, 2.61,
                       2.74, 2.87, 3.01, 3.16, 3.32, 3.48, 3.65,
                       3.83, 4.02, 4.22, 4.42, 4.64, 4.87, 5.11,
                       5.36, 5.62, 5.90, 6.19, 6.49, 6.81, 7.15,
                       7.50, 7.87, 8.25, 8.66, 9.09, 9.53 };

char   baseE96desc[65] = "standard serie E96 @1% tolerance"; // max 64 chars
double baseE96[96] = { 1.00, 1.02, 1.05, 1.07, 1.10, 1.13, // E96 @1% tolerance, 100ppm. MIL-PRF55342H (1 to 22 MOhm). 0.1% 15ppm ESCC4001-029/VishayTNPS
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

char   baseE192desc[65] = "standard serie E192 @0.5% (and 0.1%) tolerance"; // max 64 chars
double baseE192[192] = { 1.00, 1.01, 1.02, 1.04, 1.05, 1.06, 1.07, 1.09, // E192 @0.5 (and 0.1%) tolerance. MIL-PRF55342H (1 to 22 MOhm)
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

char    userRdesc[65]=""; // description: reserve space for 65 chars
double* userR; // declare vector pointer, will be a vector of double userR[numR1]
u32 numS; // number of single values, like plv+1
u32 pfv;  // position of first single value
u32 plv;  // position of last single value
u32 pfs;  // position of first serie
u32 pls;  // position of last serie
u32 pfp;  // position of first parallel
u32 plp;  // position of last parallel
u32 numF; // save calculated numV for memory free

u08 lists = 1;  // 1 normal, 2 use userR as low precision & userR2 as hi prec
double* userR2; // declare vector pointer, will be a vector of double userR2[numR2]
float userRtol;  // userR percent tolerance: 0.1, 1, 2, 5, 10, 20, 40
float userR2tol; // userR2 percent tolerance: 0.1, 1, 2, 5, 10, 20, 40
char userR2desc[65]; // description print: reserve space for 65 chars
u16 numR2;   // number of values in second user list
u08 bestTol; // 0 normal, 1 use userR2 as 1/10 tolerance than userR
float tolRatio; // userR2 to userR tolerance
char tol1str[3]; // userRtol tolerance as strings for showVal()
char tol2str[3]; // userR2tol tolerance as strings for showVal()

//char Exx[24]=""; // description for single result when maxRp=2: "Exx serie "
char Vdesc[][17] = { "UserList ",   // 0
                     "UserList1",   // 1  low tolerance
                     "UserList2",   // 2  hi  tolerance
                     "E1  serie",   // 3
                     "E3  serie",   // 4
                     "E6  serie",   // 5
                     "E12 serie",   // 6
                     "E24 serie",   // 7
                     "E48 serie",   // 8
                     "E96 serie",   // 9
                     "E192serie",   // 10
                     "Series of",   // 11 low + low tolerance, lists=1/2
                     "Series of",   // 12 low + hi  tolerance, lists=2
                     "Series of",   // 13 hi  + hi  tolerance, lists=2
                     "Parallel ",   // 14 low + low tolerance, lists=1/2
                     "Parallel ",   // 15 low + hi  tolerance, lists=2
                     "Parallel ",   // 16 hi  + hi  tolerance, lists=2
                   }; // Possible type of values
//static struct rValuesTy rValues[numV]; /* allocate memory for single, series & parallel */
struct rValuesTy* rValues; // pointer to rValues[numV] for single, series & parallel

//static struct resultsTy results[totV]; /* allocate memory for results: [(12*7)^2] */
struct resultsTy* results; // pointer to results[totV] for results. Ex.[(12*7)^2]

u16 valTy, resTy; // sizeof struct
u32 rValueSize; // sizeof vector of struct: rValues[numV]
u64 resultSize; // sizeof vector of struct: results[totV]
u64 allocatedB; // sizeof total allocated memory in Bytes
u32 first; // first result to show

size_t resultLowSize = 0; // size of mem low vectors resultsLow[numBestRes]
struct resultsTy* resultsLowPtr; // low mem results[numBestRes], all kind solutions
struct resultsTy* results4LowPtr; // low mem results[numBestRes], 4R solutions
struct resultsTy* results3LowPtr; // low mem results[numBestRes], 3R solutions
struct resultsTy* results2LowPtr; // low mem results[numBestRes], 2R solutions
u32 w = 0; // worst solution index
u32 w4 = 0; // worst solution index
u32 w3 = 0; // worst solution index
u32 w2 = 0; // worst solution index
double deltaWorst = MaxValue; // 50 GOhm
double delta4Worst = MaxValue; // 50 GOhm
double delta3Worst = MaxValue; // 50 GOhm
double delta2Worst = MaxValue; // 50 GOhm

u08 format = 1; // 0 scientific notation, 1 engineering notation, 2 SI prefix
bool mem = 1; // 0 use old memory hungry strategy, 1 use new mem low strategy
bool gui;    // when 1, gprintf() update the GUI
bool winGuiLoop = 1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI
int (*guiUpdateOutPtr)(char*,int); // function pointer to guiUpdateOut()
bit stop = false; // used by GUI to ask stop computations

// change current working directory to binary path to let find assets files
void chDirBin(char* argList) { // call with chDirBin(argv[0]);
   char* cwdPath;
   char absPath[PATH_MAX];
   cwdPath = malloc(PATH_MAX);
   cwdPath = getcwd(cwdPath, PATH_MAX);
   //printf("cwdPath='%s'\n", cwdPath);
   free(cwdPath);
   realpath(argList, absPath);
   //printf("realpath()='%s'\n", absPath);
   int len = strlen(absPath);
   for (int l=len; l>0; l--) {
      if (absPath[l]=='/' || absPath[l]=='\\') {
         absPath[l]='\0';
         break;
      }
   }
   //printf("absPath()='%s'\n", absPath);
   chdir(absPath);
   return;
} // chDirBin()

// gprintf(): selectable printf OR asprintf(autoalloc sprintf)
// when first parameter is 0 is like printf()
// when first parameter is not 0 is like asprintf() AND call
// functionPtr(strPtr) to update the GUI, autofree allocated string ptr
// Call with:
// gprintf(int, "asprintf str:'%s' is %d\n", text, num);
int gprintf(int gui, const char* format, ...) {
   int len;
   va_list ap;
   va_start(ap, format);
   if (gui==0) { // output to stdout like printf()
      len = vprintf(format, ap);
   } else { // output to auto allocated string like asprintf()
      char* stringPtr;
      len = vasprintf(&stringPtr, format, ap);
      // then update the GUI
      if (guiUpdateOutPtr!=NULL) (*guiUpdateOutPtr)(stringPtr, len);
      free(stringPtr);
   }
   va_end(ap);
   return len;
} // gprintf()

char* siMem(u64 sizeB) { // convert an u64 to string using SI prefix, do not require libmath
   int len = 0;
   char* stringPtr;
   if      (sizeB>1E12) len = asprintf(&stringPtr, "%.1f TB", (float)sizeB/1E12);
   else if (sizeB>1E9)  len = asprintf(&stringPtr, "%.1f GB", (float)sizeB/1E9);
   else if (sizeB>1E6)  len = asprintf(&stringPtr, "%.1f MB", (float)sizeB/1E6);
   else if (sizeB>1E3)  len = asprintf(&stringPtr, "%.1f kB", (float)sizeB/1E3);
   else                 len = asprintf(&stringPtr, "%.0f  B", (float)sizeB);
   if (len==0) return NULL;
   return stringPtr; // remember: caller must free ptr
} // siMem(u64 sizeB)

// convert a double to string using engineering notation or SI prefix, require libmath
char* engStr(double num, int significant, bool sign, bool siPref) {
   if (significant<1) return NULL;
   char* strPtr;
   //printf("num:%f ", num);
   double abs = fabs(num);
   if (abs<Epsilon) { asprintf(&strPtr, "0"); return strPtr; }
   if (abs>=1 && abs<1E3) { // no exp/SIprefix
      if (sign==false)
         asprintf(&strPtr, "%.*g", significant, num);
      else
         asprintf(&strPtr, "%+.*g", significant, num);
      return strPtr;
   }
   double exd = log10(abs); // to support negative numbers
   //printf("exd:%+03g ", exd);
   int sci = (int)floor(exd);
   //printf("sci:%+03d ", sci);
   int exp = 3*(int)floor(exd/3);
   //printf("exp:%+03d ", exp);
   int divs = pow(10, sci);
   //printf("divs:%6d ", divs);
   double dive = pow(10, exp);
   //printf("dive:%6f ", dive);
   double mant = num/dive;
   if (exp==0) mant = num;
   //printf("mant:%7g\n", mant);
   //printf("      num:'%6g' eng:'%3gE%+03d'\n", num, mant, exp);
   if (significant<3) {
      int ords = pow(10, significant);
      //printf("ords:%6d ", ords);
      mant=(int)floor(num/divs*ords/10)*10*divs/ords; // round to significant
      mant = mant/dive;
      significant = 3;
   }
   //printf("mant:%7g\n", mant);
   if (mant+0.0002>1000) { // work around for 1000E-09 instead of 1E-06 with 0.9999999E-6
      exp+=3;
      mant/=1000;
   }
   //printf("num:'%6g' eng:'%3gE%+03d'\n", num, mant, exp);
   //printf("sign:%d ", sign);
   if (siPref==false) { // engineering notation
      if (sign==false)
         asprintf(&strPtr, "%.*gE%+03d", significant, mant, exp);
      else
         asprintf(&strPtr, "%+.*gE%+03d", significant, mant, exp);
      return strPtr;
   } else { // SI prefix
      char* siPre[] = { "y", "z", "a", "f", "p", "n", MICRO, "m",
                        "", "k", "M", "G", "T", "P", "E", "Z", "Y" };
      if (exp<-24 || exp>24) return NULL;
      int s = exp/3+8;
      //printf("s:%02d ", s);
      if (sign==false)
         asprintf(&strPtr, "%.*g %s", significant, mant, siPre[s]);
      else
         asprintf(&strPtr, "%+.*g %s", significant, mant, siPre[s]);
      return strPtr;
   }
} // engStr()

int isNumber(char* strPtr, bool dotComma) { // return 1 for numbers. When dotComma=1 accept dot and comma
   if (strPtr==NULL) return ERROR;
   u16 c = 0;
   //printf("strPtr:'%s'\n", strPtr);
   while ( isdigit(strPtr[c]) || strPtr[c]=='e' || strPtr[c]=='+' || (dotComma==true && (strPtr[c]=='.' || strPtr[c]==',' ) ) ) c++;
   //printf("str:'%s' bool:%d c:%d\n", strPtr, dotComma, c);
   if (c==strlen(strPtr)) return true;
   return false;
} // isNumber()

u64 powll(u32 base, u32 exp) { /* like math.h pow() but return u64 */
    u32 c;
    u64 res = 1;
    for (c=0; c<exp; c++) {
       res = res * base;
    }
    return res;
} // u64 powll(u64 base, u64 exp)

int exprCheck() { // check expression syntax
   exprVarsParser[0]=10.0; exprVarsParser[1]=1.0;
   evalExprParser (expr); /* formula syntax test */
   exprVarsParser[0]=0.0; exprVarsParser[1]=0.0;
   return OK;
} // int exprCheck()

int updateEserie(char* EseriePtr) { // update u08 Eserie from char* EseriePtr
   //gprintf(gui, "FUNCT: %s\n", __FUNCTION__);
   if (EseriePtr==NULL) {
      printf("updateEserie() called with EseriePtr==NULL\n");
      return ERROR;
   }
   if      ( !strcmp(EseriePtr,"E1"  ) ) Eserie = 1;
   else if ( !strcmp(EseriePtr,"E3"  ) ) Eserie = 3;
   else if ( !strcmp(EseriePtr,"E6"  ) ) Eserie = 6;
   else if ( !strcmp(EseriePtr,"E12" ) ) Eserie = 12;
   else if ( !strcmp(EseriePtr,"E24" ) ) Eserie = 24;
   else if ( !strcmp(EseriePtr,"E48" ) ) Eserie = 48;
   else if ( !strcmp(EseriePtr,"E96" ) ) Eserie = 96;
   else if ( !strcmp(EseriePtr,"E192") ) Eserie = 192;
   else {
      printf("Unsupported Serie:'%s'. Supported are 1, 3, 6, 12, 24, 48, 96 and 192\n", EseriePtr);
      return ERROR;
   }
   return OK;
} // updateEserie()

int updateRdesc(bit force) { // update userRdesc from baseEdesc
   //printf("FUNCT: %s\n", __FUNCTION__);
   switch (Eserie) {
   case (0):
      //printf("FUNCT: %s Eserie:%u userRdesc:'%s'\n", __FUNCTION__, Eserie, userRdesc);
      if (strcmp(userRdesc, "")==0 || force==true)
         sprintf(userRdesc, "user list of %u values @%g%% tolerance", numR1, userRtol);
      break;
   case (1): // standard Exx series resistors
      strcpy(userRdesc, baseE1desc);
      break;
   case (3):
      strcpy(userRdesc, baseE3desc);
      break;
   case (6):
      strcpy(userRdesc, baseE6desc);
      break;
   case (12):
      strcpy(userRdesc, baseE12desc);
      break;
   case (24):
      strcpy(userRdesc, baseE24desc);
      break;
   case (48):
      strcpy(userRdesc, baseE48desc);
      break;
   case (96):
      strcpy(userRdesc, baseE96desc);
      break;
   case (192):
      strcpy(userRdesc, baseE192desc);
      break;
   default:
      printf("Unsupported Eserie:%u. Supported are 1, 3, 6, 12, 24, 48, 96 and 192\n", Eserie);
      return ERROR;
   }
   //printf("FUNCT: %s Eserie:%u userRdesc:'%s'\n", __FUNCTION__, Eserie, userRdesc);
   if (lists==2) {
      //printf("FUNCT: %s lists:%u userR2desc:'%s'\n", __FUNCTION__, lists, userR2desc);
      if (strcmp(userR2desc, "")==0 || force==true)
         sprintf(userR2desc, "user list of %u values @%g%% tolerance", numR2, userR2tol);
   }
   return OK;
} // int updateRdesc()

void showQty() { // show compute quantities
   printf("%s user list  numR1:%u @%g%% tolerance\n", __FUNCTION__, numR1, userRtol);
   printf("%s user list2 numR2:%u @%g%% tolerance\n", __FUNCTION__, numR2, userR2tol);
   printf("%s user lists numR :%u\n", __FUNCTION__, numR);
   printf("%s user lists numS :%u\n", __FUNCTION__, numS);
   printf("%s user lists numV :%u\n", __FUNCTION__, numV);
   printf("%s user lists numF :%u\n", __FUNCTION__, numF);
   printf("%s user lists totV :%llu\n", __FUNCTION__, totV);
} // showQty()

void showInPos() { // show input value positions
   printf("position of first Value   :%u\n", pfv);
   printf("position of last  Value   :%u\n", plv);
   printf("position of first Serie   :%u\n", pfs);
   printf("position of last  Serie   :%u\n", pls);
   printf("position of first Parallel:%u\n", pfp);
   printf("position of last  Parallel:%u\n", plp);
} // showInPos()

int globalInit() { // basic initialization
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   //numR = numR1;
   // calculate numR & numV & totV
   //printf("%s calculate numR & numV & totV\n", __FUNCTION__);
   if (lists==1) {
      // numR number of single resistances to try
      if (Eserie>0) {
         numR=(u32)Eserie*decades; // number of existant values of resistances
      } else { /* custom list */
         numR = numR1; // number of existant values of resistances
      }
      // numV number of values to try (single, series or parallels of numR)
      if (maxRp==1) { // single only
         //numR=numR1;
         numV=numR; /* number of possible values x each position */
      }
      if (maxRp==2) { // single, series and parallels
         //numR=numR1+numR2;
         numV=2*numR+numR*numR; /* number of possible values x each position */
      }
   } else { // lists=2, imply maxRp=2 and Eserie=0
      maxRp = 2;
      Eserie = 0;
      numR=numR1+numR2;
      numV=2*numR+numR*numR; // space for all values
   }
   // totV number of solutions: numV^2
   //printf("maxRc:%u\n", maxRc);
   if (maxRc==2) { /* always: as now 2 is the only supported value */
      totV=((u64)numV)*((u64)numV); /* number of values to try */
   }
   tolRatio = userRtol/userR2tol; // 1/0.1=10
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();

   //strcpy(Exx, "E");
   //char series[4];
   //sprintf(series, "%u", Eserie);
   //strcat(Exx, series);
   //strcat(Exx, " serie "); // 11 chars
   //printf("Eserie:%u Exx:'%s'\n", Eserie, Exx);
   updateRdesc(false);
   return OK;
} // int globalInit()

void showHead(void) {
   printf("ReSolve v%s by Valerio Messina %s\n", ReSolveVer, WebLink);
   printf("\n");
}

void showHelp() {
   //showHead();
   /*printf("Syntax: reSolve targetFloatValue [numberOfResults]\n");*/
   printf("Syntax: reSolve -h|--help\n");
   printf("Syntax: reSolve [targetFloatValue ['function(a,b)' [numberOfResults]]]\n");
   printf("\n");
   printf("if (targetFloatValue==NULL)  targetFloatValue=%d\n", TargetDefault);
   printf("if (function==NULL)          function=%s\n", ExpressionDefault);
   printf("if (numberOfResults==NULL)   numberOfResults=%u\n", NumberResDefault);
   printf("functions support: numbers, variables a-b, operators +,-,*,/,%%,^,(,) any depth\n");
   printf("Note: use '.' as decimal separators for costants and targetFloatValue\n");
   printf("\n");
} // showHelp()

int showConf() { // show config set
   gprintf(gui, "Current configurations from 'reSolveConf.txt':\n");
   gprintf(gui, "Solve Formula: %s\n", expr);
   gprintf(gui, "Target Value : %11.5f\n", target);
   //printf("sizeof (rValuesTy):%u\n", valTy);
   //printf("sizeof (rValues[%lu]):%lu\n", numV, val);
   //printf("sizeof (resultsTy):%u\n", resTy);
   //printf("sizeof (results[%lu]):%lu\n", totV, res);
   if (lists==2) {
      gprintf(gui, "Custom lists number: %u\n", lists);
      gprintf(gui, "Tolerance ratio: %g\n", tolRatio);
      gprintf(gui, "Keep only values of best tolerance: ");
      if (bestTol) gprintf(gui, "TRUE\n");
      else gprintf(gui, "FALSE\n");
   }
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   //gprintf(gui, "numR2     :%u\n", numR2);
   if (Eserie>0) {
      gprintf(gui, "Resistors serie:E%u, Decades:%u\n", Eserie, decades);
      gprintf(gui, "Description:'%s'\n", userRdesc);
   } else { // user list OR lists=2
      gprintf(gui, "Resistors user list of %u values @%g%% tolerance\n", numR1, userRtol);
      gprintf(gui, "Description:'%s'\n", userRdesc);
      if (lists==2) {
         gprintf(gui, "Resistors user list2 of %u values @%g%% tolerance\n", numR2, userR2tol);
         gprintf(gui, "Description2:'%s'\n", userR2desc);
      }
   }
   //if (Eserie>0) gprintf(gui, " with %u decades", decades);
   //gprintf(gui, "\n");
   //if (lists==2) {
   //}
   gprintf(gui, "%u possible values with %u resistors per position\n", numV, maxRp);
   gprintf(gui, "Show the best %u results on total of %u results\n", numBestRes, totV);
   //gprintf(gui, "numR:%u, numV:%u, totV:%llu\n", numR, numV, totV);
   if (format==0)      gprintf(gui, "Output with scientific notation\n");
   else if (format==1) gprintf(gui, "Output with engineering notation\n");
   else                gprintf(gui, "Output with SI prefix notation\n");
   if (mem==0) // 0 use old memory hungry strategy
      gprintf(gui, "Using the old memory hungry strategy\n");
   else // 1 use new mem low strategy
      gprintf(gui, "Using the memory save strategy\n");
   //gprintf(gui, "Will allocate a total RAM of %llu B\n", allocatedB);
   char* stringPtr;
   stringPtr = siMem(allocatedB);
   gprintf(gui, "Will allocate about %s of total RAM\n", stringPtr);
   if (allocatedB>200E6) gprintf(gui, "WARN: may allocate more than 200 MB RAM !\n");
   return OK;
} // showConf()

int fillConfigVars(void) { // load and check users config file
   char fileConf[]="reSolveConf.txt";
   char* bufferPtr;
   off_t bufSize;
   char paramName[LineLen];
   char paramValue[LineLen];
   int  ret;
   char* endPtr;
   u16 size;
   uintptr_t hex;
   double* vectorPtr;

   bufSize = readFile(fileConf, &bufferPtr);
   if (bufSize==ERROR) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot read file:%s\n", __FUNCTION__, fileConf);
      return ERROR;
   }
   if (bufferPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: returned NULL pointer\n", __FUNCTION__);
      return ERROR;
   }
   //printf("bufSize=%lld\n", (s64)bufSize);
   //printf("bufferPtr:'\n%s\n':bufferPtr\n", bufferPtr);
   //printf("\n");

   // reading user configured parameters ...
   strcpy(paramName, "expr");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("expr='%s'\n", paramValue);
   int len = strlen(paramValue);
   if (len>=LineLen) {
      printf("Parameter:'%s' len:%u, max supported len:%u\n", paramName, len, LineLen);
      return ERROR;
   }
   endPtr = strcpy(expr, paramValue);
   if (endPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("expr='%s'\n", expr);

   strcpy(paramName, "target");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("target='%s'\n", paramValue);
   target = strtod(paramValue, &endPtr);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in target='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("target=%g\n", target);

   strcpy(paramName, "Eserie");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("Eserie='%s'\n", paramValue);
   Eserie = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in Eserie='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("Eserie=%u\n", Eserie);

   strcpy(paramName, "decades");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("decades='%s'\n", paramValue);
   decades = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in decades='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("decades=%u\n", decades);

#if 0
   strcpy(paramName, "listNumber");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      return ERROR;
   }
   //printf("listNumber='%s'\n", paramValue);
   listNumber = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in listNumber='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("listNumber=%u\n", listNumber);
#endif

   strcpy(paramName, "userR");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("paramValue='%s'\n", paramValue);
   /*for (u16 p=0; p<16; p++) {
      if (dbgLev>=PRINTDEBUG) printf("@:%p paramValue[%u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
   }*/
   if (dbgLev>=PRINTDEBUG) printf("paramValue[19-24]='%s'\n", &paramValue[19]);
   sscanf (&paramValue[19], "0x%04hx", &size); // extract size of userR
   if (dbgLev>=PRINTDEBUG) printf("size:0x%04x=%u\n", size, size);
   if (dbgLev>=PRINTDEBUG) printf("array size:%u=0x%x\n", size, size);
   //if (numR1!=size) {
      //if (dbgLev>=PRINTERROR) printf("ERROR %s: numR1:%u <> userR[size]:%u in config file\n", __FUNCTION__, numR1, size);
      //return ERROR;
   //}
   paramValue[18]='\0'; // clear comma/size overlapping with string terminator
   for (int p=0; p<=24; p++) {
      if (dbgLev>=PRINTDEBUG) printf("@:%p paramValue[%02u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
   }
   if (dbgLev>=PRINTDEBUG) printf("@:%p paramValue[25]=0x%02x='%c'\n", &paramValue[25], paramValue[25], paramValue[25]);
   sscanf (paramValue, "0x%16zx", &hex); // extract address of config userR
   if (dbgLev>=PRINTDEBUG) printf("hex:%zu=0x%16zx\n", hex, hex);
   vectorPtr=(void*)(size_t)hex; // recover the values vector address
   userR=malloc(size*sizeof(double)); // allocate space for custom values
   if (!userR) {
      printf("Dynamic allocation of:%zu Bytes failed:0x%8p\n", size*sizeof(double), userR);
      return ERROR;
   }
   for (u16 p=0; p<size; p++) {
      if (dbgLev>=PRINTDEBUG) printf("vectorPtr[%u]:%g\n", p, vectorPtr[p]);
      userR[p]=vectorPtr[p]; // copy custom values between vectors
   }
   free(vectorPtr); // free config values vector address
   numR1 = size;
   // now can use global userR[numR1] for custom values

   strcpy(paramName, "userRtol");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("userRtol='%s'\n", paramValue);
   userRtol = strtod(paramValue, &endPtr);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in userRtol='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("userRtol=%g\n", userRtol);

   strcpy(paramName, "userRdesc");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("userRdesc='%s'\n", paramValue);
   endPtr = strcpy(userRdesc, paramValue);
   if (endPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("userRdesc='%s'\n", userRdesc);

   strcpy(paramName, "numberOfResults");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("numberOfResults='%s'\n", paramValue);
   u64 tmpNum = strtol(paramValue, &endPtr, 10);
   u64 tmpMax=NumberResMax; // 512000U
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (tmpNum>tmpMax) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: '%s'=%llu > %llu\n", __FUNCTION__, paramName, tmpNum, tmpMax);
      return ERROR;
   }
   numBestRes = tmpNum;
   if (dbgLev>=PRINTDEBUG) printf("numberOfResults='%u'\n", numBestRes);

   strcpy(paramName, "maxRp");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("maxRp='%s'\n", paramValue);
   maxRp = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in maxRp='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("maxRp=%u\n", maxRp);

   strcpy(paramName, "format");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("format='%s'\n", paramValue);
   format = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in format='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("format=%u\n", format);

   strcpy(paramName, "mem");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("mem='%s'\n", paramValue);
   mem = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in mem='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("mem=%u\n", mem);

#if 0
   strcpy(paramName, "maxRc");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("maxRc='%s'\n", paramValue);
   maxRc = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in maxRc='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("maxRc=%u\n", maxRc);
#endif

   strcpy(paramName, "lists");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("lists='%s'\n", paramValue);
   lists = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in lists='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("lists=%u\n", lists);

   strcpy(paramName, "userR2"); // second list of custom values
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("paramValue='%s'\n", paramValue);
   /*for (u16 p=0; p<16; p++) {
      if (dbgLev>=PRINTDEBUG) printf("@:%p paramValue[%u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
   }*/
   if (dbgLev>=PRINTDEBUG) printf("paramValue[19-24]='%s'\n", &paramValue[19]);
   sscanf (&paramValue[19], "0x%04hx", &size); // extract size of userR
   if (dbgLev>=PRINTDEBUG) printf("size:0x%04x=%u\n", size, size);
   if (dbgLev>=PRINTDEBUG) printf("array size:%u=0x%x\n", size, size);
   //if (numR2!=size) {
      //if (dbgLev>=PRINTERROR) printf("WARN %s: numR2:%u <> userR[size]:%u in config file\n", __FUNCTION__, numR2, size);
      //return ERROR;
   //}
   paramValue[18]='\0'; // clear comma/size overlapping with string terminator
   for (int p=0; p<=24; p++) {
      if (dbgLev>=PRINTDEBUG) printf("@:%p paramValue[%02u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
   }
   if (dbgLev>=PRINTDEBUG) printf("@:%p paramValue[25]=0x%02x='%c'\n", &paramValue[25], paramValue[25], paramValue[25]);
   sscanf (paramValue, "0x%16zx", &hex); // extract address of config userR2
   if (dbgLev>=PRINTDEBUG) printf("hex:%zu=0x%16zx\n", hex, hex);
   vectorPtr=(void*)(size_t)hex; // recover the values vector address
   userR2 = malloc(size*sizeof(double)); // allocate space for custom values
   if (!userR) {
      printf("Dynamic allocation of:%zu Bytes failed:0x%8p\n", size*sizeof(double), userR);
      return ERROR;
   }
   for (u16 p=0; p<size; p++) {
      if (dbgLev>=PRINTDEBUG) printf("vectorPtr[%u]:%g\n", p, vectorPtr[p]);
      userR2[p]=vectorPtr[p]; // copy custom values between vectors
   }
   free(vectorPtr); // free config values vector address
   numR2 = size;
   // now can use global userR2[numR2] for custom values

   strcpy(paramName, "userR2tol");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("userR2tol='%s'\n", paramValue);
   userR2tol = strtod(paramValue, &endPtr);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in userR2tol='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("userR2tol=%g\n", userR2tol);

   strcpy(paramName, "userR2desc");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("userR2desc='%s'\n", paramValue);
   endPtr = strcpy(userR2desc, paramValue);
   if (endPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot copy paramValue='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("userR2desc='%s'\n", userR2desc);

   strcpy(paramName, "bestTol");
   ret = parseConf (bufferPtr, paramName, paramValue);
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find:%s in config file\n", __FUNCTION__, paramName);
      free(bufferPtr);
      return ERROR;
   }
   //printf("bestTol='%s'\n", paramValue);
   bestTol = strtol(paramValue, &endPtr, 10);
   if (endPtr==paramValue) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: cannot find digits in bestTol='%s'\n", __FUNCTION__, paramValue);
      return ERROR;
   }
   if (dbgLev>=PRINTDEBUG) printf("bestTol=%u\n", bestTol);

   //dbgLev=PRINTF;
   //printf("Freeing buffer ...\n");
   free(bufferPtr);

   // check supported user request -------------------------------------------
   switch (Eserie) { // values: 1, 3, 6, 12, 24, 48, 96, 192. Use 0 for custom list
   case 0: case 1: case 3: case 6: case 12: case 24: case 48: case 96: case 192:
      if (dbgLev>=PRINTDEBUG) printf("supported Eserie\n");
      break;
   default:
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported Eserie:%u\n", __FUNCTION__, Eserie);
      return ERROR;
   }
   if (decades==0 || decades>8) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported decades:%u\n", __FUNCTION__, decades);
      return ERROR;
   }
   if (numR1==0 || numR1>1536) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported numR1:%u\n", __FUNCTION__, numR1);
      return ERROR;
   }
   if (maxRp==0 || maxRp>2) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported maxRp:%u\n", __FUNCTION__, maxRp);
      return ERROR;
   }
   if (maxRc<2 || maxRc>2) { // as now only two variables are supported
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported maxRc:%u\n", __FUNCTION__, maxRc);
      return ERROR;
   }
   //if (mem>1) {
   //   if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported mem:%u\n", __FUNCTION__, mem);
   //   return ERROR;
   //}
   if (userRdesc[0]=='\0') {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: empty string 'userRdesc'\n", __FUNCTION__);
      return ERROR;
   }
   for (u16 p=0; p<numR1; p++) {
      //if (dbgLev>=PRINTF) printf("%g, ", userR[p]);
      if (userR[p]==0) {
         if (dbgLev>=PRINTERROR) printf("ERROR %s: value 0 in 'userR' not allowed\n", __FUNCTION__);
         return ERROR;
      }
   }
   u08 userRtolInt = (u08)lround(userRtol);
   if (userRtol==0.5) userRtolInt = 50;
   switch (userRtolInt) {
   case 80: case 40: case 20: case 10: case 5: case 2: case 1: case 50: case 0:
      if (dbgLev>=PRINTDEBUG) printf("supported userRtol\n");
      break;
   default:
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported userRtol:%g\n", __FUNCTION__, userRtol);
      return ERROR;
   }
   u08 userR2tolInt = (u08)lround(userR2tol);
   if (userR2tol==0.5) userR2tolInt = 50;
   switch (userR2tolInt) {
   case 80: case 40: case 20: case 10: case 5: case 2: case 1: case 50: case 0:
      if (dbgLev>=PRINTDEBUG) printf("supported userR2tol\n");
      break;
   default:
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported userR2tol:%g\n", __FUNCTION__, userR2tol);
      return ERROR;
   }
   if (bestTol>1) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported bestTol:%u\n", __FUNCTION__, bestTol);
      return ERROR;
   }
   if (lists==0 || lists>2) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported lists:%u\n", __FUNCTION__, lists);
      return ERROR;
   }
   if (numR2==0 || numR2>1536) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: unsupported numR2:%u\n", __FUNCTION__, numR2);
      return ERROR;
   }
   if (userR2desc[0]=='\0') {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: empty string 'userR2desc'\n", __FUNCTION__);
      return ERROR;
   }
   for (u16 p=0; p<numR2; p++) {
      //if (dbgLev>=PRINTF) printf("%g, ", userR[p]);
      if (userR2[p]==0) {
         if (dbgLev>=PRINTERROR) printf("ERROR %s: value 0 in 'userR2' not allowed\n", __FUNCTION__);
         return ERROR;
      }
   }
   return  (OK);
} // int fillConfigVars()

int memInpCalc() { // memory size calculation for input values
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   // 3 - calculate the needed memory
   valTy = sizeof(struct rValuesTy);
   rValueSize = numV*valTy; // expected no more than '1536x48=73728'
   if (dbgLev>=PRINTDEBUG) printf("numV:%u X valTy:%u = rValueSize:%u\n", numV, valTy, rValueSize);
   //printf("calcRvalues numR:%d\n", numR);
   //printf("calcRvalues numV:%d\n", numV);
   char* stringPtr = siMem(rValueSize);
   //printf("Will allocate about %s of RAM for inputs ...\n", stringPtr);
   free(stringPtr);
   return OK;
} // int memInpCalc()

int memResCalc() { // memory size calculation for results
   char* stringPtr;
   // 3 - calculate the needed memory
   resTy = sizeof(struct resultsTy);
   if (mem==0) {
      resultSize=(u64)totV*resTy;
      if (dbgLev>=PRINTDEBUG) printf("totV:%llu X resTy:%u = resultSize:%llu\n", totV, resTy, resultSize);
      //printf("Will allocate about %f MB of RAM for val\n", rValueSize/1048576.0);
      //printf("size of struct resultsTy:%u\n", resTy);
      //printf("Will allocate about %f MB of RAM for res\n", resultSize/1048576.0);
      stringPtr = siMem(resultSize);
      allocatedB = rValueSize+resultSize;
      //printf("will allocateMB:'%.1f'\n", allocatedB);
   } else { // low mem size
      if (maxRp==1) {
         resultLowSize = numBestRes*resTy;
      }
      if (maxRp==2) {
         resultLowSize = 4*numBestRes*resTy;
      }
      //printf("will allocate low mem: structSize:%u numBestRes:%u size:%zu\n", resTy, numBestRes, resultLowSize);
      stringPtr = siMem(resultLowSize);
      allocatedB=rValueSize+resultLowSize;
      //printf("Will allocate about %llu B of total RAM for %llu solutions\n", allocatedB, totV);
   }
   //printf("allocat:'%s'\n", stringPtr);
   //printf("Will allocate about %s of RAM for results ...\n", stringPtr);
   free(stringPtr);
   //printf("Will allocate a total RAM of %llu B, with %llu solutions\n", allocatedB, totV);
   return OK;
} // int memResCalc()

int memInpAlloc() { // memory allocation for input values
   // 5 - allocate the memory asking to the OS a malloc()
   /* allocated memory for all R: [12*7]+([12*7]*[12*7])+[12*7] : */
   //struct rValuesTy rValues[numV]; /* single, series & parallel */
   //struct rValuesTy* rValues; /* pointer to memory for single, series & parallel */
   char* stringPtr = siMem(rValueSize);
   //printf("allocat:'%s'\n", stringPtr);
   gprintf(gui, "Allocating about %s of RAM for inputs\n", stringPtr);
   free(stringPtr);
   rValues = malloc(rValueSize); // numV*sizeof(struct rValuesTy)
   // allocation fail with 2'930'000'000 on a 4 GB RAM, 8 GB swap, 32 bit OS
   if (!rValues) {
      printf("Dynamic allocation of:%u Bytes failed:0x%8p\n", rValueSize, rValues);
      return ERROR;
   }
   // 5 - create the structure's vector inside the allocated memory
   for (u32 e=0; e<numV; e++) {
      rValues[e].rp = (double*) malloc(maxRp*sizeof (double));
      if (!rValues[e].rp) {
         printf("Dynamic allocation of:%zu Bytes failed:0x%8p\n", maxRp*sizeof (double), rValues[e].rp);
         return ERROR;
      }
   }
   // now can use global rValues[].rp[], rValues[].r for input values
   return OK;
} // memInpAlloc()

int memResAlloc() { // memory allocation for results
   char* stringPtr=NULL;
   if (mem==0) {
      stringPtr = siMem(resultSize);
      gprintf(gui, "Allocating about %s of RAM for results\n", stringPtr);
      free(stringPtr);
      //printf("size_t size:%u Bytes ptr:%lu\n", sizeof (size_t), (u32) pow(2, 8*sizeof (size_t)));
      if (resultSize > pow(2, 8*sizeof (size_t))) {
         printf("ERROR: This machine cannot handle allocation of:%llu Bytes\n", resultSize);
         return ERROR;
      }
      results = malloc(resultSize); // (u64)totV*sizeof(struct resultsTy)
      if (dbgLev>=PRINTDEBUG) printf("allocated %llu Bytes at:0x%8p\n", resultSize, results);
      if (!results) {
         printf("Dynamic allocation of:%llu Bytes failed:0x%8p\n", resultSize, results);
         return ERROR;
      }
      // now can use global results[].pos[], results[].delta for results
      if (dbgLev>=PRINTDEBUG) printf("Done. Dynamically allocated 'results' at:%p\n", results);
   } else { // low mem size
      size_t size, structSize;
      structSize = sizeof(struct resultsTy);
      size = numBestRes*structSize;
      if (maxRp==1) {
         stringPtr = siMem(size);
         //printf("Allocating low mem: structSize:%zu numBestRes:%u size:%zu for results ...\n", structSize, numBestRes, size);
         gprintf(gui, "Allocating about %s of RAM for results\n", stringPtr);
         results2LowPtr = malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
         if (results2LowPtr==NULL) {
            printf("malloc error, quit\n");
            return ERROR;
         }
         //printf("Allocated low mem: structSize:%zu numBestRes:%u size:%zu for results ...\n", structSize, numBestRes, size);
      }
      if (maxRp==2) {
         stringPtr = siMem(4*size);
         //printf("Allocating low mem: structSize:%zu numBestRes:%u 4size:%zu for results ...\n", structSize, numBestRes, 4*size);
         gprintf(gui, "Allocating about %s of RAM for results\n", stringPtr);
         resultsLowPtr = malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
         if (resultsLowPtr==NULL) {
            printf("malloc error, quit\n");
            return ERROR;
         }
         results4LowPtr = malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
         if (results4LowPtr==NULL) {
            printf("malloc error, quit\n");
            return ERROR;
         }
         results3LowPtr = malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
         if (results3LowPtr==NULL) {
            printf("malloc error, quit\n");
            return ERROR;
         }
         results2LowPtr = malloc(size); // (u64)numBestRes*sizeof(struct resultsTy)
         if (results2LowPtr==NULL) {
            printf("malloc error, quit\n");
            return ERROR;
         }
         //printf("Allocated low mem: structSize:%zu numBestRes:%u size:%zu 4size:%zu for results ...\n", structSize, numBestRes, size, 4*size);
         //printf("resultsLowPtr :%p\n", resultsLowPtr);
         //printf("results4LowPtr:%p dist:%td dec:%llu\n", results4LowPtr, results4LowPtr-resultsLowPtr, (u64)results4LowPtr-(u64)resultsLowPtr);
         //printf("results3LowPtr:%p dist:%td dec:%llu\n", results3LowPtr, results3LowPtr-results4LowPtr, (u64)results3LowPtr-(u64)results4LowPtr);
         //printf("results2LowPtr:%p dist:%td dec:%llu\n", results2LowPtr, results2LowPtr-results3LowPtr, (u64)results2LowPtr-(u64)results3LowPtr);
      }
      free(stringPtr);
   }
   return OK;
} // int memResAlloc()

/* using one decade values and 'decades', calculate all standard Ex series values */
// 'baseExx[Eserie/numR1]' ==> rValues[pos].r, rValues[pos].rp[p], rValues[pos].descIdx
s16 calcEserie(void) { // or fill with the custom list of values when Eserie=0
   u08 decade;
   u08 rBase;
   s16 pos = 0; // expected max 7*192=1344
   double val;
   u08 p;
   u08 descIdx;
   if (Eserie>0) { // standard Exx series resistors
      for (decade=0; decade<decades; decade++) {
         double powDecade = powll(10, decade);
         for (rBase=0; rBase<Eserie; rBase++) {
            pos = rBase + decade*Eserie;
            switch (Eserie) {
            case (1):
               /*printf("rBase:%u baseE1[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE1[rBase], decade, pos);*/
               val = baseE1[rBase] * powDecade;
               descIdx = 3;
               break;
            case (3):
               /*printf("rBase:%u baseE3[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE3[rBase], decade, pos);*/
               val = baseE3[rBase] * powDecade;
               descIdx = 4;
               break;
            case (6):
               /*printf("rBase:%u baseE6[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE6[rBase], decade, pos);*/
               val = baseE6[rBase] * powDecade;
               descIdx = 5;
               break;
            case (12):
               /*printf("rBase:%u baseE12[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE12[rBase], decade, pos);*/
               val = baseE12[rBase] * powDecade;
               descIdx = 6;
               break;
            case (24):
               /*printf("rBase:%u baseE24[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE24[rBase], decade, pos);*/
               val = baseE24[rBase] * powDecade;
               descIdx = 7;
               break;
            case (48):
               /*printf("rBase:%u baseE48[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE48[rBase], decade, pos);*/
               val = baseE48[rBase] * powDecade;
               descIdx = 8;
               break;
            case (96):
               /*printf("rBase:%u baseE96[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE96[rBase], decade, pos);*/
               val = baseE96[rBase] * powDecade;
               descIdx = 9;
               break;
            case (192):
               /*printf("rBase:%u baseE192[rBase]:%lu decade:%u pos:%ld\n", rBase, baseE192[rBase], decade, pos);*/
               val = baseE192[rBase] * powDecade;
               descIdx = 10;
               break;
            default:
               printf("Unsupported Series:%u. Supported are 1, 3, 6, 12, 24, 48, 96 and 192\n", Eserie);
               return ERROR;
            }
            rValues[pos].r = val;
            for (p=0; p<maxRp; p++) {
               rValues[pos].rp[p] = val; // to simplify showVal()
            }
            //strcpy(rValues[pos].descIdx, "Exx series"); // 11 chars
            //strcpy(rValues[pos].descIdx, Exx); // description
            rValues[pos].descIdx = descIdx; // 'EXXXserie' description
            if (dbgLv>=PRINTDEBUG) printf("rValue:%g\n", val);
         }
      }
   } else { // Eserie=0 custom list of values
      if (lists==1) {
         //printf("calcEserie numR:%d\n", numR);
         for (pos=0; pos<numR; pos++) { // populate with custom values
            val = userR[pos];
            rValues[pos].r = val;
            for (p=0; p<maxRp; p++) {
               rValues[pos].rp[p] = val; // to simplify showVal()
            }
            //strcpy(rValues[pos].desc, "Custom list"); // 11 chars
            rValues[pos].descIdx = 0; // 'UserList ' description
            if (dbgLv>=PRINTVERBOSE) printf("rValue:%g\n", val);
         }
      } else { // lists=2
         //printf("calcEserie numR:%d\n", numR);
         for (pos=0; pos<numR; pos++) { // populate with custom values
            if (pos<numR2) {
               val = userR2[pos]; // fill with hi precision custom values for lists=2
               //strcpy(rValues[pos].desc, "CustomList2"); // 11 chars
               rValues[pos].descIdx = 2; // 'UserList2' description
            } else {
               val = userR[pos-numR2];
               //strcpy(rValues[pos].desc, "CustomList1"); // 11 chars
               rValues[pos].descIdx = 1; // 'UserList1' description
            }
            rValues[pos].r = val;
            for (p=0; p<maxRp; p++) {
               rValues[pos].rp[p] = val; // to simplify showVal()
            }
            if (dbgLv>=PRINTVERBOSE) printf("rValue:%g\n", val);
         }
      }
      pos--; // so point to last filled position
   }
   if (dbgLv>=PRINTDEBUG) printf("calcEserie pos:%d\n", pos);
   //printf("calcEserie pos:%d\n", pos);
   //printf("calcEserie numR:%d\n", numR);
   //printf("calcEserie numR1:%d\n", numR1);
   numS = pos+1;
   return pos; // last filled position
} // s16 calcEserie(void)

void showEserie() { // show all Eserie resistor values
   printf("Resistor values (Eserie/custom):\n");
   for (u16 pos=0; pos<numS; pos++) {
      printf("pos:%4u", pos);
      printf(" r:%8.2f", rValues[pos].r);
      /*printf(" rp[0]:%8.0f", rValues[pos].rp[0]);
      for (u08 p=1; p<maxRp; p++) {
         printf(" rp[%u]:%8.0f", p, rValues[pos].rp[p]);
      }*/
      printf(" desc:'%s'\n", Vdesc[rValues[pos].descIdx]);
   }
} // void showEserie()

/* calculate all possible resistive values using 'MaxRp' resistances */
// 'baseExx[Eserie/numR1]' ==> rValues[pos].r, rValues[pos].rp[p], rValues[pos].desc
// use pre-calculated [numR] ==> rValues[numV]
int calcRvalues(void) { /* lists=1, when MaxRp=2 also in series and parallel */
   u32 pos;
   double val;
   u16 rp1, rp2;
   u08 p;
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   pfv = 0; // position of first single value
   /* at first calculate the single values (Eserie) or fill custom values */
   pos = calcEserie(); // calculate all standard Exx series values
   if (pos==ERROR) {
      printf("calcEserie returned ERROR\n");
      return ERROR;
   }
   //printf("numR:%u numV:%u totV:%llu numBestRes:%u\n", numR, numV, totV, numBestRes);
   if (dbgLv>=PRINTDEBUG) printf("calcRvalues pos:%d\n", pos);
   plv = pos; // position of last single value
   //showEserie();
   if (maxRp==2) { // as now support only maxRp=2 resistances per position
      /* now calculate all series values with 2 resistances per position */
      //printf("calcRvalues pos:%d numR:%d\n", pos, numR);
      pos++; // first serie, pos = numR;
      pfs = pos; // position of fist serie
      for (rp1=0; rp1<numR; rp1++) {
         for (rp2=0; rp2<=rp1; rp2++) { /* skip triangular matrix */
            val = rValues[rp1].r + rValues[rp2].r;
            rValues[pos].r = val;
            rValues[pos].rp[0] = rValues[rp1].r;
            for (p=1; p<maxRp; p++) {
               rValues[pos].rp[p] = rValues[rp2].r;
            }
            //strcpy(rValues[pos].desc, "Series   of"); // 11 chars
            rValues[pos].descIdx = 11; // 'Series of' description
            /*printf("r1:%g + r2:%g = ", rValues[rp1].r, rValues[rp2].r);
            printf("rValue:%g\n", val);*/
            //printf("rp1:%u rp2:%u pos:%u\n", rp1, rp2, pos);
            pos++;
         }
      }
      if (dbgLv>=PRINTDEBUG) printf("calcRvalues pos:%d\n", pos);
      pls = pos-1; // position of last serie
      pfp = pos; // position of first parallel
      /* now calculate all parallel values with 2 resistances per position */
      for (rp1=0; rp1<numR; rp1++) {
         for (rp2=0; rp2<=rp1; rp2++) { /* skip triangular matrix */
            val = rValues[rp1].r * rValues[rp2].r / (rValues[rp1].r + rValues[rp2].r);
            rValues[pos].r = val;
            rValues[pos].rp[0] = rValues[rp1].r;
            for (p=1; p<maxRp; p++) {
               rValues[pos].rp[p] = rValues[rp2].r;
            }
            //strcpy(rValues[pos].desc, "Parallel of"); // 11 chars
            rValues[pos].descIdx = 14; // 'Parallel ' description
            /*printf("r1:%g // r2:%g = ", rValues[rp1].r, rValues[rp2].r);
            printf("rValue:%g\n", val);*/
            pos++;
         }
      }
      pos--;
      plp = pos; // position of last parallel
   }
   if (dbgLv>=PRINTDEBUG) printf("calcRvalues pos:%d end\n", pos);
   //printf("calcRvalues numV:%d\n", numV);
   //showInPos();
   //showRvalues();
   numF=numV; // save calculated numV for memory free
   if (maxRp>2) {
      printf("Unsupported maxRp:%u. Supported maxRp 1 or 2\n", maxRp);
      return ERROR;
   }
   return OK;
} // int calcRvalues(void)

/* calculate all possible resistive values using 'MaxRp' resistances */
// when lists=2, series: skip when 10*R1%>R0.1%, parallel: skip when 10*R1%<R0.1%
// 'baseExx[Eserie/numR1]' ==> rValues[pos].r, rValues[pos].rp[p], rValues[pos].desc
// use pre-calculated [numR] ==> rValues[numV]
int calcR2values() { // lists=2, bestTol=1: using userR[]:1%+userR2[]:0.1% and R1%//R0.1%
   // valid only for: lists=2, Eserie=0, maxRp==2
   u32 pos;
   double val, v1, v2;
   u16 rp1, rp2;
   u08 p;
   u08 descIdx;
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   pfv = 0; // position of first single value
   //if (Eserie!=0) Eserie = 0; // at first fill custom values
   pos = calcEserie(); // fill with hi precision custom values for lists=2
   if (pos==ERROR) {
      printf("calcEserie returned ERROR\n");
      return ERROR;
   }
   //printf("calcRvalues pos:%d numR2:%d\n", pos, numR2);
   if (dbgLv>=PRINTDEBUG) printf("calcR2values pos:%d\n", pos);
   plv = pos; // position of last single value
   //showEserie();
   pos = numR2; // so keep Custom list2 and overwrite Custom list1 @1%
   numS = pos; // number of single values
   plv = pos-1; // position of last single value after overwrite
   pfs = pos; // position of first serie after overwrite
   // now calculate all series values: R1%+R0.1%
   for (rp1=0;rp1<numR;rp1++) { // 1% tolerance
      for (rp2=0;rp2<=rp1;rp2++) { // 0.1% tolerance
         //printf("rp1:%u rp2:%u pos:%u\n", rp1, rp2, pos);
         if (rp1<numR2) {
            v1 = userR2[rp1];      // hi precision
            descIdx = 13;
         } else {
            v1 = userR[rp1-numR2]; // low precision
            descIdx = 11;
         }
         if (rp2<numR2) {
            v2 = userR2[rp2];      // hi precision
            if (descIdx==11) descIdx = 12;
         } else {
            v2 = userR[rp2-numR2]; // low precision
            descIdx = 11;
         }
         //printf("v2:%f v1:%f\n", v2, v1);
         if ((bestTol==1 && rp2>=numR2) ||
             (bestTol==1 && rp1>=numR2 && tolRatio*v1>v2)) { // series: skip when 10*R1%>R0.1%
            //val = 0; v1 = 0; v2 = 0;
            continue;
         } //else
         val = v1 + v2; // low + hi precision
         rValues[pos].r = val;
         rValues[pos].rp[0] = v1;
         for (p=1; p<maxRp; p++) {
            rValues[pos].rp[p] = v2;
         }
         //strcpy(rValues[pos].desc, "Series   of"); // 11 chars
         rValues[pos].descIdx = descIdx; // 'Series of' description
         /*printf("r1:%g + r2:%g = ", rValues[rp1].r, rValues[rp2].r);
         printf("rValue:%g\n", val);*/
         //printf("rp1:%u rp2:%u pos:%u\n", rp1, rp2, pos);
         //if (val==0) continue;
         pos++;
      }
   }
   if (dbgLv>=PRINTDEBUG) printf("calcR2values pos:%d\n", pos);
   pls = pos-1; // position of last serie
   pfp = pos; // position of first parallel
   // now calculate all parallel values: R1%//R0.1%
   for (rp1=0;rp1<numR;rp1++) { // 1% tolerance
      for (rp2=0;rp2<=rp1;rp2++) { // 0.1% tolerance
         //printf("rp1:%u rp2:%u pos:%u\n", rp1, rp2, pos);
         if (rp1<numR2) {
            v1 = userR2[rp1];      // hi precision
            descIdx = 16;
         } else {
            v1 = userR[rp1-numR2]; // low precision
            descIdx = 14;
         }
         if (rp2<numR2) {
            v2 = userR2[rp2];      // hi precision
            if (descIdx==14) descIdx = 15;
         } else {
            v2 = userR[rp2-numR2]; // low precision
            descIdx = 14;
         }
         //printf("rp1:%05u v2:%8g v1:%8g\n", rp1, v2, v1);
         if ((bestTol==1 && rp2>=numR2) ||
             (bestTol==1 && rp1>=numR2 && v1<tolRatio*v2)) { // parallel: skip when R1%<10*R0.1%
            //val = 0; v1 = 0; v2 = 0;
            continue;
         } //else
         val = v1 * v2 / (v1 + v2); // low // hi precision
         rValues[pos].r = val;
         rValues[pos].rp[0] = v1;
         for (p=1; p<maxRp; p++) {
            rValues[pos].rp[p] = v2;
         }
         //strcpy(rValues[pos].desc, "Parallel of"); // 11 chars
         rValues[pos].descIdx = descIdx; // 'Parallel ' description
         /*printf("r1:%g // r2:%g = ", rValues[rp1].r, rValues[rp2].r);
         printf("rValue:%g\n", val);*/
         //if (val==0) continue;
         pos++;
      }
   }
   plp = pos-1; // position of last parallel
   //printf("calcR2values pos:%d end\n", pos);
   //showInPos();
   //showRvalues();
   gprintf(gui, "Found %d high precision values\n", pos);
   if (bestTol==1) gprintf(gui, "Discarding %u low precision values: skip ++ %g*R1%%>R0.1%%, // R1%%<%g*R0.1%%\n", numV-pos, tolRatio, tolRatio);
   numF=numV; // save calculated numV for memory free
   numV=pos; // reduce to the real found values
   totV=numV*numV; // reduce to the real found values
   //printf("calcR2values numV:%d\n", numV);
   return OK;
} // calcR2values()

void showRvalues() { // show all input resistor values
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   printf("Input values (single, serie, parallel):\n");
   for (u32 pos=0; pos<numV; pos++) {
      printf("pos:%5u", pos);
      printf(" r:%10.2f", rValues[pos].r);
      printf(" rp[0]:%10.2f", rValues[pos].rp[0]);
      for (u08 p=1; p<maxRp; p++) {
         printf(" rp[%u]:%10.2f", p, rValues[pos].rp[p]);
      }
      printf(" desc[%02u]:'%s'\n", rValues[pos].descIdx, Vdesc[rValues[pos].descIdx]);
   }
} // void showRvalues()

/* calculate all formula results using 'maxRc' resistances: mem=0,lists=1/2 */
// use rValues[numV] ==> results[totV]
int calcFm0values(void) {
   u32 rc1, rc2;
   /*float res[maxRc];*/ /* single case, normally 2 resistances */
   u64 per; // percentage progress
   double val;
   double delta = 0;
   u32 pos = 0;
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showInPos();
   if (dbgLv>=PRINTDEBUG) printf("numV:%u, totV:%llu\n", numV, totV);
   if (dbgLv>=PRINTDEBUG) printf("expr:'%s'\n", expr);
   if (dbgLv>=PRINTF) gprintf(gui, "sol="); // here print something for user feedback (take time)
   fflush(NULL); // user space stdout flush
   fsync(1);   // kernel space stdout flush
   for (rc1=0; rc1<numV; rc1++) { // all input values
      exprVarsParser[0] = rValues[rc1].r; /* estract a value to try */
      if (dbgLv>=PRINTDEBUG) printf("exprVarsParser[0]:%g\n", exprVarsParser[0]);
      //printf("rc1:%d 0:%f\n", rc1, exprVarsParser[0]);
      //if (exprVarsParser[0]==0.) {
         //printf("rc1:%d skip:%f\n", rc1, exprVarsParser[0]);
      //   continue;
      //}
      for (rc2=0; rc2<numV; rc2++) { // all input values
         exprVarsParser[1] = rValues[rc2].r; /* estract a value to try */
         if (dbgLv>=PRINTDEBUG) printf("exprVarsParser[1]:%g\n", exprVarsParser[1]);
         //printf("rc1:%d rc2:%d 0:%f 1:%f\n", rc1, rc2, exprVarsParser[0], exprVarsParser[1]);
         //if (exprVarsParser[1]==0.) {
            //printf("rc1:%d rc2:%d skip:%f\n", rc1, rc2, exprVarsParser[1]);
         //   continue;
         //}
         val = evalExprParser (expr);
         delta = val - target;
         /*printf("rc1:%04u rc2:%04u pos:%04u val:%8g delta:%8g\n", rc1, rc2, pos, val, delta);*/
         results[pos].pos[0] = rc1;
         results[pos].pos[1] = rc2;
         results[pos].delta  = delta;
         /*double a1 = rValues[rc1].rp[0];
         double a2 = rValues[rc1].rp[1];
         double b1 = rValues[rc2].rp[0];
         double b2 = rValues[rc2].rp[1];*/
         //printf("rc1:%04u rc2:%04u pos:%04u %s:r1:%8g&%8g %s:r2:%8g&%8g val:%8g delta:%8g\n", rc1, rc2, pos, rValues[results[pos].pos[1]].desc, a1, a2, rValues[results[pos].pos[1]].desc, b1, b2, val, delta);
         //printf("rc1:%04u rc2:%04u pos:%04u %s:r1:%8g&%8g=%8g %s:r2:%8g&%8g=%8g val:%8g delta:%8g\n", rc1, rc2, pos, rValues[results[pos].pos[1]].desc, a1, a2, exprVarsParser[0], rValues[results[pos].pos[1]].desc, b1, b2, exprVarsParser[1], val, delta);
         pos++;
      }
      if (rc1%1000==0) { // here print something for user feedback (take time)
         //if (dbgLv>=PRINTF) gprintf(gui, "%u,", rc1*numV);
         per=(u64)rc1*numV*100/totV;
         if (dbgLv>=PRINTF) gprintf(gui, "%d%%,", per);
         fflush(NULL); // user space stdout flush
         fsync(1);   // kernel space stdout flush
      }
      if (stop==true) {
         gprintf(gui, "\nGUI asked to stop, break!\n");
         break;
      }
   }
   //if (dbgLv>=PRINTF) gprintf(gui, "%u\n", rc1*numV-1); // here print something for user feedback (take time)
   if (dbgLv>=PRINTF) gprintf(gui, "100%%\n"); // here print something for user feedback (take time)
   //printf("pos:%u totV:%llu\n", pos, totV);
   return OK;
} // int calcFm0values(void)

// find worst (higher delta) low mem solution
int findWorst(struct resultsTy* resultsNLowPtr, u32* wNPtr, double* deltaNWorstPtr) {
   double max = 0;
   double delta;
   for (int s=0; s<numBestRes; s++) {
      delta = fabs(resultsNLowPtr[s].delta);
      //printf("s:%d delta:%f\n", s, delta);
      if (delta > max) {
         if (dbgLv>=PRINTDEBUG) printf("s:%d old max:%f at:%d, new max:%f at:%d\n", s, max, *wNPtr, delta, s);
         max = delta;
         *wNPtr = s; // take note of worst value position
      }
   }
   *deltaNWorstPtr = max; // take note of worst value
   if (dbgLv>=PRINTDEBUG) gprintf(gui, "worst:%f max:%f at:%d\n", *deltaNWorstPtr, max, *wNPtr);
   return OK;
} // findWorst()

bool is_double_le(double a, double b, double epsilon) {
    return a < b + epsilon;
}

/* calculate best formula results using 'maxRc' resistances: mem=1,lists=1/2 */
// use rValues[numV] ==> resultsXLow[totV]
int calcFm1values(void) {
   u32 rc1, rc2;
   u64 per; // percentage progress
   double val;
   double delta = 0;
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showInPos();
   if (dbgLv>=PRINTDEBUG) printf("numR:%u, numV:%u, totV:%llu\n", numR, numV, totV);
   if (dbgLv>=PRINTDEBUG) printf("expr:'%s'\n", expr);
   w2 = 0;
   delta2Worst = MaxValue; // 50 GOhm
   for (u32 s=0; s<numBestRes; s++) {
      results2LowPtr[s].delta=MaxValue; // 50 GOhm
   }
   if (maxRp==2) { // 
      w = 0; w4 = 0; w3 = 0;
      deltaWorst = MaxValue; // 50 GOhm
      delta4Worst = MaxValue; // 50 GOhm
      delta3Worst = MaxValue; // 50 GOhm
      for (u32 s=0; s<numBestRes; s++) {
         resultsLowPtr[s].delta=MaxValue; // 50 GOhm
         results4LowPtr[s].delta=MaxValue; // 50 GOhm
         results3LowPtr[s].delta=MaxValue; // 50 GOhm
      }
   }
   if (dbgLv>=PRINTF) gprintf(gui, "sol="); // here print something for user feedback (take time)
   fflush(NULL); // user space stdout flush
   fsync(1);   // kernel space stdout flush
   //printf("\n");
   //printf("numR:%u numV:%u totV:%llu numBestRes:%u\n", numR, numV, totV, numBestRes);
   //printf("rValues[numV:%u] results[totV:%llu] results4LowPtr[numBestRes:%u]\n", numV, totV, numBestRes);
   //printf("firstSeries   =numR                     :%u\n", numR);
   if (dbgLv>=PRINTDEBUG) printf("numR:%u, numV:%u, totV:%llu, numBestRes:%u\n", numR, numV, totV, numBestRes);
   for (rc1=0; rc1<numV; rc1++) { // all input values
      exprVarsParser[0] = rValues[rc1].r; /* estract a value to try */
      if (dbgLv>=PRINTDEBUG) printf("exprVarsParser[0]:%g\n", exprVarsParser[0]);
      for (rc2=0; rc2<numV; rc2++) { // all input values
         exprVarsParser[1] = rValues[rc2].r; /* estract a value to try */
         if (dbgLv>=PRINTDEBUG) printf("exprVarsParser[1]:%g\n", exprVarsParser[1]);
         //printf("\nexprVarsParser[0]:%g\n", exprVarsParser[0]);
         //printf("exprVarsParser[1]:%g\n", exprVarsParser[1]);
         val = evalExprParser (expr);
         delta = val - target;
         //printf("rc1:%u rc2:%u val:%g delta:%g\n", rc1, rc2, val, delta);
         if (maxRp==1) { // single value only
//            if (fabs(delta) <= delta2Worst) {
//            if (delta2Worst > fabs(delta)) {
//            if ( definitelyGreaterThan(delta2Worst, fabs(delta), Epsilon )) {
            //gprintf(gui, "rc1:%d rc2:%d val:%.1f delta:%f delta2Worst:%f, w2:%u\n", rc1, rc2, val, fabs(delta), delta2Worst, w2);
            if (is_double_le(fabs(delta), delta2Worst, Epsilon)) { // this is for 2 resistors
               //printf("new better result:%f delta:%.4f oldDeltaMin:%.4f\n", val, delta, deltaWorst);
               //gprintf(gui, "rc1:%03u rc2:%03u oldDelta:%.2f fill at:%d with newDelta:%.2f\n", rc1, rc2, delta2Worst, w2, fabs(delta));
               results2LowPtr[w2].pos[0] = rc1;   // always insert in worst pos
               results2LowPtr[w2].pos[1] = rc2;   // always insert in worst pos
               results2LowPtr[w2].delta  = delta; // always insert in worst pos
               findWorst(results2LowPtr, &w2, &delta2Worst); // find worst (higher delta) solution
            }
         }
         if (maxRp==2) { // single value, series and parallels
            if (is_double_le(fabs(delta), deltaWorst, Epsilon)) { // this is for up to 4 resistors
               //printf("new better result:%f delta:%.4f oldDeltaMax:%.4f\n", val, delta, deltaWorst);
               //printf("rc1:%03u rc2:%03u oldDelta:%.2f fill at:%d with newDelta:%.2f\n", rc1, rc2, deltaWorst, w, fabs(delta));
               resultsLowPtr[w].pos[0] = rc1;   // always insert in worst pos
               resultsLowPtr[w].pos[1] = rc2;   // always insert in worst pos
               resultsLowPtr[w].delta  = delta; // always insert in worst pos
               findWorst(resultsLowPtr, &w, &deltaWorst); // find worst (higher delta) solution
            }
            bool cmp0, cmp1;
            if (is_double_le(fabs(delta), delta4Worst, Epsilon)) { // this is for 4 resistors
               //printf("new better result with 4 res:%.1f delta:%.4f oldDelta:%.2f\n", val, delta, delta4Worst);
               //printf("rc1:%03u rc2:%03u oldDelta:%.2f fill at:%d with newDelta:%.2f\n", rc1, rc2, delta4Worst, w4, fabs(delta));
               if (rc1<numS) cmp0 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp0 = 1;
               if (rc2<numS) cmp1 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp1 = 1;
               //printf("rc1:%d rc2:%d cmp0:%d cmp1:%d w4:%d\n", rc1, rc2, cmp0, cmp1, w4);
               if (cmp0==0 || cmp1==0) goto n3; // skip when one is 0 (so skip when are less than 4 resistances, do when both are series/parallel)
               //printf("==>write rc1:%d rc2:%d cmp0:%d cmp1:%d w4:%d\n", rc1, rc2, cmp0, cmp1, w4);
               results4LowPtr[w4].pos[0] = rc1;   // always insert in worst pos
               results4LowPtr[w4].pos[1] = rc2;   // always insert in worst pos
               results4LowPtr[w4].delta  = delta; // always insert in worst pos
               findWorst(results4LowPtr, &w4, &delta4Worst); // find worst (higher delta) solution
            }
            n3:
            if (is_double_le(fabs(delta), delta3Worst, Epsilon)) { // this is for 3 resistors
               if (rc1<numS) cmp0 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp0 = 1;
               if (rc2<numS) cmp1 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp1 = 1;
               if (cmp0==cmp1) goto n2; // skip when both 0 or both 1 (both Exx/custom or both series/parallel, so do when are 3 resistances)
               results3LowPtr[w3].pos[0] = rc1;   // always insert in worst pos
               results3LowPtr[w3].pos[1] = rc2;   // always insert in worst pos
               results3LowPtr[w3].delta  = delta; // always insert in worst pos
               findWorst(results3LowPtr, &w3, &delta3Worst); // find worst (higher delta) solution
            }
            n2:
            if (is_double_le(fabs(delta), delta2Worst, Epsilon)) { // this is for 2 resistors
               //printf("find new better with 2 res\n");
               if (rc1<numS) cmp0 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp0 = 1;
               if (rc2<numS) cmp1 = 0; // 0 when single (Exx or custom), 1 when two (series or //)
               else cmp1 = 1;
               //printf("rc1:%d rc2:%d cmp0:%d cmp1:%d w2:%d\n", rc1, rc2, cmp0, cmp1, w2);
               if (cmp0!=0 || cmp1!=0) continue; // skip when one is 1 (so skip when are more than 2 resistances, do when both are Exx/custom)
               //printf("rc1:%d rc2:%d cmp0:%d cmp1:%d w2:%d\n", rc1, rc2, cmp0, cmp1, w2);
               //printf("rc1:%03u rc2:%03u oldDelta:%.2f fill @:%d newDelta:%.2f val:%.2f\n", rc1, rc2, delta2Worst, w2, fabs(delta), target+delta);
               results2LowPtr[w2].pos[0] = rc1;   // always insert in worst pos
               results2LowPtr[w2].pos[1] = rc2;   // always insert in worst pos
               results2LowPtr[w2].delta  = delta; // always insert in worst pos
               findWorst(results2LowPtr, &w2, &delta2Worst); // find worst (higher delta) solution
               //printf("new worst at w2:%u ---\n", w2);
            }
         } // if (maxRp==2)
      } // inner for ()
      //printf("\n");
      if (rc1%1000==0) { // here print something for user feedback (take time)
         //if (dbgLv>=PRINTF) gprintf(gui, "%u,", rc1*numV);
         per=(u64)rc1*numV*100/totV;
         if (dbgLv>=PRINTF) gprintf(gui, "%d%%,", per); // here print something for user feedback (take time)
         fflush(NULL); // user space stdout flush
         fsync(1);   // kernel space stdout flush
      }
      if (stop==true) {
         gprintf(gui, "\nGUI asked to stop, break!\n");
         break;
      }
   } // outer for ()
   //if (dbgLv>=PRINTF) gprintf(gui, "%u\n", rc1*numV-1);
   if (dbgLv>=PRINTF) gprintf(gui, "100%%\n");
   return OK;
} // int calcFm1values(void)

s64 tot; // used for debug link between 'structQuickSort()' & 'qsStruct()'

/* QuickSort for vector of structs of type resultsTy. */
/* Sorting criteria: field 'abs(delta)' */
void qsStruct(struct resultsTy results[], s32 left, s32 right) {
   s32 l, r, m; /* position left, right and median */
   u64 per; // percentage progress
   float  mp; /* will contain data in center position */
   struct resultsTy temp;            /* temp struct for swap */
   static u64 qs;
   if (left==0 && right==tot-1) qs = 0; // so next run of GUI start from 0
   if (qs%20000000==0) {
      //if (dbgLv>=PRINTF) gprintf(1, "qs:%llu tot:%lld ", qs, tot);
      per = qs*100/tot;
      if (dbgLv>=PRINTF) gprintf(gui, "%llu%%,", per);
      fflush(NULL); // user space stdout flush
      fsync(1);   // kernel space stdout flush
   }
   qs++;
   if (dbgLv>=PRINTDEBUG) {
      for (s32 c=0; c<tot; c++) {
         printf("s[%2d]=%11G\n", c, results[c].delta);
      }
   }
   if (dbgLv>=PRINTDEBUG) printf("left:%9d right:%9d\n", left, right);
   l = left; r = right;
   m = (left+right)/2; /* center position rounded down */
   mp = fabs(results[m].delta); /* take center element absolute delta */
   if (dbgLv>=PRINTDEBUG) printf("meanPos(left+right)/2:%9d meanValue_delta[meanPos]:%11G\n", m, mp);
   do { /* at least once, until is true (l<=r) */
      if (dbgLv>=PRINTDEBUG) printf("l:%9d r:%9d\n", l, r);
      /* find first element at left <= of center element */
      while ( ((mp-fabs(results[l].delta))<0) && (l<right) ) l++; /* test, if true exec */
      /* find first element at right > of center element */
      while ( ((mp-fabs(results[r].delta))>0) && (r>left ) ) r--; /* test, if true exec */
      if (dbgLv>=PRINTDEBUG) printf("limits: value[l=%9d]=%11G value[r=%9d]=%11G\n", l, results[l].delta, r, results[r].delta);
      if (l <= r) { /* if not crossed, swap */
         if (dbgLv>=PRINTDEBUG) printf("swap: l:%9d r:%9d\n", l, r);
         temp = results[l];
         results[l] = results[r];
         results[r] = temp;
         l++; r--;
      }
      if (dbgLv>=PRINTDEBUG) {
         for (s32 c=0; c<tot; c++) {
            printf(" [%2d]=%11G\n", c, results[c].delta);
         }
      }
   } while (l <= r);
   if (dbgLv>=PRINTVERBOSE) {
      for (s32 c=left; c<right; c++) {
         printf("value[%9d]=%11G\n", c, results[c].delta);
      }
   }
   if (left < r) {
      if (dbgLv>=PRINTDEBUG) printf("analisys: value[left=%9d]=%11G value[r=%9d]=%11G\n", left, results[left].delta, r, results[r].delta);
      if (dbgLv>=PRINTDEBUG) printf("\n");
      qsStruct(results, left , r);
   }
   if (l < right) {
      if (dbgLv>=PRINTDEBUG) printf("analisys: value[l=%9d]=%11G value[right=%9d]=%11G\n", l, results[l].delta, right, results[right].delta);
      if (dbgLv>=PRINTDEBUG) printf("\n");
      qsStruct(results, l, right);
   }
   if (dbgLv>=PRINTDEBUG) printf("\n");
} // void qsStruct(struct resultsTy results[], s32 left, s32 right)

/* QuickSort for vector of structs of type resultsTy, using field 'abs(delta)' */
/* put results[0] the greatest(worst), results[totNumber-1] the smallest(best) */
int structQuickSort(struct resultsTy results[], s32 totNumber) {
   if (totNumber==0) return ERROR;
   tot = totNumber;
   if (dbgLv>=PRINTF) {
      gprintf(gui, "qs="); // here print something for user feedback (take time)
      fflush(NULL); // user space stdout flush
      fsync(1);   // kernel space stdout flush
   }
   qsStruct(results, 0UL, totNumber-1);
   //if (dbgLv>=PRINTF) gprintf(gui, "%d\n", totNumber-1);
   if (dbgLv>=PRINTF) gprintf(gui, "100%%\n");
   if (dbgLv>=PRINTDEBUG) {
      for (s64 c=0; c<tot; c++) {
         printf("f[%2lld]=%11G\n", c, results[c].delta);
      }
   }
   return OK;
} // int structQuickSort(struct resultsTy results[], s32 totNumber)

int doCalc() { // fill inputs, calcs, sort solutions
   int ret;
   winGuiLoop = 1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // 6 - fill the input vectors with needed data
   if (maxRp>1) {
      gprintf(gui, "Calculating all:%u values with max:%u resistors per position ...\n", numV, maxRp);
   } else {
      gprintf(gui, "Populating all:%u values with max:%u resistors per position ...\n", numV, maxRp);
   }
   if (lists==1)
      ret = calcRvalues();  // populate all possible values, only for: lists=1
   else // lists==2
      ret = calcR2values(); // populate all possible values, only for: lists=2
   if (ret==ERROR) {
      printf("calcRvalues returned ERROR\n");
      free(results);
      return ERROR;
   }
   //printf("FUNCT: %s\n", __FUNCTION__);
   //showQty();
   //showInPos();
   //showRvalues();

   // 7 - calculus of solutions
   if (mem==0) {
      gprintf(gui, "Calculating all:%llu solutions with a formula of:%u resistors ...\n", totV, maxRc);
      ret = calcFm0values(); // calculate all results and delta: lists=1/2
      if (ret==ERROR) {
         printf("calcM0Fvalues returned ERROR\n");
         free(results);
         return ERROR;
      }
   } else { // low mem=1
      gprintf(gui, "Calculating best:%u/%llu solutions with a formula of:%u resistors ...\n", numBestRes, totV, maxRc);
      ret = calcFm1values(); // calculate best results and delta: lists=1/2
      if (ret==ERROR) {
         printf("calcM1Fvalues returned ERROR\n");
         free(results);
         return ERROR;
      }
   }
   if (numBestRes>totV) numBestRes = totV;

   // 8 - sorting of solutions
   if (mem==0) {
      gprintf(gui, "Sorting all:%llu found solutions ...\n", totV);
      structQuickSort(results, totV); /* QuickSort on all results */
   } else { // low mem
      gprintf(gui, "Sorting best:%u found solutions ...\n", numBestRes);
      structQuickSort(results2LowPtr, numBestRes); /* QuickSort on new results */
      if (maxRp==2) { //
         structQuickSort(resultsLowPtr, numBestRes); /* QuickSort on new results */
         structQuickSort(results4LowPtr, numBestRes); /* QuickSort on new results */
         structQuickSort(results3LowPtr, numBestRes); /* QuickSort on new results */
      }
   }
   // prepare output tolerance as strings for showVal()
   //printf("userRtol:'%g' userR2tol:'%g'\n", userRtol, userR2tol);
   if (userRtol<1)
      sprintf(tol1str, ".%u", (unsigned)(10*userRtol));
   else
      sprintf(tol1str, " %u", (unsigned)userRtol);
   if (userR2tol<1)
      sprintf(tol2str, ".%u", (unsigned)(10*userR2tol));
   else
      sprintf(tol2str, " %u", (unsigned)userR2tol);
   //printf("tol1str:'%s' tol2str:'%s'\n", tol1str, tol2str);
   return OK;
} // int doCalc()

/* print last 'numBestRes = totV-first' results, mem=0 */
int showVal2to4(u32 first) { // show Solutions when mem=0
   u32 pos;
   int cmp0, cmp1;
   u08 descIdx;
   char* tol0;
   char* tol1;

   for (pos=first; pos<totV; pos++) { // 'totV-1' is the best solution
      //printf("\npos:%9lu\n", pos);
      //printf("\npos:%9lu results[pos].pos[0]:%u results[pos].pos[1]:%u\n", pos, results[pos].pos[0], results[pos].pos[1]);
      gprintf(gui, "val:%11G   delta:%11.4G", target+results[pos].delta, results[pos].delta);
      gprintf(gui, " e%%:%9.2G", results[pos].delta/target*100);
      gprintf(gui, "   a:%11G b:%11G\n", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      //cmp0 = (strcmp(rValues[results[pos].pos[0]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[0]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[0]].descIdx<11) cmp0 = 0; else cmp0 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      //cmp1 = (strcmp(rValues[results[pos].pos[1]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[1]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[1]].descIdx<11) cmp1 = 0; else cmp1 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      if (maxRp==2) { // maybe a series or parallel value or lists=2
         descIdx = rValues[results[pos].pos[0]].descIdx;
         //printf("descIdx.pos0:%u\n", descIdx);
         if (cmp0==0) { // single resistor
            tol0 = tol1str;
            if (descIdx==2) tol0 = tol2str;
            gprintf(gui, "a:%-9s:%8G@%2s%%                 ", Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0);
         } else { // 2 resistors in series or parallel
            tol0 = tol1str;
            if (descIdx==2 || descIdx==13 || descIdx==16) tol0 = tol2str;
            tol1 = tol1str;
            if (descIdx==2 || descIdx==12 || descIdx==13 || descIdx==15 || descIdx==16) tol1 = tol2str;
            if (descIdx<14) // series
               gprintf(gui, "a:%-9s:%8G@%2s%% ++%8G@%2s%%  " , Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0, rValues[results[pos].pos[0]].rp[1], tol1);
            else // parallel
               gprintf(gui, "a:%-9s:%8G@%2s%% //%8G@%2s%%  " , Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0, rValues[results[pos].pos[0]].rp[1], tol1);
         }
         descIdx = rValues[results[pos].pos[1]].descIdx;
         //printf("\ndescIdx.pos1:%u\n", descIdx);
         if (cmp1==0) { // single resistor
            tol0 = tol1str;
            if (descIdx==2) tol0 = tol2str;
            gprintf(gui, "b:%-9s:%8G@%2s%%", Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0);
         } else { // 2 resistors in series or parallel
            tol0 = tol1str;
            if (descIdx==2 || descIdx==13 || descIdx==16) tol0 = tol2str;
            tol1 = tol1str;
            if (descIdx==2 || descIdx==12 || descIdx==13 || descIdx==15 || descIdx==16) tol1 = tol2str;
            if (descIdx<14) // series
               gprintf(gui, "b:%-9s:%8G@%2s%% ++%8G@%2s%%", Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0, rValues[results[pos].pos[1]].rp[1], tol1);
            else // parallel
               gprintf(gui, "b:%-9s:%8G@%2s%% //%8G@%2s%%", Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0, rValues[results[pos].pos[1]].rp[1], tol1);
         }
         gprintf(gui, "\n");
      }
   }
   return OK;
} // int showVal2to4(u32 first)

/* print best 'numBestRes' results, mem=0 */
int showVal43(u32 numBestRes, u08 numRes) { // Solutions with 4/3 resistors
   s32 pos, best;
   int cmp0, cmp1;
   u08 descIdx;
   char* tol0;
   char* tol1;
   u32 count = 0;
   u32 res[NumberResMax];

   if (numRes<3 || numRes>4) {
      printf("numRes:%hu can be 3 or 4\n", numRes);
      return ERROR;
   }
   for (pos=totV-1; pos>=0; pos--) { // [totV] is the best solution
      /*printf("pos:%9lu", pos);*/
      if (count>=numBestRes) break; // found 'numBestRes' results
      //cmp0 = (strcmp(rValues[results[pos].pos[0]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[0]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[0]].descIdx<11) cmp0 = 0; else cmp0 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      //cmp1 = (strcmp(rValues[results[pos].pos[1]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[1]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[1]].descIdx<11) cmp1 = 0; else cmp1 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      if (numRes==4) if (cmp0==0 || cmp1==0) continue; // skip when one is 0 (so skip when are less than 4 resistances, do when both are series/parallel)
      if (numRes==3) if (cmp0==cmp1) continue;         // skip when both 0 or both 1 (both User/Exx or both series/parallel, so do when are 3 resistances)
      res[count]=pos;
      //printf("%s added pos:%ld\n", __FUNCTION__, pos);
      count++;
   }
   for (best=count-1; best>=0; best--) { // [0] is the best solution
      pos = res[best];
      //printf("pos:%d results[pos].pos[0]:%u, results[pos].pos[1]:%u\n", pos, results[pos].pos[0], results[pos].pos[1]);

      //cmp0 = (strcmp(rValues[results[pos].pos[0]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[0]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[0]].descIdx<11) cmp0 = 0; else cmp0 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      //cmp1 = (strcmp(rValues[results[pos].pos[1]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[1]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[1]].descIdx<11) cmp1 = 0; else cmp1 = 1; // 0 when single (Exx or custom), 1 when two (series or //)

      gprintf(gui, "val:%11G   delta:%11.4G", target+results[pos].delta, results[pos].delta);
      gprintf(gui, " e%%:%9.2G", results[pos].delta/target*100);
      gprintf(gui, "   a:%11G b:%11G\n", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      if (maxRp==2) { // maybe a series or parallel value, when maxRp==1 is a repetition of 'a' and 'b' as are User list
         descIdx = rValues[results[pos].pos[0]].descIdx;
         if (cmp0==0) { // single resistors
            tol0 = tol1str;
            if (descIdx==2) tol0 = tol2str;
            gprintf(gui, "a:%-9s:%8G@%2s%%                 ", Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0);
         } else { // 2 resistors in series or parallel
            tol0 = tol1str;
            if (descIdx==2 || descIdx==13 || descIdx==16) tol0 = tol2str;
            tol1 = tol1str;
            if (descIdx==2 || descIdx==12 || descIdx==13 || descIdx==15 || descIdx==16) tol1 = tol2str;
            if (descIdx<14) // series
               gprintf(gui, "a:%-9s:%8G@%2s%% ++%8G@%2s%%  " , Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0, rValues[results[pos].pos[0]].rp[1], tol1);
            else // parallel
               gprintf(gui, "a:%-9s:%8G@%2s%% //%8G@%2s%%  " , Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0, rValues[results[pos].pos[0]].rp[1], tol1);
         }
         descIdx = rValues[results[pos].pos[1]].descIdx;
         if (cmp1==0) { // single resistors
            tol0 = tol1str;
            if (descIdx==2) tol0 = tol2str;
            gprintf(gui, "b:%-9s:%8G@%2s%%", Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0);
         } else { // 2 resistors in series or parallel
            tol0 = tol1str;
            if (descIdx==2 || descIdx==13 || descIdx==16) tol0 = tol2str;
            tol1 = tol1str;
            if (descIdx==2 || descIdx==12 || descIdx==13 || descIdx==15 || descIdx==16) tol1 = tol2str;
            if (descIdx<14) // series
               gprintf(gui, "b:%-9s:%8G@%2s%% ++%8G@%2s%%", Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0, rValues[results[pos].pos[1]].rp[1], tol1);
            else // parallel
               gprintf(gui, "b:%-9s:%8G@%2s%% //%8G@%2s%%", Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0, rValues[results[pos].pos[1]].rp[1], tol1);
         }
         gprintf(gui, "\n");
      }
   }
   return OK;
} // int showVal43(u32 numBestRes, u08 numRes)

/* print best 'numBestRes' results, mem=0 */
int showVal2(u32 numBestRes) { // Solutions with 2 resistors
   s32 pos, best;
   int cmp0, cmp1;
   u08 descIdx;
   char* tol0;
   u32 count = 0;
   u32 res[NumberResMax];

   for (pos=totV-1; pos>=0; pos--) { // [totV] is the best solution
      /*printf("pos:%9lu", pos);*/
      if (count>=numBestRes) break; // found 'numBestRes' results
      //cmp0 = (strcmp(rValues[results[pos].pos[0]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[0]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[0]].descIdx<11) cmp0 = 0; else cmp0 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      //cmp1 = (strcmp(rValues[results[pos].pos[1]].descIdx, "Custom list")!=0) &&
      //       (strcmp(rValues[results[pos].pos[1]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
      if (rValues[results[pos].pos[1]].descIdx<11) cmp1 = 0; else cmp1 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
      if (cmp0!=0 || cmp1!=0) continue; // skip when one is 1 (so skip when are more than 2 resistances, do when both are Exx/custom)
      res[count]=pos;
      //printf("%s added pos:%ld\n", __FUNCTION__, pos);
      count++;
   }
   for (best=count-1; best>=0; best--) { // [0] is the best solution
      pos = res[best];
      //printf("pos:%d results[pos].pos[0]:%u, results[pos].pos[1]:%u\n", pos, results[pos].pos[0], results[pos].pos[1]);
      gprintf(gui, "val:%11G   delta:%11.4G", target+results[pos].delta, results[pos].delta);
      gprintf(gui, " e%%:%9.2G", results[pos].delta/target*100);
      gprintf(gui, "   a:%11G b:%11G\n", rValues[results[pos].pos[0]].r, rValues[results[pos].pos[1]].r);
      if (maxRp==2) { // maybe a series or parallel value, when maxRp==1 is a repetition of 'a' and 'b' as are User list
         descIdx = rValues[results[pos].pos[0]].descIdx;
         tol0 = tol1str;
         if (descIdx==2) tol0 = tol2str;
         gprintf(gui, "a:%-9s:%8G@%2s%%                 ", Vdesc[rValues[results[pos].pos[0]].descIdx], rValues[results[pos].pos[0]].rp[0], tol0);
         descIdx = rValues[results[pos].pos[1]].descIdx;
         tol0 = tol1str;
         if (descIdx==2) tol0 = tol2str;
         gprintf(gui, "b:%-9s:%8G@%2s%%"                 , Vdesc[rValues[results[pos].pos[1]].descIdx], rValues[results[pos].pos[1]].rp[0], tol0);
         gprintf(gui, "\n");
      }
   }
   return OK;
} // int showVal2(u32 numBestRes)

/* print best 'numBestRes' LowMem results, mem=1 */
int showValMemLow(u32 numBestRes) { // show Solutions when mem=1
   struct resultsTy* resultsNLowPtr=NULL;
   u08 descIdx;
   char* tol0;
   char* tol1;

   if (dbgLv>=PRINTDEBUG) gprintf(gui, "numR:%u, numV:%u, totV:%llu, numBestRes:%u\n", numR, numV, totV, numBestRes);
   for (u08 v=5; v>=2; v--) { // up to 4, 4, 3, 2 resistors
      //printf("v:%u\n", v);
      if (maxRp==1 && v>2) continue;
      if (maxRp==2) { // when maxRp==2 need to showVal4+,4,3 ...
         if (v==5) {
            gprintf(gui, "Show best:%u solutions with up to 4 resistors:\n", numBestRes);
            resultsNLowPtr = resultsLowPtr;
         }
         if (v==4) {
            gprintf(gui, "Show best:%u solutions with 4 resistors:\n", numBestRes);
            resultsNLowPtr = results4LowPtr;
         }
         if (v==3) {
            gprintf(gui, "Show best:%u solutions with 3 resistors:\n", numBestRes);
            resultsNLowPtr = results3LowPtr;
         }
      }
      if (v==2) { // when maxRp==1 no need to showVal4+,4,3 ...
         gprintf(gui, "Show best:%u solutions with 2 resistors:\n", numBestRes);
         resultsNLowPtr = results2LowPtr;
      }
      for (int s=0; s<numBestRes; s++) { // for each bestRes
         //printf("s:%02d resultsNLowPtr[].pos[0]:%03u resultsNLowPtr[].pos[1]:%03u\n", s, resultsNLowPtr[s].pos[0], resultsNLowPtr[s].pos[1]);
         //printf("s:%02d resultsNLowPtr[].delta:%.5f val:%.5f\n", s, resultsNLowPtr[s].delta, target+resultsNLowPtr[s].delta);
         //printf("s:%02d numV:%u\n", s, numV);
         if (resultsNLowPtr[s].pos[0] >= numV || resultsNLowPtr[s].pos[1] >= numV) continue;
         //printf("s:%02d delta:%.5f MaxValue/2:%.5f\n", s, resultsNLowPtr[s].delta, MaxValue/2);
         //gprintf(gui, "-\n");
         //if (resultsNLowPtr[s].delta>MaxValue/2) continue;
         if (dbgLv>=PRINTDEBUG) gprintf(gui, "valid s:%02d resultsNLowPtr[].pos[0]:%03u resultsNLowPtr[].pos[1]:%03u\n", s, resultsNLowPtr[s].pos[0], resultsNLowPtr[s].pos[1]);
         if (format==0) {
            gprintf(gui, "val:%11G", target+resultsNLowPtr[s].delta);
            gprintf(gui, "   delta:%11.4G", resultsNLowPtr[s].delta);
            gprintf(gui, " e%%:%9.2G", resultsNLowPtr[s].delta/target*100);
            gprintf(gui, "   a:%11G", rValues[resultsNLowPtr[s].pos[0]].r);
            gprintf(gui, " b:%11G\n", rValues[resultsNLowPtr[s].pos[1]].r);
         } else {
            char* valPtr = engStr(target+resultsNLowPtr[s].delta, 6, false, format-1);
            char* deltaPtr = engStr(resultsNLowPtr[s].delta, 4, false, format-1);
            char* aPtr = engStr(rValues[resultsNLowPtr[s].pos[0]].r, 6, false, format-1);
            char* bPtr = engStr(rValues[resultsNLowPtr[s].pos[1]].r, 6, false, format-1);
            gprintf(gui, "val:%11s", valPtr);
            gprintf(gui, "   delta:%11s", deltaPtr);
            gprintf(gui, " e%%:%9.2G", resultsNLowPtr[s].delta/target*100);
            gprintf(gui, "   a:%11s", aPtr);
            gprintf(gui, " b:%11s\n", bPtr);
            free(valPtr);
            free(deltaPtr);
            free(aPtr);
            free(bPtr);
         }
         if (maxRp==2) { // maybe a series or parallel value
            int cmp0, cmp1;
            //cmp0 = (strcmp(rValues[resultsNLowPtr[s].pos[0]].descIdx, "Custom list")!=0) &&
            //       (strcmp(rValues[resultsNLowPtr[s].pos[0]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
            if (rValues[resultsNLowPtr[s].pos[0]].descIdx<11) cmp0 = 0; else cmp0 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
            //cmp1 = (strcmp(rValues[resultsNLowPtr[s].pos[1]].descIdx, "Custom list")!=0) &&
            //       (strcmp(rValues[resultsNLowPtr[s].pos[1]].descIdx, Exx          )!=0);   // 0 when single (Exx or custom), 1 when two (series or //)
            if (rValues[resultsNLowPtr[s].pos[1]].descIdx<11) cmp1 = 0; else cmp1 = 1; // 0 when single (Exx or custom), 1 when two (series or //)
            if (dbgLv>=PRINTDEBUG) gprintf(gui, "s:%d cmp0:%d cmp1:%d\n", s, cmp0, cmp1);
            descIdx = rValues[resultsNLowPtr[s].pos[0]].descIdx;
            if (cmp0==0) { // single resistors
               tol0 = tol1str;
               if (descIdx==2) tol0 = tol2str;
               if (format==0) { // sci format
                  //gprintf(gui, "a:%-9s:%8G             ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], rValues[resultsNLowPtr[s].pos[0]].rp[0]);
                  gprintf(gui, "a:%-9s:%8G@%2s%%                 ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], rValues[resultsNLowPtr[s].pos[0]].rp[0], tol0);
               } else { // eng/SIprefix format
                  char* aPtr = engStr(rValues[resultsNLowPtr[s].pos[0]].rp[0], 6, false, format-1);
                  gprintf(gui, "a:%-9s:%8s@%2s%%                 ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], aPtr, tol0);
                  free(aPtr);
               }
            } else { // 2 resistors in series or parallel
               tol0 = tol1str;
               if (descIdx==2 || descIdx==13 || descIdx==16) tol0 = tol2str;
               tol1 = tol1str;
               if (descIdx==2 || descIdx==12 || descIdx==13 || descIdx==15 || descIdx==16) tol1 = tol2str;
               if (format==0) { // sci format
                  //gprintf(gui, "a:%-9s:%8G & %8G  " , Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], rValues[resultsNLowPtr[s].pos[0]].rp[0], rValues[resultsNLowPtr[s].pos[0]].rp[1]);
                  if (descIdx<14) // series
                     gprintf(gui, "a:%-9s:%8G@%2s%% ++%8G@%2s%%  ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], rValues[resultsNLowPtr[s].pos[0]].rp[0], tol0, rValues[resultsNLowPtr[s].pos[0]].rp[1], tol1);
                  else // parallel
                     gprintf(gui, "a:%-9s:%8G@%2s%% //%8G@%2s%%  ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], rValues[resultsNLowPtr[s].pos[0]].rp[0], tol0, rValues[resultsNLowPtr[s].pos[0]].rp[1], tol1);
               } else { // eng/SIprefix format
                  char* a0Ptr = engStr(rValues[resultsNLowPtr[s].pos[0]].rp[0], 6, false, format-1);
                  char* a1Ptr = engStr(rValues[resultsNLowPtr[s].pos[0]].rp[1], 6, false, format-1);
                  if (descIdx<14) // series
                     gprintf(gui, "a:%-9s:%8s@%2s%% ++%8s@%2s%%  ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], a0Ptr, tol0, a1Ptr, tol1);
                  else // parallel
                     gprintf(gui, "a:%-9s:%8s@%2s%% //%8s@%2s%%  ", Vdesc[rValues[resultsNLowPtr[s].pos[0]].descIdx], a0Ptr, tol0, a1Ptr, tol1);
                  free(a0Ptr);
                  free(a1Ptr);
               }
            }
            descIdx = rValues[resultsNLowPtr[s].pos[1]].descIdx;
            if (cmp1==0) { // single resistors
               tol0 = tol1str;
               if (descIdx==2) tol0 = tol2str;
               if (format==0) { // sci format
                  //gprintf(gui, "b:%-9s:%8G"       , Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], rValues[resultsNLowPtr[s].pos[1]].rp[0]);
                  gprintf(gui, "b:%-9s:%8G@%2s%%", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], rValues[resultsNLowPtr[s].pos[1]].rp[0], tol0);
               } else { // eng/SIprefix format
                  char* bPtr = engStr(rValues[resultsNLowPtr[s].pos[1]].rp[0], 6, false, format-1);
                  gprintf(gui, "b:%-9s:%8s@%2s%%", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], bPtr, tol0);
                  free(bPtr);
               }
            } else { // 2 resistors in series or parallel
               tol0 = tol1str;
               if (descIdx==2 || descIdx==13 || descIdx==16) tol0 = tol2str;
               tol1 = tol1str;
               if (descIdx==2 || descIdx==12 || descIdx==13 || descIdx==15 || descIdx==16) tol1 = tol2str;
               if (format==0) { // sci format
                  //gprintf(gui, "b:%-9s:%8G & %8G ", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], rValues[resultsNLowPtr[s].pos[1]].rp[0], rValues[resultsNLowPtr[s].pos[1]].rp[1]);
                  if (descIdx<14) // series
                     gprintf(gui, "b:%-9s:%8G@%2s%% ++%8G@%2s%%", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], rValues[resultsNLowPtr[s].pos[1]].rp[0], tol0, rValues[resultsNLowPtr[s].pos[1]].rp[1], tol1);
                  else // parallel
                     gprintf(gui, "b:%-9s:%8G@%2s%% //%8G@%2s%%", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], rValues[resultsNLowPtr[s].pos[1]].rp[0], tol0, rValues[resultsNLowPtr[s].pos[1]].rp[1], tol1);
               } else { // eng/SIprefix format
                  char* b0Ptr = engStr(rValues[resultsNLowPtr[s].pos[1]].rp[0], 6, false, format-1);
                  char* b1Ptr = engStr(rValues[resultsNLowPtr[s].pos[1]].rp[1], 6, false, format-1);
                  if (descIdx<14) // series
                     gprintf(gui, "b:%-9s:%8s@%2s%% ++%8s@%2s%%", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], b0Ptr, tol0, b1Ptr, tol1);
                  else // parallel
                     gprintf(gui, "b:%-9s:%8s@%2s%% //%8s@%2s%%", Vdesc[rValues[resultsNLowPtr[s].pos[1]].descIdx], b0Ptr, tol0, b1Ptr, tol1);
                  free(b0Ptr);
                  free(b1Ptr);
               }
            } // single or serie/parallel
            gprintf(gui, "\n");
         } // maxRp=2 maybe a series or parallel value
      } // for each bestRes
      gprintf(gui, "\n");
   } // up to 4, 4, 3, 2 resistors
   return OK;
} // int showValMemLow(u32 numBestRes)

/* print last 'numBestRes = totV-first' results */
int showVal(u32 numBestRes) { // show Solutions
   int ret = OK;

   //printf("numBestRes:%ld NumberResMax:%d\n", numBestRes, NumberResMax);
   if (numBestRes>NumberResMax) numBestRes=NumberResMax;
   //printf("numBestRes:%ld NumberResMax:%d\n", numBestRes, NumberResMax);
   first = totV-numBestRes; /* 0 worse, 'first' 1st printed, 'totV' best, numBestRes # of best print */
   //printf("totV:%llu numBestRes:%u first:%u\n", totV, numBestRes, first);

   if (mem==0) {
      for (u08 v=5; v>=2; v--) { // up to 4, 4, 3, 2 resistors
         //printf("v:%u\n", v);
         if (maxRp==1 && v>2) continue;
         if (maxRp==2) { // when maxRp==2 need to showVal4+,4,3 ...
            if (v==5) {
               gprintf(gui, "Show %u solutions with up to 4 resistors:\n", numBestRes);
               ret = showVal2to4(first);
            }
            if (v==4) {
               gprintf(gui, "Show %u solutions with 4 resistors:\n", numBestRes);
               ret = showVal43(numBestRes, 4);
            }
            if (v==3) {
               gprintf(gui, "Show %u solutions with 3 resistors:\n", numBestRes);
               ret = showVal43(numBestRes, 3);
            }
         }
         if (v==2) { // when maxRp==1 no need to showVal4+,4,3 ...
            gprintf(gui, "Show %u solutions with 2 resistors:\n", numBestRes);
            ret = showVal2(numBestRes);
         }
         gprintf(gui, "\n");
      } // up to 4, 4, 3, 2 resistors
   } else { // mem=1
      ret = showValMemLow(numBestRes); // show Solutions when mem=1
   }
   return ret;
} // int showVal(u32 numBestRes)

int freeMem() { // free memory
   numV=numF; // if needed restore numV
   memInpCalc(); // if needed recalc the allocated size
   // 10 - freeing dynamic allocated memory ...
   for (u32 e=0; e<numV; e++) { // for input values
      if (rValues[e].rp) free(rValues[e].rp);
   }
   if (rValues) free(rValues); // for input values with parallel/series
   if (mem==0) { // for results old memory hungry strategy
      if (results) free(results);
   } else { // for results new mem low strategy
      if (results2LowPtr) free(results2LowPtr);
      if (maxRp==2) {
         if (resultsLowPtr) free(resultsLowPtr);
         if (results4LowPtr) free(results4LowPtr);
         if (results3LowPtr) free(results3LowPtr);
      }
   }
   return OK;
} // int freeMem()
