/* ReSolve v0.10.09h 2023/04/05 solve math expressions using discrete values*/
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

int main(int numPar, char* param[]) { // CLI entry point
   u08  len, pos;
   u08  p;
   char c;
   int  ret;
   clock_t start, stop;
   double time;

   gui=0; // mean gprintf() do not update GUI
   guiUpdateOutPtr = NULL; // no function pointer to guiUpdateOut()
   winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // 1 - load configuration file and params
   ret = baseInit (); // LIB: basic initialization: load config from file
   if (ret != 0) {
      printf ("baseInit returned:%u, quit\n", ret);
      return ERROR;
   }
   // 2 - read and set user request
   // 3 - calculate the needed memory
   ret = memValCalc(); // LIB: memory size calculation for input values
   if (algo==0) // 0 use old memory hungry strategy
      ret = memCalc(); // LIB: calculate the needed memory
   else // 1 use new mem low strategy
      ret = memLowCalc(); // LIB: calculate the needed memory

   // 4 - checking arguments syntax/config value validity
   /*printf ("numPar:%u\n", numPar);*/
   /*printf ("float:%u, double:%u\n", sizeof(float), sizeof (double));*/
   numPar--;
   /*printf ("numPar:%u\n", numPar);*/
   if (numPar>0) {
      if (!strcmp(param[1],"-h") || !strcmp(param[1],"--help")) {
         showHelp (allocatedB);
         return ERROR;
      }
   }
   if (param[1] != NULL) {
      /*printf ("param1Str:%s\n", param[1]);*/
      desired = atof (param[1]);
      /*printf ("param1Num:%f\n", desired);*/
   }
   if (desired==0.0) desired = DesiredDefault; /* letters case */
   if (numPar>1) { /* at least searched value and formula are present */
      if (param[2] != NULL) {
         /*printf ("param2Str:%s\n", param[2]);*/
         len = strlen (param[2]);
         if (len < 60) {
            pos = strcspn (param[2], "!\"#$&',:;<=>?@[\\]_`{|}~�簧");
            if (pos!=len) { // filter out: !"#$&',:;<=>?@[\]_`{|}~�簧
               showHelp (allocatedB);
               printf ("Unsupported char: '%c'\n", *(param[2]+pos));
               return ERROR;
            }
            for (p=0; p<len; p++) {
               c = *(param[2]+p);
               if ((u08)c>127) {
                  showHelp (allocatedB);
                  printf ("Unsupported char: '%c'\n", *(param[2]+pos));
                  return ERROR;
               }
               if (isalpha ((u08)c)) {
                  if ( (toupper ((u08)c)-'A') >= MaxRc ) {
                     showHelp (allocatedB);
                     printf ("ERROR: Unsupported Variable:'%c'\n\n", c);
                     return ERROR;
                  }
               }
            }
            strcpy(expr, param[2]);
         } else {
            showHelp (allocatedB);
            printf ("Formula too long: %u chars\n", len);
            return ERROR;
         }
      }
      if (numBestRes==0) numBestRes = NumberResDefault; /* letters case */
   }
   if (numPar>2) { /* searched value, formula and number of results are present */
      if (param[3] != NULL) {
         /*printf ("param3Str:%s\n", param[3]);*/
         numBestRes = atoi (param[3]);
         /*printf ("param3Num:%u\n", numBestRes);*/
      }
      if (numBestRes==0) numBestRes = NumberResDefault; /* letters case */
      if (numPar>3) printf ("WARNING: Ignoring numPar[%u++]='%s'\n", numPar, param[4]);
   }
   // 5 - show config values
   showHead (); // LIB: show header
   gprintf (gui, "Found and loaded config file: 'reSolveConf.txt'\n");

   // 6 - allocate the memory asking to the OS a malloc()
   // 7 - create the structure's vector inside the allocated memory
   ret = memValAlloc(); // LIB: memory allocation for input values
   if (ret != 0) {
      printf ("memValAlloc() returned:%u, quit\n", ret);
      return ERROR;
   }
   if (algo==0) // 0 use old memory hungry strategy
      ret = memAlloc(); // LIB: memory allocation for results
   else // 1 use new mem low strategy
      ret = memLowAlloc(); // LIB: allocate low mem for results
   if (ret != 0) {
      printf ("memLowAlloc() returned:%u, quit\n", ret);
      return ERROR;
   }

   ret=showConf(); // LIB: show config set

   // 8 - fill the input vectors with needed data
   // 9 - calculus of solutions
   // 10 - sorting of solutions
   start = clock();
   if (algo==0) // 0 use old memory hungry strategy
      ret = doCalc(); // LIB: fill inputs, calcs, sort solutions
   else { // 1 use new mem low strategy
      ret = doMemLowCalc(); // LIB: fill inputs, calcs, sort solutions
      //ret = doMemLowCalc(); // LIB: fill inputs, calcs, sort solutions
      //ret = doMemLowCalc(); // LIB: fill inputs, calcs, sort solutions
   }
   stop = clock();
   time = (double)(stop - start) / CLOCKS_PER_SEC; time+=0;
   //time = time/3;
   printf ("Compute time: %f s\n", time);

   // 11 - print results
   if (algo==0) { // old memory hungry strategy
      gprintf (gui, "Printing best:%u solutions (top worst, botton best) in all configurations ...\n\n", numBestRes);
      if (maxRp==1) { // no need to showVal4,3,2 ...
         gprintf (gui, "Show %u solutions with 2 resistors:\n", numBestRes);
         ret = showVal2 (numBestRes); // LIB:
      } else {
         gprintf (gui, "Show %u solutions with up to 4 resistors:\n", numBestRes);
         ret = showVal (first); // LIB:
         gprintf (gui, "\n");
         gprintf (gui, "Show %u solutions with 4 resistors:\n", numBestRes);
         ret = showVal4 (numBestRes); // LIB:
         gprintf (gui, "\n");
         gprintf (gui, "Show %u solutions with 3 resistors:\n", numBestRes);
         ret = showVal3 (numBestRes); // LIB:
         gprintf (gui, "\n");
         gprintf (gui, "Show %u solutions with 2 resistors:\n", numBestRes);
         ret = showVal2 (numBestRes); // LIB:
      }
   } else { // new mem low strategy
      gprintf (gui, "Show best:%llu found solutions ...\n", numBestRes);
      if (maxRp==1) { // no need to showVal4,3,2 ...
         gprintf (gui, "Show best:%u solutions with 2 resistors:\n", numBestRes);
         ret = showValMemLow (numBestRes, results2LowPtr); // LIB:
      } else {
         gprintf (gui, "Show best:%u solutions with up to 4 resistors:\n", numBestRes);
         ret = showValMemLow (numBestRes, resultsLowPtr); // LIB:
         gprintf (gui, "\n");
         gprintf (gui, "Show best:%u solutions with 4 resistors:\n", numBestRes);
         ret = showValMemLow (numBestRes, results4LowPtr); // LIB:
         gprintf (gui, "\n");
         gprintf (gui, "Show best:%u solutions with 3 resistors:\n", numBestRes);
         ret = showValMemLow (numBestRes, results3LowPtr); // LIB:
         gprintf (gui, "\n");
         gprintf (gui, "Show best:%u solutions with 2 resistors:\n", numBestRes);
         ret = showValMemLow (numBestRes, results2LowPtr); // LIB:
      }
   }
   gprintf (gui, "\n");

   // 12 - freeing dynamic allocated memory ...
   ret = freeMem(); // LIB: free memory
   if (baseR) free (baseR); // free mem allocated by fillConfigVars()

   return EXIT_SUCCESS;
}
