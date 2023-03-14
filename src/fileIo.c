/* ReSolve v0.09.09h 2023/03/14 solve math expressions using discrete values*/
/* Copyright 2005-2023 Valerio Messina http://users.iol.it/efa              */
/* fileIo.c is part of ReSolve
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

/* fileIo.c file i/o needed to load and save. */
/* this version support 32/64 bit systems, file size up to 4 GB */

#include "fileIo.h"

#define MaxSize32 ((((unsigned long int)1)<<31)-1) /* 2^31-1 = 2147483647 */
#define MaxSize64 ((((unsigned long int)1)<<63)-1) /* 2^63-1 = 9223372036854775807 */
#define TERM      '\0'                          /* Char String Terminator */
#define _FILE_OFFSET_BITS 64

int printNchar(char* startPtr, u64 num) { /* print N wchar from start */
   u64 pos;
   if (startPtr == NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: startPtr point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   if (num == 0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: num must be >0\n", __FUNCTION__);
      return (ERROR);
   }
   if (dbgLev>=PRINTVERBOSE) printf ("%2lluChar:'%s'\n", num, startPtr);
   for (pos=0; pos<num; pos++) {
      if (dbgLev>=PRINTF) printf ("%c", *(startPtr+pos));
      if (*(startPtr+pos)=='\0') break;
   }
   if (dbgLev>=PRINTDEBUG) printf ("@%p\n", startPtr);
   if (dbgLev>=PRINTVERBOSE) printf ("\n");
   return (OK);
} // printNchar()

/* open a fileName in ReadOnly and return the filePtr or NULL on ERROR */
FILE* openRead(char* fileName) {
   FILE* out;
   if (fileName==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: fileName point to NULL\n", __FUNCTION__);
      return (NULL);
   }
   out = fopen (fileName, "r");
   if (out == NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot open File:\"%s\"\n", __FUNCTION__, fileName);
   }
   return (out);
} // openRead()

/* open a fileName in WriteOnly and return the filePtr or NULL on ERROR */
FILE* openWrite(char* fileName) {
   FILE* out;
   if (fileName==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: fileName point to NULL\n", __FUNCTION__);
      return (NULL);
   }
   out = fopen (fileName, "w");
   if (out == NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot open File:\"%s\"\n", __FUNCTION__, fileName);
   }
   return (out);
} // openWrite()

/* support 64 bit systems and file size greather than 4 GB, require C99 */
/* open a file in ReadOnly and return the filePtr, file size or ERROR */
off_t getFileSize(char* fileName, FILE** filePtrPtr) {
   FILE* filePtr;
   off_t prev; /* SUS: off_t is signed long long, so max 9223372036854775801 */
   int out;
   off_t len;  /* SUS: off_t is signed long long, so max 9223372036854775801 */
   if (fileName==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: fileName point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   if (filePtrPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: filePtrPtr point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   filePtr = openRead (fileName);
   if (filePtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: openRead returned NULL on File:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   // ftello is in SingleUnixSpecification not Posix, so no support in old Cygwin
   prev = ftello (filePtr); // read current position (start)
   if (prev<0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: ftell failed on File:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   out = fseeko (filePtr, 0, SEEK_END); // go to end of file
   if (out!=0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: seek failed on File:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   len = ftello (filePtr); // read current position (end)
   if (dbgLev>=PRINTVERBOSE) printf ("File length:\"%s\" is %lld\n", fileName, (s64)len);
   if (len<0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: ftell failed on File:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   out = fseeko (filePtr, prev, SEEK_SET); // seek to start
   if (out != prev) {
      if (dbgLev>=PRINTWARN) printf ("WARN %s: seek to prev:%lld failed on File:\"%s\"\n", __FUNCTION__, (s64)prev, fileName);
      return (ERROR);
   }
   *filePtrPtr = filePtr;
   return (len);
} // getFileSize()

/* support 64 bit systems, but not file size greather than 4 GB, require C99 */
/* open and copy a file in RAM and return allocated bufferPtr, buffer size or ERROR */
/* Then the file is closed. The user must free the bufferPtr at end of use */
off_t readFile(char* fileName, char** bufferPtrPtr) {
   FILE* filePtr;
   off_t len;                             /* SUS: off_t is signed long long */
   size_t Nch;          /* U32: like an unsigned int, not an unsigned long! */
   int out;
   if (fileName==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: fileName point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   if (bufferPtrPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: bufferPtrPtr point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   len = getFileSize (fileName, &filePtr);
   if (len<0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: getFileSize returned:%lld on File:\"%s\"\n", __FUNCTION__, (s64)len, fileName);
      return (ERROR);
   }
   if (dbgLev>=PRINTVERBOSE) printf ("file length:\"%s\" is %lld\n", fileName, (s64)len);
   if (len>MaxSize32) { /* limit memory allocation to 2 GB */
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: file \"%s\" too big:%lld\n", __FUNCTION__, fileName, (s64)len);
      return (ERROR);
   }
   *bufferPtrPtr = (char*) malloc (len+1);               /* room for NULL */
   //printf ("allocated %lld Bytes @%p\n", (s64)len+1, *bufferPtrPtr);
   if (*bufferPtrPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot allocate %lld bytes of memory\n", __FUNCTION__, (s64)len);
      return (ERROR);
   }
   if (dbgLev>=PRINTVERBOSE) printf ("%s: buffer allocated at %p\n", __FUNCTION__, *bufferPtrPtr);
   if (len >= MaxSize32) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot read so much:%lld with size_t fread(3)\n", __FUNCTION__, (s64)len);
      return (ERROR);
   }
   Nch = fread (*bufferPtrPtr, 1, len, filePtr);
   if (Nch != len) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot read Nch/len:%zu/%ld from file:\"%s\"\n", __FUNCTION__, Nch, len, fileName);
      return (Nch);
   }
   if (dbgLev>=PRINTVERBOSE) printf ("%s(): file:'%s' of '%zu' chars readed in buffer\n", __FUNCTION__, fileName, Nch);
   //if (dbgLev>=PRINTVERBOSE) printNchar ( ((*bufferPtrPtr)+len-5), 5 );
   *((*bufferPtrPtr)+len) = TERM; /* insert terminator */
   if (dbgLev>=PRINTVERBOSE) printf ("%s() line:%u\n", __FUNCTION__, __LINE__);
   out = fclose (filePtr);
   if (out != 0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot close file:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   return (len);
} // readFile()

/* support 64 bit systems, but not file size greather than 4 GB, require C99 */
/* copy RAM on created file and return written bytes or ERROR */
size_t writeFile(char* fileName, char* bufferPtr) { /* copy from RAM to file */
   FILE* filePtr;
   size_t Nblk;                      /* U32: like an unsigned int, not long */
   size_t len;
   int out;
   if (fileName==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: fileName point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   if (bufferPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: bufferPtr point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   filePtr = openWrite (fileName);
   if (filePtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: Cannot write to file:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   len = strlen (bufferPtr);
   if (dbgLev>=PRINTVERBOSE) printf ("File:\"%s\" lenght is %zu\n", fileName, len);
   if (dbgLev>=PRINTVERBOSE) printf ("Buffer to transfer at %p\n", bufferPtr);
   if (len >= MaxSize32) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot write so much:%lld with size_t fwrite(3)\n", __FUNCTION__, (s64)len);
      return (ERROR);
   }
   if (dbgLev>=PRINTVERBOSE) printf ("Buffer to transfer at %p\n", bufferPtr);
   Nblk = fwrite (bufferPtr, 1, len, filePtr);
   if (Nblk != len) {
      if (dbgLev>=PRINTERROR) printf ("WARN %s: Cannot write Nblk/len:%zd/%lld to file:\"%s\"\n", __FUNCTION__, Nblk, (s64)len, fileName);
      return (Nblk);
   }
   if (dbgLev>=PRINTVERBOSE) printf ("File of '%zd' char written from buffer\n", Nblk);
   if (dbgLev>=PRINTVERBOSE) printNchar ( ((bufferPtr)+len-5), 5 );
   /**((*buffer)+len) = TERM;*//* add terminator at buffer end */
   if (dbgLev>=PRINTVERBOSE) printf ("line:%u\n", __LINE__);
   out = fclose (filePtr);
   if (out != 0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: Cannot close file:\"%s\"\n", __FUNCTION__, fileName);
      return (ERROR);
   }
   return (Nblk);
} // writeFile()

/* parse of configuration buffer for parameter value. Return value or ERROR */
errOk parseConf(char* bufPtr, char* paramPtr, char paramValue[LineLen]) {
   u08 len;
   char* chPtr;
   char* chPos;
   char ch;
   if (bufPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: bufPtr point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   if (paramPtr==NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: paramPtr point to NULL\n", __FUNCTION__);
      return (ERROR);
   }
   len = strlen (paramPtr);
   if (len==0) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: paramPtr len is zero\n", __FUNCTION__);
      return (ERROR);
   }
   if (len>LineLen) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: paramPtr len>%u unsupported\n", __FUNCTION__, LineLen);
      return (ERROR);
   }
   //printf ("'@%p\n", bufPtr);
   //printf ("bufPtr:'%s'\n", bufPtr);
   //printNchar (bufPtr, 5); printf ("\n");
   //printNchar (paramPtr, 5); printf ("\n");
   if (dbgLev>=PRINTDEBUG) printf ("%s: look for '%s' len:%u\n", __FUNCTION__, paramPtr, len);
   chPtr = strstr (bufPtr, paramPtr); // find start of parameter
   if (chPtr == NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: param:'%s' not found\n", __FUNCTION__, paramPtr);
      return (ERROR);
   }
   //printNchar (chPtr, 5); printf ("\n");
   chPtr+=len; // go to end of searched parameter
   if (dbgLev>=PRINTVERBOSE) printNchar (chPtr, 5);
   /*for ( ; chPos!=NULL; chPos=strstr(chPos,paramPtr) ) { // find last occurrence
      if (dbgLev>=PRINTVERBOSE) printf ("%s: use next occurrence of param:'%s'\n", __FUNCTION__, paramPtr);
      chPtr = chPos;
      chPos++;
   }*/
   if (*(chPtr)!=' ' && *(chPtr)!='=') {
      //if (dbgLev>=PRINTDEBUG) printf ("%s: use next occurrence of param:'%s'\n", __FUNCTION__, paramPtr);
      chPos = strstr (chPtr, paramPtr); // try to find another occurrence
      chPtr=chPos+len; // go to end of searched parameter
   }
   //printNchar (chPos, 5); printf ("\n");
   chPtr = strstr (chPtr, "=");
   if (chPtr == NULL) {
      if (dbgLev>=PRINTERROR) printf ("ERROR %s: configFile syntax error, miss '='\n", __FUNCTION__);
      return (ERROR);
   }
   //if (dbgLev>=PRINTDEBUG) printNchar (chPtr, 5);

   if (*(chPtr+1)=='{' || *(chPtr+2)=='{') { // extract vector of double
      if (dbgLev>=PRINTDEBUG) printf ("vector\n");
      chPtr++;
      if (*(chPtr)=='{') chPtr++;
      //if (dbgLev>=PRINTDEBUG) printNchar (chPtr, 7);
      chPos = strstr (chPtr, "}");
      u32 sizeChr=chPos-chPtr;
      char* vectorChr = malloc (sizeChr+1);
      strncpy (vectorChr, chPtr, sizeChr);
      *(vectorChr+sizeChr)='\0';
      //if (dbgLev>=PRINTDEBUG) printf ("vector:'%s'\n", vectorChr);
      chPtr = strchr (vectorChr, ',');
      u16 size=0; // count how many commas
      for (; chPtr; chPtr++) {
         size++;
         //if (dbgLev>=PRINTDEBUG) printf ("size:%u\n", size);
         chPtr = strchr (chPtr, ',');
         if (chPtr==NULL) break;
         //if (dbgLev>=PRINTDEBUG) printNchar (chPtr, 10);
      }
      if (dbgLev>=PRINTDEBUG) printf ("size:%u\n", size);
      double* vectorVal = malloc (size*sizeof(double));
      chPtr = vectorChr;
      char val[LineLen];
      u16 p;
      for (p=0; p<size; p++) {
         //if (dbgLev>=PRINTDEBUG) printf ("p:%u\n", p);
         chPos = strchr (chPtr, ',');
         if (chPos==NULL)
            strcpy (val, chPtr);
         else {
            sizeChr=chPos-chPtr;
            strncpy (val, chPtr, sizeChr);
            val[sizeChr]='\0';
         }
         //if (dbgLev>=PRINTDEBUG) printf ("val:'%s'\n", val);
         vectorVal[p] = strtod (val, &chPos);
         if (vectorVal[p]==0 && chPos==val) {
            if (dbgLev>=PRINTERROR) printf ("ERROR %s: cannot find digits in val='%s'\n", __FUNCTION__, val);
            return (ERROR);
         }
         chPtr=chPtr+sizeChr+1;
      }
      free (vectorChr);
      for (p=0; p<size; p++) {
         if (dbgLev>=PRINTDEBUG) printf ("vectorVal[%02u]:%g\n", p, vectorVal[p]);
      }
      //if (dbgLev>=PRINTF) printf ("vectorVal :@'0x%16zx'\n", (size_t)vectorVal);
      if (dbgLev>=PRINTDEBUG) printf ("vectorVal :@'%16p'\n", vectorVal);
      // now return the vector of double as it's address and size:
      // printing them as hex chars in paramValue[0-17] for the 0Xaddress,
      // set paramValue[18]=',' and paramValue[19-24] for the size
      if (dbgLev>=PRINTDEBUG) printf ("paramValue:@'%16p'\n", paramValue);
      sprintf (paramValue, "0x%016zx", (uintptr_t)vectorVal); // copy the address of vectorVal as 0x 16 hex chars
      /*for (p=0; p<=17; p++) {
         if (dbgLev>=PRINTDEBUG) printf ("@:%p paramValue[%02u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
      }*/
      paramValue[18]=','; // overwrite NULL
      if (dbgLev>=PRINTDEBUG) printf ("size:%u=0x%04X\n", size, size);
      sprintf (&paramValue[19], "0x%04X", size); // print 'u16 size' of vectorVal as 0x 4 hex chars at position 17-22
      //paramValue[25]='\0'; // NULL
      for (p=0; p<=24; p++) {
         if (dbgLev>=PRINTDEBUG) printf ("@:%p paramValue[%02u]=0x%02x='%c'\n", &paramValue[p], p, paramValue[p], paramValue[p]);
      } // check there is the terminator NULL @ pos 25
      if (dbgLev>=PRINTDEBUG) printf ("@:%p paramValue[25]=0x%02x='%c'\n", &paramValue[25], paramValue[25], paramValue[25]);
      return (OK);
   } // end extract vector of double

   chPos = strstr (chPtr, "\""); // if present it is text string
   //if (dbgLev>=PRINTDEBUG) printNchar (chPos, 5); // printf ("\n");
   if (chPos == NULL || chPos-chPtr > 2) { // not starting with ", so it is a number
      if (dbgLev>=PRINTDEBUG) printf ("number\n");
      //if (dbgLev>=PRINTERROR) printf ("ERROR %s: configFile syntax error, miss start '\"'\n", __FUNCTION__);
      ch='\n'; // set end character to find
   } else { // starting with ", so it is a text string
      if (dbgLev>=PRINTDEBUG) printf ("string\n");
      chPtr++; // skip "
      ch='\"'; // set end character to find
   }
   if (dbgLev>=PRINTDEBUG) printNchar (chPtr, 5); // printf ("\n");
   chPtr++; // skip =
   if (dbgLev>=PRINTDEBUG) printNchar (chPtr, 5); // printf ("\n");
   for (chPos=chPtr; chPos-chPtr<LineLen; chPos++) {
      paramValue[chPos-chPtr]=*chPos;
      if (*chPos==';' || *chPos==ch || *chPos=='\0') {
         paramValue[chPos-chPtr]='\0';
         break;
      }
      if (*chPos==' ' && ch=='\n') { // spaces after number
         paramValue[chPos-chPtr]='\0';
         break;
      }
   }
   if (dbgLev>=PRINTDEBUG) printf ("'%s'\n", paramValue);
   return (OK);
} // parseConf()
