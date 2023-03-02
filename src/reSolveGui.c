/* ReSolve V0.09.09h 2023/03/01 solve math expressions using discrete values*/
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

#include <stdlib.h>
#include <gtk/gtk.h>
#include "reSolveLib.h"    /* main source include */

GtkBuilder* builderPtr;
GObject* windowPtr;
GObject* widgetPtr;

int runReSolve();

int updateLabelDesc() {
   widgetPtr = gtk_builder_get_object (builderPtr, "description");
   gtk_label_set_text ((GtkLabel*)widgetPtr, baseRdesc);
   return 0;
}

int updateLabelMem() {
   char allocateStr[80]="";
   //printf("allocateStr:'%s'\n", allocateStr);
   //allocateStr[0]='\0';
   strcat(allocateStr, "Will allocate about ");
   //printf("allocatedMB:'%f'\n", allocatedMB);
   char doubleStr[25];
   sprintf(doubleStr, "%.0f", allocatedMB);
   strcat(allocateStr, doubleStr);
   strcat(allocateStr, " MB of total RAM");
   printf("allocateStr:'%s'\n", allocateStr);
   GObject* widget2Ptr = gtk_builder_get_object (builderPtr, "allocate");
   gtk_label_set_text ((GtkLabel*)widget2Ptr, allocateStr);
   return 0;
}

static void formulaPre(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("FormulaPre dropdown\n");
   gchar* comboValPtr = gtk_combo_box_text_get_active_text((GtkComboBoxText*)widgetPtr);
   g_print("value:'%s'\n", comboValPtr);
   widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
   if (!strcmp(comboValPtr, "Series"))
      gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit00.png");
   if (!strcmp(comboValPtr, "Parallel"))
      gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit01.png");
   if (!strcmp(comboValPtr, "Partitor"))
      gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit02.png");
   if (!strcmp(comboValPtr, "VoltReg"))
      gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
   g_free(comboValPtr);
}

static void formula(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Formula editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   g_print("text:'%s'\n", textPtr);
   int len = strlen(textPtr);
   expr = malloc(len+1);
   expr[0]='\0';
   strcat(expr, textPtr);
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
}

static void customList(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("CustomList editbox\n");
   const gchar* textPtr = gtk_entry_get_text((GtkEntry*)widgetPtr);
   g_print("text:'%s'\n", textPtr);
   //g_free(textPtr);
}

static void Rp(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Rp checkButton\n");
   guint checkVal = gtk_toggle_button_get_active((GtkToggleButton*)widgetPtr);
   g_print("value:'%u'\n", checkVal);
   if (checkVal==0) {
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "formulaList"));
      formulaPre(widgetPtr, NULL);
      g_print("set ID:circuits with pixbuf='circuit03.png'\n");
      //widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      //gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03.png");
      maxRp=1;
   }
   if (checkVal==1) {
      g_print("set ID:circuits with pixbuf='circuit03p.png'\n");
      widgetPtr=GTK_WIDGET(gtk_builder_get_object(builderPtr, "circuits"));
      gtk_image_set_from_file(GTK_IMAGE(widgetPtr), "circuit03p.png");
      maxRp=2;
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

int quit() {
   // free mem allocated by fillConfigVars()
   if (baseR) free (baseR);
   if (expr) free (expr);
   gtk_main_quit();
   return 0;
} // quit()

static void quitButton(GtkWidget* widgetPtr, gpointer dataPtr) {
   g_print("Quit button\n");
   quit();
}

int guiUpdateOut() { // update widgets with output/results values
   
   return 0;
} // guiUpdateOut()

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
   g_print("ESeries:'%u'\n", Eseries);
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

#if 0
   GError* errorPtr = NULL;
   if (gtk_builder_add_from_file(builderPtr, "reSolve.glade", &errorPtr) == 0) {
      g_printerr ("Error loading file: %s\n", errorPtr->message);
      g_clear_error (&errorPtr);
      return 1;
   }
#endif
   builderPtr=gtk_builder_new_from_file("reSolve.glade");
   if (builderPtr == NULL) {
      //g_printerr ("Error loading file: %s\n", errorPtr->message);
      //g_clear_error (&errorPtr);
      return 1;
   }

   /* Connect signal handlers to the constructed widgets */
   windowPtr = gtk_builder_get_object (builderPtr, "window");
   g_signal_connect (windowPtr, "destroy", G_CALLBACK(quit), NULL);

   //gtk_builder_connect_signals(builderPtr, NULL); // seems unnecessary

   widgetPtr = gtk_builder_get_object (builderPtr, "formulaList");
   g_signal_connect (widgetPtr, "changed", G_CALLBACK(formulaPre), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "formula");
   g_signal_connect (widgetPtr, "activate", G_CALLBACK(formula), NULL);
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
   g_signal_connect (widgetPtr, "activate", G_CALLBACK(customList), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "Rp");
   g_signal_connect (widgetPtr, "toggled", G_CALLBACK(Rp), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "results");
   g_signal_connect (widgetPtr, "activate", G_CALLBACK(resultsFct), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "resolveButton");
   g_signal_connect (widgetPtr, "clicked", G_CALLBACK(resolveButton), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "aboutButton");
   g_signal_connect (widgetPtr, "clicked", G_CALLBACK(aboutButton), NULL);
   widgetPtr = gtk_builder_get_object (builderPtr, "quitButton");
   g_signal_connect (widgetPtr, "clicked", G_CALLBACK(quitButton), NULL);

   //gtk_widget_show(GTK_WIDGET(windowPtr)); // seems unnecessary

   return 0;
} // guiInit()

int runReSolve() {
   int  ret;

   // 2 - read and set user request
   // 3 - calculate the needed memory
   ret = memCalc();

   // 6 - allocate the memory asking to the OS a malloc()
   // 7 - create the structure's vector inside the allocated memory
   ret = memAlloc(); // memory allocation

   ret=showConf(); // show config set

   // 8 - fill the input vectors with needed data
   // 9 - calculus of solutions
   // 10 - sorting of solutions
   ret = doCalc(); // fill inputs, calcs, sort solutions

   // 11 - print of results
   printf ("Show %u solutions with 2 resistors:\n", numBestRes);
   ret = showVal2 (numBestRes);
   printf ("\n");
   //for (u32 e=0; e<numV; e++) {
      //if (rValues[e].rp) free (rValues[e].rp);
   //}
   //if (results) free (results);
   ret = guiUpdateOut();

   // 12 - freeing dynamic allocated memory ...
   ret = freeMem(); // free memory
   if (ret != 0) {
      printf ("freeMem returned:%u, quit\n", ret);
      return -1;
   }

   return 0;
} // runReSolve()

int main(int argc, char *argv[]) {
   int  ret;

   // 1 - load configuration file and params
   ret = baseInit(); // basic initialization: load config from file+memSize
   if (ret != 0) {
      printf ("baseInit returned:%u, quit\n", ret);
      return -1;
   }
   // 2 - read and set user request
   // 3 - calculate the needed memory
   ret = memCalc();

   // 4 - checking config value validity

   // 5 - show config value in CLI or GUI
   showHead ();
   printf ("Found and loaded config file: 'reSolveConf.txt'\n");
   ret = guiInit(argc, argv);
   ret = guiUpdateIn();

   //ret=runReSolve(); // steps 6 to 12 called by widget callbacks

   gtk_main();

   return EXIT_SUCCESS;
} // int main()
