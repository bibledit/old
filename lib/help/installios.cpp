/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <help/installios.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_installios_url ()
{
  return "help/installios";
}


bool help_installios_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_installios (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Installation"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("Install Bibledit on iOS"));

  builder.clear();
  builder.sentence (gettext("This assumes a standard iPad or iPhone running iOS 7."));
  builder.sentence (gettext("The steps below have been tested on an iPad mini running iOS 7.1.2."));
  view.set_variable ("intro", builder.result());

  builder.clear();
  builder.sentence (gettext("Do an untethered jailbreak of iOS 7.1.x on the device with Pangu or another tool."));
  builder.sentence (gettext("A number of guides on the Internet provide assistance."));
  view.set_variable ("jailbreak", builder.result());

  view.set_variable ("cydia1", gettext("If the jailbreaking did not install Cydia:"));
  view.set_variable ("cydia2", gettext("Install Cydia iOS 7 on the device."));
  view.set_variable ("cydia3", gettext("Open Cydia for the first time and let it install the software and reboot the device."));

  view.set_variable ("url", gettext("Open Cydia, go to Sources, and add the following APT URL:"));

  view.set_variable ("packages1", gettext("In Cydia, search for and install the following package:"));

  builder.clear();
  builder.sentence (gettext("Open Safari on the device."));
  builder.sentence (gettext("Browse to the following address:"));
  view.set_variable ("safari", builder.result());
  view.set_variable ("ok", gettext("It should display a page saying that it works."));

  view.set_variable ("settings1", gettext("In the iOS Settings, there is a new entry:"));
  builder.clear();
  builder.sentence (gettext("Tap it, look for the MySQL Server, and disable it."));
  builder.sentence (gettext("This saves system resources."));
  view.set_variable ("settings2", builder.result());

  view.set_variable ("packages2", gettext("In Cydia, search for and install the following packages:"));

  view.set_variable ("terminal", gettext("Open the 'Terminal' app."));

  builder.clear();
  builder.sentence (gettext("Become root to escalate your privileges."));
  builder.sentence (gettext("In the terminal, type the command below, and press return:"));
  view.set_variable ("root", builder.result());
  builder.clear();
  builder.sentence (gettext("It asks for the password."));
  builder.sentence (gettext("In the terminal, type the password below, and press return:"));
  view.set_variable ("alpine", builder.result());
  view.set_variable ("invisible", gettext("The password is invisible in the terminal."));

  builder.clear();
  builder.sentence (gettext("Change to the home directory."));
  builder.sentence (gettext("In the terminal, type the command below, and press return:"));
  view.set_variable ("cd", builder.result());

  builder.clear();
  builder.sentence (gettext("Download the Bibledit software."));
  builder.sentence (gettext("In the terminal, type the command below, and press return:"));
  view.set_variable ("download1", builder.result());
  view.set_variable ("download2", gettext("The download takes some time."));

  builder.clear();
  builder.sentence (gettext("Unpack the downloaded software."));
  builder.sentence (gettext("In the terminal, type the command below, and press return:"));
  view.set_variable ("unzip", builder.result());
  view.set_variable ("inflate", gettext("It will inflate a lot of files."));

  builder.clear();
  builder.sentence (gettext("Run the Bibledit installation script."));
  builder.sentence (gettext("In the terminal, type the command below, and press return:"));
  view.set_variable ("script", builder.result());
  view.set_variable ("messages", gettext("It will output messages but no errors should occur."));

  builder.clear();
  builder.sentence (gettext("Open Safari on the device."));
  builder.sentence (gettext("Browse to the following address:"));
  view.set_variable ("open", builder.result());
  builder.clear();
  builder.sentence (gettext("It should display Bibledit."));
  builder.sentence (gettext("Follow the steps in Safari to complete the installation and open Bibledit."));
  view.set_variable ("configure", builder.result());

  builder.clear();
  builder.sentence (gettext("Note:"));
  builder.sentence (gettext("Work on a version of Bibledit on iOS that does not require jailbreaking the device is in progress."));
  view.set_variable ("future", builder.result());

  page += view.render ("help", "installios");

  page += Assets_Page::footer ();

  return page;
}
