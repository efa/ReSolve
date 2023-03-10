/* ReSolve V0.09.09h 2023/03/12 solve math expressions using discrete values*/
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

#include "reSolveLib.h"    /* main source include */

int main(int numPar, char* param[]) {
   u08  len, pos;
   u08  p;
   char c;
   int  ret;

   gui=0; // mean gprintf() do not update GUI
   guiUpdateOutPtr = NULL; // no function pointer to guiUpdateOut()

   // 1 - load configuration file and params
   ret = baseInit (); // LIB: basic initialization: load config from file+memSize
   if (ret != 0) {
      printf ("baseInit returned:%u, quit\n", ret);
      return -1;
   }
   // 2 - read and set user request
   // 3 - calculate the needed memory
   ret = memCalc(); // LIB: calculate the needed memory

   // 4 - checking arguments syntax
   /*printf ("numPar:%u\n", numPar);*/
   /*printf ("float:%u, double:%u\n", sizeof(float), sizeof (double));*/
   numPar--;
   /*printf ("numPar:%u\n", numPar);*/
   if (numPar>0) {
      if (!strcmp(param[1],"-h") || !strcmp(param[1],"--help")) {
         showHelp (allocatedB);
         return (ERROR);
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
            pos = strcspn (param[2], "!\"#$&',:;<=>?@[\\]_`{|}~??????");
            if (pos!=len) { // filter out: !"#$&',:;<=>?@[\]_`{|}~??????
               showHelp (allocatedB);
               printf ("Unsupported char: '%c'\n", *(param[2]+pos));
               return (ERROR);
            }
            for (p=0; p<len; p++) {
               c = *(param[2]+p);
               if ((u08)c>127) {
                  showHelp (allocatedB);
                  printf ("Unsupported char: '%c'\n", *(param[2]+pos));
                  return (ERROR);
               }
               if (isalpha ((u08)c)) {
                  if ( (toupper ((u08)c)-'A') >= MaxRc ) {
                     showHelp (allocatedB);
                     printf ("ERROR: Unsupported Variable:'%c'\n\n", c);
                     return (ERROR);
                  }
               }
            }
            strcpy(expr, param[2]);
         } else {
            showHelp (allocatedB);
            printf ("Formula too long: %u chars\n", len);
            return (ERROR);
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
   ret = memAlloc(); // LIB: memory allocation
   if (ret != 0) {
      printf ("memAlloc() returned:%u, quit\n", ret);
      return -1;
   }

   ret=showConf(); // LIB: show config set

   // 8 - fill the input vectors with needed data
   // 9 - calculus of solutions
   // 10 - sorting of solutions
   ret = doCalc(); // LIB: fill inputs, calcs, sort solutions

   // 11 - print of results
   gprintf (gui, "Printing best:%u solutions (top worst, botton best) in all configurations ...\n\n", numBestRes);
   if (maxRp==1) { // no need to showVal4,3,2 ...
      gprintf (gui, "Show %u solutions with 2 resistors:\n", numBestRes);
      ret = showVal2 (numBestRes);
   } else {
      gprintf (gui, "Show %u solutions with up to 4 resistors:\n", numBestRes);
      ret = showVal (first);
      gprintf (gui, "\n");
      gprintf (gui, "Show %u solutions with 4 resistors:\n", numBestRes);
      ret = showVal4 (numBestRes);
      gprintf (gui, "\n");
      gprintf (gui, "Show %u solutions with 3 resistors:\n", numBestRes);
      ret = showVal3 (numBestRes);
      gprintf (gui, "\n");
      gprintf (gui, "Show %u solutions with 2 resistors:\n", numBestRes);
      ret = showVal2 (numBestRes);
   }
   gprintf (gui, "\n");

   // 12 - freeing dynamic allocated memory ...
   ret = freeMem(); // LIB: free memory
   if (baseR) free (baseR); // free mem allocated by fillConfigVars()

   return (OK);
}
