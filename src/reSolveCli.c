/* ReSolve v0.11.09h 2023/08/29 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* reSolveCli.c is part of ReSolve
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

/* reSolveCli.c This is the main CLI source */

#include "reSolveLib.h"   /* LIB: include */

int parCheck(int numPar, char* paramPtr[]) { // check CLI parameters
   u08  len, pos;
   u08  p;
   char c;
   /*printf("numPar:%u\n", numPar);*/
   /*printf("float:%u, double:%u\n", sizeof(float), sizeof (double));*/
   numPar--;
   /*printf("numPar:%u\n", numPar);*/
   if (numPar>0) {
      if (!strcmp(paramPtr[1],"-h") || !strcmp(paramPtr[1],"--help")) {
         showHelp();
         baseInit(); // LIB: set internal variables
         memInpCalc(); // LIB: memory size calculation for input values
         memResCalc(); // LIB: calculate the needed memory for results
         showConf();
         exit(EXIT_SUCCESS);
      }
   }
   if (paramPtr[1] != NULL) {
      /*printf("param1Str:%s\n", paramPtr[1]);*/
      target = atof (paramPtr[1]);
      /*printf("param1Num:%f\n", target);*/
   }
   if (target==0.0) target = TargetDefault; /* letters case */
   if (numPar>1) { /* at least searched value and formula are present */
      if (paramPtr[2] != NULL) {
         /*printf("param2Str:%s\n", paramPtr[2]);*/
         len = strlen(paramPtr[2]);
         if (len < 60) {
            pos = strcspn(paramPtr[2], "!\"#$&',:;<=>?@[\\]_`{|}~�簧");
            if (pos!=len) { // filter out: !"#$&',:;<=>?@[\]_`{|}~�簧
               showHelp();
               printf("Unsupported char: '%c'\n", *(paramPtr[2]+pos));
               return ERROR;
            }
            for (p=0; p<len; p++) {
               c = *(paramPtr[2]+p);
               if ((u08)c>127) {
                  showHelp();
                  printf("Unsupported char: '%c'\n", *(paramPtr[2]+pos));
                  return ERROR;
               }
               if (isalpha((u08)c)) {
                  if ( (toupper((u08)c) - 'A') >= MaxRc ) {
                     showHelp();
                     printf("ERROR: Unsupported Variable:'%c'\n\n", c);
                     return ERROR;
                  }
               }
            }
            strcpy(expr, paramPtr[2]);
         } else {
            showHelp();
            printf("Formula too long: %u chars\n", len);
            return ERROR;
         }
      }
      if (numBestRes==0) numBestRes = NumberResDefault; /* letters case */
   }
   if (numPar>2) { /* searched value, formula and number of results are present */
      if (paramPtr[3] != NULL) {
         /*printf("param3Str:%s\n", paramPtr[3]);*/
         numBestRes = atoi(paramPtr[3]);
         /*printf("param3Num:%u\n", numBestRes);*/
      }
      if (numBestRes==0) numBestRes = NumberResDefault; /* letters case */
      if (numPar>3) printf("WARNING: Ignoring numPar[%u++]='%s'\n", numPar, paramPtr[4]);
   }
   return numPar;
} // int parCheck(int numPar, char* paramPtr[])

int main(int numPar, char* paramPtr[]) { // CLI entry point
   int ret;

   gui=0; // mean gprintf() do not update GUI
   guiUpdateOutPtr = NULL; // no function pointer to guiUpdateOut()
   winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   showHead(); // LIB: show header
   chDirBin(paramPtr[0]); // change current working directory to binary path
   // 0 - load configuration file and params
   ret = fillConfigVars(); // LIB: load and check users config file
   if (ret!=OK) {
      if (dbgLev>=PRINTERROR) printf("ERROR %s: fillConfigVars returned:%u not OK, quit\n", __FUNCTION__, ret);
      return ERROR;
   }
   //printf("Found and loaded config file: 'reSolveConf.txt'\n");
   // 1 - checking arguments syntax/config value validity
   numPar = parCheck(numPar, paramPtr); // CLI:
   ret = exprCheck(); // LIB: check expression syntax
   // 2 - read and set user request
   ret = baseInit(); // LIB: set internal variables

   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   ret = memResCalc(); // LIB: calculate the needed memory for results

   // 4 - show config values
   ret=showConf(); // LIB: show config set

   printf("\nReSolve ...\n");
   // 5 - allocate the memory asking to the OS a malloc()
   //     create the structure's vector inside the allocated memory
   ret = memInpAlloc(); // LIB: memory allocation for input values
   if (ret != 0) {
      printf("memInpAlloc() returned:%u, quit\n", ret);
      return ERROR;
   }
   ret = memResAlloc(); // LIB: memory allocation for results
   if (ret != 0) {
      printf("memResAlloc() returned:%u, quit\n", ret);
      return ERROR;
   }
   char* stringPtr;
   stringPtr=siMem(allocatedB);
   printf("Allocated about %s of total RAM\n", stringPtr);
   free(stringPtr);

   // 6 - fill the input vectors with needed data
   // 7 - calculus of solutions
   // 8 - sorting of solutions
   clock_t start, stop;
   double time;
   start = clock();
   ret = doCalc(); // LIB: fill inputs, calcs, sort solutions
   //ret = doCalc(); // LIB: fill inputs, calcs, sort solutions
   //ret = doCalc(); // LIB: fill inputs, calcs, sort solutions
   stop = clock();
   time = (double)(stop - start) / CLOCKS_PER_SEC; time+=0;
   //time = time/3;
   printf("Compute time: %f s\n", time);

   // 9 - print results
   if (mem==0) { // old memory hungry strategy
      gprintf(gui, "Printing best:%u found solutions (top worst, botton best) in all configurations\n\n", numBestRes);
      if (maxRp==1) { // no need to showVal4,3,2 ...
         gprintf(gui, "Show %u solutions with 2 resistors:\n", numBestRes);
         ret = showVal2(numBestRes); // LIB:
      } else {
         gprintf(gui, "Show %u solutions with up to 4 resistors:\n", numBestRes);
         ret = showVal(first); // LIB:
         gprintf(gui, "\n");
         gprintf(gui, "Show %u solutions with 4 resistors:\n", numBestRes);
         ret = showVal4(numBestRes); // LIB:
         gprintf(gui, "\n");
         gprintf(gui, "Show %u solutions with 3 resistors:\n", numBestRes);
         ret = showVal3(numBestRes); // LIB:
         gprintf(gui, "\n");
         gprintf(gui, "Show %u solutions with 2 resistors:\n", numBestRes);
         ret = showVal2(numBestRes); // LIB:
      }
   } else { // new mem low strategy
      gprintf(gui, "Printing best:%u found solutions (top worst, botton best) in all configurations\n\n", numBestRes);
      if (maxRp==1) { // no need to showVal4,3,2 ...
         gprintf(gui, "Show best:%u solutions with 2 resistors:\n", numBestRes);
         ret = showValMemLow(numBestRes, results2LowPtr); // LIB:
      } else {
         gprintf(gui, "Show best:%u solutions with up to 4 resistors:\n", numBestRes);
         ret = showValMemLow(numBestRes, resultsLowPtr); // LIB:
         gprintf(gui, "\n");
         gprintf(gui, "Show best:%u solutions with 4 resistors:\n", numBestRes);
         ret = showValMemLow(numBestRes, results4LowPtr); // LIB:
         gprintf(gui, "\n");
         gprintf(gui, "Show best:%u solutions with 3 resistors:\n", numBestRes);
         ret = showValMemLow(numBestRes, results3LowPtr); // LIB:
         gprintf(gui, "\n");
         gprintf(gui, "Show best:%u solutions with 2 resistors:\n", numBestRes);
         ret = showValMemLow(numBestRes, results2LowPtr); // LIB:
      }
   }
   gprintf(gui, "\n");

   // 10 - freeing dynamic allocated memory ...
   //printf("Quit...\n");
   memInpCalc(); // if needed restore numV to the allocated value
   ret = freeMem(); // LIB: free memory
   if (userR) free(userR); // free mem allocated by fillConfigVars()

   return EXIT_SUCCESS;
}
