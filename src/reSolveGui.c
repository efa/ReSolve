/* ReSolve v0.11.09h 2023/12/29 solve math expressions using discrete values*/
/* Copyright 2022-2023 Valerio Messina http://users.iol.it/efa              */
/* reSolveGui.c is part of ReSolve
   ReSolve is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ReSolve is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with ReSolve.  If not, see <http://www.gnu.org/licenses/>. */

// This program will show the ReSolve GTK3 widgets GUI
// build with:
// $ gcc reSolveGui.c -o reSolveGui `pkg-config --cflags --libs gtk+-3.0`

#define _GNU_SOURCE       /* vasprintf() */
#include <stdlib.h>
#include <locale.h>       // setlocale()
#include <gtk/gtk.h>
#include "reSolveLib.h"   /* LIB: include */

GtkBuilder* builderPtr;
GtkWindow* windowPtr;

char    exprConf[LineLen]; // backup config  expression
double* userRconf=NULL;    // backup config  list values
double* userR2conf=NULL;   // backup config  list2 values
u16     numR1conf;         // backup config  list quantity
u16     numR2conf;         // backup config  list2 quantity
char    exprGui[LineLen];  // backup userGui expression
double* userRgui=NULL;     // backup userGui list values
double* userR2gui=NULL;    // backup userGui list2 values
u16     numR1gui;          // backup userGui list quantity
u16     numR2gui;          // backup userGui list2 quantity
u08     EserieBack;        // backup lists2 Eserie
u16     maxRpBack;         // backup lists2 maxRp

static void userListWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr); // called on user list type values
static void userList2Whf(GtkWidget* widgetWhfPtr, gpointer dataPtr); // called on userList2 type values
int guiUpdateOut(char* txtPtr, int l); // update output widgets with results values
int runReSolve(); // memSize, memAlloc, doCalc, show output, freeMem

int backConfVal() { // backup config 'expr' and 'userR'
   numR1conf = numR1;
   numR2conf = numR2;
   strcpy(exprConf, expr);
   userRconf = calloc(numR1conf, sizeof(double));
   for (uint16_t r=0; r<numR1conf; r++) {
      userRconf[r]=userR[r];
   }
   userR2conf = calloc(numR2conf, sizeof(double));
   for (uint16_t r=0; r<numR2conf; r++) {
      userR2conf[r]=userR2[r];
   }
   return OK;
} // backConfVal()

int backGuiVal() { // backup gui 'expr' and 'userR'
   numR1gui = numR1;
   numR2gui = numR2;
   strcpy(exprConf, expr);
   userRgui = calloc(numR1gui, sizeof(double));
   for (uint16_t r=0; r<numR1gui; r++) {
      userRgui[r]=userR[r];
   }
   userR2gui = calloc(numR2gui, sizeof(double));
   for (uint16_t r=0; r<numR2gui; r++) {
      userR2gui[r]=userR2[r];
   }
   return OK;
} // backGuiVal()

int updateLabelDesc() { // called to update Eserie desc
   GtkWidget* widgetPtr;
   widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "description"));
   gtk_label_set_text((GtkLabel*)widgetPtr, userRdesc);
   widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "description2"));
   gtk_label_set_text((GtkLabel*)widgetPtr, userR2desc);
   return OK;
} // updateLabelDesc()

int updateLabelMem() { // called to update shown memory allocation
   GtkWidget* widgetPtr;
   char* stringPtr;
   stringPtr = siMem(allocatedB);
   //g_print("FUNCT: %s allocat:'%s'\n", __FUNCTION__, stringPtr);
   char* strPtr;
   asprintf(&strPtr, "Will allocate about %s of total RAM", stringPtr);
   free(stringPtr);
   widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "allocate"));
   gtk_label_set_text((GtkLabel*)widgetPtr, strPtr);
   free(strPtr);
   return OK;
} // updateLabelMem()

static void formulaDropWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on dropdown preset formula selector
   GtkWidget* widgetPtr;
   g_print("FormulaDrop ComboBoxText\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetWhfPtr);
   g_print("value:'%s'\n", comboValPtr);
   widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
   if (!strcmp(comboValPtr, "Series")) {
      if (maxRp==1)
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit00.png");
      else
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit00p.png");
      char formula[]="a+b";
      strcpy(expr, formula);
   }
   if (!strcmp(comboValPtr, "Parallel")) {
      if (maxRp==1)
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit01.png");
      else
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit01p.png");
      char formula[]="a*b/(a+b)";
      strcpy(expr, formula);
   }
   if (!strcmp(comboValPtr, "Partitor")) {
      if (maxRp==1)
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit02.png");
      else
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit02p.png");
      char formula[]="(a+b)/b";
      strcpy(expr, formula);
   }
   if (!strcmp(comboValPtr, "VoltReg")) {
      if (maxRp==1)
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
      else
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
      char formula[]="1.25*(1+b/a)";
      strcpy(expr, formula);
   }
   if (!strcmp(comboValPtr, "custom")) {
      if (maxRp==1)
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
      else
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaUser"));
      gtk_widget_set_sensitive(widgetPtr, true);
      if (exprGui[0]!='\0') { // user has already type a formula
         gtk_entry_set_text((GtkEntry*)widgetPtr, exprGui);
         strcpy(expr, exprGui);
      } else { // use config file
         gtk_entry_set_text((GtkEntry*)widgetPtr, exprConf);
         strcpy(expr, exprConf);
      }
   } else { // blank custom formula
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaUser"));
      gtk_widget_set_sensitive(widgetPtr, false);
      gtk_entry_set_text((GtkEntry*)widgetPtr, "");
   }
   printf("expr:'%s'\n", expr);
   //printf("exprConf:'%s'\n", exprConf);
   //printf("exprGui:'%s'\n", exprGui);
   g_free(comboValPtr);
} // formulaDropWhf()

static void formulaUserWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on custom formula type
   GtkWidget* widgetPtr;
   g_print("FormulaUser Entry\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetWhfPtr);
   g_print("text:'%s'\n", textPtr);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   if (!strcmp(comboValPtr, "custom formula")) {
      // CHECK: for string len
      int len = strlen(textPtr);
      if (len>=LineLen) {
         printf("Custom formula text len:%u, max supported len:%u\n", len, LineLen);
         return;
      }
      if (textPtr[0]!='\0') {
         strcpy(exprGui, textPtr);
         strcpy(expr, textPtr);
      }
   }
   printf("expr:'%s'\n", expr);
   //printf("exprConf:'%s'\n", exprConf);
   //printf("exprGui:'%s'\n", exprGui);
   //g_free(textPtr);
} // formulaUserWhf()

static void targetWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on target value change
   g_print("Desired SpinButton\n");
   gdouble spinVal = gtk_spin_button_get_value((GtkSpinButton*)widgetWhfPtr);
   g_print("value:'%g'\n", spinVal);
   target = spinVal;
} // targetWhf()

static void standardSerieWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on radio button standard series
   GtkWidget* widgetPtr;
   int ret;
   g_print("standardSerie RadioButton\n");
   guint radioVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetWhfPtr);
   g_print("value:'%u'\n", radioVal);
   if (radioVal==0) { // custom values
      // update numR1 from custom list qty
      // will do customValues()
      g_print("standardSerie RadioButton break\n");
      return;
   } // so is standard Eseries
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "EseriesList"));
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   //char valStr[5];
   //sprintf(valStr, "%u", radioVal);
   //g_print("EseriesList combo value:'%s'\n", comboValPtr);
   ret = updateEserie(comboValPtr); // LIB: update u08 Eserie from char* EseriePtr
   g_print("Eserie:E%u\n", Eserie);
   g_free(comboValPtr);
   ret = updateRdesc(true); // LIB: 
   //g_print("userRdesc:'%s'\n", userRdesc);
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   ret = updateLabelDesc();
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "EseriesList"));
   gtk_widget_set_sensitive(widgetPtr, true);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "decades"));
   gtk_widget_set_sensitive(widgetPtr, true);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userRtolWdg"));
   gtk_widget_set_sensitive(widgetPtr, false);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList"));
   gtk_widget_set_sensitive(widgetPtr, false);
   gtk_entry_set_text((GtkEntry*)widgetPtr, "");
   //g_print("standardSerie RadioButton quit\n");
} // standardSerieWhf()

static void EseriesListWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on Eseries dropdown
   int ret;
   g_print("EseriesList ComboBox\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetWhfPtr);
   //char valStr[5];
   //sprintf(valStr, "%u", radioVal);
   //g_print("value:'%s'\n", comboValPtr);
   ret = updateEserie(comboValPtr); // LIB: update Eserie
   g_print("Eserie:E%u\n", Eserie);
   g_free(comboValPtr);
   updateRdesc(true); // LIB: 
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   updateLabelDesc();
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   //widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "standardSeries"));
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   //widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "customValues"));
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
} // EseriesListWhf()

static void decadesWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on Decades change
   int ret;
   g_print("Decades SpinButton\n");
   gdouble spinVal = gtk_spin_button_get_value((GtkSpinButton*)widgetWhfPtr);
   //g_print("value:'%g'\n", spinVal);
   decades = spinVal;
   printf("decades:%u\n", decades);
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
} // decadesWhf()

static void userValuesWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on radio button user values
   GtkWidget* widgetPtr;
   int ret;
   g_print("userValues RadioButton\n");
   guint radioVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetWhfPtr);
   g_print("value:'%u'\n", radioVal);
   if (radioVal==0) { // standard Eseries
      // update numR1 from Eserie list qty
      // will do standardSeries()
      g_print("userValues RadioButton break\n");
      return;
   } // so 1 = custom values
   //g_print("numR1:%u\n", numR1);
   //g_print("numR1conf:%u\n", numR1conf);
   Eserie = 0;
   //g_print("Eserie:%u\n", Eserie);
   //printf("%s user list  numR1:%u @%g%% tolerance\n", __FUNCTION__, numR1, userRtol);
   //printf("%s user list2 numR2:%u @%g%% tolerance\n", __FUNCTION__, numR2, userR2tol);
   //printf("%s user lists numR :%u\n", __FUNCTION__, numR);
   //gprintf(gui, "numR1     :%u\n", numR1);
   if (lists==1) numR = numR1;
   else          numR = numR1+numR2;
   char* doubleList;
   if (userRgui!=NULL) { // user has already type custom list values
      g_print("populate customValue list with userGui values...\n");
      numR1 = numR1gui;
      uint16_t len = 0;
      char doubleStr[25];
      for (uint16_t r=0; r<numR1; r++) {
         sprintf(doubleStr, "%g", userRgui[r]);
         len+=strlen(doubleStr); // count chars
      }
      len+=numR1; // space for commas
      doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<numR1; r++) {
         sprintf(doubleStr, "%g", userRgui[r]);
         strcat(doubleList, doubleStr);
         if (r<numR1-1) strcat(doubleList, ",");
      }
      //g_print("custom values:'%s'\n", doubleList);
   } else { // use config file
      g_print("populate customValue list with config values...\n");
      numR1 = numR1conf;
      uint16_t len = 0;
      char doubleStr[25];
      for (uint16_t r=0; r<numR1; r++) {
         sprintf(doubleStr, "%g", userRconf[r]);
         len+=strlen(doubleStr); // count chars
      }
      len+=numR1; // space for commas
      doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<numR1; r++) {
         sprintf(doubleStr, "%g", userRconf[r]);
         strcat(doubleList, doubleStr);
         if (r<numR1-1) strcat(doubleList, ",");
      }
      //g_print("custom values:'%s'\n", doubleList);
   }
   //g_print("%s numR1:%u\n", __FUNCTION__, numR1);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "EseriesList"));
   gtk_widget_set_sensitive(widgetPtr, false);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "decades"));
   gtk_widget_set_sensitive(widgetPtr, false);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userRtolWdg"));
   gtk_widget_set_sensitive(widgetPtr, true);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList"));
   gtk_widget_set_sensitive(widgetPtr, true);
   g_signal_handlers_block_by_func(widgetPtr, userListWhf, NULL);
   //g_print("userRgui:%p\n", userRgui);
   gtk_entry_set_text((GtkEntry*)widgetPtr, doubleList);
   //g_print("userRgui:%p\n", userRgui);
   g_signal_handlers_unblock_by_func(widgetPtr, userListWhf, NULL);
   free(doubleList);
   updateRdesc(true); // LIB: 
   //g_print("userRdesc:'%s'\n", userRdesc);
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   updateLabelDesc(); // GUI
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem(); // GUI
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   //g_print("customValues RadioButton end\n");
} // userValuesWhf()

static void userListWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on user list type values
   int ret;
   g_print("UserList Entry\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetWhfPtr);
   //g_print("text:'%s'\n", textPtr);
   // make a copy of textPtr[]
   int l = strlen(textPtr);
   gchar* txtPtr = malloc(l+1);
   strcpy(txtPtr, textPtr);
   // check are only digits, dots and commas
   ret = isNumber(txtPtr, true);
   //printf("str:'%s' ret:%d (1=number, 0=other, -1=ERROR)\n", txtPtr, ret);
   if (ret!=1) {
      //printf("str:'%s' not only numbers\n", txtPtr);
      printf("WARN: %s not only numbers\n", __FUNCTION__);
      g_free(txtPtr);
      return;
   }
   int c;
   //for (c=0; c<numR1conf; c++) {
   //   printf("userR[%d]:%f\n", c, userR[c]);
   //}
   char* chrPtr;
   char* prvPtr = txtPtr;
   for (c=0; ; c++) { // count commas
      chrPtr = strchr(prvPtr, ',');
      if (chrPtr==NULL) break;
      prvPtr = chrPtr+1;
      //printf("prvPtr:%p *prvPtr:%c c:%d\n", prvPtr, *prvPtr, c);
   }
   if (c==0) return;
   c++;
   //printf("tot c:%d\n", c);
   if (userR) free(userR);
   userR = calloc(c, sizeof(double));

   int n, len;
   prvPtr = txtPtr;
   for (n=0; n<c; n++) {
      double num = 0;
      while (num<1 && *prvPtr!='\0') { // skip letters and consecutive commas
         chrPtr = strchr(prvPtr, ',');
         //printf("prvPtr:%p chrPtr:%p\n", prvPtr, chrPtr);
         //printf("*prvPtr:'%c' str:'%s' ", *prvPtr, prvPtr);
         //if (chrPtr!=NULL) printf("*chrPtr:'%c' nstr:'%s'\n", *chrPtr, chrPtr);
         //else printf("\n");
         if (chrPtr==NULL)
            len = strlen(prvPtr);
         else
            len = chrPtr-prvPtr;
         //printf("len:%d\n", len);
         //char doubleStr[len+1];
         char doubleStr[12];
         strncpy(doubleStr, prvPtr, len);
         doubleStr[len]='\0';
         //printf("doubleStr:'%s'", doubleStr);
         num = strtod(doubleStr, NULL);
         //printf(" num:%g\n", num);
         prvPtr = chrPtr+1;
         if (num==0) { c--; /*printf("c:%d\n", c);*/ prvPtr = chrPtr+1; }
      }
      if (num!=0) {
         userR[n]=num;
         //printf("userR[%d]:%g\n", n, userR[n]);
      } else c--;
      if (chrPtr==NULL) break;
   }
   //printf("c final:%d\n", c);
   //for (n=0; n<c; n++) {
   //   printf("userR[%d]:%f\n", n, userR[n]);
   //}
   //printf("numR1:%u\n", numR1);
   numR1 = c; // new user quantity
   //printf("numR1:%u\n", numR1);
   //printf("numR1conf:%u\n", numR1conf);
   //printf("numR1gui :%u\n", numR1gui);
   ret = backGuiVal(); // backup gui 'expr' and 'userR'
   //printf("numR1:%u\n", numR1);
   //printf("numR1conf:%u\n", numR1conf);
   //printf("numR1gui :%u\n", numR1gui);
   //for (c=0; c<numR1gui; c++) {
   //   printf("userRgui[%d]:%f\n", c, userRgui[c]);
   //}

   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   ret = updateRdesc(true); // LIB
   updateLabelDesc(); // GUI
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   g_free(txtPtr);
   g_print("UserList Entry end\n");
} // userListWhf()

static void bestTolWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on bestTol
   //GtkWidget* widgetPtr;
   //int ret;
   g_print("bestTol CheckButton\n");
   guint checkVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetWhfPtr);
   g_print("value:'%u'\n", checkVal);
   if (checkVal == TRUE)
      bestTol = 1;
   else
      bestTol = 0;
} // bestTolWhf()

static void userRtolWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on userRtol
   GtkWidget* widgetPtr;
   //int ret;
   g_print("userRtol ComboBox\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetWhfPtr);
   g_print("value:'%s'\n", comboValPtr);
   userRtol = strtof(comboValPtr, NULL);
   updateRdesc(true); // LIB
   updateLabelDesc(); // GUI
   tolRatio = userRtol/userR2tol; // 1/0.1=10
   char* tolRatioStr = malloc(LineLen);
   sprintf(tolRatioStr, "tolerance ratio: %g ", tolRatio);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "tolRatioWdg"));
   gtk_label_set_text((GtkLabel*)widgetPtr, tolRatioStr);
   free(tolRatioStr);
} // userRtolWhf()

static void listsWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on 2nd list
   GtkWidget* widgetPtr;
   int ret;
   g_print("lists CheckButton\n");
   guint checkVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetWhfPtr);
   g_print("value:'%u'\n", checkVal);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList2"));
   if (checkVal==false) { // one list
      //g_print("back to one list\n");
      //g_print("EserieBack:%u\n", EserieBack);
      Eserie = EserieBack;
      maxRp = maxRpBack;
      lists = 1;
      gtk_entry_set_text((GtkEntry*)widgetPtr, "");
      gtk_widget_set_sensitive(widgetPtr, false);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userR2tolWdg"));
      gtk_widget_set_sensitive(widgetPtr, false);
      if (Eserie == 0) { // was user list
         widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userValues"));
         gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, true);
         widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "standardSeries"));
         gtk_widget_set_sensitive(widgetPtr, true);
      } else { // standard Eserie
         g_print("back to standard series\n");
         widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "standardSeries"));
         gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, true);
         gtk_widget_set_sensitive(widgetPtr, true);
      }
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "Rp"));
      if (maxRp == 1)
         gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, false);
      else
         gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, true);
   } else { // two lists
      EserieBack = Eserie; // backup
      maxRpBack = maxRp; // backup
      lists = 2;
      Eserie = 0;
      //g_print("EserieBack:%u\n", EserieBack);
      //g_print("Eserie:%u\n", Eserie);
      gtk_widget_set_sensitive(widgetPtr, true);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userR2tolWdg"));
      gtk_widget_set_sensitive(widgetPtr, true);
      char* doubleList;
      if (userR2gui!=NULL) { // user has already type custom list values
         g_print("populate customValue list2 with userGui values...\n");
         numR2 = numR2gui;
         uint16_t len = 0;
         char doubleStr[25];
         for (uint16_t r=0; r<numR2; r++) {
            sprintf(doubleStr, "%g", userR2gui[r]);
            len+=strlen(doubleStr); // count chars
         }
         len+=numR2; // space for commas
         doubleList = calloc(len+1,1);
         for (uint16_t r=0; r<numR2; r++) {
            sprintf(doubleStr, "%g", userR2gui[r]);
            strcat(doubleList, doubleStr);
            if (r<numR2-1) strcat(doubleList, ",");
         }
         //g_print("custom values:'%s'\n", doubleList);
      } else { // use config file
         g_print("populate customValue list2 with config values...\n");
         numR2 = numR2conf;
         uint16_t len = 0;
         char doubleStr[25];
         for (uint16_t r=0; r<numR2; r++) {
            sprintf(doubleStr, "%g", userR2conf[r]);
            len+=strlen(doubleStr); // count chars
         }
         len+=numR2; // space for commas
         doubleList = calloc(len+1,1);
         for (uint16_t r=0; r<numR2; r++) {
            sprintf(doubleStr, "%g", userR2conf[r]);
            strcat(doubleList, doubleStr);
            if (r<numR2-1) strcat(doubleList, ",");
         }
         //g_print("custom values:'%s'\n", doubleList);
      }
      //g_print("%s numR2:%u\n", __FUNCTION__, numR2);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList2"));
      //g_print("userRgui:%p\n", userRgui);
      //g_print("userR2gui:%p\n", userR2gui);
      g_signal_handlers_block_by_func(widgetPtr, userList2Whf, NULL);
      gtk_entry_set_text((GtkEntry*)widgetPtr, doubleList);
      g_signal_handlers_unblock_by_func(widgetPtr, userList2Whf, NULL);
      free(doubleList);
      //g_print("userRgui:%p\n", userRgui);
      //g_print("userR2gui:%p\n", userR2gui);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userValues"));
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, true);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "Rp"));
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, true);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "standardSeries"));
      gtk_widget_set_sensitive(widgetPtr, false);
   }
   updateRdesc(true); // LIB: update userRdesc from baseEdesc
   //g_print("userR2desc:'%s'\n", userR2desc);
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   updateLabelDesc(); // GUI
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem(); // GUI
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   //g_print("lists CheckButton end\n");
} // listsWhf()

static void userList2Whf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on userList2 type values
   int ret;
   g_print("UserList2 Entry\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetWhfPtr);
   //g_print("text:'%s'\n", textPtr);
   // make a copy of textPtr[]
   int l = strlen(textPtr);
   gchar* txtPtr = malloc(l+1);
   strcpy(txtPtr, textPtr);
   // check are only digits, dots and commas
   ret = isNumber(txtPtr, true);
   //printf("str:'%s' ret:%d (1=number, 0=other, -1=ERROR)\n", txtPtr, ret);
   if (ret!=1) {
      //printf("str:'%s' not only numbers\n", txtPtr);
      printf("WARN: %s not only numbers\n", __FUNCTION__);
      g_free(txtPtr);
      return;
   }
   int c;
   //for (c=0; c<numR1conf; c++) {
   //   printf("userR[%d]:%f\n", c, userR[c]);
   //}
   char* chrPtr;
   char* prvPtr = txtPtr;
   for (c=0; ; c++) { // count commas
      chrPtr = strchr(prvPtr, ',');
      if (chrPtr==NULL) break;
      prvPtr = chrPtr+1;
      //printf("prvPtr:%p *prvPtr:%c c:%d\n", prvPtr, *prvPtr, c);
   }
   if (c==0) return;
   c++;
   //printf("tot c:%d\n", c);
   if (userR2) free(userR2);
   userR2 = calloc(c, sizeof(double));

   int n, len;
   prvPtr = txtPtr;
   for (n=0; n<c; n++) {
      double num = 0;
      while (num<1 && *prvPtr!='\0') { // skip letters and consecutive commas
         chrPtr = strchr(prvPtr, ',');
         //printf("prvPtr:%p chrPtr:%p\n", prvPtr, chrPtr);
         //printf("*prvPtr:'%c' str:'%s' ", *prvPtr, prvPtr);
         //if (chrPtr!=NULL) printf("*chrPtr:'%c' nstr:'%s'\n", *chrPtr, chrPtr);
         //else printf("\n");
         if (chrPtr==NULL)
            len = strlen(prvPtr);
         else
            len = chrPtr-prvPtr;
         //printf("len:%d\n", len);
         //char doubleStr[len+1];
         char doubleStr[12];
         strncpy(doubleStr, prvPtr, len);
         doubleStr[len]='\0';
         //printf("doubleStr:'%s'", doubleStr);
         num = strtod(doubleStr, NULL);
         //printf(" num:%g\n", num);
         prvPtr = chrPtr+1;
         if (num==0) { c--; /*printf("c:%d\n", c);*/ prvPtr = chrPtr+1; }
      }
      if (num!=0) {
         userR2[n]=num;
         //printf("userR2[%d]:%g\n", n, userR2[n]);
      } else c--;
      if (chrPtr==NULL) break;
   }
   //printf("c final:%d\n", c);
   //for (n=0; n<c; n++) {
   //   printf("userR2[%d]:%f\n", n, userR2[n]);
   //}
   //printf("numR2:%u\n", numR2);
   numR2 = c; // new user quantity
   //printf("numR2:%u\n", numR2);
   //printf("numR2conf:%u\n", numR2conf);
   //printf("numR2gui :%u\n", numR2gui);
   ret = backGuiVal(); // backup gui 'expr' and 'userR'
   //printf("numR2:%u\n", numR2);
   //printf("numR2conf:%u\n", numR2conf);
   //printf("numR2gui :%u\n", numR2gui);
   //for (c=0; c<numR2gui; c++) {
   //   printf("userR2gui[%d]:%f\n", c, userR2gui[c]);
   //}

   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   ret = updateRdesc(true); // LIB
   updateLabelDesc(); // GUI
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   g_free(txtPtr);
} // userList2Whf()

static void userR2tolWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on userR2tol
   GtkWidget* widgetPtr;
   //int ret;
   g_print("userR2tol ComboBox\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetWhfPtr);
   g_print("value:'%s'\n", comboValPtr);
   userR2tol = strtof(comboValPtr, NULL);
   updateRdesc(true); // LIB
   updateLabelDesc(); // GUI
   tolRatio = userRtol/userR2tol; // 1/0.1=10
   char* tolRatioStr = malloc(LineLen);
   sprintf(tolRatioStr, "tolerance ratio: %g ", tolRatio);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "tolRatioWdg"));
   gtk_label_set_text((GtkLabel*)widgetPtr, tolRatioStr);
   free(tolRatioStr);
} // userR2tolWhf()

static void RpWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on series/parallel tick
   GtkWidget* widgetPtr;
   int ret;
   g_print("Rp CheckButton\n");
   guint checkVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetWhfPtr);
   g_print("value:'%u'\n", checkVal);
   if (checkVal==0) {
      maxRp = 1;
      g_print("single values\n");
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaDropWhf(widgetPtr, NULL);
      //g_print("set ID:circuits with pixbuf='circuit03.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
   }
   if (checkVal==1) {
      maxRp = 2;
      g_print("series/parallel allowed\n");
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaDropWhf(widgetPtr, NULL);
      //g_print("set ID:circuits with pixbuf='circuit03p.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
   }
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
} // RpWhf()

static void resultsWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on Results shown edit
   int ret;
   g_print("Results Entry\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetWhfPtr);
   //g_print("text:'%s'\n", textPtr);
   // make a copy of textPtr[]
   int l = strlen(textPtr);
   gchar* txtPtr = malloc(l+1);
   strcpy(txtPtr, textPtr);
   // check are only digits
   ret = isNumber(txtPtr, false);
   //printf("str:'%s' ret:%d (1=number, 0=other, -1=ERROR)\n", txtPtr, ret);
   if (ret!=1) {
      printf("str:'%s' not digits only\n", txtPtr);
      g_free(txtPtr);
      //widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "results"));
      char unsignedStr[25];
      sprintf(unsignedStr, "%u", numBestRes);
      gtk_entry_set_text((GtkEntry*)widgetWhfPtr, unsignedStr);
      return;
   }
   u64 tmpNum = atoi(txtPtr);
   u64 tmpMax=NumberResMax; // 512000U
   if (tmpNum==0 || tmpNum>tmpMax) {
      printf("Unsupported:'%s'\n", txtPtr);
      numBestRes=NumberResDefault;
   } else numBestRes = tmpNum;
   g_free(txtPtr);
   printf("numBestRes:%u\n", numBestRes);
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
} // resultsWhf()

static void resolveWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on ReSolve button
   int ret;
   g_print("Resolve Button\n");
   stop = false;
   ret = runReSolve();
   if (ret != 0) {
      printf("runReSolve returned:%u, quit\n", ret);
      return;
   }
} // resolveWhf()

static void aboutWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on about button
   g_print("About Button\n");
   const gchar* auth[]={ Author, 0 };
   GdkPixbuf* logoPtr = gdk_pixbuf_new_from_file("./ReSolve.png", NULL);
   GtkWidget* aboutPtr = gtk_about_dialog_new();
   gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(aboutPtr), AppName);
   gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(aboutPtr), logoPtr);
   gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(aboutPtr), ReSolveVer);
   gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(aboutPtr),"Copyright "CopyrightYear" "Author);
   gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(aboutPtr), GTK_LICENSE_GPL_3_0);
   gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(aboutPtr), "Solve math expressions using discrete values");
   gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(aboutPtr), auth);
   gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(aboutPtr), "http://"WebLink);
   gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(aboutPtr), "http://"WebLabel);
   gtk_dialog_run(GTK_DIALOG(aboutPtr));
   g_object_unref(logoPtr), logoPtr = NULL;
} // aboutWhf()

static void stopWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on stop button
   stop = true;
   g_print("Stop Button\n");
} // stopWhf()

int quit() { // called also on Window destroy
   // free mem allocated by fillConfigVars()
   if (userR) free(userR);
   if (userRconf) free(userRconf); // only when GUI
   if (userRgui) free(userRgui); // only when GUI
   gtk_main_quit();
   return EXIT_SUCCESS;
} // quit()

static void quitWhf(GtkWidget* widgetWhfPtr, gpointer dataPtr) { // called on quit button
   g_print("Quit Button\n");
   quit();
} // quitWhf()

// this is called by gprintf() when guiUpdateOutPtr!=NULL
// Note: clear output buffer on empty string
int guiUpdateOut(char* txtPtr, int l) { // update output widgets with results values
   GtkWidget* widgetPtr;
   GtkTextBuffer* textBufferPtr;
   //g_print("guiUpdateOut start\n");
   textBufferPtr = GTK_TEXT_BUFFER(gtk_builder_get_object(builderPtr, "output"));
   if (txtPtr==NULL) { // clear
      //g_print("GUI output clear\n");
      gtk_text_buffer_set_text((GtkTextBuffer*)textBufferPtr, "", -1);
   } else { // append to prev
      //g_print("GUI output append\n");
      GtkTextIter iter;
      gtk_text_buffer_get_end_iter((GtkTextBuffer*)textBufferPtr, &iter);
      //g_print("txtPtr:'%s'\n", txtPtr);
      gtk_text_buffer_insert((GtkTextBuffer*)textBufferPtr, &iter, txtPtr, l);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "textview"));
      gtk_text_view_scroll_to_iter((GtkTextView*)widgetPtr,&iter,0,0,1,1);
   }
   int c = 0;
   while(gtk_events_pending()) { // while is TRUE, force GUI update
      #ifdef _WIN32
         if (c>0 && winGuiLoop==0) break; // on Win skip next calls
      #endif
      gtk_main_iteration_do(0);
      c++;
   }
   //g_print("guiUpdateOut end\n");
   return OK;
} // guiUpdateOut()

int runReSolve() { // memSize, memAlloc, doCalc, show output, freeMem
   int  ret;
   winGuiLoop = 1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // clear output widget
   ret = guiUpdateOut(NULL, 0);

   // 1 - checking arguments syntax/config value validity
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables

   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ERROR; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ERROR; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ERROR; }

   // 4 - show config values
   ret = showConf(); // LIB: show config set

   gprintf(gui, "\nReSolve ...\n");
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
   stringPtr = siMem(allocatedB);
   gprintf(gui, "Allocated about %s of total RAM\n", stringPtr);
   free(stringPtr);

   // 6 - fill the input vectors with needed data
   // 7 - calculus of solutions
   // 8 - sorting of solutions
   clock_t start, stop;
   double time;
   start = clock();
   ret = doCalc(); // LIB: fill inputs, calcs, sort solutions
   stop = clock();
   time = (double)(stop - start) / CLOCKS_PER_SEC; time+=0;
   gprintf(gui, "Compute time: %f s\n", time);

   // 9 - print results
   winGuiLoop = 0; // Win loop gtk_events_pending/gtk_main_iteration to update GUI
   gprintf(gui, "Printing best:%u solutions (top worst, botton best) in all configurations\n\n", numBestRes);
   ret = showVal(numBestRes); // LIB: show Solutions
   winGuiLoop = 1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // 10 - freeing dynamic allocated memory ...
   ret = freeMem(); // LIB: free memory

   return OK;
} // runReSolve()

int guiUpdateStart() { // update widgets with input/config values at startup
   GtkWidget* widgetPtr;
   //g_print("guiUpdateStart start\n");

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
   //gtk_combo_box_set_active((GtkComboBox*)widgetPtr, 3);
   gtk_combo_box_set_id_column((GtkComboBox*)widgetPtr, 0);
   gboolean done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "VoltReg");
   if (done == FALSE) {
      printf("WARN: gtk_combo_box_set_active_id() ret FALSE\n");
   }

   //widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaUser"));
   //gtk_entry_set_text((GtkEntry*)widgetPtr, expr);

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "target"));
   gtk_spin_button_set_value((GtkSpinButton*)widgetPtr, target);

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "EseriesList"));
   gtk_combo_box_set_id_column((GtkComboBox*)widgetPtr, 0);
   switch (Eserie) {
   case (0):
      break;
   case (1):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E1");
      break;
   case (3):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E3");
      break;
   case (6):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E6");
      break;
   case (12):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E12");
      break;
   case (24):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E24");
      break;
   case (48):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E48");
      break;
   case (96):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E96");
      break;
   case (192):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "E192");
      break;
   default:
      printf("Unsupported Series:%u. Supported are 0, 1, 3, 6, 12, 24, 48, 96 and 192\n", Eserie);
      return ERROR;
   }

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "decades"));
   gtk_spin_button_set_value((GtkSpinButton*)widgetPtr, decades);

   if (Eserie==0) {
      uint16_t len = 0;
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList"));
      //printf("numR1:%u\n", numR1);
      char doubleStr[25];
      for (uint16_t r=0; r<numR1; r++) {
         sprintf(doubleStr, "%g", userR[r]);
         len+=strlen(doubleStr);
      }
      len+=numR1;
      char* doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<numR1; r++) {
         sprintf(doubleStr, "%g", userR[r]); // switch to engStr()
         strcat(doubleList, doubleStr);
         if (r<numR1-1) strcat(doubleList, ",");
      }
      //printf("user values:'%s'\n", doubleList);
      gtk_entry_set_text((GtkEntry*)widgetPtr, doubleList);
      free(doubleList);
   }

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "standardSeries"));
   //g_print("Eserie:'%u'\n", Eserie);
   if (Eserie>0) { // standard Eseries
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userValues"));
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else { // custom list
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userValues"));
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   done = FALSE;
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userRtolWdg"));
   gtk_combo_box_set_id_column((GtkComboBox*)widgetPtr, 0);
   u08 userRtolInt = (u08)lround(userRtol);
   if (userRtol==0.5) userRtolInt = 50;
   switch (userRtolInt) {
   case (80):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "80");
      break;
   case (40):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "40");
      break;
   case (20):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "20");
      break;
   case (10):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "10");
      break;
   case (5):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "5");
      break;
   case (2):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "2");
      break;
   case (1):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "1");
      break;
   case (50):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "0.5");
      break;
   case (0):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "0.1");
      break;
   default:
      printf("Unsupported Tolerance:%g. Supported are 80, 40, 20, 10, 5, 2, 1, 0.5 and 0.1\n", userRtol);
      return ERROR;
   }

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "listsWdg"));
   //g_print("lists:%u\n", lists);
   if (lists==1) {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else { // 2nd list checked
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "valTolBestWdg"));
   if (bestTol==1) { // bestTol checked
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   } else {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   }

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "tolRatioWdg"));
   char* tolRatioStr = malloc(LineLen);
   //g_print("tolerance ratio: %g\n", tolRatio);
   sprintf(tolRatioStr, "tolerance ratio: %g ", tolRatio);
   gtk_label_set_text((GtkLabel*)widgetPtr, tolRatioStr);
   free(tolRatioStr);

   if (lists==2) {
      uint16_t len = 0;
      widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList2"));
      printf("numR2:%u\n", numR2);
      char doubleStr[25];
      for (uint16_t r=0; r<numR2; r++) {
         sprintf(doubleStr, "%g", userR2[r]);
         len+=strlen(doubleStr);
      }
      len+=numR2;
      char* doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<numR2; r++) {
         sprintf(doubleStr, "%g", userR2[r]); // switch to engStr()
         strcat(doubleList, doubleStr);
         if (r<numR2-1) strcat(doubleList, ",");
      }
      //printf("user2 values:'%s'\n", doubleList);
      gtk_entry_set_text((GtkEntry*)widgetPtr, doubleList);
      free(doubleList);
   }

   done = FALSE;
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userR2tolWdg"));
   gtk_combo_box_set_id_column((GtkComboBox*)widgetPtr, 0);
   u08 userR2tolInt = (u08)lround(userR2tol);
   if (userR2tol==0.5) userR2tolInt = 50;
   switch (userR2tolInt) {
   case (80):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "80");
      break;
   case (40):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "40");
      break;
   case (20):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "20");
      break;
   case (10):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "10");
      break;
   case (5):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "5");
      break;
   case (2):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "2");
      break;
   case (1):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "1");
      break;
   case (50):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "0.5");
      break;
   case (0):
      done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "0.1");
      break;
   default:
      printf("Unsupported Tolerance:%g. Supported are 80, 40, 20, 10, 5, 2, 1, 0.5 and 0.1\n", userRtol);
      return ERROR;
   }

   updateLabelDesc();

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "Rp"));
   if (maxRp==1) {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "results"));
   char unsignedStr[25];
   sprintf(unsignedStr, "%u", numBestRes);
   gtk_entry_set_text((GtkEntry*)widgetPtr, unsignedStr);

   updateLabelMem();
   //g_print("guiUpdateStart end\n");
   return OK;
} // guiUpdateStart()

int guiInit(int numPar, char* paramPtr[]) { // create the main window
   GtkWidget* widgetPtr;
   gtk_init(&numPar, &paramPtr);

   /* Construct a GtkBuilder instance and load our UI description */
#if 0
   builderPtr = gtk_builder_new_from_file("reSolve.glade");
   printf("gtk_builder_new_from_file() returned:%p\n", builderPtr);
   if (builderPtr == NULL) {
      //g_printerr("Error loading file: %s\n", errorPtr->message);
      //g_clear_error(&errorPtr);
      printf("'reSolve.glade' not found, quit\n");
      return ERROR;
   }
#endif
   builderPtr = gtk_builder_new();
   //printf("Try to load 'reSolve.glade'\n");
   GError* errorPtr = NULL;
   gboolean bit;
   bit = gtk_builder_add_from_file(builderPtr, "reSolve.glade", &errorPtr);
   if (bit == 0) {
      g_printerr("Error loading file: %s\n", errorPtr->message);
      g_clear_error(&errorPtr);
      return ERROR;
   }
   //printf("'reSolve.glade' loaded\n");

   /* Connect signal handlers to the constructed widgets */
   windowPtr = GTK_WINDOW(gtk_builder_get_object(builderPtr, "window"));
   g_signal_connect(windowPtr, "destroy", G_CALLBACK(quit), NULL);

   //gtk_builder_connect_signals(builderPtr, NULL); // maybe unnecessary
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(formulaDropWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaUser"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(formulaUserWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "target"));
   g_signal_connect(widgetPtr, "value-changed", G_CALLBACK(targetWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "standardSeries"));
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(standardSerieWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "EseriesList"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(EseriesListWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "decades"));
   g_signal_connect(widgetPtr, "value-changed", G_CALLBACK(decadesWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userValues"));
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(userValuesWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(userListWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "valTolBestWdg"));
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(bestTolWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userRtolWdg"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(userRtolWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "listsWdg"));
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(listsWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userList2"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(userList2Whf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "userR2tolWdg"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(userR2tolWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "Rp"));
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(RpWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "results"));
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(resultsWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "resolveButton"));
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(resolveWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "aboutButton"));
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(aboutWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "stopButton"));
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(stopWhf), NULL);
   widgetPtr = GTK_WIDGET(gtk_builder_get_object(builderPtr, "quitButton"));
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(quitWhf), NULL);

   //gtk_widget_show(GTK_WIDGET(windowPtr)); // seems unnecessary

   return OK;
} // guiInit()

int main(int numPar, char* paramPtr[]) { // GUI entry point
   int ret;

   gui = 0; // mean gprintf() update GUI

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
   ret = exprCheck(); // LIB: check expression syntax
   // 2 - read and set user request
   ret = globalInit(); // LIB: set internal variables

   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   ret = memResCalc(); // LIB: calculate the needed memory for results

   // 4 - show config values
   ret = showConf(); // LIB: show config set

   ret = backConfVal(); // GUI: backup config 'expr' and 'userR'

   //putenv("LANG=C"); // as now use C locale to avoid trouble with .|,
   setlocale(LC_ALL,"C");
   gtk_disable_setlocale();
   printf("Starting GUI ...\n");
   ret = guiInit(numPar, paramPtr);
   if (ret!=OK) {
      printf("GUI cannot be initialized, quit\n");
      exit(1);
   }
   gui = 1; // mean gprintf() update GUI
   guiUpdateOutPtr = &guiUpdateOut; // function pointer to guiUpdateOut()
   winGuiLoop = 1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   ret = guiUpdateStart();

   //ret = runReSolve(); // steps 5 to 10 called by widget callbacks

   gtk_main();

   return EXIT_SUCCESS;
} // int main()
