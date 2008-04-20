/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * 2002-2005 Stephane Pontier <shadow_walker@users.sourceforge.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "common.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkhpaned.h>
#include <gtk/gtkhseparator.h>
#include <gtk/gtkimagemenuitem.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtkradiomenuitem.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkstock.h>
#include <gtk/gtkvbox.h>

#include "callbacks.h"
#include "interface.h"
#include "gui.h"

/* Callbacks for main window */

/* Close the main window */
static gboolean
on_MainWindow_delete_event (GtkWidget       *widget,
			    GdkEvent        *event,
			    gpointer         user_data)
{
	exit_gmameui ();
	return TRUE;
}

GtkWidget *
create_MainWindow (void)
{

  GtkWidget *MainWindow;
  GtkWidget *vbox1;
  GtkWidget *toolbar1;
  GtkWidget *hpanedLeft;
  GtkWidget *scrolledwindowFilters;
  GtkWidget *hpanedRight;
  GtkWidget *scrolledwindowGames;
  GtkWidget *tri_status_bar;
  GtkWidget *statusbar1;
  GtkWidget *statusbar2;
  GtkWidget *statusbar3;
  GtkWidget *combo_progress_bar;
  GtkWidget *status_progress_bar;
  GtkWidget *progress_progress_bar;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;
  PangoFontDescription *fontdesc;
  gint font_size;
	
	GtkActionGroup *action_group;
	GError *error = NULL;
	
	GtkWidget *menubar;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  MainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_object_set_data (G_OBJECT (MainWindow), "MainWindow", MainWindow);
  gtk_window_set_title (GTK_WINDOW (MainWindow), _("GMAMEUI Arcade Machine Emulator"));
  gtk_window_set_default_size (GTK_WINDOW (MainWindow), 640, 400);

  vbox1 = gtk_vbox_new (FALSE, 1);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (MainWindow), vbox1);
	
	main_gui.manager = gtk_ui_manager_new ();
	
	action_group = gtk_action_group_new ("GmameuiWindowAlwaysSensitiveActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_always_sensitive_menu_entries,
				      G_N_ELEMENTS (gmameui_always_sensitive_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	/* TODO window->priv->always_sensitive_action_group = action_group;*/
	
	action_group = gtk_action_group_new ("GmameuiWindowROMActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_rom_menu_entries,
				      G_N_ELEMENTS (gmameui_rom_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_rom_action_group = action_group;

	action_group = gtk_action_group_new ("GmameuiWindowViewActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_view_expand_menu_entries,
				      G_N_ELEMENTS (gmameui_view_expand_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_view_action_group = action_group;
	
	action_group = gtk_action_group_new ("GmameuiWindowToggleActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_toggle_actions (action_group,
					     gmameui_view_toggle_menu_entries,
					     G_N_ELEMENTS (gmameui_view_toggle_menu_entries),
					     MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);

	action_group = gtk_action_group_new ("GmameuiWindowRadioActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_radio_actions (action_group,
					    gmameui_view_radio_menu_entries,
					    G_N_ELEMENTS (gmameui_view_radio_menu_entries),
					    0,  /* TODO */
					    G_CALLBACK (on_view_type_changed),
					    MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);

	/* Column popup */
	action_group = gtk_action_group_new ("GmameuiColumnPopupActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_column_entries,
				      G_N_ELEMENTS (gmameui_column_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	
	/* Now load the UI definition */
	gtk_ui_manager_add_ui_from_file (main_gui.manager, GMAMEUI_UI_DIR "gmameui-ui.xml", &error);
	if (error != NULL)
	{
		GMAMEUI_DEBUG ("Error loading gmameui-ui.xml: %s", error->message);
		g_error_free (error);
	}
	

	menubar = gtk_ui_manager_get_widget (main_gui.manager, "/MenuBar");
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    menubar, 
			    FALSE, 
			    FALSE, 
			    0);
	
  toolbar1 = gtk_toolbar_new ();
  gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar1), GTK_ORIENTATION_HORIZONTAL);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH);
  gtk_widget_show (toolbar1);
  main_gui.toolbar = GTK_TOOLBAR (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);

  hpanedLeft = gtk_hpaned_new ();
  gtk_widget_show (hpanedLeft);
  main_gui.hpanedLeft = GTK_PANED (hpanedLeft);
  gtk_box_pack_start (GTK_BOX (vbox1), hpanedLeft, TRUE, TRUE, 0);
  gtk_paned_set_position (GTK_PANED (hpanedLeft), 150);
  scrolledwindowFilters = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindowFilters);
  main_gui.scrolled_window_filters = scrolledwindowFilters;
  gtk_paned_pack1 (GTK_PANED (hpanedLeft), scrolledwindowFilters, FALSE, FALSE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowFilters), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  hpanedRight = gtk_hpaned_new ();
  gtk_widget_show (hpanedRight);
  main_gui.hpanedRight = GTK_PANED (hpanedRight);
  gtk_paned_pack2 (GTK_PANED (hpanedLeft), hpanedRight, TRUE, FALSE);
  gtk_paned_set_position (GTK_PANED (hpanedRight), 300);

  scrolledwindowGames = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindowGames);
  main_gui.scrolled_window_games = scrolledwindowGames;
  gtk_paned_pack1 (GTK_PANED (hpanedRight), scrolledwindowGames, TRUE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowGames), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	/* Create the screenshot and history sidebar */
	GMAMEUISidebar *sidebar = gmameui_sidebar_new ();
	gtk_paned_pack2 (GTK_PANED (hpanedRight), sidebar, FALSE, FALSE);
	main_gui.screenshot_hist_frame = GMAMEUI_SIDEBAR (sidebar);
	
	
  tri_status_bar = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (tri_status_bar);
  main_gui.tri_status_bar = tri_status_bar;
  gtk_box_pack_start (GTK_BOX (vbox1), tri_status_bar, FALSE, FALSE, 0);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_show (statusbar1);
  main_gui.statusbar1 = GTK_STATUSBAR (statusbar1);
  gtk_box_pack_start (GTK_BOX (tri_status_bar), statusbar1, TRUE, TRUE, 0);

  fontdesc = pango_font_description_copy (GTK_WIDGET (tri_status_bar)->style->font_desc);
  font_size = pango_font_description_get_size (fontdesc);

  statusbar2 = gtk_statusbar_new ();
  gtk_widget_show (statusbar2);
  main_gui.statusbar2 = GTK_STATUSBAR (statusbar2);
  gtk_box_pack_start (GTK_BOX (tri_status_bar), statusbar2, FALSE, FALSE, 0);
  gtk_widget_set_size_request (statusbar2, PANGO_PIXELS (font_size) * 10, -1);

  statusbar3 = gtk_statusbar_new ();
  gtk_widget_show (statusbar3);
  main_gui.statusbar3 = GTK_STATUSBAR (statusbar3);
  gtk_box_pack_end (GTK_BOX (tri_status_bar), statusbar3, FALSE, FALSE, 0);
  gtk_widget_set_size_request (statusbar3, PANGO_PIXELS (font_size) * 10, -1);

  combo_progress_bar = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (combo_progress_bar);
  main_gui.combo_progress_bar = combo_progress_bar;
  gtk_box_pack_start (GTK_BOX (vbox1), combo_progress_bar, FALSE, FALSE, 0);

  status_progress_bar = gtk_statusbar_new ();
  gtk_widget_show (status_progress_bar);
  main_gui.status_progress_bar = GTK_STATUSBAR (status_progress_bar);
  gtk_box_pack_start (GTK_BOX (combo_progress_bar), status_progress_bar, TRUE, TRUE, 0);

  progress_progress_bar = gtk_progress_bar_new ();
  gtk_widget_show (progress_progress_bar);
  main_gui.progress_progress_bar = GTK_PROGRESS_BAR (progress_progress_bar);
  gtk_box_pack_end (GTK_BOX (combo_progress_bar), progress_progress_bar, TRUE, TRUE, 0);

  g_signal_connect (G_OBJECT (MainWindow), "delete_event",
                      G_CALLBACK (on_MainWindow_delete_event),
                      NULL);

  g_object_set_data (G_OBJECT (MainWindow), "tooltips", tooltips);

  gtk_window_add_accel_group (GTK_WINDOW (MainWindow), accel_group);

  return MainWindow;
}

