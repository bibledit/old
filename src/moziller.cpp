/*
** Copyright (©) 2003-2008 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "moziller.h"
#include <glib.h>
#include <gtkmozembed.h>
#include <gtkmozembed_internal.h>

#include <nsIWebBrowser.h>
#include <nsIClipboardCommands.h>
#include <nsCOMPtr.h>
#include <nsIPrefService.h>
#define MOZILLA_INTERNAL_API
#include <nsIServiceManager.h>
#undef MOZILLA_INTERNAL_API
#include <nsIDOMWindow.h>
#include <nsIInterfaceRequestorUtils.h>
#include <nsMemory.h>



void moziller_preference_set (const char *preference_name, const char *new_value);
void moziller_preference_set_boolean (const char *preference_name, gboolean  new_boolean_value);
void moziller_preference_set_int (const char *preference_name, int new_int_value);
void moziller_save_preferences();

bool moziller_copy_selection_to_clipboard (GtkWidget * widget)
{
  nsresult rv = NS_ERROR_FAILURE;
  nsIWebBrowser *webbrowser;
  gtk_moz_embed_get_nsIWebBrowser (GTK_MOZ_EMBED (widget), &webbrowser);
  NS_ENSURE_TRUE (webbrowser, rv);
  nsCOMPtr<nsIClipboardCommands> clipboard (do_GetInterface(webbrowser));
  clipboard->CopySelection();  
}

  
void moziller_set_preferences ()
// Set Bibledit's preferred settings.
// Also visble through about:config.
{
  moziller_preference_set_boolean ("javascript.enabled", TRUE);
  moziller_preference_set_boolean ("plugin.default_plugin_disabled", FALSE);
  moziller_preference_set_boolean ("xpinstall.enabled", FALSE);
  moziller_preference_set_boolean ("mozilla.widget.raise-on-setfocus", FALSE);
  // this prevents popup dialogs and gives HTML error pages instead
  moziller_preference_set_boolean("browser.xul.error_pages.enabled", TRUE);
  // Save preferences.
  moziller_save_preferences();
}


void moziller_preference_set (const char *preference_name, const char *new_value)
{
  if (!preference_name) return;
  if (!new_value) return;
  nsCOMPtr<nsIPrefService> prefService = do_GetService (NS_PREFSERVICE_CONTRACTID);
  nsCOMPtr<nsIPrefBranch> pref;
  prefService->GetBranch ("", getter_AddRefs(pref));
  if (pref) {
    pref->SetCharPref (preference_name, new_value);            
  }
}


void moziller_preference_set_boolean (const char *preference_name, gboolean  new_boolean_value)
{
  if (!preference_name) return;
  nsCOMPtr<nsIPrefService> prefService = do_GetService (NS_PREFSERVICE_CONTRACTID);
  nsCOMPtr<nsIPrefBranch> pref;
  prefService->GetBranch ("", getter_AddRefs(pref));
  if (pref) {
    pref->SetBoolPref (preference_name, new_boolean_value ? PR_TRUE : PR_FALSE);
  }
}


void moziller_preference_set_int (const char *preference_name, int new_int_value)
{
  if (!preference_name) return;
  nsCOMPtr<nsIPrefService> prefService = do_GetService (NS_PREFSERVICE_CONTRACTID);
  nsCOMPtr<nsIPrefBranch> pref;
  prefService->GetBranch ("", getter_AddRefs(pref));
  if (pref) {
    pref->SetIntPref (preference_name, new_int_value);
  }
}


void moziller_save_preferences()
{
  nsCOMPtr<nsIPrefService> prefService = do_GetService (NS_PREFSERVICE_CONTRACTID);
  if (prefService) return;
  prefService->SavePrefFile (nsnull);
}
