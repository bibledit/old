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



bool moziller_copy_selection_to_clipboard (GtkWidget * widget)
{
  nsresult rv = NS_ERROR_FAILURE;
  nsIWebBrowser *webbrowser;
  gtk_moz_embed_get_nsIWebBrowser (GTK_MOZ_EMBED (widget), &webbrowser);
  NS_ENSURE_TRUE (webbrowser, rv);
  nsCOMPtr<nsIClipboardCommands> clipboard (do_GetInterface(webbrowser));
  clipboard->CopySelection();  
}
