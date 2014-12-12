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


#include <help/installation.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_installation_url ()
{
  return "help/installation";
}


bool help_installation_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_installation (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Installation"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("Installation"));

  view.set_variable ("installation", gettext("Standard installation"));

  view.set_variable ("upgrading1", gettext("Upgrading"));
  builder.clear();
  builder.sentence (gettext("A certain release of Bibledit can upgrade the most recent previous release, but not older releases."));
  builder.sentence (gettext("A new release can be installed over the previous release."));
  builder.sentence (gettext("There should be no loss of information."));
  builder.sentence (gettext("Create a backup of the web space before upgrading, just to be sure."));
  view.set_variable ("upgrading2", builder.result());

  view.set_variable ("client1", gettext("Client"));
  builder.clear();
  builder.sentence (gettext("Bibledit normally runs in Server mode."));
  builder.sentence (gettext("It can also be set to Client mode."));
  builder.sentence (gettext("A Client stores the data offline."));
  builder.sentence (gettext("You can translate the Bible on a Client."));
  builder.sentence (gettext("The user needs to synchronize the data on the client with the data on the server regularly."));
  builder.sentence (gettext("A Client is useful when the Internet connection to the Bibledit server is not fast or not stable."));
  view.set_variable ("client2", builder.result());
  builder.clear();
  builder.sentence (gettext("Install the Client as you would install Bibledit."));
  builder.sentence (gettext("During setup you can switch it to client mode."));
  builder.sentence (gettext("In client mode, connect to the server once, and provide your username and password as you normally log in on the server."));
  builder.sentence (gettext("Then synchronize the data."));
  builder.sentence (gettext("The first time all Bible data and Notes will be downloaded from the server to the client."));
  builder.sentence (gettext("That may take a while."));
  builder.sentence (gettext("After that, synchronizing will send and receive the changes only."));
  view.set_variable ("client3", builder.result());
  builder.clear();
  builder.sentence (gettext("The client mode is useful in a situation where the Internect connection is not optimal."));
  builder.sentence (gettext("All data is stored locally on your computing device."));
  builder.sentence (gettext("During translation, the Internet connection is not used."));
  builder.sentence (gettext("It is used during synchronization."));
  view.set_variable ("client4", builder.result());

  view.set_variable ("email1", gettext("Email"));
  builder.clear();
  builder.sentence (gettext("Bibledit works with email."));
  builder.sentence (gettext("After completing the installation of Bibledit, sending and receiving emails does not yet work."));
  builder.sentence (gettext("This needs to be set up."));
  builder.sentence (gettext("Setting up the email means to pick an email address and setting details for the mail servers in Bibledit."));
  builder.sentence (gettext("This can be done from within the application."));
  view.set_variable ("email2", builder.result());

  view.set_variable ("teams1", gettext("Multiple translation teams"));
  view.set_variable ("teams2", gettext("There are two ways to have multiple translation teams on one server."));
  builder.clear();
  builder.sentence (gettext("Install multiple parallel copies of Bibledit on one server."));
  builder.sentence (gettext("Unpack the installation zip archive in sub folders with different names."));
  builder.sentence (gettext("Each copy has its own web address."));
  builder.sentence (gettext("Everything is completely separated between the parallel copies."));
  builder.sentence (gettext("This includes the data and the databases."));
  builder.sentence (gettext("Every copy has its own independent databases."));
  view.set_variable ("teams3", builder.result());
  builder.clear();
  builder.sentence (gettext("Use one installation of Bibledit."));
  builder.sentence (gettext("Give various users read or write access to various Bibles."));
  builder.sentence (gettext("Users with write access together constitute a translation team that can edit the Bible."));
  view.set_variable ("teams4", builder.result());

  view.set_variable ("dev1", gettext("Install development version"));
  view.set_variable ("dev2", gettext("Read the instructions for a standard Bibledit installation to find out about server setup and prerequisites."));

  view.set_variable ("git1", gettext("The Bibledit code is kept in a git repository."));
  view.set_variable ("git2", gettext("The repository is accessible from the Savannah Project Page."));
  builder.clear();
  builder.sentence (gettext("To view the code on the web, click Source Code - Browse Sources Repository."));
  builder.sentence (gettext("To use the repository in another way, click on Source Code - Use Git."));
  builder.sentence (gettext("Information will be provided there."));
  builder.sentence (gettext("The code can be checked out from the repository."));
  builder.sentence (gettext("This gives the most recent version of Bibledit."));
  builder.sentence (gettext("The code is supported."));
  view.set_variable ("git3", builder.result());
  view.set_variable ("git4", gettext("To get the most recent code, run the following in a terminal:"));
  builder.clear();
  builder.sentence (gettext("This clones the code repository to your computer."));
  builder.sentence (gettext("It will create directory"));
  view.set_variable ("git5", builder.result());
  builder.clear();
  builder.sentence (gettext("Change into that directory."));
  builder.sentence (gettext("Then create an installation package:"));
  view.set_variable ("git6", builder.result());
  builder.clear();
  builder.sentence (gettext("It will display where the package will be stored."));
  builder.sentence (gettext("This package can be used to do a standard installation of Bibledit."));
  view.set_variable ("git7", builder.result());

  view.set_variable ("open1", gettext("Open installation"));
  builder.clear();
  builder.sentence (gettext("You can configure Bibledit to be open."));
  builder.sentence (gettext("It means that there is no need to login."));
  builder.sentence (gettext("The website appears to be logged in as user 'admin' to any visitor."));
  builder.sentence (gettext("This is useful for demo installations, or to provide a free service to the the community."));
  builder.sentence (gettext("A disadvantage is that the crawlers of the search engines keep clicking links, and will disturb data and settings."));
  builder.sentence (gettext("Open the following file:"));
  view.set_variable ("open2", builder.result());
  view.set_variable ("open4", gettext("Update one line as follows:"));

  view.set_variable ("hosting1", gettext("Hosting providers"));
  builder.clear();
  builder.sentence (gettext("Bibledit is not supported on a shared Linux hosting account."));
  builder.sentence (gettext("On several occassions Bibledit has been used on such an account though."));
  builder.sentence (gettext("Bibledit is a heavy application."));
  builder.sentence (gettext("It uses a lot of resources compared to what is available on a shared hosting account."));
  builder.sentence (gettext("The hosting providers have responded in various ways to having Bibledit on their shared platform:"));
  view.set_variable ("hosting2", builder.result());
  view.set_variable ("hosting3", gettext("Closing the website saying it causes inconvenience on the shared platform."));
  view.set_variable ("hosting4", gettext("Silently modifying the scheduled tasks to run less often, crippling Bibledit operations."));
  view.set_variable ("hosting5", gettext("Suggesting it to be moved to a virtual private server."));
  view.set_variable ("hosting6", gettext("Slowing down the network connection."));
  view.set_variable ("hosting7", gettext("Slowing down the shell server."));
  view.set_variable ("hosting8", gettext("It has also been observed that the Bibledit databases regularly got corrupted due to insufficient file locking on the shared platform."));
  builder.clear();
  builder.sentence (gettext("To make good use of Bibledit, a VPS (virtual private server) is recommended."));
  builder.sentence (gettext("Good providers are:"));
  view.set_variable ("hosting9", builder.result());

  page += view.render ("help", "installation");

  page += Assets_Page::footer ();

  return page;
}
