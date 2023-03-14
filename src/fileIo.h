/* ReSolve v0.09.09h 2023/03/14 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* fileIo.h is part of ReSolve
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

/* fileIo.h interface to file i/o needed to load and save. */
/* this version support 32/64 bit systems, file size up to 4 GB */

#ifndef _INCfileIoh
#define _INCfileIoh

//#define _FILE_OFFSET_BITS 64 // set off_t to 64 bit
#include <stdio.h> // to have 'off_t' require compilation with -std=gnu99
#include <stdint.h>     /* uintptr_t */
#include <stdlib.h>
#include <string.h>
#include "comType.h"

#define LineLen 160

extern u08 dbgLev;                                     /* Interaction level */

/* open a fileName in ReadOnly and return the filePtr or NULL on ERROR */
FILE* openRead(char* fileName);

/* open a fileName in WriteOnly and return the filePtr or NULL on ERROR */
FILE* openWrite(char* fileName);

/* support 64 bit systems and file size greather than 4 GB, require C99 */
/* open a file in ReadOnly and return the filePtr, file size or ERROR */
off_t getFileSize(char* fileName, FILE** filePtrPtr);

/* support 64 bit systems, but not file size greather than 4 GB, require C99 */
/* open and copy a file in RAM and return allocated bufferPtr, buffer size or ERROR */
/* Then the file is closed. The user must free the bufferPtr at end of use */
off_t readFile(char* fileName, char** bufferPtrPtr);

/* support 64 bit systems, but not file size greather than 4 GB, require C99 */
/* copy RAM on created file and return written bytes or ERROR */
size_t writeFile(char* fileName, char* bufferPtr);

/* parse of configuration buffer for parameter value. Return value or ERROR */
errOk parseConf(char* bufPtr, char* paramPtr, char paramValue[LineLen]);

#endif /* _INCfileIoh */
