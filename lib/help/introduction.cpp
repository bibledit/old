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


#include <help/introduction.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_introduction_url ()
{
  return "help/introduction";
}


bool help_introduction_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_introduction (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Introduction"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("Introduction"));
  builder.clear ();
  builder.sentence (gettext("Bibledit is a web-based application for translating the Bible"));
  builder.sentence (gettext("It is part of the Bibledit suite."));
  view.set_variable ("intro", builder.result());
  view.set_variable ("signup", gettext("People can sign up for an account and log in."));
  view.set_variable ("role", gettext("Each user has a certain role within the application."));
  builder.clear ();
  builder.sentence (gettext("The first role is Member.")); // Todo fails
  builder.sentence (gettext("A member is somebody who has signed up to the site, and has confirmed this signup through his email."));
  view.set_variable ("member", builder.result());
  builder.clear ();
  builder.sentence (gettext("The second role is Consultant."));
  builder.sentence (gettext("Consultants are people who can be consulted because of their knowledge of the original language such as Hebrew or Greek, or because of their knowledge of their target language in case they are language scholars, or for any other knowledge they have."));
  view.set_variable ("consultant", builder.result());
  builder.clear ();
  builder.sentence (gettext("The next role is Translator."));
  builder.sentence (gettext("They are the ones who have all the rights of the Consultant, but in addition can change the Bible text."));
  view.set_variable ("translator", builder.result());
  builder.clear ();
  builder.sentence (gettext("Then there's the Manager."));
  builder.sentence (gettext("The manager has all the privileges of the previous levels, but in addition can add books to the Bible, delete books, add chapters, copy Bibles, and so on."));
  builder.sentence (gettext("The manager can assign privileges to the translators, consultants, and so on."));
  view.set_variable ("manager", builder.result());
  builder.clear ();
  builder.sentence (gettext("Finally, there is the Administrator."));
  builder.sentence (gettext("The administrator has all rights to do anything, and will do the technical setup and maintenance of the site."));
  view.set_variable ("administrator", builder.result());
  builder.clear ();
  builder.sentence (gettext("Everybody who has an account can enable the personal notifications."));
  builder.sentence (gettext("Those who have switched the relevant notifications on can receive email from Bibledit."));
  builder.sentence (gettext("They receive email when new consultation notes get created, or when consultation notes get changed, or when consultation notes get assigned to them."));
  builder.sentence (gettext("They can also get a daily email detailing the changes that have been made in the Bible since the previous email."));
  view.set_variable ("notifications", builder.result());
  view.set_variable ("email", gettext("Everybody who has an account can create consultation notes by email, and respond to these by email."));
  view.set_variable ("export", gettext("Members can view and download all exported Bibles and lists with recent changes in the Bibles."));
  view.set_variable ("consultants1", gettext("Consultants can create consultation notes online, view these, set parameters for these, assign the notes to people, and delete notes."));
  view.set_variable ("translators1", gettext("Translators can do the following things:"));
  view.set_variable ("translators2", gettext("Edit the Bible text."));
  view.set_variable ("translators3", gettext("Switch the stylesheet."));
  view.set_variable ("translators4", gettext("View and suppress results of the automatic checks."));
  view.set_variable ("managers1", gettext("Managers can do several things:"));
  view.set_variable ("managers2", gettext("Edit the Bible in raw USFM format."));
  builder.clear ();
  builder.sentence (gettext("Send and receive the Bible text."));
  builder.sentence (gettext("This means that they can synchronize the Bible text with a remote git repository."));
  builder.sentence (gettext("In this way they can collaborate with people who use Bibledit-Gtk."));
  view.set_variable ("managers3", builder.result());
  view.set_variable ("managers4", gettext("View and clear the logbook."));
  view.set_variable ("managers5", gettext("Process the changes, so that lists are created, and email is sent out."));
  view.set_variable ("managers6", gettext("View and edit people who have an account, and create new accounts."));
  view.set_variable ("managers7", gettext("Create and delete Bibles, books, chapters, and import USFM data."));
  view.set_variable ("managers8", gettext("Initiate the daily export of selected Bibles to OpenDocument, USFM, html, Online Bible."));
  view.set_variable ("managers9", gettext("Create and edit versification systems."));
  view.set_variable ("managers10", gettext("Create and edit stylesheets."));
  view.set_variable ("managers11", gettext("Import consultation notes from Bibledit-Gtk and remove duplicates."));
  view.set_variable ("managers13", gettext("Deal with automatic hyphenation when exporting to OpenDocument."));
  view.set_variable ("managers14", gettext("Make settings for the automatic check and select Bibles to run them on."));
  view.set_variable ("admins1", gettext("Administrator can do several things:"));
  view.set_variable ("admins2", gettext("Set up Bibledit's email account."));
  view.set_variable ("admins3", gettext("Set up collaboration through setting up a connection with a remote git repository."));
  view.set_variable ("admins4", gettext("Maintain the database."));
  view.set_variable ("admins5", gettext("Set the site's timezone."));
  view.set_variable ("admins6", gettext("Set the site's language."));

  page += view.render ("help", "introduction");

  page += Assets_Page::footer ();

  return page;
}
