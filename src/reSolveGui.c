/* ReSolve v0.11.09h 2023/08/29 solve math expressions using discrete values*/
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
GObject* windowPtr;
GObject* widgetPtr;

char    exprConf[LineLen]; // backup config expression
double* userRconf;         // backup config list values
u16     listNumberConf;    // backup config list quantity
char    exprGui[LineLen];  // backup userGui expression
double* userRgui;          // backup userGui list values
u16     listNumberGui;     // backup userGui list quantity

int runReSolve(); // memSize, memAlloc, doCalc, show output, freeMem

int backVal() { // backup 'expr' and 'userR'
   strcpy(exprConf, expr);
   int size=sizeof(double)*listNumberConf;
   userRconf=malloc(size);
   for (uint16_t r=0; r<listNumberConf; r++) {
      userRconf[r]=userR[r];
   }
   return OK;
} // backVal()

int updateLabelDesc() { // called to update Eserie desc
   widgetPtr = gtk_builder_get_object(builderPtr, "description");
   gtk_label_set_text((GtkLabel*)widgetPtr, userRdesc);
   return OK;
} // updateLabelDesc()

int updateLabelMem() { // called to update shown memory allocation
   char* stringPtr;
   stringPtr=siMem(allocatedB);
   //g_print("FUNCT: %s allocat:'%s'\n", __FUNCTION__, stringPtr);
   char* strPtr;
   asprintf(&strPtr, "Will allocate about %s of total RAM", stringPtr);
   free(stringPtr);
   GObject* widget2Ptr = gtk_builder_get_object(builderPtr, "allocate");
   gtk_label_set_text((GtkLabel*)widget2Ptr, strPtr);
   free(strPtr);
   return OK;
} // updateLabelMem()

static void formulaPre(GtkWidget* widgetPtr, gpointer dataPtr) { // called on dropdown preset formula selector
   g_print("FormulaPre dropdown\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
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
      widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "formula");
      gtk_widget_set_sensitive((GtkWidget*)widgetPtr, true);
      if (exprGui[0]!='\0') { // user has already type a formula
         gtk_entry_set_text((GtkEntry*)widgetPtr, exprGui);
         strcpy(expr, exprGui);
      } else { // use config file
         gtk_entry_set_text((GtkEntry*)widgetPtr, exprConf);
         strcpy(expr, exprConf);
      }
   } else { // blank custom formula
      widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "formula");
      gtk_widget_set_sensitive((GtkWidget*)widgetPtr, false);
      gtk_entry_set_text((GtkEntry*)widgetPtr, "");
   }
   printf("expr:'%s'\n", expr);
   //printf("exprConf:'%s'\n", exprConf);
   //printf("exprGui:'%s'\n", exprGui);
   g_free(comboValPtr);
} // formulaPre()

static void formula(GtkWidget* widgetPtr, gpointer dataPtr) { // called on custom formula type
   g_print("Formula editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   g_print("text:'%s'\n", textPtr);
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "formulaList");
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
} // formula()

static void desiredFct(GtkWidget* widgetPtr, gpointer dataPtr) { // called on target value change
   g_print("Desired spinButton\n");
   gdouble spinVal = gtk_spin_button_get_value((GtkSpinButton*)widgetPtr);
   g_print("value:'%g'\n", spinVal);
   target=spinVal;
} // desiredFct()

static void standardSeries(GtkWidget* widgetPtr, gpointer dataPtr) { // called on radio button standard series
   int ret;
   //g_print("standardSeries radioButton\n");
   guint radioVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", radioVal);
   if (radioVal==0) { // custom values
      // update listNumber from custom list qty
      // wil do customValues()
      g_print("standardSeries radioButton break\n");
      return;
   } // standard Eseries
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "EseriesList");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   //char valStr[5];
   //sprintf(valStr, "%u", radioVal);
   //g_print("EseriesList combo value:'%s'\n", comboValPtr);
   ret = updateEserie(comboValPtr); // LIB: update u08 Eserie from char* EseriePtr
   g_print("Eserie:E%u\n", Eserie);
   g_free(comboValPtr);
   ret = updateRdesc(true); // LIB: 
   //g_print("userRdesc:'%s'\n", userRdesc);
   ret = updateLabelDesc();
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "EseriesList");
   gtk_widget_set_sensitive(widgetPtr, true);
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "decades");
   gtk_widget_set_sensitive(widgetPtr, true);
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "customList");
   gtk_widget_set_sensitive(widgetPtr, false);
   gtk_entry_set_text((GtkEntry*)widgetPtr, "");
   //g_print("standardSeries radioButton quit\n");
} // standardSeries()

static void EseriesList(GtkWidget* widgetPtr, gpointer dataPtr) { // called on Eseries dropdown
   int ret;
   g_print("EseriesList dropdown\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   //char valStr[5];
   //sprintf(valStr, "%u", radioVal);
   //g_print("value:'%s'\n", comboValPtr);
   ret = updateEserie(comboValPtr); // LIB: update Eserie
   g_print("Eserie:E%u\n", Eserie);
   g_free(comboValPtr);
   updateRdesc(true); // LIB: 
   updateLabelDesc();
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   //widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "standardSeries");
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   //widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "customValues");
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
} // EseriesList()

static void decadesFct(GtkWidget* widgetPtr, gpointer dataPtr) { // called on Decades change
   int ret;
   g_print("Decades spinButton\n");
   gdouble spinVal = gtk_spin_button_get_value((GtkSpinButton*)widgetPtr);
   //g_print("value:'%g'\n", spinVal);
   decades=spinVal;
   printf("decades:%u\n", decades);
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
} // decadesFct()

static void customValues(GtkWidget* widgetPtr, gpointer dataPtr) { // called on radio button custom values
   int ret;
   //g_print("customValues radioButton\n");
   guint radioVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", radioVal);
   if (radioVal==0) { // standard Eseries
      // update listNumber from Eserie list qty
      // wil do standardSeries()
      g_print("customValues radioButton break\n");
      return;
   } // custom values
   //g_print("listNumber:%u\n", listNumber);
   //g_print("listNumberConf:%u\n", listNumberConf);
   Eserie=0;
   char* doubleList;
   if (userRgui!=NULL) { // user has already type custom list values
      g_print("populate customValue list with user values...\n");
      listNumber=listNumberGui;
      uint16_t len=0;
      char doubleStr[25];
      for (uint16_t r=0; r<listNumberGui; r++) {
         sprintf(doubleStr, "%g", userRgui[r]);
         len+=strlen(doubleStr); // count chars
      }
      len+=listNumberGui; // space for commas
      doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<listNumberGui; r++) {
         sprintf(doubleStr, "%g", userRgui[r]);
         strcat(doubleList, doubleStr);
         if (r<listNumberGui-1) strcat(doubleList, ",");
      }
      //g_print("custom values:'%s'\n", doubleList);
   } else { // use config file
      g_print("populate customValue list with config values...\n");
      listNumber=listNumberConf;
      uint16_t len=0;
      char doubleStr[25];
      for (uint16_t r=0; r<listNumberConf; r++) {
         sprintf(doubleStr, "%g", userRconf[r]);
         len+=strlen(doubleStr); // count chars
      }
      len+=listNumberConf; // space for commas
      doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<listNumberConf; r++) {
         sprintf(doubleStr, "%g", userRconf[r]);
         strcat(doubleList, doubleStr);
         if (r<listNumberConf-1) strcat(doubleList, ",");
      }
      g_print("custom values:'%s'\n", doubleList);
   }
   g_print("listNumber:%u\n", listNumber);
   numR1=listNumber;
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "EseriesList");
   gtk_widget_set_sensitive(widgetPtr, false);
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "decades");
   gtk_widget_set_sensitive(widgetPtr, false);
   widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "customList");
   gtk_widget_set_sensitive(widgetPtr, true);
   gtk_entry_set_text((GtkEntry*)widgetPtr, doubleList);
   free(doubleList);
   updateRdesc(true); // LIB: 
   //g_print("userRdesc:'%s'\n", userRdesc);
   updateLabelDesc(); // GUI
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem(); // GUI
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   //g_print("customValues radioButton end\n");
} // customValues()

static void customList(GtkWidget* widgetPtr, gpointer dataPtr) { // called on custom list type values
   int ret;
   g_print("CustomList editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   //g_print("text:'%s'\n", textPtr);
   // make a copy of textPtr[]
   int l=strlen(textPtr);
   gchar* txtPtr=malloc(l+1);
   strcpy(txtPtr, textPtr);
   // check are only digits, dots and commas
   ret=isNumber(txtPtr, true);
   //printf("str:'%s' ret:%d (1=number, 0=other, -1=ERROR)\n", txtPtr, ret);
   if (ret!=1) {
      printf("str:'%s' not only numbers\n", txtPtr);
      g_free(txtPtr);
      return;
   }
   int c;
   //for (c=0; c<listNumberConf; c++) {
   //   printf("userR[%d]:%f\n", c, userR[c]);
   //}
   char* chrPtr;
   char* prvPtr=txtPtr;
   for (c=0; ; c++) { // count commas
      chrPtr=strchr(prvPtr, ',');
      if (chrPtr==NULL) break;
      prvPtr=chrPtr+1;
      //printf("prvPtr:%p *prvPtr:%c c:%d\n", prvPtr, *prvPtr, c);
   }
   if (c==0) return;
   c++;
   //printf("tot c:%d\n", c);
   if (userR) free(userR);
   userR = calloc(c, sizeof(double));

   int n, len;
   prvPtr=txtPtr;
   for (n=0; n<c; n++) {
      double num=0;
      while (num<1 && *prvPtr!='\0') { // skip letters and consecutive commas
         chrPtr=strchr(prvPtr, ',');
         //printf("prvPtr:%p chrPtr:%p\n", prvPtr, chrPtr);
         //printf("*prvPtr:'%c' str:'%s' ", *prvPtr, prvPtr);
         //if (chrPtr!=NULL) printf("*chrPtr:'%c' nstr:'%s'\n", *chrPtr, chrPtr);
         //else printf("\n");
         if (chrPtr==NULL)
            len=strlen(prvPtr);
         else
            len=chrPtr-prvPtr;
         //printf("len:%d\n", len);
         //char doubleStr[len+1];
         char doubleStr[12];
         strncpy(doubleStr, prvPtr, len);
         doubleStr[len]='\0';
         //printf("doubleStr:'%s'", doubleStr);
         num=strtod(doubleStr, NULL);
         //printf(" num:%g\n", num);
         prvPtr=chrPtr+1;
         if (num==0) { c--; /*printf("c:%d\n", c);*/ prvPtr=chrPtr+1; }
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
   //printf("listNumber:%u\n", listNumber);
   listNumber=c;
   //printf("listNumber:%u\n", listNumber);
   //printf("listNumberConf:%u\n", listNumberConf);
   //printf("listNumberGui:%u\n", listNumberGui);
   userRgui=calloc(c, sizeof(double));
   listNumberGui=c;
   for (c=0; c<listNumberGui; c++) {
      userRgui[c]=userR[c];
   }
   //printf("listNumber:%u\n", listNumber);
   //printf("listNumberConf:%u\n", listNumberConf);
   //printf("listNumberGui:%u\n", listNumberGui);
   //for (c=0; c<listNumberGui; c++) {
   //   printf("userRgui[%d]:%f\n", c, userRgui[c]);
   //}

   //widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "customValues");
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);

   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   g_free(txtPtr);
} // customList()

static void Rp(GtkWidget* widgetPtr, gpointer dataPtr) { // called on series/parallel tick
   int ret;
   g_print("Rp checkButton\n");
   guint checkVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", checkVal);
   if (checkVal==0) {
      maxRp=1;
      g_print("single values\n");
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaPre(widgetPtr, NULL);
      //g_print("set ID:circuits with pixbuf='circuit03.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
   }
   if (checkVal==1) {
      maxRp=2;
      g_print("series/parallel allowed\n");
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaPre(widgetPtr, NULL);
      //g_print("set ID:circuits with pixbuf='circuit03p.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
   }
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
} // Rp()

static void resultsFct(GtkWidget* widgetPtr, gpointer dataPtr) { // called on Results shown edit
   int ret;
   g_print("Results editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   //g_print("text:'%s'\n", textPtr);
   // make a copy of textPtr[]
   int l=strlen(textPtr);
   gchar* txtPtr=malloc(l+1);
   strcpy(txtPtr, textPtr);
   // check are only digits
   ret=isNumber(txtPtr, false);
   //printf("str:'%s' ret:%d (1=number, 0=other, -1=ERROR)\n", txtPtr, ret);
   if (ret!=1) {
      printf("str:'%s' not digits only\n", txtPtr);
      g_free(txtPtr);
      //widgetPtr = (GtkWidget*)gtk_builder_get_object(builderPtr, "results");
      char unsignedStr[25];
      sprintf(unsignedStr, "%u", numBestRes);
      gtk_entry_set_text((GtkEntry*)widgetPtr, unsignedStr);
      return;
   }
   u64 tmpNum=atoi(txtPtr);
   u64 tmpMax=(1<<(8*sizeof(numBestRes)))-1; // 65535
   if (tmpNum==0 || tmpNum>tmpMax) {
      printf("Unsupported:'%s'\n", txtPtr);
      numBestRes=NumberResDefault;
   } else numBestRes=tmpNum;
   g_free(txtPtr);
   printf("numBestRes:%u\n", numBestRes);
   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ; }
} // resultsFct()

static void resolveButton(GtkWidget* widgetPtr, gpointer dataPtr) { // called on ReSolve button
   int ret;
   g_print("Resolve button\n");
   ret=runReSolve();
   if (ret != 0) {
      printf("runReSolve returned:%u, quit\n", ret);
      return;
   }
} // resolveButton()

static void aboutButton(GtkWidget* widgetPtr, gpointer dataPtr) { // called on about button
   g_print("About button\n");
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
} // aboutButton()

static void stopButton(GtkWidget* widgetPtr, gpointer dataPtr) { // called on stop button
   g_print("Stop button\n");
} // stopButton()

int quit() { // called also on Window destroy
   // free mem allocated by fillConfigVars()
   if (userR) free(userR);
   if (userRconf) free(userRconf); // only when GUI
   if (userRgui) free(userRgui); // only when GUI
   gtk_main_quit();
   return EXIT_SUCCESS;
} // quit()

static void quitButton(GtkWidget* widgetPtr, gpointer dataPtr) { // called on quit button
   g_print("Quit button\n");
   quit();
} // quitButton()

int guiUpdateIn() { // update widgets with input/config values
   widgetPtr = gtk_builder_get_object(builderPtr, "formulaList");
   //gtk_combo_box_set_active((GtkComboBox*)widgetPtr, 3);
   gtk_combo_box_set_id_column((GtkComboBox*)widgetPtr, 0);
   gboolean done = gtk_combo_box_set_active_id((GtkComboBox*)widgetPtr, "VoltReg");
   if (done == FALSE) {
      printf("WARN: gtk_combo_box_set_active_id() ret FALSE\n");
   }

   //widgetPtr = gtk_builder_get_object(builderPtr, "formula");
   //gtk_entry_set_text((GtkEntry*)widgetPtr, expr);

   widgetPtr = gtk_builder_get_object(builderPtr, "desired");
   gtk_spin_button_set_value((GtkSpinButton*)widgetPtr, target);

   widgetPtr = gtk_builder_get_object(builderPtr, "EseriesList");
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

   widgetPtr = gtk_builder_get_object(builderPtr, "decades");
   gtk_spin_button_set_value((GtkSpinButton*)widgetPtr, decades);

   widgetPtr = gtk_builder_get_object(builderPtr, "customList");
   //printf("listNumber:%u\n", listNumber);
   uint16_t len=0;
   char doubleStr[25];
   for (uint16_t r=0; r<listNumber; r++) {
      sprintf(doubleStr, "%g", userR[r]);
      len+=strlen(doubleStr);
   }
   len+=listNumber;
   char* doubleList = calloc(len+1,1);
   for (uint16_t r=0; r<listNumber; r++) {
      sprintf(doubleStr, "%g", userR[r]);
      strcat(doubleList, doubleStr);
      if (r<listNumber-1) strcat(doubleList, ",");
   }
   //printf("custom values:'%s'\n", doubleList);
   gtk_entry_set_text((GtkEntry*)widgetPtr, doubleList);
   free(doubleList);
   widgetPtr = gtk_builder_get_object(builderPtr, "standardSeries");
   //g_print("ESerie:'%u'\n", Eserie);
   if (Eserie>0) { // standard Eseries
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
      widgetPtr = gtk_builder_get_object(builderPtr, "customValues");
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else { // custom list
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
      widgetPtr = gtk_builder_get_object(builderPtr, "customValues");
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   updateLabelDesc();

   widgetPtr = gtk_builder_get_object(builderPtr, "Rp");
   if (maxRp==1) {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   widgetPtr = gtk_builder_get_object(builderPtr, "results");
   char unsignedStr[25];
   sprintf(unsignedStr, "%u", numBestRes);
   gtk_entry_set_text((GtkEntry*)widgetPtr, unsignedStr);

   updateLabelMem();
   //printf("guiUpdateIn end\n");
   return OK;
} // guiUpdateIn()

int guiInit(int numPar, char* paramPtr[]) { // create the main window
   gtk_init(&numPar, &paramPtr);

   /* Construct a GtkBuilder instance and load our UI description */
   builderPtr = gtk_builder_new();
   //printf("Try to load 'reSolve.glade'\n");
#if 0
   builderPtr=gtk_builder_new_from_file("reSolve.glade");
   printf("gtk_builder_new_from_file() returned:%p\n", builderPtr);
   if (builderPtr == NULL) {
      //g_printerr("Error loading file: %s\n", errorPtr->message);
      //g_clear_error(&errorPtr);
      printf("'reSolve.glade' not found, quit\n");
      return ERROR;
   }
#endif
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
   windowPtr = gtk_builder_get_object(builderPtr, "window");
   g_signal_connect(windowPtr, "destroy", G_CALLBACK(quit), NULL);

   //gtk_builder_connect_signals(builderPtr, NULL); // seems unnecessary
   widgetPtr = gtk_builder_get_object(builderPtr, "formulaList");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(formulaPre), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "formula");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(formula), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "desired");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(desiredFct), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "standardSeries");
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(standardSeries), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "EseriesList");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(EseriesList), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "decades");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(decadesFct), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "customValues");
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(customValues), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "customList");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(customList), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "Rp");
   g_signal_connect(widgetPtr, "toggled", G_CALLBACK(Rp), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "results");
   g_signal_connect(widgetPtr, "changed", G_CALLBACK(resultsFct), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "resolveButton");
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(resolveButton), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "aboutButton");
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(aboutButton), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "stopButton");
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(stopButton), NULL);
   widgetPtr = gtk_builder_get_object(builderPtr, "quitButton");
   g_signal_connect(widgetPtr, "clicked", G_CALLBACK(quitButton), NULL);

   //gtk_widget_show(GTK_WIDGET(windowPtr)); // seems unnecessary

   return OK;
} // guiInit()

int guiUpdateOut(char* txtPtr, int l) { // update widgets with results values
   //g_print("guiUpdateOut()\n");
   widgetPtr = gtk_builder_get_object(builderPtr, "output");
   if (txtPtr==NULL) { // clear
      //printf("GUI output clear\n");
      gtk_text_buffer_set_text((GtkTextBuffer*)widgetPtr, "", -1);
   } else { // append to prev
      //printf("GUI output append\n");
      GtkTextIter iter;
      gtk_text_buffer_get_end_iter((GtkTextBuffer*)widgetPtr, &iter);
      //printf("txtPtr:'%s'\n", txtPtr);
      gtk_text_buffer_insert((GtkTextBuffer*)widgetPtr, &iter, txtPtr, l);
      widgetPtr = gtk_builder_get_object(builderPtr, "textview");
      gtk_text_view_scroll_to_iter((GtkTextView*)widgetPtr,&iter,0,0,1,1);
   }
   int c=0;
   while(gtk_events_pending()) { // while is TRUE, force GUI update
      #ifdef _WIN32
         if (c>0 && winGuiLoop==0) break; // on Win skip next calls
      #endif
      gtk_main_iteration_do(0);
      c++;
   }
   return OK;
} // guiUpdateOut()

int runReSolve() { // memSize, memAlloc, doCalc, show output, freeMem
   int  ret;
   winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // 1 - checking arguments syntax/config value validity
   // 2 - read and set user request

   // clear output widget
   ret=guiUpdateOut(NULL, 0);

   // 3 - calculate the needed memory
   ret = memInpCalc(); // LIB: memory size calculation for input values
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ERROR; }
   ret = memResCalc(); // LIB: calculate the needed memory for results
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ERROR; }
   ret = updateLabelMem();
   if (ret!=OK) { printf("file:%s func:%s line:%d\n", __FILE__, __FUNCTION__, __LINE__); return ERROR; }

   // 4 - show config values
   ret=showConf(); // LIB: show config set

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
   stringPtr=siMem(allocatedB);
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
   winGuiLoop=0; // Win loop gtk_events_pending/gtk_main_iteration to update GUI
   if (mem==0) { // old memory hungry strategy
      gprintf(gui, "Printing best:%u solutions (top worst, botton best) in all configurations\n\n", numBestRes);
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
      gprintf(gui, "Printing best:%u solutions (top worst, botton best) in all configurations\n\n", numBestRes);
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
   winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   // 10 - freeing dynamic allocated memory ...
   ret = freeMem(); // LIB: free memory

   return OK;
} // runReSolve()

int main(int numPar, char* paramPtr[]) { // GUI entry point
   int ret;
   bool flag=false;

   gui=0; // mean gprintf() update GUI

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
   if (lists==2) {
      printf("WARN: GUI as now do not support lists=2. Ignoring ...\n");
      flag=true;
      lists=1;
      //exit(1);
   }
   ret = exprCheck(); // LIB: check expression syntax
   // 2 - read and set user request
   ret = baseInit(); // LIB: set internal variables

   // 3 - calculate the needed memory
   listNumberConf=listNumber;
   ret = memInpCalc(); // LIB: memory size calculation for input values
   ret = memResCalc(); // LIB: calculate the needed memory for results

   // 4 - show config values
   ret=showConf(); // LIB: show config set

   ret = backVal(); // backup 'expr' and 'userR'

   //putenv("LANG=C"); // as now use C locale to avoid trouble with .|,
   setlocale(LC_ALL,"C");
   gtk_disable_setlocale();
   printf("Starting GUI ...\n");
   ret = guiInit(numPar, paramPtr);
   if (ret!=OK) {
      printf("GUI cannot be initialized, quit\n");
      exit(1);
   }
   gui=1; // mean gprintf() update GUI
   guiUpdateOutPtr = &guiUpdateOut; // function pointer to guiUpdateOut()
   winGuiLoop=1; // Win loop gtk_events_pending/gtk_main_iteration to update GUI

   ret = guiUpdateIn();

   if (flag==true) {
      gprintf(gui, "WARN: GUI as now do not support lists=2. Ignoring ...\n");
   }

   //ret=runReSolve(); // steps 5 to 10 called by widget callbacks

   gtk_main();

   return EXIT_SUCCESS;
} // int main()
