/* ReSolve V0.09.09h 2023/03/12 solve math expressions using discrete values*/
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

#define _GNU_SOURCE     /* vasprintf() */
#include <stdlib.h>
#include <locale.h>
#include <gtk/gtk.h>
#include "reSolveLib.h"    /* main source include */

GtkBuilder* builderPtr;
GObject* windowPtr;
GObject* widgetPtr;

char exprConf[LineLen]; // config expression backup
double* baseRconf;      // config custom list backup
u16 listNumberConf;     // config custom list quantity backup
char exprUser[LineLen]; // user expression backup
double* baseRuser;      // user custom list backup
u16 listNumberUser;     // user custom list quantity backup

int runReSolve(); // memSize, memAlloc, doCalc, show output, freeMem

int updateLabelDesc() {
   widgetPtr = gtk_builder_get_object (builderPtr, "description");
   gtk_label_set_text ((GtkLabel*)widgetPtr, baseRdesc);
   return 0;
}

int updateLabelMem() {
   char* stringPtr;
   stringPtr=siMem(allocatedB);
   //printf ("allocat:'%s'\n", stringPtr);
   char* strPtr;
   asprintf (&strPtr, "Will allocate about %s of total RAM", stringPtr);
   free(stringPtr);
   GObject* widget2Ptr = gtk_builder_get_object (builderPtr, "allocate");
   gtk_label_set_text ((GtkLabel*)widget2Ptr, strPtr);
   free(strPtr);
   return 0;
}

static void formulaPre(GtkWidget* widgetPtr, gpointer dataPtr) {
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
   if (!strcmp(comboValPtr, "custom formula")) {
      if (maxRp==1)
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
      else
         gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
   }
   widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "formula");
   if (!strcmp(comboValPtr, "custom formula")) {
      if (exprUser[0]!='\0') { // user has already type a formula
         gtk_entry_set_text ((GtkEntry*)widgetPtr, exprUser);
         strcpy(expr, exprUser);
      } else { // use config file
         gtk_entry_set_text ((GtkEntry*)widgetPtr, exprConf);
         strcpy(expr, exprConf);
      }
   } else {
      gtk_entry_set_text ((GtkEntry*)widgetPtr, "");
   }
   //printf("expr:'%s'\n", expr);
   //printf("exprConf:'%s'\n", exprConf);
   //printf("exprUser:'%s'\n", exprUser);
   g_free(comboValPtr);
}

static void formula(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Formula editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   g_print("text:'%s'\n", textPtr);
   if (textPtr[0]!='\0') strcpy(exprUser, textPtr);
   widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "formulaList");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   if (!strcmp(comboValPtr, "custom formula")) {
      int len = strlen(textPtr);
      if (len>=LineLen) {
         printf ("Custom formula text len:%u, max supported len:%u\n", len, LineLen);
         return;
      }
      strcpy(expr, textPtr);
   }
   printf("expr:'%s'\n", expr);
   //printf("exprConf:'%s'\n", exprConf);
   //printf("exprUser:'%s'\n", exprUser);
   //g_free(textPtr);
}

static void desiredFct(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Desired spinButton\n");
   gdouble spinVal = gtk_spin_button_get_value((GtkSpinButton*)widgetPtr);
   g_print("value:'%g'\n", spinVal);
   desired=spinVal;
}

static void standardSeries(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("standardSeries radioButton\n");
   guint radioVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", radioVal);
   widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "EseriesList");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   //char valStr[5];
   //sprintf(valStr, "%u", radioVal);
   g_print("EseriesList combo value:'%s'\n", comboValPtr);
   if ( !strcmp(comboValPtr,"E1"  ) ) Eseries=1;
   if ( !strcmp(comboValPtr,"E3"  ) ) Eseries=3;
   if ( !strcmp(comboValPtr,"E6"  ) ) Eseries=6;
   if ( !strcmp(comboValPtr,"E12" ) ) Eseries=12;
   if ( !strcmp(comboValPtr,"E24" ) ) Eseries=24;
   if ( !strcmp(comboValPtr,"E48" ) ) Eseries=48;
   if ( !strcmp(comboValPtr,"E96" ) ) Eseries=96;
   if ( !strcmp(comboValPtr,"E192") ) Eseries=192;
   printf("Eserie:E%u\n", Eseries);
   g_free(comboValPtr);
   updateRdesc();
   //printf("baseRdesc:'%s'\n", baseRdesc);
   updateLabelDesc();
   memCalc();
   updateLabelMem();
   widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "customList");
   gtk_entry_set_text ((GtkEntry*)widgetPtr, "");
}

static void EseriesList(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("EseriesList dropdown\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   //char valStr[5];
   //sprintf(valStr, "%u", radioVal);
   g_print("value:'%s'\n", comboValPtr);
   if ( !strcmp(comboValPtr,"E1"  ) ) Eseries=1;
   if ( !strcmp(comboValPtr,"E3"  ) ) Eseries=3;
   if ( !strcmp(comboValPtr,"E6"  ) ) Eseries=6;
   if ( !strcmp(comboValPtr,"E12" ) ) Eseries=12;
   if ( !strcmp(comboValPtr,"E24" ) ) Eseries=24;
   if ( !strcmp(comboValPtr,"E48" ) ) Eseries=48;
   if ( !strcmp(comboValPtr,"E96" ) ) Eseries=96;
   if ( !strcmp(comboValPtr,"E192") ) Eseries=192;
   printf("Eserie:E%u\n", Eseries);
   g_free(comboValPtr);
   updateRdesc();
   updateLabelDesc();
   memCalc();
   updateLabelMem();
   //widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "standardSeries");
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   //widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "customValues");
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
}

static void decadesFct(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Decades spinButton\n");
   gdouble spinVal = gtk_spin_button_get_value((GtkSpinButton*)widgetPtr);
   g_print("value:'%g'\n", spinVal);
   decades=spinVal;
   printf("decades:%u\n", decades);
   memCalc();
   updateLabelMem();
}

static void customValues(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("customValues radioButton\n");
   guint radioVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", radioVal);
   //printf("listNumber:%u\n", listNumber);
   //printf("listNumberConf:%u\n", listNumberConf);
   Eseries=0;
   char* doubleList;
   if (baseRuser!=NULL) { // user has already type custom list values
      listNumber=listNumberUser;
      uint16_t len=0;
      char doubleStr[25];
      for (uint16_t r=0; r<listNumberUser; r++) {
         sprintf(doubleStr, "%0.f", baseRuser[r]);
         len+=strlen(doubleStr);
      }
      len+=listNumberUser;
      doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<listNumberUser; r++) {
         sprintf(doubleStr, "%0.f", baseRuser[r]);
         strcat(doubleList, doubleStr);
         if (r<listNumberUser-1) strcat(doubleList, ",");
      }
      //printf("custom values:'%s'\n", doubleList);
      printf("populate customValue list with user values...\n");
   } else { // use config file
      listNumber=listNumberConf;
      uint16_t len=0;
      char doubleStr[25];
      for (uint16_t r=0; r<listNumberConf; r++) {
         sprintf(doubleStr, "%0.f", baseRconf[r]);
         len+=strlen(doubleStr);
      }
      len+=listNumberConf;
      doubleList = calloc(len+1,1);
      for (uint16_t r=0; r<listNumberConf; r++) {
         sprintf(doubleStr, "%0.f", baseRconf[r]);
         strcat(doubleList, doubleStr);
         if (r<listNumberConf-1) strcat(doubleList, ",");
      }
      //printf("custom values:'%s'\n", doubleList);
      printf("populate customValue list with config values...\n");
   }
   widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "customList");
   gtk_entry_set_text ((GtkEntry*)widgetPtr, doubleList);
   free(doubleList);
   updateRdesc();
   //printf("baseRdesc:'%s'\n", baseRdesc);
   updateLabelDesc();
   memCalc();
   updateLabelMem();
} // customValues()

static void customList(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("CustomList editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   g_print("text:'%s'\n", textPtr);
   // make a copy of textPtr[]
   int l=strlen(textPtr);
   gchar* txtPtr=malloc(l+1);
   strcpy(txtPtr, textPtr);
   int c;
   //for (c=0; c<listNumberConf; c++) {
   //   printf("baseR[%d]:%f\n", c, baseR[c]);
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
   if (baseR) free(baseR);
   baseR = calloc(c, sizeof(double));

   int n, len;
   prvPtr=txtPtr;
   
   for (n=0; n<c; n++) {
      chrPtr=strchr(prvPtr, ',');
      //printf("prvPtr:%p *prvPtr:%c chrPtr:%p *chrPtr:%c\n", prvPtr, *prvPtr, chrPtr, *chrPtr);
      if (chrPtr==NULL)
         len=strlen(prvPtr);
      else
         len=chrPtr-prvPtr;
      //printf("len:%d\n", len);
      char doubleStr[len+1];
      strncpy(doubleStr, prvPtr, len);
      doubleStr[len]='\0';
      //printf("doubleStr:'%s'\n", doubleStr);
      double num=strtod(doubleStr, NULL);
      baseR[n]=num;
      //printf("baseR[%d]:%f\n", n, baseR[n]);
      prvPtr=chrPtr+1;
   }
   //for (c=0; c<listNumberConf; c++) {
   //   printf("baseR[%d]:%f\n", c, baseR[c]);
   //}
   //printf("listNumber:%u\n", listNumber);
   listNumber=c;
   //printf("listNumber:%u\n", listNumber);
   //printf("listNumberConf:%u\n", listNumberConf);
   //printf("listNumberUser:%u\n", listNumberUser);
   baseRuser=calloc(c, sizeof(double));
   listNumberUser=c;
   for (c=0; c<listNumberUser; c++) {
      baseRuser[c]=baseR[c];
   }
   //printf("listNumber:%u\n", listNumber);
   //printf("listNumberConf:%u\n", listNumberConf);
   //printf("listNumberUser:%u\n", listNumberUser);
   //for (c=0; c<listNumberUser; c++) {
   //   printf("baseRuser[%d]:%f\n", c, baseRuser[c]);
   //}

   //widgetPtr = (GtkWidget*)gtk_builder_get_object (builderPtr, "customValues");
   //gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   g_free(txtPtr);
} // customList()

static void Rp(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Rp checkButton\n");
   guint checkVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", checkVal);
   if (checkVal==0) {
      maxRp=1;
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaPre(widgetPtr, NULL);
      //g_print("set ID:circuits with pixbuf='circuit03.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
   }
   if (checkVal==1) {
      maxRp=2;
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaPre(widgetPtr, NULL);
      //g_print("set ID:circuits with pixbuf='circuit03p.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
   }
   memCalc();
   updateLabelMem();
}

static void resultsFct(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Results editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   g_print("text:'%s'\n", textPtr);
   numBestRes=atoi(textPtr);
   printf("numBestRes:%u\n", numBestRes);
   //g_free(textPtr);
}

static void resolveButton(GtkWidget* widgetPtr, gpointer dataPtr) {
   int ret;
   g_print("Resolve button\n");
   ret=runReSolve();
   if (ret != 0) {
      printf ("runReSolve returned:%u, quit\n", ret);
      return;
   }
}

static void aboutButton(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("About button\n");
}

int quit() { // called also on Window destroy
   // free mem allocated by fillConfigVars()
   if (baseR) free (baseR);
   if (baseRconf) free (baseRconf);
   if (baseRuser) free (baseRuser);
   gtk_main_quit();
   return 0;
} // quit()

static void quitButton(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Quit button\n");
   quit();
}

int guiUpdateIn() { // update widgets with input/config values
   widgetPtr = gtk_builder_get_object (builderPtr, "formulaList");
   //gtk_combo_box_set_active((GtkComboBox*)widgetPtr, 3);
   gtk_combo_box_set_id_column ((GtkComboBox*)widgetPtr, 0);
   gboolean done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "VoltReg");
   if (done == FALSE) {
      printf("WARN: gtk_combo_box_set_active_id() ret FALSE\n");
   }

   //widgetPtr = gtk_builder_get_object (builderPtr, "formula");
   //gtk_entry_set_text ((GtkEntry*)widgetPtr, expr);

   widgetPtr = gtk_builder_get_object (builderPtr, "desired");
   gtk_spin_button_set_value((GtkSpinButton*)widgetPtr, desired);

   widgetPtr = gtk_builder_get_object (builderPtr, "standardSeries");
   //g_print("ESeries:'%u'\n", Eseries);
   if (Eseries>0) {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
      widgetPtr = gtk_builder_get_object (builderPtr, "customValues");
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
      widgetPtr = gtk_builder_get_object (builderPtr, "customValues");
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   widgetPtr = gtk_builder_get_object (builderPtr, "EseriesList");
   gtk_combo_box_set_id_column ((GtkComboBox*)widgetPtr, 0);
   switch (Eseries) {
   case (0):
      break;
   case (1):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E1");
      break;
   case (3):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E3");
      break;
   case (6):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E6");
      break;
   case (12):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E12");
      break;
   case (24):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E24");
      break;
   case (48):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E48");
      break;
   case (96):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E96");
      break;
   case (192):
      done = gtk_combo_box_set_active_id ((GtkComboBox*)widgetPtr, "E192");
      break;
   default:
      printf ("Unsupported Series:%u. Supported are 0, 1, 3, 6, 12, 24, 48, 96 and 192\n", Eseries);
      return 1;
   }

   widgetPtr = gtk_builder_get_object (builderPtr, "decades");
   gtk_spin_button_set_value((GtkSpinButton*)widgetPtr, decades);

   widgetPtr = gtk_builder_get_object (builderPtr, "customList");
   //printf("listNumber:%u\n", listNumber);
   uint16_t len=0;
   char doubleStr[25];
   for (uint16_t r=0; r<listNumber; r++) {
      sprintf(doubleStr, "%0.f", baseR[r]);
      len+=strlen(doubleStr);
   }
   len+=listNumber;
   char* doubleList = calloc(len+1,1);
   for (uint16_t r=0; r<listNumber; r++) {
      sprintf(doubleStr, "%0.f", baseR[r]);
      strcat(doubleList, doubleStr);
      if (r<listNumber-1) strcat(doubleList, ",");
   }
   //printf("custom values:'%s'\n", doubleList);
   gtk_entry_set_text ((GtkEntry*)widgetPtr, doubleList);
   free(doubleList);

   updateLabelDesc();

   widgetPtr = gtk_builder_get_object (builderPtr, "Rp");
   if (maxRp==1) {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, FALSE);
   } else {
      gtk_toggle_button_set_active((GtkToggleButton*)widgetPtr, TRUE);
   }

   widgetPtr = gtk_builder_get_object (builderPtr, "results");
   char unsignedStr[25];
   sprintf(unsignedStr, "%u", numBestRes);
   gtk_entry_set_text ((GtkEntry*)widgetPtr, unsignedStr);

   updateLabelMem();

   return 0;
} // guiUpdateIn()

int guiInit(int argc, char *argv[]) {
   gtk_init(&argc, &argv);

   /* Construct a GtkBuilder instance and load our UI description */
   builderPtr = gtk_builder_new();
   //printf ("Try to load 'reSolve.glade'\n");
#if 0
   builderPtr=gtk_builder_new_from_file("reSolve.glade");
   printf ("gtk_builder_new_from_file() returned:%p\n", builderPtr);
   if (builderPtr == NULL) {
      //g_printerr ("Error loading file: %s\n", errorPtr->message);
      //g_clear_error (&errorPtr);
      printf ("'reSolve.glade' not found, quit\n");
      return 1;
   }
#endif
   GError* errorPtr = NULL;
   gboolean bit;
   bit = gtk_builder_add_from_file(builderPtr, "reSolve.glade", &errorPtr);
   if (bit == 0) {
      g_printerr ("Error loading file: %s\n", errorPtr->message);
      g_clear_error (&errorPtr);
      return 1;
   }
   //printf ("'reSolve.glade' loaded\n");

   /* Connect signal handlers to the constructed widgets */
   windowPtr = gtk_builder_get_object (builderPtr, "window");
   g_signal_connect (windowPtr, "destroy", G_CALLBACK(quit), NULL);

   //gtk_builder_connect_signals(builderPtr, NULL); // seems unnecessary
   widgetPtr = gtk_builder_get_object (builderPtr, "formulaList");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(formulaPre), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "formula");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(formula), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "desired");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(desiredFct), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "standardSeries");
   g_signal_connect (widgetPtr, "toggled", G_CALLBACK(standardSeries), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "EseriesList");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(EseriesList), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "decades");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(decadesFct), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "customValues");
   g_signal_connect (widgetPtr, "toggled", G_CALLBACK(customValues), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "customList");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(customList), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "Rp");
   g_signal_connect (widgetPtr, "toggled", G_CALLBACK(Rp), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "results");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(resultsFct), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "resolveButton");
   g_signal_connect (widgetPtr, "clicked", G_CALLBACK(resolveButton), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "aboutButton");
   g_signal_connect (widgetPtr, "clicked", G_CALLBACK(aboutButton), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "quitButton");
   g_signal_connect (widgetPtr, "clicked", G_CALLBACK(quitButton), NULL);

   //gtk_widget_show(GTK_WIDGET(windowPtr)); // seems unnecessary

   return 0;
} // guiInit()

int backVal() { // backup 'expr' and 'baseR'
   strcpy(exprConf, expr);
   int size=sizeof(double)*listNumberConf;
   //printf("size:%u\n", size);
   //printf("listNumber:%u\n", listNumber);
   //printf("listNumberConf:%u\n", listNumberConf);
   //printf("baseR[r]:");
   //for (uint16_t r=0; r<listNumberConf; r++) {
   //   printf("%0.f,", baseR[r]);
   //}
   //printf("\n");

   baseRconf=malloc(size);
   //memcpy(baseRconf, baseR, size);
   for (uint16_t r=0; r<listNumberConf; r++) {
      baseRconf[r]=baseR[r];
   }

   //printf("baseRconf[r]:");
   //for (uint16_t r=0; r<listNumberConf; r++) {
   //   printf("%0.f,", baseRconf[r]);
   //}
   //printf("\n");
   return 0;
} // backVal()

int guiUpdateOut(char* txtPtr, int l) { // update widgets with results values
   //g_print("guiUpdateOut()\n");
   widgetPtr = gtk_builder_get_object (builderPtr, "output");
   if (txtPtr==NULL) { // clear
      //printf("GUI output clear\n");
      gtk_text_buffer_set_text((GtkTextBuffer*)widgetPtr, "", -1);
   } else { // append to prev
      //printf("GUI output append\n");
      GtkTextIter iter;
      gtk_text_buffer_get_end_iter((GtkTextBuffer*)widgetPtr, &iter);
      //printf("txtPtr:'%s'\n", txtPtr);
      gtk_text_buffer_insert((GtkTextBuffer*)widgetPtr, &iter, txtPtr, l);
      widgetPtr = gtk_builder_get_object (builderPtr, "textview");
      gtk_text_view_scroll_to_iter ((GtkTextView*)widgetPtr,&iter,0,0,1,1);
   }
   while (gtk_events_pending ()) // force GUI update
      gtk_main_iteration ();
   return 0;
} // guiUpdateOut()

int runReSolve() { // memSize, memAlloc, doCalc, show output, freeMem
   int  ret;

   // clear output widget
   ret=guiUpdateOut(NULL, 0);

   // 2 - read and set user request
   // 3 - calculate the needed memory
   ret = memCalc(); // LIB: calculate the needed memory

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
   //gprintf (gui, "\n");

   // 12 - freeing dynamic allocated memory ...
   ret = freeMem(); // LIB: free memory

   return 0;
} // runReSolve()

int main(int argc, char *argv[]) {
   int ret;

   gui=1; // mean gprintf() update GUI
   guiUpdateOutPtr = &guiUpdateOut; // function pointer to guiUpdateOut()

   // 1 - load configuration file and params
   ret = baseInit(); // LIB: basic initialization: load config from file+memSize
   if (ret != 0) {
      printf ("baseInit returned:%u, quit\n", ret);
      return -1;
   }
   // 2 - read and set user request
   // 3 - calculate the needed memory
   listNumberConf=listNumber;
   ret = memCalc(); // LIB: calculate the needed memory

   // 4 - checking config value validity

   // 5 - show config values
   showHead (); // LIB: show header
   printf ("Found and loaded config file: 'reSolveConf.txt'\n");

   ret = backVal(); // backup 'expr' and 'baseR'
   //putenv("LANG=C"); // as now use C locale to avoid trouble with .|,
   setlocale(LC_ALL,"C");
   gtk_disable_setlocale();
   printf ("Starting GUI ...\n");
   ret = guiInit(argc, argv);
   if (ret!=0) {
      printf ("GUI cannot be initialized, quit\n");
      exit (1);
   }
   ret = guiUpdateIn();

   //ret=runReSolve(); // steps 6 to 12 called by widget callbacks

   gtk_main();

   return EXIT_SUCCESS;
} // int main()
