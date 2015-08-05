/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <unittests/databases.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/date.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>
#include <database/search.h>
#include <database/bibleactions.h>
#include <database/check.h>
#include <database/localization.h>
#include <database/confirm.h>
#include <database/ipc.h>
#include <database/jobs.h>
#include <database/kjv.h>
#include <database/morphhb.h>
#include <database/sblgnt.h>
#include <database/offlineresources.h>
#include <database/sprint.h>
#include <database/mail.h>
#include <database/navigation.h>
#include <database/usfmresources.h>
#include <database/mappings.h>
#include <database/noteactions.h>
#include <database/versifications.h>
#include <database/modifications.h>
#include <database/notes.h>
#include <database/volatile.h>
#include <database/state.h>
#include <database/imageresources.h>
#include <bible/logic.h>
#include <notes/logic.h>
#include <sync/logic.h>
#include <styles/logic.h>
#include <resource/external.h>
#include <config.h>


#ifdef HAVE_UNITTESTS


void test_database_config_general ()
{
  // Tests for Database_Config_General.
  evaluate (__LINE__, __func__, "Bibledit Cloud", Database_Config_General::getSiteMailName ());
  
  string ref = "unittest";
  Database_Config_General::setSiteMailName (ref);
  evaluate (__LINE__, __func__, ref, Database_Config_General::getSiteMailName ());
  
  evaluate (__LINE__, __func__, "", Database_Config_General::getMailStorageProtocol ());
}


void test_database_config_bible ()
{
  // Random basic tests.
  {
    evaluate (__LINE__, __func__, false, Database_Config_Bible::getViewableByAllUsers ("testbible"));
    
    bool ref = true;
    Database_Config_Bible::setViewableByAllUsers ("testbible", ref);
    evaluate (__LINE__, __func__, ref, Database_Config_Bible::getViewableByAllUsers ("testbible"));
  }
  // Versification / Mapping
  {
    string versification = Database_Config_Bible::getVersificationSystem ("phpunit");
    evaluate (__LINE__, __func__, "English", versification);
    string mapping = Database_Config_Bible::getVerseMapping ("phpunit");
    evaluate (__LINE__, __func__, "English", mapping);
    versification = Database_Config_Bible::getVersificationSystem ("x");
    evaluate (__LINE__, __func__, "English", versification);
    mapping = Database_Config_Bible::getVerseMapping ("x");
    evaluate (__LINE__, __func__, "English", mapping);
    Database_Config_Bible::setVersificationSystem ("phpunit", "VersificatioN");
    versification = Database_Config_Bible::getVersificationSystem ("phpunit");
    evaluate (__LINE__, __func__, "VersificatioN", versification);
    Database_Config_Bible::setVerseMapping ("phpunit", "VersificatioN");
    mapping = Database_Config_Bible::getVerseMapping ("phpunit");
    evaluate (__LINE__, __func__, "VersificatioN", mapping);
  }
}


void test_database_config_user ()
{
  // Tests for Database_Config_User.
  {
    // Setup.
    refresh_sandbox (true);
    Webserver_Request request = Webserver_Request ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    database_users.addNewUser ("username", "password", 5, "");
    request.session_logic ()->attemptLogin ("username", "password", true);
    Database_Search database_search = Database_Search ();
    database_search.create ();
    
    // Testing setList, getList, plus add/removeUpdatedSetting.
    evaluate (__LINE__, __func__, {}, request.database_config_user ()->getUpdatedSettings ());
    
    vector <int> standard1 = {123, 456};
    request.database_config_user ()->setUpdatedSettings (standard1);
    evaluate (__LINE__, __func__, standard1, request.database_config_user ()->getUpdatedSettings ());
    
    request.database_config_user ()->addUpdatedSetting (789);
    standard1.push_back (789);
    evaluate (__LINE__, __func__, standard1, request.database_config_user ()->getUpdatedSettings ());
    
    request.database_config_user ()->removeUpdatedSetting (456);
    vector <int> standard2 = {123, 789};
    evaluate (__LINE__, __func__, standard2, request.database_config_user ()->getUpdatedSettings ());
    
    // Testing the Sprint month and its trim () function.
    // It should get today's month.
    int month = filter_date_numerical_month (filter_date_seconds_since_epoch ());
    evaluate (__LINE__, __func__, month, request.database_config_user ()->getSprintMonth ());
    // Set the sprint month to another month value: It should get this value back from the database.
    int newmonth = 123;
    request.database_config_user ()->setSprintMonth (newmonth);
    evaluate (__LINE__, __func__, newmonth, request.database_config_user ()->getSprintMonth ());
    // Trim: The sprint month should not be reset.
    request.database_config_user ()->trim ();
    evaluate (__LINE__, __func__, newmonth, request.database_config_user ()->getSprintMonth ());
    // Set the modification time of the sprint month record to more than two days ago:
    // Trimming resets the sprint month to the current month.
    string filename = filter_url_create_path (testing_directory, "databases", "config", "user", "username", "sprint-month");
    struct stat foo;
    struct utimbuf new_times;
    stat (filename.c_str(), &foo);
    new_times.actime = filter_date_seconds_since_epoch () - (2 * 24 * 3600) - 10;
    new_times.modtime = filter_date_seconds_since_epoch () - (2 * 24 * 3600) - 10;
    utime (filename.c_str(), &new_times);
    request.database_config_user ()->trim ();
    evaluate (__LINE__, __func__, month, request.database_config_user ()->getSprintMonth ());
    
    // Test boolean setting.
    evaluate (__LINE__, __func__, false, request.database_config_user ()->getSubscribeToConsultationNotesEditedByMe ());
    request.database_config_user ()->setSubscribeToConsultationNotesEditedByMe (true);
    evaluate (__LINE__, __func__, true, request.database_config_user ()->getSubscribeToConsultationNotesEditedByMe ());
    
    // Test integer setting.
    evaluate (__LINE__, __func__, 0, request.database_config_user ()->getConsultationNotesPassageSelector ());
    request.database_config_user ()->setConsultationNotesPassageSelector (11);
    evaluate (__LINE__, __func__, 11, request.database_config_user ()->getConsultationNotesPassageSelector ());
    
    // Test string setting.
    evaluate (__LINE__, __func__, "", request.database_config_user ()->getConsultationNotesAssignmentSelector ());
    request.database_config_user ()->setConsultationNotesAssignmentSelector ("test");
    evaluate (__LINE__, __func__, "test", request.database_config_user ()->getConsultationNotesAssignmentSelector ());
    
    evaluate (__LINE__, __func__, filter_date_numerical_year (filter_date_seconds_since_epoch ()), request.database_config_user ()->getSprintYear ());
    
    // Test getting a Bible that does not exist: It creates one.
    evaluate (__LINE__, __func__, "Bibledit Sample Bible", request.database_config_user ()->getBible ());
  }
}


void test_database_logs ()
{
  // Tests for Database_Logs.
  {
    refresh_sandbox (true);
    // Log some items.
    Database_Logs::log ("description1", 2);
    Database_Logs::log ("description2", 3);
    Database_Logs::log ("description3", 4);
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    database_logs.checkup ();
    // Move the items from the filesystem into the SQLite database.
    database_logs.rotate ();
    // Get the items from the SQLite database.
    string lastfilename;
    vector <string> result = database_logs.get (0, lastfilename);
    evaluate (__LINE__, __func__, 3, (int)result.size ());
    refresh_sandbox (false);
  }
  {
    // Check the database: It should recreate the database and then create one entry in the Journal.
    // This entry is proof that it recreated the database.
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.checkup ();
    string lastfilename = "1111111111";
    vector <string> result = database_logs.get (0, lastfilename);
    evaluate (__LINE__, __func__, 1, (int)result.size ());
    refresh_sandbox (false);
  }
  {
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    int now = filter_date_seconds_since_epoch ();
    int min1days = now - 86400 - 10;
    int min2days = min1days - 86400;
    int min3days = min2days - 86400;
    int min4days = min3days - 86400;
    int min5days = min4days - 86400;
    int min6days = min5days - 86400;
    string lastfilename;
    vector <string> result;

    // Log entry for 6 days ago.
    Database_Logs::log ("Six days ago");
    database_logs.rotate ();
    database_logs.update (now, min6days);
    lastfilename = "0";
    result = database_logs.get (6, lastfilename);
    evaluate (__LINE__, __func__, 1, (int)result.size ());
    lastfilename = "0";
    result = database_logs.get (5, lastfilename);
    evaluate (__LINE__, __func__, 0, (int)result.size ());
    // Rotate that entry away.
    database_logs.rotate ();
    lastfilename = "0";
    result = database_logs.get (6, lastfilename);
    evaluate (__LINE__, __func__, 0, (int)result.size ());

    // Log entry for 2 days, 1 day ago, and now.
    Database_Logs::log ("Two days ago");
    database_logs.rotate ();
    database_logs.update (now, min2days);
    Database_Logs::log ("One day ago");
    database_logs.rotate ();
    database_logs.update (now, min1days);
    Database_Logs::log ("Now");
    lastfilename = "0";
    result = database_logs.get (2, lastfilename);
    evaluate (__LINE__, __func__, 1, (int)result.size ());
    // Gets it from the filesystem, not the database, because this last entry was not yet rotated.
    lastfilename = "0";
    result = database_logs.get (0, lastfilename);
    evaluate (__LINE__, __func__, 1, (int)result.size ());
    // The "lastsecond" variable, test it.
    int lastsecond = convert_to_int (lastfilename.substr (0, 10));
    if ((lastsecond < now ) || (lastsecond > now + 1)) evaluate (__LINE__, __func__, now, lastsecond);
    refresh_sandbox (false);
  }
  {
    // Test huge journal entry.
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    string huge (10000, 'x');
    Database_Logs::log (huge);
    database_logs.rotate ();
    string s = "0";
    vector <string> result = database_logs.get (0, s);
    if (result.size () == 1) {
      string s = result[0];
      size_t pos = s.find ("This entry was too large and has been truncated: 10000 bytes");
      if (pos == string::npos) {
        error_message (__LINE__, __func__, "Should be truncated", s);
      }
    } else {
      evaluate (__LINE__, __func__, 1, (int)result.size ());
    }
    refresh_sandbox (false);
  }
  {
    // Test the getNext function of the Journal.
    refresh_sandbox (true);
    Database_Logs::log ("description");
    Database_Logs database_logs = Database_Logs ();
    int second = filter_date_seconds_since_epoch ();
    string filename = convert_to_string (second) + "00000000";
    // First time: getNext gets the logged entry.
    string s;
    s = database_logs.getNext (filename);
    if (s == "") evaluate (__LINE__, __func__, "...description", s);
    // Since variable "filename" is updated and set to the last filename,
    // next time function getNext gets nothing.
    s = database_logs.getNext (filename);
    evaluate (__LINE__, __func__, "", s);
    refresh_sandbox (false);
  }
}


void test_database_users ()
{
  // Tests for Database_Users.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    database_users.upgrade ();

    string username = "unit test";
    string password = "pazz";
    int level = 10;
    string email = "email@site.nl";

    database_users.optimize ();
    database_users.trim ();
    
    database_users.addNewUser (username, password, level, email);

    evaluate (__LINE__, __func__, true, database_users.matchUsernamePassword (username, password));
    evaluate (__LINE__, __func__, false, database_users.matchUsernamePassword (username, "wrong password"));

    evaluate (__LINE__, __func__, true, database_users.matchEmailPassword (email, password));
    evaluate (__LINE__, __func__, false, database_users.matchEmailPassword (email, "wrong password"));

    string ref = "INSERT INTO users (username, password, level, email) VALUES ('unit test', '014877e71841e82d44ce524d66dcc732', 10, 'email@site.nl');";
    string act = database_users.addNewUserQuery (username, password, level, email);
    evaluate (__LINE__, __func__, ref, act);

    evaluate (__LINE__, __func__, username, database_users.getEmailToUser (email));
    evaluate (__LINE__, __func__, "", database_users.getEmailToUser ("wrong email"));

    evaluate (__LINE__, __func__, email, database_users.getUserToEmail (username));
    evaluate (__LINE__, __func__, "", database_users.getUserToEmail ("wrong username"));

    evaluate (__LINE__, __func__, true, database_users.usernameExists (username));
    evaluate (__LINE__, __func__, false, database_users.usernameExists ("invalid username"));

    evaluate (__LINE__, __func__, true, database_users.emailExists (email));
    evaluate (__LINE__, __func__, false, database_users.emailExists ("invalid email"));

    evaluate (__LINE__, __func__, level, database_users.getUserLevel (username));
    evaluate (__LINE__, __func__, Filter_Roles::guest (), database_users.getUserLevel ("invalid username"));

    level = 7;
    database_users.updateUserLevel (username, level);
    evaluate (__LINE__, __func__, level, database_users.getUserLevel (username));

    database_users.removeUser (username);
    evaluate (__LINE__, __func__, false, database_users.usernameExists (username));
    
    evaluate (__LINE__, __func__, "UPDATE users SET email = 'email@site.nl' WHERE username = 'unit test';", database_users.updateEmailQuery (username, email));
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username1 = "unit test1";
    string username2 = "unit test2";
    string password = "pazz";
    int level = Filter_Roles::admin();
    string email = "email@site";

    database_users.addNewUser (username1, password, level, email);
    vector <string> admins = database_users.getAdministrators ();
    evaluate (__LINE__, __func__, 1, (int)admins.size());
    if (!admins.empty()) evaluate (__LINE__, __func__, username1, admins [0]);
    
    database_users.addNewUser (username2, password, level, email);
    admins = database_users.getAdministrators ();
    evaluate (__LINE__, __func__, 2, (int)admins.size());
    
    email = "new@email.address";
    database_users.updateUserEmail (username1, email);
    evaluate (__LINE__, __func__, email, database_users.getUserToEmail (username1));
    
    vector <string> users = database_users.getUsers ();
    evaluate (__LINE__, __func__, 2, (int)users.size());
    
    evaluate (__LINE__, __func__, md5 (password), database_users.getmd5 (username1));
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username = "unit test";
    string password = "pazz";
    string email = "email@site";
    string address = "192.168.1.0";
    string agent = "Browser's user agent";
    string fingerprint = "ԴԵԶԸ";
    database_users.setTokens (username, address, agent, fingerprint, true);
    evaluate (__LINE__, __func__, username, database_users.getUsername (address, agent, fingerprint));
    database_users.removeTokens (username);
    evaluate (__LINE__, __func__, "", database_users.getUsername (address, agent, fingerprint));

    evaluate (__LINE__, __func__, 0, database_users.getTimestamp (username));
    database_users.pingTimestamp (username);
    int timestamp = database_users.getTimestamp (username);
    int second = filter_date_seconds_since_epoch ();
    if ((timestamp != second) && (timestamp != second + 1)) evaluate (__LINE__, __func__, second, timestamp);
  }
  // Test touch-enabled settings.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username = "unittest";
    string password = "pass";
    string email = "mail@site.nl";
    string address = "192.168.1.2";
    string agent = "Browser's user agent";
    string fingerprint = "abcdef";
    bool touch = true;
    database_users.setTokens (username, address, agent, fingerprint, touch);
    evaluate (__LINE__, __func__, true, database_users.getTouchEnabled (address, agent, fingerprint));
    
    database_users.removeTokens (username);
    evaluate (__LINE__, __func__, false, database_users.getTouchEnabled (address, agent, fingerprint));
    
    database_users.setTokens (username, address, agent, fingerprint, touch);
    evaluate (__LINE__, __func__, false, database_users.getTouchEnabled (address, agent, fingerprint + "x"));
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username1 = "unit test 1";
    string username2 = "unit test 2";
    string bible1 = "bible one";
    string bible2 = "bible two";
    
    vector <string> teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 0, (int)teams.size());
    
    database_users.grantAccess2Bible (username1, bible1);
    teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 1, (int)teams.size());

    database_users.grantAccess2Bible (username1, bible1);
    teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 1, (int)teams.size());
    if (!teams.empty ()) evaluate (__LINE__, __func__, bible1, teams [0]);
    
    database_users.grantAccess2Bible (username1, bible2);
    teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 2, (int)teams.size());
    
    vector <string> bibles = database_users.getBibles4User (username1);
    evaluate (__LINE__, __func__, 2, (int)bibles.size());
    if (!bibles.empty ()) evaluate (__LINE__, __func__, bible1, bibles [0]);
    
    database_users.revokeAccess2Bible (username1, bible1);
    bibles = database_users.getBibles4User (username1);
    evaluate (__LINE__, __func__, 1, (int)bibles.size());

    database_users.grantAccess2Bible (username2, bible2);
    vector <string> users = database_users.getUsers4Bible (bible2);
    evaluate (__LINE__, __func__, 2, (int)users.size());
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username1 = "unit test 1";
    string username2 = "unit test 2";
    string bible1 = "bible one";
    string bible2 = "bible two";
    
    // No teams: Any user has access to any Bible.
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible1));
    
    // One team: Any user has access to any Bible.
    database_users.grantAccess2Bible (username1, bible1);
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible1));
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username2, bible2));

    // Two teams: User access control applies.
    database_users.grantAccess2Bible (username1, bible1);
    database_users.grantAccess2Bible (username1, bible2);
    database_users.grantAccess2Bible (username2, bible1);
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible1));
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible2));
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username2, bible1));
    evaluate (__LINE__, __func__, false, database_users.hasAccess2Bible (username2, bible2));
    
    // Admin has access to any Bible.
    database_users.addNewUser (username2, "", Filter_Roles::admin (), "");
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username2, bible2));
    
    // Read only access for known user.
    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Bible (username1, bible1));
    database_users.setReadOnlyAccess2Book (username1, bible1, 2, true);
    evaluate (__LINE__, __func__, true, database_users.hasReadOnlyAccess2Book (username1, bible1, 2));
    
    // No read-only access for unknown user.
    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Book ("unknown", bible1, 2));
  }
  // Test upgrading read-only settings.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    database_users.upgrade ();

    database_users.grantAccess2Bible ("user1", "bible1");
    database_users.upgrade ();
    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Book ("user1", "bible1", 1));

    string sql = "UPDATE teams SET readonly = 1 WHERE username = 'user1' AND bible = 'bible1';";
    database_users.execute (sql);
    database_users.upgrade ();
    evaluate (__LINE__, __func__, true, database_users.hasReadOnlyAccess2Book ("user1", "bible1", 1));
    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Book ("user1", "bible1", 67));

    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Book ("user1", "bible1", 67));
    database_users.setReadOnlyAccess2Book ("user1", "bible1", 67, true);
    evaluate (__LINE__, __func__, true, database_users.hasReadOnlyAccess2Book ("user1", "bible1", 67));
  }
}


void test_database_styles ()
{
  // Tests for Database_Styles.
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();

    vector <string> sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { styles_logic_standard_sheet () }, sheets);

    database_styles.createSheet ("phpunit");
    sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { styles_logic_standard_sheet (), "phpunit" }, sheets);

    database_styles.deleteSheet ("phpunit");
    sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { styles_logic_standard_sheet () }, sheets);

    database_styles.deleteSheet (styles_logic_standard_sheet ());
    sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { styles_logic_standard_sheet () }, sheets);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");

    vector <string> markers;
    
    markers = database_styles.getMarkers (styles_logic_standard_sheet ());
    evaluate (__LINE__, __func__, 179, (int)markers.size ());

    markers = database_styles.getMarkers ("phpunit");
    evaluate (__LINE__, __func__, 179, (int)markers.size ());

    string marker = "p";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "not found");
    marker = "add";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "not found");

    map <string, string> markers_names = database_styles.getMarkersAndNames ("phpunit");
    evaluate (__LINE__, __func__, 179, (int)markers_names.size());
    evaluate (__LINE__, __func__, "Blank Line", markers_names ["b"]);
    evaluate (__LINE__, __func__, "Normal, First Line Indent", markers_names ["p"]);
    evaluate (__LINE__, __func__, "* Translational Addition", markers_names ["add"]);

    database_styles.deleteMarker ("phpunit", "p");
    markers = database_styles.getMarkers ("phpunit");
    marker = "p";
    if (find (markers.begin (), markers.end (), marker) != markers.end ()) evaluate (__LINE__, __func__, marker, "should not be there");
    marker = "add";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "not found");

    markers_names = database_styles.getMarkersAndNames ("phpunit");
    evaluate (__LINE__, __func__, "", markers_names ["p"]);
    evaluate (__LINE__, __func__, "* Translational Addition", markers_names ["add"]);    
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    Database_Styles_Item data = database_styles.getMarkerData ("phpunit", "add");
    evaluate (__LINE__, __func__, "add", data.marker);
    evaluate (__LINE__, __func__, "st", data.category);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    database_styles.updateName ("phpunit", "add", "Addition");
    Database_Styles_Item data = database_styles.getMarkerData ("phpunit", "add");
    evaluate (__LINE__, __func__, "Addition", data.name);
    database_styles.updateInfo ("phpunit", "p", "Paragraph");
    data = database_styles.getMarkerData ("phpunit", "p");
    evaluate (__LINE__, __func__, "Paragraph", data.info);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    // A user does not have write access to the stylesheet.
    bool write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, false, write);

    // Grant write access, and test it for this user, and for another user.
    database_styles.grantWriteAccess ("user", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, true, write);
    write = database_styles.hasWriteAccess ("user2", "phpunit");
    evaluate (__LINE__, __func__, false, write);
    write = database_styles.hasWriteAccess ("user", "phpunit2");
    evaluate (__LINE__, __func__, false, write);

    // Revoke write access for a user, test it in various ways.
    database_styles.revokeWriteAccess ("user2", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, true, write);
    database_styles.revokeWriteAccess ("user", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, false, write);
    
    // Revoking write access for all users.
    database_styles.grantWriteAccess ("user1", "phpunit");
    database_styles.grantWriteAccess ("user2", "phpunit");
    database_styles.revokeWriteAccess ("", "phpunit");
    write = database_styles.hasWriteAccess ("user1", "phpunit");
    evaluate (__LINE__, __func__, false, write);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    // Get markers.
    vector <string> markers = database_styles.getMarkers ("phpunit");
    string marker = "zhq";
    if (find (markers.begin (), markers.end (), marker) != markers.end ()) evaluate (__LINE__, __func__, marker, "should not be there");
  
    // Add marker.
    database_styles.addMarker ("phpunit", marker);
    markers = database_styles.getMarkers ("phpunit");
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "should be there");
  }
}


// Tests for the Database_Books object.
void test_database_books ()
{
  refresh_sandbox (true);
  evaluate (__LINE__, __func__, 69, (int)Database_Books::getIDs ().size());
  evaluate (__LINE__, __func__, 2, Database_Books::getIdFromEnglish ("Exodus"));
  evaluate (__LINE__, __func__, 0, Database_Books::getIdFromEnglish ("exodus"));
  evaluate (__LINE__, __func__, "Leviticus", Database_Books::getEnglishFromId (3));
  evaluate (__LINE__, __func__, "NUM", Database_Books::getUsfmFromId (4));
  evaluate (__LINE__, __func__, "Deu", Database_Books::getBibleworksFromId (5));
  evaluate (__LINE__, __func__, 22, Database_Books::getIdFromUsfm ("SNG"));
  evaluate (__LINE__, __func__, 13, Database_Books::getIdFromOsis ("1Chr"));
  evaluate (__LINE__, __func__, 12, Database_Books::getIdFromBibleworks ("2Ki"));
  evaluate (__LINE__, __func__, 12, Database_Books::getIdLikeText ("2Ki"));
  evaluate (__LINE__, __func__, 14, Database_Books::getIdLikeText ("2Chron"));
  evaluate (__LINE__, __func__, 1, Database_Books::getIdLikeText ("Genes"));
  evaluate (__LINE__, __func__, 12, Database_Books::getIdFromOnlinebible ("2Ki"));
  evaluate (__LINE__, __func__, "De", Database_Books::getOnlinebibleFromId (5));
  evaluate (__LINE__, __func__, "5", Database_Books::getSequenceFromId (5));
  evaluate (__LINE__, __func__, "nt", Database_Books::getType (40));
  evaluate (__LINE__, __func__, "ot", Database_Books::getType (39));
  evaluate (__LINE__, __func__, "", Database_Books::getType (0));
}


// Tests for Database_Search
void test_database_search_setup ()
{
  string standardUSFM1 =  "\\c 1"
                          "\\p"
                          "\\v 1 Text of the first verse."
                          "\\v 2 \\add Text of the second \\add*verse."
                          "\\s Heading"
                          "\\p"
                          "\\v 3 Text of the 3rd third verse."
                          "\\v 4 Text of the 4th \\add fourth\\add* verse."
                          "\\v 5 Text of the 5th fifth verse is this: Verse five ✆."
                          "\\v 6 Text of the 6th sixth verse ✆."
                          "\\v 7 Text of the seventh verse with some UTF-8 characters: ✆ ❼ ሯ."
                          "\\v 8 Verse eight ✆ ❼ ሯ."
                          "\\v 9 Verse nine nine."
                          "\\v 10 خدا بود و کلمه کلمه خدا بود. در ابتدا کلمه بود و کلمه نزد.";
  string standardUSFM2 =  "\\c 1"
                          "\\p"
                          "\\v 3 And he said.";
  Database_Search database_search = Database_Search ();
  database_search.create ();
  Database_Bibles database_bibles = Database_Bibles ();
  database_bibles.createBible ("phpunit");
  database_bibles.storeChapter ("phpunit", 2, 3, standardUSFM1);
  database_bibles.createBible ("phpunit2");
  database_bibles.storeChapter ("phpunit2", 4, 5, standardUSFM2);
}


void test_database_search ()
{
  // Test create, optimize, health.
  {
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();

    database_search.optimize ();
    bool healthy = database_search.checkup ();
    evaluate (__LINE__, __func__, false, healthy);

    string path = filter_url_create_root_path ("databases", "search.sqlite");
    filter_url_file_put_contents (path, "damaged database");
    healthy = database_search.checkup ();
    evaluate (__LINE__, __func__, true, healthy);
    healthy = database_search.checkup ();
    evaluate (__LINE__, __func__, false, healthy);
    refresh_sandbox (false);
  }
  // Test updating search fields.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
  }
  // Test searching and getting Bible passage
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    vector <int> hits = database_search.searchText ("sixth", {"phpunit"});
    evaluate (__LINE__, __func__, 1, (int)hits.size());
    if (!hits.empty ()) {
      int id = hits [0];
      Passage passage = database_search.getBiblePassage (id);
      evaluate (__LINE__, __func__, "phpunit", passage.bible);
      evaluate (__LINE__, __func__, 2, passage.book);
      evaluate (__LINE__, __func__, 3, passage.chapter);
      evaluate (__LINE__, __func__, "6", passage.verse);
    }
  }
  // Test getting Bible verse text.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    string text = database_search.getBibleVerseText ("phpunit", 2, 3, 5);
    evaluate (__LINE__, __func__, "Text of the 5th fifth verse is this: Verse five ✆.", filter_string_trim (text));
  }
  // Test search Bible
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    database_search.updateSearchFields ("phpunit2", 4, 5);
    vector <int> ids = database_search.searchBibleText ("phpunit", "sixth");
    evaluate (__LINE__, __func__, 1, (int)ids.size ());
    ids = database_search.searchBibleText ("phpunit2", "sixth");
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
    ids = database_search.searchBibleText ("phpunit2", "said");
    evaluate (__LINE__, __func__, 1, (int)ids.size ());
  }
  // Test search Bible case sensitive.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 1);
    database_search.updateSearchFields ("phpunit", 2, 3);
    database_search.updateSearchFields ("phpunit2", 4, 5);
    vector <int> ids = database_search.searchBibleTextCaseSensitive ("phpunit", "Verse");
    evaluate (__LINE__, __func__, 3, (int)ids.size ());
    ids = database_search.searchBibleText ("phpunit", "sixth");
    evaluate (__LINE__, __func__, 1, (int)ids.size ());
    ids = database_search.searchBibleText ("phpunit2", "said");
    evaluate (__LINE__, __func__, 1, (int)ids.size ());
  }
  // Test getting Bibles.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <string> bibles = database_search.getBibles ();
    evaluate (__LINE__, __func__, { "phpunit", "phpunit2" }, bibles);
    database_search.deleteBible ("phpunit");
    bibles = database_search.getBibles ();
    evaluate (__LINE__, __func__, { "phpunit2" }, bibles);
  }
  // Test getting books.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <int> books = database_search.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 2 }, books);
    database_search.deleteBook ("phpunit", 2);
    books = database_search.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { }, books);
  }
  // Test getting chapters.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <int> chapters = database_search.getChapters ("phpunit", 2);
    evaluate (__LINE__, __func__, { 3 }, chapters);
    database_search.deleteChapter ("phpunit", 2, 3);
    chapters = database_search.getChapters ("phpunit", 2);
    evaluate (__LINE__, __func__, { }, chapters);
  }
  // Test getVerses.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <int> verses = database_search.getVerses ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, verses);
  }
  // Test getVerseCount ()
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    int count = database_search.getVerseCount ("phpunit");
    evaluate (__LINE__, __func__, 11, count);
  }
}


void test_database_bibleactions ()
{
  refresh_sandbox (true);
  Database_BibleActions database_bibleactions = Database_BibleActions ();
  database_bibleactions.create ();
  
  database_bibleactions.optimize ();
  
  vector <string> bibles = database_bibleactions.getBibles ();
  evaluate (__LINE__, __func__, { }, bibles);

  database_bibleactions.record ("phpunit1", 1, 2, "data112");
  database_bibleactions.record ("phpunit1", 1, 3, "data113");
  database_bibleactions.record ("phpunit1", 2, 4, "data124");
  database_bibleactions.record ("phpunit2", 5, 6, "data256");
  database_bibleactions.record ("phpunit2", 5, 6, "data256: Not to be stored");

  bibles = database_bibleactions.getBibles ();
  evaluate (__LINE__, __func__, {"phpunit1", "phpunit2"}, bibles);

  vector <int> books = database_bibleactions.getBooks ("phpunit1");
  evaluate (__LINE__, __func__, {1, 2}, books);

  vector <int> chapters = database_bibleactions.getChapters ("phpunit1", 1);
  evaluate (__LINE__, __func__, {2, 3}, chapters);
  
  chapters = database_bibleactions.getChapters ("phpunit1", 2);
  evaluate (__LINE__, __func__, {4}, chapters);
  
  database_bibleactions.erase ("phpunit1", 2, 3);
  
  chapters = database_bibleactions.getChapters ("phpunit1", 2);
  evaluate (__LINE__, __func__, {4}, chapters);
  
  database_bibleactions.erase ("phpunit1", 2, 4);
  
  chapters = database_bibleactions.getChapters ("phpunit1", 2);
  evaluate (__LINE__, __func__, { }, chapters);
  
  string usfm = database_bibleactions.getUsfm ("phpunit2", 5, 5);
  evaluate (__LINE__, __func__, "", usfm);
  
  usfm = database_bibleactions.getUsfm ("phpunit2", 5, 6);
  evaluate (__LINE__, __func__, "data256", usfm);
}


void test_database_check ()
{
  {
    // Test Optimize
    refresh_sandbox (true);
    Database_Check database_check = Database_Check ();
    database_check.optimize ();
  }
  {
    // Test Record Get Truncate.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();

    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, (int)hits.size());
  
    database_check.recordOutput ("phpunit", 1, 2, 3, "test");
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)hits.size());
    
    database_check.truncateOutput ("");
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, (int)hits.size());
  }
  {
    // Test getting details.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 5, 2, 3, "test");
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)hits.size());
    evaluate (__LINE__, __func__, 1, hits [0].bible);
    evaluate (__LINE__, __func__, 5, hits [0].book);
    evaluate (__LINE__, __func__, 2, hits [0].chapter);
    evaluate (__LINE__, __func__, "test", hits [0].data);
  }
  {
    // Test approvals.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();

    database_check.recordOutput ("phpunit", 3, 4, 5, "test1");
    database_check.recordOutput ("phpunit", 3, 4, 5, "test2");
  
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, (int)hits.size());
  
    int id = hits [0].rowid;
    database_check.approve (id);
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)hits.size());
    
    vector <Database_Check_Hit> suppressions = database_check.getSuppressions ();
    evaluate (__LINE__, __func__, 1, (int)suppressions.size());
    
    id = suppressions [0].rowid;
    
    database_check.release (1);
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, (int)hits.size());
  }
  {
    // Test delete.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 3, 4, 5, "test1");
    database_check.recordOutput ("phpunit", 3, 4, 5, "test2");
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, (int)hits.size());
    int id = hits [0].rowid;
    database_check.erase (id);
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)hits.size());
  }
  {
    // Test passage.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 3, 4, 5, "test1");
    database_check.recordOutput ("phpunit", 6, 7, 8, "test2");
    Passage passage = database_check.getPassage (2);
    evaluate (__LINE__, __func__, 6, passage.book);
    evaluate (__LINE__, __func__, 7, passage.chapter);
    evaluate (__LINE__, __func__, "8", passage.verse);
  }
  {
    // Test same checks overflow.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 3, 4, 5, "once");
    for (unsigned int i = 0; i < 100; i++) {
      database_check.recordOutput ("phpunit", i, i, i, "multiple");
    }
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 12, (int)hits.size());
  }
}


void test_database_localization ()
{
  refresh_sandbox (true);
  string file_po = filter_url_create_root_path ("unittests", "tests", "nl.po");
  Database_Localization database_localization = Database_Localization ("nl");
  database_localization.create (file_po);

  string msgid = "phpunit";
  string msgstr = "phpunit";
  string result = database_localization.translate (msgid);
  evaluate (__LINE__, __func__, msgstr, result);

  msgid = "When this workbench will be opened, it will display all the notes that refer to the focused passage.";
  msgstr = "Als de werkbank geopend wordt, dan toont het alle aantekeningen die betrekking hebben op de gefocuste passage.";
  result = database_localization.translate (msgid);
  evaluate (__LINE__, __func__, msgstr, result);
}


void test_database_confirm ()
{
  refresh_sandbox (true);
  Database_Confirm database_confirm = Database_Confirm ();
  database_confirm.create ();

  database_confirm.optimize ();
  database_confirm.trim ();

  // New ID generation test.
  int id = database_confirm.getNewID ();
  if (id < 10000) evaluate (__LINE__, __func__, "Should be greater than 10000", id);
  
  // Store data for the ID.
  database_confirm.store (id, "SELECT x, y, z FROM a;", "email", "subject", "body");
  
  // Search for this ID based on subject.
  int id2 = database_confirm.searchID ("Subject line CID" + convert_to_string (id) + " Re:");
  evaluate (__LINE__, __func__, id, id2);

  // Retrieve data for the ID.
  string query = database_confirm.getQuery (id);
  evaluate (__LINE__, __func__,"SELECT x, y, z FROM a;", query);

  string to = database_confirm.getMailTo (id);
  evaluate (__LINE__, __func__,"email", to);

  string subject = database_confirm.getSubject (id);
  evaluate (__LINE__, __func__,"subject", subject);

  string body = database_confirm.getBody (id);
  evaluate (__LINE__, __func__,"body", body);

  // Delete this ID.
  database_confirm.erase (id);
  query = database_confirm.getQuery (id);
  evaluate (__LINE__, __func__,"", query);
}


void test_database_ipc ()
{
   // Test Trim
  {
    refresh_sandbox (true);
    Database_Ipc database_ipc = Database_Ipc (NULL);
    database_ipc.trim ();
  }
  // Test Store Retrieve
  {
    refresh_sandbox (true);
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);

    int id = 1;
    string user = "phpunit";
    string channel = "channel";
    string command = "command";
    string message = "message";
  
    database_ipc.storeMessage (user, channel, command, message);
   
    Database_Ipc_Message data = database_ipc.retrieveMessage (id, user, channel, command);
    evaluate (__LINE__, __func__, 0, data.id);
  
    database_ipc.storeMessage (user, channel, command, message);
  
    data = database_ipc.retrieveMessage (id, user, channel, command);
    evaluate (__LINE__, __func__, 2, data.id);
    evaluate (__LINE__, __func__, message, data.message);
  }
  // Test Delete
  {
    refresh_sandbox (true);
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);

    int id = 1;
    string user = "phpunit";
    string channel = "channel";
    string command = "command";
    string message = "message";
    
    database_ipc.storeMessage (user, channel, command, message);
    
    Database_Ipc_Message data = database_ipc.retrieveMessage (0, user, channel, command);
    evaluate (__LINE__, __func__, id, data.id);
    
    database_ipc.deleteMessage (id);
  
    data = database_ipc.retrieveMessage (0, user, channel, command);
    evaluate (__LINE__, __func__, 0, data.id);
  }
  // Test Get Focus
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);

    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "focus";
  
    string passage = database_ipc.getFocus ();
    evaluate (__LINE__, __func__, "1.1.1", passage);
  
    string message = "2.3.4";
    database_ipc.storeMessage (user, channel, command, message);
    passage = database_ipc.getFocus ();
    evaluate (__LINE__, __func__, message, passage);
  
    message = "5.6.7";
    database_ipc.storeMessage (user, channel, command, message);
    passage = database_ipc.getFocus ();
    evaluate (__LINE__, __func__, message, passage);
  }
  // Test Get Note.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);
    
    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "opennote";
  
    Database_Ipc_Message note = database_ipc.getNote ();
    evaluate (__LINE__, __func__, 0, note.id);
  
    string message = "12345";
    database_ipc.storeMessage (user, channel, command, message);
    note = database_ipc.getNote ();
    evaluate (__LINE__, __func__, message, note.message);
  
    message = "54321";
    database_ipc.storeMessage (user, channel, command, message);
    note = database_ipc.getNote ();
    evaluate (__LINE__, __func__, message, note.message);
  }
  // Test Notes Alive.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);
    
    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "notesalive";
  
    bool alive = database_ipc.getNotesAlive ();
    evaluate (__LINE__, __func__, false, alive);
  
    string message = "1";
    database_ipc.storeMessage (user, channel, command, message);
    alive = database_ipc.getNotesAlive ();
    evaluate (__LINE__, __func__, convert_to_bool (message), alive);
  
    message = "0";
    database_ipc.storeMessage (user, channel, command, message);
    alive = database_ipc.getNotesAlive ();
    evaluate (__LINE__, __func__, convert_to_bool (message), alive);
  }
}


void test_database_jobs ()
{
  {
    refresh_sandbox (true);
    Database_Jobs database_jobs = Database_Jobs ();
    database_jobs.create ();

    // Test Optimize
    database_jobs.optimize ();

    // Test Identifiers
    int id = database_jobs.getNewId ();
    bool exists = database_jobs.idExists (id);
    evaluate (__LINE__, __func__, true, exists);

    // Test Level.
    id = database_jobs.getNewId ();
    int level = database_jobs.getLevel (id);
    evaluate (__LINE__, __func__, 0, level);
    database_jobs.setLevel (id, 123);
    level = database_jobs.getLevel (id);
    evaluate (__LINE__, __func__, 123, level);

    // Test Start
    id = database_jobs.getNewId ();
    string start = database_jobs.getStart (id);
    evaluate (__LINE__, __func__, "", start);
    database_jobs.setStart (id, "start");
    start = database_jobs.getStart (id);
    evaluate (__LINE__, __func__, "start", start);
    
    // Test Progress
    id = database_jobs.getNewId ();
    string progress = database_jobs.getProgress (id);
    evaluate (__LINE__, __func__, "", progress);
    database_jobs.setProgress (id, "progress");
    progress = database_jobs.getProgress (id);
    evaluate (__LINE__, __func__, "progress", progress);
    
    // Test Percentage
    id = database_jobs.getNewId ();
    string percentage = database_jobs.getPercentage (id);
    evaluate (__LINE__, __func__, "", percentage);
    database_jobs.setPercentage (id, 55);
    percentage = database_jobs.getPercentage (id);
    evaluate (__LINE__, __func__, "55", percentage);
    
    // Test Result.
    id = database_jobs.getNewId ();
    string result = database_jobs.getResult (id);
    evaluate (__LINE__, __func__, "", result);
    database_jobs.setResult (id, "result");
    result = database_jobs.getResult (id);
    evaluate (__LINE__, __func__, "result", result);
  }
}


void test_database_kjv ()
{
  Database_Kjv database_kjv = Database_Kjv ();

  vector <Database_Kjv_Item> data = database_kjv.getVerse (43, 11, 35);
  evaluate (__LINE__, __func__, 3, (int)data.size());

  evaluate (__LINE__, __func__, "G3588", data[0].strong);
  evaluate (__LINE__, __func__, "Jesus", data[0].english);

  evaluate (__LINE__, __func__, "G2424", data[1].strong);
  evaluate (__LINE__, __func__, "Jesus", data[1].english);

  evaluate (__LINE__, __func__, "G1145", data[2].strong);
  evaluate (__LINE__, __func__, "wept",  data[2].english);

  vector <Passage> passages = database_kjv.searchStrong ("G909");
  evaluate (__LINE__, __func__, 4, (int)passages.size());

  evaluate (__LINE__, __func__, 41,   passages[0].book);
  evaluate (__LINE__, __func__, 7,    passages[0].chapter);
  evaluate (__LINE__, __func__, "4",  passages[0].verse);

  evaluate (__LINE__, __func__, 41,   passages[1].book);
  evaluate (__LINE__, __func__, 7,    passages[1].chapter);
  evaluate (__LINE__, __func__, "8",  passages[1].verse);

  evaluate (__LINE__, __func__, 58,   passages[2].book);
  evaluate (__LINE__, __func__, 6,    passages[2].chapter);
  evaluate (__LINE__, __func__, "2",  passages[2].verse);

  evaluate (__LINE__, __func__, 58,   passages[3].book);
  evaluate (__LINE__, __func__, 9,    passages[3].chapter);
  evaluate (__LINE__, __func__, "10", passages[3].verse);
}


void test_database_morphhb ()
{
  Database_Morphhb database_morphhb = Database_Morphhb ();

  vector <string> data = database_morphhb.getVerse (18, 3, 2);
  evaluate (__LINE__, __func__, { "וַיַּ֥עַן", "אִיּ֗וֹב", "וַיֹּאמַֽר" }, data);

  vector <Passage> passages = database_morphhb.searchHebrew ("יָדְע֥וּ");
  evaluate (__LINE__, __func__, 2, (int)passages.size());

  evaluate (__LINE__, __func__, 19,   passages[0].book);
  evaluate (__LINE__, __func__, 95,   passages[0].chapter);
  evaluate (__LINE__, __func__, "10", passages[0].verse);

  evaluate (__LINE__, __func__, 30,   passages[1].book);
  evaluate (__LINE__, __func__, 3,    passages[1].chapter);
  evaluate (__LINE__, __func__, "10", passages[1].verse);
}


void test_database_sblgnt ()
{
  Database_Sblgnt database_sblgnt = Database_Sblgnt ();

  vector <string> data = database_sblgnt.getVerse (43, 11, 35);
  evaluate (__LINE__, __func__, { "ἐδάκρυσεν", "ὁ", "Ἰησοῦς" }, data);

  vector <Passage> passages = database_sblgnt.searchGreek ("βαπτισμῶν");
  evaluate (__LINE__, __func__, 1,   (int)passages.size());
  evaluate (__LINE__, __func__, 58,  passages[0].book);
  evaluate (__LINE__, __func__, 6,   passages[0].chapter);
  evaluate (__LINE__, __func__, "2", passages[0].verse);
}


void test_database_offlineresourcese ()
{
  // Test Store / Exists / Get.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    bool exists = database_offlineresources.exists ("phpunit", 1, 2, 3);
    evaluate (__LINE__, __func__, true, exists);
    exists = database_offlineresources.exists ("phpunit", 1, 2, 4);
    evaluate (__LINE__, __func__, false, exists);
    string html = database_offlineresources.get ("phpunit", 1, 2, 3);
    evaluate (__LINE__, __func__, "xyz", html);
    html = database_offlineresources.get ("phpunit", 1, 2, 4);
    evaluate (__LINE__, __func__, "", html);
  }
  // Test Count / Delete.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    int count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 0, count);
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 1, count);
    database_offlineresources.store ("phpunit", 4, 5, 6, "xyz");
    count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 2, count);
    database_offlineresources.erase ("phpunit");
    count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 0, count);
  }
  // Test Names.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    vector <string> names = database_offlineresources.names ();
    evaluate (__LINE__, __func__, {}, names);
  
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    names = database_offlineresources.names ();
    evaluate (__LINE__, __func__, {"phpunit"}, names);
    
    database_offlineresources.store ("phpunit2", 1, 2, 3, "xyz");
    names = database_offlineresources.names ();
    evaluate (__LINE__, __func__, {"phpunit", "phpunit2"}, names);
  }
  // Test Files.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();

    vector <string> files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {}, files);
    
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite"}, files);
  
    database_offlineresources.store ("phpunit", 1, 4, 5, "xyz");
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite"}, files);
  
    database_offlineresources.store ("phpunit", 11, 22, 33, "xyz");
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite", "11.sqlite"}, files);
  }
  // Test Size.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  
    int size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 0, size);
  
    database_offlineresources.store ("phpunit", 1, 2, 3, string (100, 's'));
    size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 2048, size);
  
    database_offlineresources.store ("phpunit", 1, 3, 4, string (1000, 's'));
    size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 3072, size);
  }
  // Test Save / Load.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    
    int size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 0, size);
    
    string standard = "abc";
    database_offlineresources.save ("phpunit", "1.sqlite", standard);
    string contents = database_offlineresources.load ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, standard, contents);
  }
  // Test Unlink.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    vector <string> files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite"}, files);
    
    database_offlineresources.unlink ("phpunit", "2");
    
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite"}, files);
  
    database_offlineresources.unlink ("phpunit", "1.sqlite");
    
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {}, files);
  }
  // Test http get
  {
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    string http = database_offlineresources.httpget ("ResourceName", "1.sqlite");
    evaluate (__LINE__, __func__, "/databases/offlineresources/ResourceName/1.sqlite", http);
  }
}


void test_database_sprint ()
{
  // Maintenance.
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    database.optimize ();
  }
  // Store Task / GetTask
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    vector <int> ids = database.getTasks ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, {}, ids);
    database.storeTask ("phpunit", 2014, 1, "phpunit");
    ids = database.getTasks ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 1, (int)ids.size ());
    ids = database.getTasks ("phpunit", 2014, 2);
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
    ids = database.getTasks ("phpunit2", 2014, 1);
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
  }
  // GetTitle
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    database.storeTask ("phpunit", 2014, 1, "xyz");
    vector <int> ids = database.getTasks ("phpunit", 2014, 1);
    string title = database.getTitle (ids[0]);
    evaluate (__LINE__, __func__, "xyz", title);
  }
  // Complete
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    database.storeTask ("phpunit", 2014, 1, "phpunit");
    vector <int> ids = database.getTasks ("phpunit", 2014, 1);
    int id = ids[0];
    int complete = database.getComplete (id);
    evaluate (__LINE__, __func__, 0, complete);
    database.updateComplete (id, 95);
    complete = database.getComplete (id);
    evaluate (__LINE__, __func__, 95, complete);
  }
  // History
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();

    // Expect no history at all for January 2014.
    vector <Database_Sprint_Item> history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 0, (int)history.size());

    // Log values for January 2014, and check that the database returns those values.
    database.logHistory ("phpunit", 2014, 1, 10, 15, 50);
    history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 50, history[0].complete);

    // Log values for February 2014, and don't expect them when requesting the history for January ...
    database.logHistory ("phpunit", 2014, 2, 10, 15, 51);
    history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 50, history[0].complete);

    // ... but get those values when requesting history for February.
    history = database.getHistory ("phpunit", 2014, 2);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 51, history[0].complete);

    // Log another history entry for January 2014, and expect two correct entries for this month.
    database.logHistory ("phpunit", 2014, 1, 11, 16, 55);
    history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 50, history[0].complete);
    evaluate (__LINE__, __func__, 11, history[1].day);
    evaluate (__LINE__, __func__, 16, history[1].tasks);
    evaluate (__LINE__, __func__, 55, history[1].complete);
  }
}


void test_database_mail ()
{
  // Optimize / Trim.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    database_mail.optimize ();
    database_mail.trim ();
  }
  // Empty.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    request.session_logic ()->setUsername ("phpunit");

    int count = database_mail.getMailCount ();
    evaluate (__LINE__, __func__, 0, count);
    
    vector <Database_Mail_User> mails = database_mail.getMails ();
    evaluate (__LINE__, __func__, 0, (int)mails.size());
    
    vector <int> mails_to_send = database_mail.getMailsToSend ();
    evaluate (__LINE__, __func__, {}, mails_to_send);
  }
  // Normal Cycle
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    request.session_logic ()->setUsername ("phpunit");

    database_mail.send ("phpunit", "subject", "body");

    int count = database_mail.getMailCount ();
    evaluate (__LINE__, __func__, 1, count);

    vector <Database_Mail_User> mails = database_mail.getMails ();
    evaluate (__LINE__, __func__, "subject", mails [0].subject);
    
    Database_Mail_Item mail = database_mail.get (1);
    evaluate (__LINE__, __func__, "phpunit", mail.username);
    evaluate (__LINE__, __func__, "body", mail.body);

    database_mail.erase (1);

    count = database_mail.getMailCount ();
    evaluate (__LINE__, __func__, 0, count);
  }
  // Normal Postpone
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    request.session_logic ()->setUsername ("phpunit");

    database_mail.send ("phpunit", "subject", "body");

    vector <int> mails = database_mail.getMailsToSend ();
    evaluate (__LINE__, __func__, 1, (int)mails.size ());
    
    database_mail.postpone (1);
    mails = database_mail.getMailsToSend ();
    evaluate (__LINE__, __func__, 0, (int)mails.size ());
  }
}


void test_database_navigation ()
{
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    database.trim ();
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();

    // Use current time.
    int time = filter_date_seconds_since_epoch ();

    // Record one entry. As a result there should be no previous entry.
    database.record (time, "phpunit", 1, 2, 3);
    bool previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry, with the same time.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry 4 seconds later.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    time += 4;
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry 5 seconds later.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    time += 5;
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry 6 seconds later.
    // This should not remove the already existing entry.
    // As a result there should be a previous entry.
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, true, previous);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_date_seconds_since_epoch ();
    // Record one entry, and another 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    // Get previous entry, which should be the first one entered.
    Passage passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_date_seconds_since_epoch ();
    // Record one entry, and another 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    // Get previous entry for another user: It should not be there.
    Passage passage = database.getPrevious ("phpunit2");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_date_seconds_since_epoch ();
    // Record three entries, each one 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    time += 6;
    database.record (time, "phpunit", 7, 8, 9);
    // Get previous entry, which should be the second one entered.
    Passage passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 4, passage.book);
    evaluate (__LINE__, __func__, 5, passage.chapter);
    evaluate (__LINE__, __func__, "6", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_date_seconds_since_epoch ();
    // Record five entries, each one 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    time += 6;
    database.record (time, "phpunit", 7, 8, 9);
    time += 6;
    database.record (time, "phpunit", 10, 11, 12);
    time += 6;
    database.record (time, "phpunit", 13, 14, 15);
    // Get previous entry, which should be the last but one passage recorded.
    Passage passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 10, passage.book);
    evaluate (__LINE__, __func__, 11, passage.chapter);
    evaluate (__LINE__, __func__, "12", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // There should be no next passage.
    Passage passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_date_seconds_since_epoch ();
    // Record several entries, all spaced apart by 6 seconds.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    Passage passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Record two entries at an interval.
    int time = filter_date_seconds_since_epoch ();
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    // Next entry is not there.
    Passage passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
    // Previous entry should be there.
    passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    // Next entry should be there since we moved to the previous one.
    passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 4, passage.book);
    evaluate (__LINE__, __func__, 5, passage.chapter);
    evaluate (__LINE__, __func__, "6", passage.verse);
    // Previous entry should be there.
    passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    // Previous entry before previous entry should not be there.
    passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
    // Next entry should be there since we moved to the previous one.
    passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 4, passage.book);
    evaluate (__LINE__, __func__, 5, passage.chapter);
    evaluate (__LINE__, __func__, "6", passage.verse);
    // The entry next to the next entry should not be there.
    passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
}


void test_database_resources ()
{
  refresh_sandbox (true);
  vector <string> names = resource_external_names ();
  bool hit = false;
  for (auto & name : names) if (name == "Statenbijbel GBS") hit = true;
  evaluate (__LINE__, __func__, true, hit);
}


void test_database_usfmresources ()
{
  // Empty
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    vector <string> resources = database_usfmresources.getResources ();
    bool hit = false;
    for (auto & resource : resources) if (resource == "bibledit") hit = true;
    evaluate (__LINE__, __func__, false, hit);
  }
  // Names
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "");
    vector <string> resources = database_usfmresources.getResources ();
    bool hit = false;
    for (auto & resource : resources) if (resource == "bibledit") hit = true;
    evaluate (__LINE__, __func__, true, hit);
  }
  // Store Get Chapter
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    string usfm = database_usfmresources.getUsfm ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, "usfm", usfm);
    usfm = database_usfmresources.getUsfm ("bibledit", 2, 4);
    evaluate (__LINE__, __func__, "", usfm);
  }
  // Books
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    database_usfmresources.storeChapter ("bibledit", 3, 4, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 1, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 2, "usfm");
    vector <int> books = database_usfmresources.getBooks ("bibledit");
    evaluate (__LINE__, __func__, {1, 2, 3}, books);
  }
  // Chapters
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    database_usfmresources.storeChapter ("bibledit", 3, 4, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 1, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 2, "usfm");
    vector <int> chapters = database_usfmresources.getChapters ("bibledit", 1);
    evaluate (__LINE__, __func__, {1, 2}, chapters);
    chapters = database_usfmresources.getChapters ("bibledit", 2);
    evaluate (__LINE__, __func__, {3}, chapters);
  }
  // Sizes
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();

    int size = database_usfmresources.getSize ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, 0, size);

    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    size = database_usfmresources.getSize ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, 4, size);
    
    database_usfmresources.storeChapter ("bibledit", 2, 3, "chapter");
    size = database_usfmresources.getSize ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, 7, size);
  }
  // Delete Book
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    vector <int> books = database_usfmresources.getBooks ("bibledit");
    evaluate (__LINE__, __func__, {2}, books);
    database_usfmresources.deleteBook ("bibledit", 2);
    books = database_usfmresources.getBooks ("bibledit");
    evaluate (__LINE__, __func__, {}, books);
  }
}


void test_database_mappings ()
{
  // Setup
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    database_mappings.defaults ();
    database_mappings.create2 ();
    database_mappings.optimize ();
    vector <string> names = database_mappings.names ();
    evaluate (__LINE__, __func__, {"Dutch Traditional", "English", "French Louise Segond", "Hebrew Greek", "Russian Canonical", "Russian Orthodox", "Russian Protestant", "Spanish", "Vulgate"}, names);
  }
  // Import Export
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import = 
      "2 Chronicles 14:15 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("phpunit", import);
    vector <string> names = database_mappings.names ();
    evaluate (__LINE__, __func__, {"Hebrew Greek", "phpunit"}, names);
    string output = database_mappings.output ("phpunit");
    evaluate (__LINE__, __func__, filter_string_trim (import), filter_string_trim (output));
  }
  // Create
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    database_mappings.create ("phpunit");
    vector <string> names = database_mappings.names ();
    evaluate (__LINE__, __func__, {"Hebrew Greek", "phpunit"}, names);
  }
  // Translate Same
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    vector <Passage> passages = database_mappings.translate ("ABC", "ABC", 14, 14, 15);
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    Passage standard = Passage ("", 14, 14, "15");
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
    passages = database_mappings.translate ("--X", "--X", 15, 16, 17);
    standard = Passage ("", 15, 16, "17");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
  }
  // Translate
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import = 
      "2 Chronicles 14:15 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("ABC", import);
    import =
      "2 Chronicles 14:15 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("XYZ", import);
    // Test mapping 2 Chronicles.
    vector <Passage> passages = database_mappings.translate ("ABC", "XYZ", 14, 14, 15);
    Passage standard = Passage ("", 14, 14, "15");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
  }
  // Translate
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import =
      "2 Chronicles 14:15 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("ABC", import);
    import =
      "2 Chronicles 14:13 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("XYZ", import);
    // Test mapping 2 Chronicles.
    vector <Passage> passages = database_mappings.translate ("ABC", "XYZ", 14, 14, 15);
    Passage standard = Passage ("", 14, 14, "13");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
  }
  // Translate Double Result.
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import =
      "2 Chronicles 14:15 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("ABC", import);
    import =
      "2 Chronicles 14:12 = 2 Chronicles 14:14\n"
      "2 Chronicles 14:13 = 2 Chronicles 14:14\n"
      "Nehemiah 4:1 = Nehemiah 3:33\n"
      "Song of Solomon 7:2 = Song of Solomon 7:3\n";
    database_mappings.import ("XYZ", import);
    // Test mapping 2 Chronicles.
    vector <Passage> passages = database_mappings.translate ("ABC", "XYZ", 14, 14, 15);
    evaluate (__LINE__, __func__, 2, (int)passages.size ());
    Passage standard = Passage ("", 14, 14, "12");
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
    standard = Passage ("", 14, 14, "13");
    evaluate (__LINE__, __func__, true, passages[1].equal (standard));
  }
  // Translate From Original
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import = "2 Chronicles 14:12 = 2 Chronicles 14:14";
    database_mappings.import ("VVV", import);
    vector <Passage> passages = database_mappings.translate ("Hebrew Greek", "VVV", 14, 14, 14);
    Passage standard = Passage ("", 14, 14, "12");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
  }
  // Translate From Original Double
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import =
      "2 Chronicles 14:12 = 2 Chronicles 14:14\n"
      "2 Chronicles 14:13 = 2 Chronicles 14:14\n";
    database_mappings.import ("VVV", import);
    vector <Passage> passages = database_mappings.translate ("Hebrew Greek", "VVV", 14, 14, 14);
    evaluate (__LINE__, __func__, 2, (int)passages.size ());
    Passage standard = Passage ("", 14, 14, "12");
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
    standard = Passage ("", 14, 14, "13");
    evaluate (__LINE__, __func__, true, passages[1].equal (standard));
  }
  // Translate From Original No Mapping
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import = "2 Chronicles 14:12 = 2 Chronicles 14:14";
    database_mappings.import ("VVV", import);
    vector <Passage> passages = database_mappings.translate ("Hebrew Greek", "VVV", 14, 15, 14);
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    Passage standard = Passage ("", 14, 15, "14");
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
  }
  // Translate To Original
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import = "2 Chronicles 14:12 = 2 Chronicles 14:14";
    database_mappings.import ("ABA", import);
    vector <Passage> passages = database_mappings.translate ("ABA", "Hebrew Greek", 14, 14, 12);
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    Passage standard = Passage ("", 14, 14, "14");
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
  }
  // Translate To Original Double
  {
    refresh_sandbox (true);
    Database_Mappings database_mappings = Database_Mappings ();
    database_mappings.create1 ();
    string import =
      "2 Chronicles 14:12 = 2 Chronicles 14:13\n"
      "2 Chronicles 14:12 = 2 Chronicles 14:14\n";
    database_mappings.import ("ABA", import);
    vector <Passage> passages = database_mappings.translate ("ABA", "Hebrew Greek", 14, 14, 12);
    evaluate (__LINE__, __func__, 2, (int)passages.size ());
    Passage standard = Passage ("", 14, 14, "13");
    evaluate (__LINE__, __func__, true, passages[0].equal (standard));
    standard = Passage ("", 14, 14, "14");
    evaluate (__LINE__, __func__, true, passages[1].equal (standard));
  }

}


void test_database_noteactions ()
{
  // Basic tests: create / clear / optimize.
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    database.clear ();
    database.optimize ();
  }
  // Record
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    database.record ("phpunit", 2, 3, "content");
    vector <int> notes = database.getNotes ();
    evaluate (__LINE__, __func__, {2}, notes);
  }
  // Get Notes
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    database.record ("phpunit", 2, 3, "content");
    database.record ("phpunit", 2, 4, "content");
    database.record ("phpunit", 3, 3, "content");
    vector <int> notes = database.getNotes ();
    evaluate (__LINE__, __func__, {2, 3}, notes);
  }
  // Get Note Data
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    database.record ("phpunit1", 2, 3, "content3");
    database.record ("phpunit2", 2, 4, "content4");
    database.record ("phpunit3", 3, 4, "content5");
    vector <Database_Note_Action> data = database.getNoteData (2);
    evaluate (__LINE__, __func__, 2, (int)data.size());
    int now = filter_date_seconds_since_epoch ();
    evaluate (__LINE__, __func__, 1, data[0].rowid);
    evaluate (__LINE__, __func__, "phpunit1", data[0].username);
    if ((data[0].timestamp < now - 1) || (data[0].timestamp > now + 2)) evaluate (__LINE__, __func__, now, data[0].timestamp);
    evaluate (__LINE__, __func__, 3, data[0].action);
    evaluate (__LINE__, __func__, "content3", data[0].content);
    evaluate (__LINE__, __func__, 2, data[1].rowid);
    evaluate (__LINE__, __func__, "phpunit2", data[1].username);
    if ((data[1].timestamp < now) || (data[1].timestamp > now + 1)) evaluate (__LINE__, __func__, now, data[1].timestamp);
    evaluate (__LINE__, __func__, 4, data[1].action);
    evaluate (__LINE__, __func__, "content4", data[1].content);
  }
  // Update Notes.
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    database.record ("phpunit", 2, 3, "content");
    database.record ("phpunit", 2, 4, "content");
    database.record ("phpunit", 3, 3, "content");
    database.updateNotes (2, 12345);
    vector <int> notes = database.getNotes ();
    evaluate (__LINE__, __func__, {12345, 3}, notes);
  }
  // Delete.
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    database.record ("phpunit1", 2, 3, "content1");
    database.record ("phpunit2", 4, 5, "content2");
    database.erase (1);
    vector <int> notes = database.getNotes ();
    evaluate (__LINE__, __func__, {4}, notes);
  }
  // Exists
  {
    refresh_sandbox (true);
    Database_NoteActions database = Database_NoteActions ();
    database.create ();
    evaluate (__LINE__, __func__, false, database.exists (2));
    database.record ("phpunit1", 2, 3, "content1");
    database.record ("phpunit2", 4, 5, "content2");
    evaluate (__LINE__, __func__, true, database.exists (2));
    evaluate (__LINE__, __func__, false, database.exists (3));
  }
}


void test_database_versifications ()
{
  // Basic operations, create, delete.
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications = Database_Versifications ();
    database_versifications.create ();
    database_versifications.optimize ();
    int id = database_versifications.createSystem ("phpunit");
    evaluate (__LINE__, __func__, 1000, id);
    id = database_versifications.getID ("phpunit");
    evaluate (__LINE__, __func__, 1000, id);
    vector <string> systems = database_versifications.getSystems ();
    evaluate (__LINE__, __func__, {"phpunit"}, systems);
    database_versifications.erase ("phpunit");
    systems = database_versifications.getSystems ();
    evaluate (__LINE__, __func__, {}, systems);
  }
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications = Database_Versifications ();
    database_versifications.create ();
    database_versifications.defaults ();

    // GetID
    int id = database_versifications.getID ("English");
    evaluate (__LINE__, __func__, 2 , id);

    // Test books.
    vector <int> books = database_versifications.getBooks ("English");
    vector <int> standard;
    for (unsigned int i = 1; i <= 66; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, books);

    // Test chapters.
    vector <int> chapters = database_versifications.getChapters ("English", 1);
    standard.clear ();
    for (unsigned int i = 1; i <= 50; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, chapters);
    chapters = database_versifications.getChapters ("English", 1, true);
    standard.clear ();
    for (unsigned int i = 0; i <= 50; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, chapters);

    // Test verses.
    vector <int> verses = database_versifications.getVerses ("English", 1, 2);
    standard.clear ();
    for (unsigned int i = 0; i <= 25; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, verses);

    // Verses In Chapter Zero.
    verses = database_versifications.getVerses ("English", 1, 0);
    evaluate (__LINE__, __func__, {0}, verses);

    // Books Chapters Verses.
    vector <Passage> data = database_versifications.getBooksChaptersVerses ("English");
    evaluate (__LINE__, __func__, 1189, (int)data.size());
    evaluate (__LINE__, __func__, "31", data [0].verse);
  }
  // Import Export
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications = Database_Versifications ();
    database_versifications.create ();
    string input = 
      "Genesis 1:31\n"
      "Genesis 2:25\n";
    database_versifications.input (input, "phpunit");
    int id = database_versifications.getID ("phpunit");
    evaluate (__LINE__, __func__, 1000, id);
    vector <Passage> data = database_versifications.getBooksChaptersVerses ("phpunit");
    evaluate (__LINE__, __func__, 2, (int)data.size ());
    evaluate (__LINE__, __func__, "25", data [1].verse);
    string output = database_versifications.output ("phpunit");
    evaluate (__LINE__, __func__, filter_string_trim (input), filter_string_trim (output));
  }
}


void test_database_modifications_user ()
{
  // Create, erase, clear.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.erase ();
    database_modifications.create ();
    database_modifications.clearUserUser ("phpunit");
  }
  // Usernames
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    vector <string> users = database_modifications.getUserUsernames ();
    evaluate (__LINE__, __func__, {}, users);
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    users = database_modifications.getUserUsernames ();
    evaluate (__LINE__, __func__, {"phpunit1"}, users);
    database_modifications.recordUserSave ("phpunit2", "bible", 1, 2, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit3", "bible", 1, 2, 3, "old", 6, "new");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 7, "new");
    database_modifications.recordUserSave ("phpunit2", "bible", 1, 2, 3, "old", 8, "new");
    database_modifications.recordUserSave ("phpunit3", "bible", 1, 2, 3, "old", 9, "new");
    users = database_modifications.getUserUsernames ();
    evaluate (__LINE__, __func__, {"phpunit1", "phpunit2", "phpunit3"}, users);
  }
  // Bibles
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    vector <string> bibles = database_modifications.getUserBibles ("phpunit1");
    evaluate (__LINE__, __func__, {}, bibles);
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 6, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 7, "new");
    bibles = database_modifications.getUserBibles ("phpunit1");
    evaluate (__LINE__, __func__, {"bible1"}, bibles);
  }
  // Books
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    vector <int> books = database_modifications.getUserBooks ("phpunit1", "bible1");
    evaluate (__LINE__, __func__, {}, books);
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 2, 2, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 3, 2, 3, "old", 6, "new");
    database_modifications.recordUserSave ("phpunit1", "bible2", 4, 2, 3, "old", 7, "new");
    books = database_modifications.getUserBooks ("phpunit1", "bible1");
    evaluate (__LINE__, __func__, {1, 2, 3}, books);
  }
  // Chapters
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    vector <int> chapters = database_modifications.getUserChapters ("phpunit1", "bible1", 1);
    evaluate (__LINE__, __func__, {}, chapters);
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 3, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 4, 3, "old", 6, "new");
    chapters = database_modifications.getUserChapters ("phpunit1", "bible1", 1);
    evaluate (__LINE__, __func__, {2, 3, 4}, chapters);
  }
  // Identifiers.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 4, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 5, "old", 6, "new");
    vector <Database_Modifications_Id> identifiers = database_modifications.getUserIdentifiers ("phpunit1", "bible", 1, 2);
    evaluate (__LINE__, __func__, 3, (int)identifiers.size());
    evaluate (__LINE__, __func__, 3, identifiers[0].oldid);
    evaluate (__LINE__, __func__, 4, identifiers[0].newid);
    evaluate (__LINE__, __func__, 4, identifiers[1].oldid);
    evaluate (__LINE__, __func__, 5, identifiers[1].newid);
    evaluate (__LINE__, __func__, 5, identifiers[2].oldid);
    evaluate (__LINE__, __func__, 6, identifiers[2].newid);
  }
  // Chapter
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old1", 4, "new1");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 4, "old2", 5, "new2");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 5, "old3", 6, "new3");
    Database_Modifications_Text chapter = database_modifications.getUserChapter ("phpunit1", "bible", 1, 2, 4);
    evaluate (__LINE__, __func__, "old1", chapter.oldtext);
    evaluate (__LINE__, __func__, "new1", chapter.newtext);
    chapter = database_modifications.getUserChapter ("phpunit1", "bible", 1, 2, 5);
    evaluate (__LINE__, __func__, "old2", chapter.oldtext);
    evaluate (__LINE__, __func__, "new2", chapter.newtext);
  }
  // Timestamp
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old1", 4, "new1");
    int time = database_modifications.getUserTimestamp ("phpunit1", "bible", 1, 2, 4);
    int currenttime = filter_date_seconds_since_epoch ();
    if ((time < currenttime - 1) || (time > currenttime + 1)) evaluate (__LINE__, __func__, currenttime, time);
  }
}


void test_database_modifications_team ()
{
  // Basics.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");
  }
  // Team Existence
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");
 
    // Initially no diff should exist.
    bool exists = database_modifications.teamDiffExists ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, false, exists);

    // After storing a chapter there should be a diff.
    Bible_Logic::storeChapter ("phpunit", 1, 2, "chapter text one");
    exists = database_modifications.teamDiffExists ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, true, exists);

    // After storing a chapter for the second time, the diff should still exist.
    Bible_Logic::storeChapter ("none", 1, 2, "chapter text two");
    exists = database_modifications.teamDiffExists ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, true, exists);
    
    // The diff data should not exist for another chapter.
    exists = database_modifications.teamDiffExists ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, false, exists);
  }
  // Team Book
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 2, 1));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 2, 2));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 2, 3));

    Bible_Logic::storeChapter ("phpunit", 2, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 2, 2, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 2, 3, "chapter text");

    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 2));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 3));

    database_modifications.truncateTeams ();

    // Test function storeTeamDiffBook.
    database_modifications.storeTeamDiffBook ("phpunit", 2);
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 2));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 3));
  }
  // Team Bible
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 3, 1));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 4, 1));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 5, 1));

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 4, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 5, 1, "chapter text");

    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 3, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 4, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 5, 1));

    database_modifications.truncateTeams ();

    // Test function storeTeamDiffBible. It stores diff data for the whole bible.
    database_modifications.storeTeamDiffBible ("phpunit");

    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 3, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 4, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 5, 1));
  }
  // Team Get Diff
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    string diff = database_modifications.getTeamDiff ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, "", diff);

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    database_modifications.truncateTeams ();
    Bible_Logic::storeChapter ("phpunit", 3, 1, "longer chapter text");
    diff = database_modifications.getTeamDiff ("phpunit", 3, 1);
    evaluate (__LINE__, __func__, "chapter text", diff);

    diff = database_modifications.getTeamDiff ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, "", diff);

    Bible_Logic::storeChapter ("phpunit", 5, 5, "chapter text");
    database_modifications.truncateTeams ();
    Bible_Logic::storeChapter ("phpunit", 5, 5, "longer chapter text");
    diff = database_modifications.getTeamDiff ("phpunit", 5, 5);
    evaluate (__LINE__, __func__, "chapter text", diff);
  }
  // Team Get Diff Chapters
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 5, "chapter text");

    vector <int> chapters = database_modifications.getTeamDiffChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, {}, chapters);

    chapters = database_modifications.getTeamDiffChapters ("phpunit", 3);
    evaluate (__LINE__, __func__, {1, 3, 5}, chapters);

    chapters = database_modifications.getTeamDiffChapters ("phpunit2", 3);
    evaluate (__LINE__, __func__, {}, chapters);
  }
  // Team Diff Bible
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 5, "chapter text");
    
    vector <string> bibles = database_modifications.getTeamDiffBibles ();
    evaluate (__LINE__, __func__, {"phpunit"}, bibles);
    
    database_modifications.deleteTeamDiffBible ("phpunit2");

    bibles = database_modifications.getTeamDiffBibles ();
    evaluate (__LINE__, __func__, {"phpunit"}, bibles);

    database_modifications.deleteTeamDiffBible ("phpunit");

    bibles = database_modifications.getTeamDiffBibles ();
    evaluate (__LINE__, __func__, {}, bibles);
  }
  // Team Diff Chapter.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 5, "chapter text");
    
    vector <int> chapters = database_modifications.getTeamDiffChapters ("phpunit", 3);
    evaluate (__LINE__, __func__, {1, 3, 5}, chapters);

    database_modifications.deleteTeamDiffChapter ("phpunit", 3, 1);

    chapters = database_modifications.getTeamDiffChapters ("phpunit", 3);
    evaluate (__LINE__, __func__, {3, 5}, chapters);
  }
  // Team Diff Book
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 4, 5, "chapter text");

    vector <int> books = database_modifications.getTeamDiffBooks ("phpunit");
    evaluate (__LINE__, __func__, {3, 4}, books);

    books = database_modifications.getTeamDiffBooks ("phpunit2");
    evaluate (__LINE__, __func__, {}, books);
  }
  // Get Team Diff Count
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit2", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit2", 3, 5, "chapter text");

    int count = database_modifications.getTeamDiffCount ("phpunit");
    evaluate (__LINE__, __func__, 1, count);

    count = database_modifications.getTeamDiffCount ("phpunit2");
    evaluate (__LINE__, __func__, 2, count);

    count = database_modifications.getTeamDiffCount ("phpunit3");
    evaluate (__LINE__, __func__, 0, count);
  }
  
}


void test_database_modifications_notifications ()
{
  // Basics.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    for (auto id : ids) {
      database_modifications.deleteNotification (id);
    }
  }
  // Trim.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();

    // Record two entries.
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {1, 2}, ids);

    // After filter_string_trimming the two entries should still be there.
    database_modifications.indexTrimAllNotifications ();
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {1, 2}, ids);

    // Set the time back, re-index, filter_string_trim, and check one entry's gone.
    string file = database_modifications.notificationTimeFile (1);
    database_modifications.indexTrimAllNotifications ();
    filter_url_file_put_contents (file, convert_to_string (filter_date_seconds_since_epoch () - 7776001));
    database_modifications.indexTrimAllNotifications ();
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {2}, ids);
  }
  // Next Identifier.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    int identifier = database_modifications.getNextAvailableNotificationIdentifier ();
    evaluate (__LINE__, __func__, 1, identifier);
    database_modifications.recordNotification ({"phpunit1"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    identifier = database_modifications.getNextAvailableNotificationIdentifier ();
    evaluate (__LINE__, __func__, 2, identifier);
    database_modifications.deleteNotification (1);
    identifier = database_modifications.getNextAvailableNotificationIdentifier ();
    evaluate (__LINE__, __func__, 1, identifier);
  }
  // Record Details Retrieval.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();

    // Start with no identifiers.
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {}, ids);

    // Record three notifications and reindex.
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit2", "phpunit3"}, "A", "1", 4, 5, 6, "old2", "mod2", "new2");
    database_modifications.recordNotification ({"phpunit3", "phpunit4"}, "B", "1", 7, 8, 9, "old3", "mod3", "new3");
    database_modifications.indexTrimAllNotifications ();

    // There should be six notifications now: Two users per recordNotification call.
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {1, 2, 3, 4, 5, 6}, ids);
    
    // Test notifications per user.
    ids = database_modifications.getNotificationIdentifiers ("phpunit1");
    evaluate (__LINE__, __func__, {1}, ids);
    ids = database_modifications.getNotificationIdentifiers ("phpunit3");
    evaluate (__LINE__, __func__, {4, 5}, ids);
  }
  // Timestamps
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();

    int timestamp = database_modifications.getNotificationTimeStamp (0);
    int currenttime = filter_date_seconds_since_epoch ();
    if ((timestamp < currenttime) || (timestamp > currenttime + 1)) evaluate (__LINE__, __func__, currenttime, timestamp);

    int time = filter_date_seconds_since_epoch () - 21600;
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    timestamp = database_modifications.getNotificationTimeStamp (1);
    if ((timestamp < time) || (timestamp > time + 1)) evaluate (__LINE__, __func__, time, timestamp);
  }
  // Category
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string category = database_modifications.getNotificationCategory (1);
    evaluate (__LINE__, __func__, "A", category);
    category = database_modifications.getNotificationCategory (2);
    evaluate (__LINE__, __func__, "", category);
  }
  // Bible.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string bible = database_modifications.getNotificationBible (1);
    evaluate (__LINE__, __func__, "1", bible);
    bible = database_modifications.getNotificationBible (3);
    evaluate (__LINE__, __func__, "", bible);
  }
  // Passage
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    Passage passage = database_modifications.getNotificationPassage (1);
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    passage = database_modifications.getNotificationPassage (3);
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  // OldText
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string old = database_modifications.getNotificationOldText (1);
    evaluate (__LINE__, __func__, "old1", old);
    old = database_modifications.getNotificationOldText (3);
    evaluate (__LINE__, __func__, "", old);
  }
  // Modification.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string modification = database_modifications.getNotificationModification (1);
    evaluate (__LINE__, __func__, "mod1", modification);
    modification = database_modifications.getNotificationOldText (3);
    evaluate (__LINE__, __func__, "", modification);
  }
  // New Text
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string newtext = database_modifications.getNotificationNewText (1);
    evaluate (__LINE__, __func__, "new1", newtext);
    newtext = database_modifications.getNotificationNewText (3);
    evaluate (__LINE__, __func__, "", newtext);
  }
  // Clear User
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit1", "phpunit2", "phpunit3"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 3, (int)ids.size ());

    database_modifications.clearNotificationsUser ("phpunit2");

    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 2, (int)ids.size ());

    ids = database_modifications.getNotificationIdentifiers ("phpunit2");
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
  }
  // Clear Matches One
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "P", "1", 2, 3, 4, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit"}, "T", "1", 2, 3, 4, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 2, (int)ids.size ());
    database_modifications.clearNotificationMatches ("phpunit", "P", "T");
    database_modifications.indexTrimAllNotifications ();
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
  }
  // Notification Personal Identifiers
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit2", "phpunit1"}, "B", "1", 1, 2, 3, "old2", "mod2", "new2");
    database_modifications.recordNotification ({"phpunit3", "phpunit4"}, "B", "1", 7, 8, 9, "old3", "mod3", "new3");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationPersonalIdentifiers ("phpunit1", "A");
    evaluate (__LINE__, __func__, {1, 4}, ids);
  }
  // Notification Team Identifiers
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit2", "phpunit1"}, "B", "1", 1, 2, 3, "old2", "mod2", "new2");
    database_modifications.recordNotification ({"phpunit3", "phpunit4"}, "B", "1", 7, 8, 9, "old3", "mod3", "new3");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationTeamIdentifiers ("phpunit1", "A");
    evaluate (__LINE__, __func__, {1}, ids);
    ids = database_modifications.getNotificationTeamIdentifiers ("phpunit1", "B");
    evaluate (__LINE__, __func__, {4}, ids);
  }
}


void test_database_notes ()
{
  // DatabasePath
  {
    refresh_sandbox (true);
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    string path = database_notes.database_path ();
    evaluate (__LINE__, __func__, filter_url_create_root_path ("databases", "notes.sqlite"), path);
    path = database_notes.checksums_database_path ();
    evaluate (__LINE__, __func__, filter_url_create_root_path ("databases", "notes_checksums.sqlite"), path);
  }
  // TrimOptimize
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();
    database_notes.optimize ();
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "", "", false);
    database_notes.erase (identifier);
    database_notes.trim ();
    database_notes.trim_server ();
    // Since the logbook will have an entry about "Deleting empty notes folder",
    // erase that entry here.
    refresh_sandbox (false);
  }
  // Identifier.
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    int identifier = Notes_Logic::lowNoteIdentifier;
    evaluate (__LINE__, __func__, 100000000, identifier);

    identifier = Notes_Logic::highNoteIdentifier;
    evaluate (__LINE__, __func__, 999999999, identifier);

    identifier = database_notes.getNewUniqueIdentifier ();
    if ((identifier < 100000000) || (identifier > 999999999)) evaluate (__LINE__, __func__, "Out of bounds", convert_to_string (identifier));
    evaluate (__LINE__, __func__, false, database_notes.identifierExists (identifier));

    identifier = database_notes.storeNewNote ("", 0, 0, 0, "", "", false);
    evaluate (__LINE__, __func__, true, database_notes.identifierExists (identifier));
    database_notes.erase (identifier);
    evaluate (__LINE__, __func__, false, database_notes.identifierExists (identifier));
  }
  // SummaryContents
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();
    // Test inserting data for both summary and contents.
    string summary = "Summary";
    string contents = "Contents";
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, summary, contents, false);
    string value = database_notes.getSummary (identifier);
    evaluate (__LINE__, __func__, summary, value);
    value = database_notes.getContents (identifier);
    vector <string> values = filter_string_explode (value, '\n');
    value = values[1];
    evaluate (__LINE__, __func__, "<p>Contents</p>", value);
    // Test that if the summary is not given, it is going to be the first line of the contents.
    contents = "This is a note.\nLine two.";
    identifier = database_notes.storeNewNote ("", 0, 0, 0, "", contents, false);
    value = database_notes.getSummary (identifier);
    evaluate (__LINE__, __func__, "This is a note.", value);
    value = database_notes.getContents (identifier);
    values = filter_string_explode (value, '\n');
    value = values[2];
    evaluate (__LINE__, __func__, "<p>Line two.</p>", value);
    // Test setSummary function.
    database_notes.setSummary (identifier, "summary1");
    value = database_notes.getSummary (identifier);
    evaluate (__LINE__, __func__, "summary1", value);
    // Test setContents function.
    database_notes.setContents (identifier, "contents1");
    value = database_notes.getContents (identifier);
    evaluate (__LINE__, __func__, "contents1", value);
    // Test adding comment.
    value = database_notes.getContents (identifier);
    int length = value.length ();
    database_notes.addComment (identifier, "comment1");
    value = database_notes.getContents (identifier);
    if (value.length () < (size_t) (length + 30)) evaluate (__LINE__, __func__, "Should be larger than length + 30", convert_to_string ((int)value.length()));
    size_t pos = value.find ("comment1");
    if (pos == string::npos) evaluate (__LINE__, __func__, "Should contain 'comment1'", value);
  }
  // Subscriptions.
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();
    Notes_Logic notes_logic = Notes_Logic(&request);
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();

    // Normally creating a new note would subscribe the current user to the note.
    // But since this PHPUnit test runs without sessions, it would have subscribed an empty user.
    request.session_logic()->setUsername ("");
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    vector <string> subscribers = database_notes.getSubscribers (identifier);
    evaluate (__LINE__, __func__, {}, subscribers);

    // Create a note again, but this time set the session variable to a certain user.
    database_users.addNewUser ("phpunit", "", 5, "");
    request.session_logic()->setUsername ("phpunit");
    request.database_config_user()->setSubscribeToConsultationNotesEditedByMe (true);
    identifier = database_notes.storeNewNote ("", 1, 1, 1, "Summary", "Contents", false);
    notes_logic.handlerNewNote (identifier);
    subscribers = database_notes.getSubscribers (identifier);
    evaluate (__LINE__, __func__, {"phpunit"}, subscribers);
    evaluate (__LINE__, __func__, true, database_notes.isSubscribed (identifier, "phpunit"));
    request.database_config_user()->setSubscribeToConsultationNotesEditedByMe (false);
    // Test various other subscription related functions.
    evaluate (__LINE__, __func__, false, database_notes.isSubscribed (identifier, "phpunit_phpunit"));
    database_notes.unsubscribe (identifier);
    evaluate (__LINE__, __func__, false, database_notes.isSubscribed (identifier, "phpunit"));
    database_notes.subscribeUser (identifier, "phpunit_phpunit_phpunit");
    evaluate (__LINE__, __func__, true, database_notes.isSubscribed (identifier, "phpunit_phpunit_phpunit"));
    database_notes.unsubscribeUser (identifier, "phpunit_phpunit_phpunit");
    evaluate (__LINE__, __func__, false, database_notes.isSubscribed (identifier, "phpunit_phpunit_phpunit"));
  }
  // Assignments ()
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();

    request.session_logic()->setUsername ("PHPUnit2");

    // Create a note and check that it was not assigned to anybody.
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    vector <string> assignees = database_notes.getAssignees (identifier);
    evaluate (__LINE__, __func__, {}, assignees);

    // Assign the note to a user, and check that this reflects in the list of assignees.
    database_notes.assignUser (identifier, "PHPUnit");
    assignees = database_notes.getAssignees (identifier);
    evaluate (__LINE__, __func__, {"PHPUnit"}, assignees);

    // Test the setAssignees function.
    database_notes.setAssignees (identifier, {"PHPUnit"});
    assignees = database_notes.getAssignees (identifier);
    evaluate (__LINE__, __func__, {"PHPUnit"}, assignees);
        
    // Assign note to second user, and check it reflects.
    database_notes.assignUser (identifier, "PHPUnit2");
    assignees = database_notes.getAssignees (identifier);
    evaluate (__LINE__, __func__, {"PHPUnit", "PHPUnit2"}, assignees);

    // Based on the above, check the isAssigned function.
    evaluate (__LINE__, __func__, true, database_notes.isAssigned (identifier, "PHPUnit"));
    evaluate (__LINE__, __func__, true, database_notes.isAssigned (identifier, "PHPUnit2"));
    evaluate (__LINE__, __func__, false, database_notes.isAssigned (identifier, "PHPUnit3"));
    
    // Based on the above, test getAllAssignees().
    assignees = database_notes.getAllAssignees ({""});
    evaluate (__LINE__, __func__, {"PHPUnit", "PHPUnit2"}, assignees);
    
    // Based on the above, test the unassignUser function.
    database_notes.unassignUser (identifier, "PHPUnit");
    assignees = database_notes.getAssignees (identifier);
    evaluate (__LINE__, __func__, {"PHPUnit2"}, assignees);
    database_notes.unassignUser (identifier, "PHPUnit2");
    assignees = database_notes.getAssignees (identifier);
    evaluate (__LINE__, __func__, {}, assignees);
  }
  // Bible
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    request.session_logic()->setUsername ("PHPUnit");
    int identifier = database_notes.storeNewNote ("PHPUnit", 0, 0, 0, "Summary", "Contents", false);
    string bible = database_notes.getBible (identifier);
    evaluate (__LINE__, __func__, "PHPUnit", bible);
    database_notes.setBible (identifier, "PHPUnit2");
    bible = database_notes.getBible (identifier);
    evaluate (__LINE__, __func__, "PHPUnit2", bible);
    database_notes.setBible (identifier, "");
    bible = database_notes.getBible (identifier);
    evaluate (__LINE__, __func__, "", bible);
  }
  // Passage.
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    request.session_logic()->setUsername ("PHPUnit");

    // Create note for a certain passage.
    int identifier = database_notes.storeNewNote ("", 10, 9, 8, "Summary", "Contents", false);
    
    // Test the getPassages method.
    vector <Passage> passages = database_notes.getPassages (identifier);
    Passage standard = Passage ("", 10, 9, "8");
    evaluate (__LINE__, __func__, 1, (int)passages.size());
    evaluate (__LINE__, __func__, true, standard.equal (passages [0]));

    // Test the setPassage method.
    standard = Passage ("", 5, 6, "7");
    database_notes.setPassages (identifier, {standard});
    passages = database_notes.getPassages (identifier);
    evaluate (__LINE__, __func__, 1, (int)passages.size());
    evaluate (__LINE__, __func__, true, standard.equal (passages [0]));
  }
  // Status.
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    request.session_logic()->setUsername ("PHPUnit");

    // Create note.
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);

    // Test default status = New.
    string status = database_notes.getStatus (identifier);
    evaluate (__LINE__, __func__, "New", status);
    
    // Test setStatus function.
    database_notes.setStatus (identifier, "xxxxx");
    status = database_notes.getStatus (identifier);
    evaluate (__LINE__, __func__, "xxxxx", status);
    
    // Test the getStatuses function.
    vector <Database_Notes_Text> statuses = database_notes.getPossibleStatuses ();
    vector <string> rawstatuses;
    for (auto & status : statuses) {
      rawstatuses.push_back (status.raw);
    }
    evaluate (__LINE__, __func__, {"xxxxx", "New", "Pending", "In progress", "Done", "Reopened"}, rawstatuses);
  }
  // Severity
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    request.session_logic()->setUsername ("PHPUnit");

    // Create note.
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);

    // Test default severity = Normal.
    string severity = database_notes.getSeverity (identifier);
    evaluate (__LINE__, __func__, "Normal", severity);

    // Test setSeverity.
    database_notes.setRawSeverity (identifier, 0);
    severity = database_notes.getSeverity (identifier);
    evaluate (__LINE__, __func__, "Wish", severity);
    database_notes.setRawSeverity (identifier, 4);
    severity = database_notes.getSeverity (identifier);
    evaluate (__LINE__, __func__, "Major", severity);
    
    // Test getSeverities.
    vector <Database_Notes_Text> severities = database_notes.getPossibleSeverities ();
    vector <string> rawseverities;
    vector <string> localizedseverities;
    for (auto & severity : severities) {
      rawseverities.push_back (severity.raw);
      localizedseverities.push_back (severity.localized);
    }
    evaluate (__LINE__, __func__, {"0", "1", "2", "3", "4", "5"}, rawseverities);
    evaluate (__LINE__, __func__, {"Wish", "Minor", "Normal", "Important", "Major", "Critical"}, localizedseverities);
  }
  // Modified
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    request.session_logic()->setUsername ("PHPUnit");
    int time = filter_date_seconds_since_epoch ();

    // Create note.
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    
    // Test getModified.
    int value = database_notes.getModified (identifier);
    if ((value < time) || (value > time + 1)) evaluate (__LINE__, __func__, time, value);
    // Test setModified.
    time = 123456789;
    database_notes.setModified (identifier, time);
    value = database_notes.getModified (identifier);
    evaluate (__LINE__, __func__, time, value);;
  }
  // GetIdentifiers
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    request.session_logic()->setUsername ("phpunit");
    
    // Create a few notes.
    vector <int> standardids;
    for (unsigned int i = 0; i < 3; i++) {
      int identifier = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
      standardids.push_back (identifier);
    }
    
    // Get the identifiers.
    vector <int> identifiers = database_notes.getIdentifiers ();
    sort (standardids.begin(), standardids.end());
    sort (identifiers.begin(), identifiers.end());
    evaluate (__LINE__, __func__, standardids, identifiers);
  }
  // SetIdentifier
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    // Create note.
    request.session_logic()->setUsername ("phpunit");
    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    
    // Contents of the note.
    string originalContents = database_notes.getContents (identifier);
    if (originalContents.length () <= 20) evaluate (__LINE__, __func__, "Should be greater than 20", convert_to_string ((int)originalContents.length ()));
    
    // Checksum of the note.
    string originalChecksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, 32, (int)originalChecksum.length());
    
    // Change the identifier.
    int newId = 1234567;
    database_notes.setIdentifier (identifier, newId);
    
    // Check old and new identifier.
    string contents = database_notes.getContents (identifier);
    evaluate (__LINE__, __func__, "", contents);
    contents = database_notes.getContents (newId);
    evaluate (__LINE__, __func__, originalContents, contents);

    string checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    checksum = database_notes.getChecksum (newId);
    evaluate (__LINE__, __func__, originalChecksum, checksum);
  }
  // ExpireOneNote
  {
    // It tests whether a note marked for deletion,
    // after touching it 7 or 8 times, is returned as due for deletion,
    // and whether it is not yet due for deletion before that.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    database_notes.markForDeletion (identifier);
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    vector <int> identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {}, identifiers);
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {}, identifiers);
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {identifier}, identifiers);
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {identifier}, identifiers);
  }
  // ExpireUnmarkNote.
  {
    // It tests whethe a note marked for deletion,
    // touched 6 times, then unmarked, touched again,
    // will not be due for deletion.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    int identifier = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    database_notes.markForDeletion (identifier);
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.unmarkForDeletion (identifier);
    vector <int> identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {}, identifiers);
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {}, identifiers);
  }
  // ExpireMoreNotes.
  {
    // It tests whether three notes, marked for deletion on different days,
    // are properly touched so they keep their own number of days.
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    int identifier1 = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    int identifier2 = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    int identifier3 = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    database_notes.markForDeletion (identifier1);
    database_notes.touchMarkedForDeletion ();
    database_notes.markForDeletion (identifier2);
    database_notes.touchMarkedForDeletion ();
    database_notes.markForDeletion (identifier3);
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    database_notes.touchMarkedForDeletion ();
    vector <int> identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {}, identifiers);
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {identifier1}, identifiers);
    database_notes.unmarkForDeletion (identifier1);
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {identifier2}, identifiers);
    database_notes.unmarkForDeletion (identifier2);
    database_notes.touchMarkedForDeletion ();
    identifiers = database_notes.getDueForDeletion ();
    evaluate (__LINE__, __func__, {identifier3}, identifiers);
  }  
  // ExpireIsMarked.
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    int identifier1 = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    int identifier2 = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    int identifier3 = database_notes.storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    database_notes.markForDeletion (identifier1);
    evaluate (__LINE__, __func__, true, database_notes.isMarkedForDeletion (identifier1));
    evaluate (__LINE__, __func__, false, database_notes.isMarkedForDeletion (identifier2));
    database_notes.unmarkForDeletion (identifier2);
    evaluate (__LINE__, __func__, true, database_notes.isMarkedForDeletion (identifier1));
    evaluate (__LINE__, __func__, false, database_notes.isMarkedForDeletion (identifier2));
    database_notes.unmarkForDeletion (identifier1);
    evaluate (__LINE__, __func__, false, database_notes.isMarkedForDeletion (identifier1));
    evaluate (__LINE__, __func__, false, database_notes.isMarkedForDeletion (identifier2));
    database_notes.markForDeletion (identifier2);
    evaluate (__LINE__, __func__, true, database_notes.isMarkedForDeletion (identifier2));
    evaluate (__LINE__, __func__, false, database_notes.isMarkedForDeletion (identifier3));
  }
  // ChecksumOne
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    // Create note to work with.
    int identifier = database_notes.storeNewNote ("bible", 1, 2, 3, "summary", "contents", false);

    // Checksum of new note should be calculated.
    string good_checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, good_checksum.empty());

    // Clear checksum, and recalculate it.
    string outdated_checksum = "outdated checksum";
    database_notes.setChecksum (identifier, outdated_checksum);
    string checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, outdated_checksum, checksum);
    database_notes.sync ();
    checksum = database_notes.getChecksum (identifier);
    // Something strange happens:
    // At times the checksum gets erased as the sync routine cannot find the original note.
    // The sync (2) call did not make any difference.
    if (!checksum.empty()) evaluate (__LINE__, __func__, good_checksum, checksum);

    // Test that saving a note updates the checksum in most cases.
    database_notes.setChecksum (identifier, "");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setModified (identifier, 1234567);
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.deleteChecksum (identifier);
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setSubscribers (identifier, {"subscribers"});
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.setChecksum (identifier, "");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setBible (identifier, "phpunit");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.deleteChecksum (identifier);
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setPassages (identifier, {});
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.setChecksum (identifier, "");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setStatus (identifier, "Status");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.deleteChecksum (identifier);
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setRawSeverity (identifier, 123);
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.setChecksum (identifier, "");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setSummary (identifier, "new");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());

    database_notes.deleteChecksum (identifier);
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, "", checksum);
    database_notes.setContents (identifier, "new");
    checksum = database_notes.getChecksum (identifier);
    evaluate (__LINE__, __func__, false, checksum.empty());
  }
  // ChecksumTwo
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    // Create notes to work with.
    vector <int> identifiers;
    identifiers.push_back (database_notes.storeNewNote ("bible1", 1, 2, 3, "summary1", "contents1", false));
    identifiers.push_back (database_notes.storeNewNote ("bible2", 2, 3, 4, "summary2", "contents2", false));
    identifiers.push_back (database_notes.storeNewNote ("bible3", 3, 4, 5, "summary3", "contents3", false));

    // Checksum calculation: slow and fast methods should be the same.
    Sync_Logic sync_logic = Sync_Logic (&request);
    string checksum1 = sync_logic.checksum (identifiers);
    evaluate (__LINE__, __func__, 32, (int)checksum1.length());
    string checksum2 = database_notes.getMultipleChecksum (identifiers);
    evaluate (__LINE__, __func__, checksum1, checksum2);
  }
  // GetNotesInRangeForBibles ()
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    // Create a couple of notes to work with.
    int identifier = database_notes.storeNewNote ("bible1", 1, 2, 3, "summary", "contents", false);
    int identifier1 = 100000000;
    database_notes.setIdentifier (identifier, identifier1);

    identifier = database_notes.storeNewNote ("bible2", 1, 2, 3, "summary", "contents", false);
    int identifier2 = 500000000;
    database_notes.setIdentifier (identifier, identifier2);

    identifier = database_notes.storeNewNote ("", 1, 2, 3, "summary", "contents", false);
    int identifier3 = 999999999;
    database_notes.setIdentifier (identifier, identifier3);

    // Test selection mechanism for certain Bibles.
    vector <int> identifiers = database_notes.getNotesInRangeForBibles (100000000, 999999999, {"bible1", "bible2"}, false);
    evaluate (__LINE__, __func__, {100000000, 500000000, 999999999}, identifiers);

    identifiers = database_notes.getNotesInRangeForBibles (100000000, 999999999, {"bible1", "bible3"}, false);
    evaluate (__LINE__, __func__, {100000000, 999999999}, identifiers);

    identifiers = database_notes.getNotesInRangeForBibles (100000000, 999999999, {}, false);
    evaluate (__LINE__, __func__, {999999999}, identifiers);

    // Test selection mechanism for any Bible.
    identifiers = database_notes.getNotesInRangeForBibles (100000000, 999999999, {}, true);
    evaluate (__LINE__, __func__, {100000000, 500000000, 999999999}, identifiers);
  }
  // CreateRange
  {
    Webserver_Request request;
    Sync_Logic sync_logic = Sync_Logic (&request);
    
    vector <Sync_Logic_Range> ranges = sync_logic.create_range (100000000, 999999999);
    evaluate (__LINE__, __func__, 10, (int)ranges.size());
    evaluate (__LINE__, __func__, 100000000, ranges[0].low);
    evaluate (__LINE__, __func__, 189999998, ranges[0].high);
    evaluate (__LINE__, __func__, 189999999, ranges[1].low);
    evaluate (__LINE__, __func__, 279999997, ranges[1].high);
    evaluate (__LINE__, __func__, 279999998, ranges[2].low);
    evaluate (__LINE__, __func__, 369999996, ranges[2].high);
    evaluate (__LINE__, __func__, 369999997, ranges[3].low);
    evaluate (__LINE__, __func__, 459999995, ranges[3].high);
    evaluate (__LINE__, __func__, 459999996, ranges[4].low);
    evaluate (__LINE__, __func__, 549999994, ranges[4].high);
    evaluate (__LINE__, __func__, 549999995, ranges[5].low);
    evaluate (__LINE__, __func__, 639999993, ranges[5].high);
    evaluate (__LINE__, __func__, 639999994, ranges[6].low);
    evaluate (__LINE__, __func__, 729999992, ranges[6].high);
    evaluate (__LINE__, __func__, 729999993, ranges[7].low);
    evaluate (__LINE__, __func__, 819999991, ranges[7].high);
    evaluate (__LINE__, __func__, 819999992, ranges[8].low);
    evaluate (__LINE__, __func__, 909999990, ranges[8].high);
    evaluate (__LINE__, __func__, 909999991, ranges[9].low);
    evaluate (__LINE__, __func__, 999999999, ranges[9].high);

    ranges = sync_logic.create_range (100000000, 100000100);
    evaluate (__LINE__, __func__, 10, (int)ranges.size());
    evaluate (__LINE__, __func__, 100000000, ranges[0].low);
    evaluate (__LINE__, __func__, 100000009, ranges[0].high);
    evaluate (__LINE__, __func__, 100000010, ranges[1].low);
    evaluate (__LINE__, __func__, 100000019, ranges[1].high);
    evaluate (__LINE__, __func__, 100000020, ranges[2].low);
    evaluate (__LINE__, __func__, 100000029, ranges[2].high);
    evaluate (__LINE__, __func__, 100000030, ranges[3].low);
    evaluate (__LINE__, __func__, 100000039, ranges[3].high);
    evaluate (__LINE__, __func__, 100000040, ranges[4].low);
    evaluate (__LINE__, __func__, 100000049, ranges[4].high);
    evaluate (__LINE__, __func__, 100000050, ranges[5].low);
    evaluate (__LINE__, __func__, 100000059, ranges[5].high);
    evaluate (__LINE__, __func__, 100000060, ranges[6].low);
    evaluate (__LINE__, __func__, 100000069, ranges[6].high);
    evaluate (__LINE__, __func__, 100000070, ranges[7].low);
    evaluate (__LINE__, __func__, 100000079, ranges[7].high);
    evaluate (__LINE__, __func__, 100000080, ranges[8].low);
    evaluate (__LINE__, __func__, 100000089, ranges[8].high);
    evaluate (__LINE__, __func__, 100000090, ranges[9].low);
    evaluate (__LINE__, __func__, 100000100, ranges[9].high);
  }
  // SelectBible
  {
    refresh_sandbox (true);
    Database_State::create ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    // Create a couple of notes to work with.
    int identifier1 = database_notes.storeNewNote ("bible1", 1, 2, 3, "summary1", "contents1", false);
    int identifier2 = database_notes.storeNewNote ("bible2", 1, 2, 3, "summary2", "contents2", false);
    int identifier3 = database_notes.storeNewNote ("bible3", 1, 2, 3, "summary3", "contents3", false);

    // Select notes while varying Bible selection.
    vector <int> identifiers = database_notes.selectNotes ({"bible1"}, 0, 0, 0, 3, 0, 0, "", "bible1", "", false, -1, 0, "", -1);
    evaluate (__LINE__, __func__, {identifier1}, identifiers);

    identifiers = database_notes.selectNotes ({"bible1", "bible2"}, 0, 0, 0, 3, 0, 0, "", "bible2", "", false, -1, 0, "", -1);
    evaluate (__LINE__, __func__, {identifier2}, identifiers);

    identifiers = database_notes.selectNotes ({"bible1", "bible2"}, 0, 0, 0, 3, 0, 0, "", "", "", false, -1, 0, "", -1);
    evaluate (__LINE__, __func__, {identifier1, identifier2}, identifiers);

    identifiers = database_notes.selectNotes ({"bible1", "bible2", "bible4"}, 0, 0, 0, 3, 0, 0, "", "bible", "", false, -1, 0, "", -1);
    evaluate (__LINE__, __func__, {}, identifiers);

    identifiers = database_notes.selectNotes ({}, 0, 0, 0, 3, 0, 0, "", "", "", "", -1, 0, "", -1);
    evaluate (__LINE__, __func__, {}, identifiers);

    identifiers = database_notes.selectNotes ({"bible1", "bible2", "bible3"}, 0, 0, 0, 3, 0, 0, "", "bible3", "", false, -1, 0, "", -1);
    evaluate (__LINE__, __func__, {identifier3}, identifiers);

    identifiers = database_notes.selectNotes ({}, 0, 0, 0, 3, 0, 0, "", "bible3", "", false, -1, 0, "", -1);
    evaluate (__LINE__, __func__, {identifier3}, identifiers);
  }
  // ResilienceNotes.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    bool healthy = database_notes.healthy ();
    evaluate (__LINE__, __func__, true, healthy);
    
    string corrupted_database = filter_url_create_root_path ("unittests", "tests", "notes.sqlite.damaged");
    string path = database_notes.database_path ();
    filter_url_file_put_contents (path, filter_url_file_get_contents (corrupted_database));
    
    healthy = database_notes.healthy ();
    evaluate (__LINE__, __func__, false, healthy);
    
    database_notes.checkup ();
    healthy = database_notes.healthy ();
    evaluate (__LINE__, __func__, true, healthy);
  }
  // ResilienceChecksumsNotes.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();

    bool healthy = database_notes.checksums_healthy ();
    evaluate (__LINE__, __func__, true, healthy);

    string corrupted_database = filter_url_create_root_path ("unittests", "tests", "notes.sqlite.damaged");
    string path = database_notes.checksums_database_path ();
    filter_url_file_put_contents (path, filter_url_file_get_contents (corrupted_database));

    healthy = database_notes.checksums_healthy ();
    evaluate (__LINE__, __func__, false, healthy);
    
    database_notes.checkup_checksums ();
    healthy = database_notes.checksums_healthy ();
    evaluate (__LINE__, __func__, true, healthy);
  }
  // Availability.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request;
    Database_Notes database_notes = Database_Notes (&request);
    database_notes.create ();
    evaluate (__LINE__, __func__, true, database_notes.available ());
    database_notes.set_availability (false);
    evaluate (__LINE__, __func__, false, database_notes.available ());
    database_notes.set_availability (true);
    evaluate (__LINE__, __func__, true, database_notes.available ());
  }
}


void test_database_volatile ()
{
  // Keys.
  {
    Database_Volatile database_volatile = Database_Volatile ();
    database_volatile.create ();
    
    // There should be no keys initially.
    vector <string> keys = database_volatile.getKeys (1);
    evaluate (__LINE__, __func__, {}, keys);

    // Store two keys/values.
    // There should be two keys now.
    database_volatile.setValue (1, "key1", "value");
    database_volatile.setValue (1, "key2", "value");
    keys = database_volatile.getKeys (1);
    evaluate (__LINE__, __func__, {"key1", "key2"}, keys);

    // A different identifier should have no keys.
    keys = database_volatile.getKeys (2);
    evaluate (__LINE__, __func__, {}, keys);
  }
  // Values.
  {
    Database_Volatile database_volatile = Database_Volatile ();
    database_volatile.create ();
    
    // No value initially.
    string value = database_volatile.getValue (1, "key");
    evaluate (__LINE__, __func__, "", value);
    
    // Store value and retrieve it.
    database_volatile.setValue (2, "key2", "value2");
    value = database_volatile.getValue (2, "key2");
    evaluate (__LINE__, __func__, "value2", value);
    
    // Another key should retrieve nothing.
    value = database_volatile.getValue (2, "key1");
    evaluate (__LINE__, __func__, "", value);
  }
}


void test_database_state ()
{
  refresh_sandbox (true);
  Database_State::create ();
  // Test notes checksums.
  {
    // No checksum yet.
    evaluate (__LINE__, __func__, "",  Database_State::getNotesChecksum (100, 1000));
    
    // Store and retrieve checksum in a defined range.
    Database_State::putNotesChecksum (100, 1000, "phpunit");
    evaluate (__LINE__, __func__, "phpunit",  Database_State::getNotesChecksum (100, 1000));
    // Store it again, with a different value.
    Database_State::putNotesChecksum (100, 1000, "phpunit2");
    evaluate (__LINE__, __func__, "phpunit2",  Database_State::getNotesChecksum (100, 1000));
    
    // Erase a note within the defined range, which should erase that range.
    Database_State::eraseNoteChecksum (100);
    evaluate (__LINE__, __func__, "",  Database_State::getNotesChecksum (100, 1000));
    
    // Define a few ranges, store checksums, and erase one note within that range, and test it.
    Database_State::putNotesChecksum (100, 1000, "100-1000");
    Database_State::putNotesChecksum (200, 1100, "200-1100");
    Database_State::putNotesChecksum (300, 900,  "300-900");
    Database_State::putNotesChecksum (2000, 9000, "2000-9000");
    evaluate (__LINE__, __func__, "100-1000",   Database_State::getNotesChecksum (100,  1000));
    evaluate (__LINE__, __func__, "200-1100",   Database_State::getNotesChecksum (200,  1100));
    evaluate (__LINE__, __func__, "300-900",    Database_State::getNotesChecksum (300,  900));
    evaluate (__LINE__, __func__, "2000-9000",  Database_State::getNotesChecksum (2000, 9000));
    Database_State::eraseNoteChecksum (500);
    evaluate (__LINE__, __func__, "",   Database_State::getNotesChecksum (100,  1000));
    evaluate (__LINE__, __func__, "",   Database_State::getNotesChecksum (200,  1100));
    evaluate (__LINE__, __func__, "",    Database_State::getNotesChecksum (300,  900));
    evaluate (__LINE__, __func__, "2000-9000",  Database_State::getNotesChecksum (2000, 9000));
  }
  // Test export flagging.
  {
    // Flag some data for export.
    Database_State::setExport ("1", 2, 3);
    Database_State::setExport ("4", 5, 6);
    Database_State::setExport ("7", 8, 9);

    // Test the data.
    evaluate (__LINE__, __func__, true,  Database_State::getExport ("1", 2, 3));
    evaluate (__LINE__, __func__, true,  Database_State::getExport ("4", 5, 6));
    evaluate (__LINE__, __func__, false,  Database_State::getExport ("1", 2, 1));
    
    // Clear flag.
    Database_State::clearExport ("1", 2, 3);
    
    // Test the data.
    evaluate (__LINE__, __func__, false,  Database_State::getExport ("1", 2, 3));
    evaluate (__LINE__, __func__, true,  Database_State::getExport ("4", 5, 6));
    evaluate (__LINE__, __func__, false,  Database_State::getExport ("1", 2, 1));
  }
  // Test states of being exported.
  {
    // Not yet exported by default.
    evaluate (__LINE__, __func__, false,  Database_State::getExported ("bible", 1));

    // Set and test some exports.
    Database_State::setExported ("1", 1);
    Database_State::setExported ("2", 2);
    Database_State::setExported ("3", 3);
    evaluate (__LINE__, __func__, true,  Database_State::getExported ("1", 1));
    evaluate (__LINE__, __func__, true,  Database_State::getExported ("2", 2));
    evaluate (__LINE__, __func__, true,  Database_State::getExported ("3", 3));
    evaluate (__LINE__, __func__, false,  Database_State::getExported ("1", 2));

    // Clear some.
    Database_State::clearExported ("1", 1);
    evaluate (__LINE__, __func__, false,  Database_State::getExported ("1", 1));
    evaluate (__LINE__, __func__, true,  Database_State::getExported ("2", 2));
    evaluate (__LINE__, __func__, true,  Database_State::getExported ("3", 3));

    // Clear entire Bible.
    Database_State::clearExported ("2", 0);
    evaluate (__LINE__, __func__, false,  Database_State::getExported ("2", 2));
    evaluate (__LINE__, __func__, true,  Database_State::getExported ("3", 3));
  }
}


void test_database_imageresources () // Todo
{
  Database_ImageResources database_imageresources;
  // Empty
  {
    refresh_sandbox (true);
    vector <string> resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 0, resources.size());
  }
  // Create, names, erase.
  {
    refresh_sandbox (true);

    database_imageresources.create ("unittest");
    vector <string> resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 1, resources.size());
    bool hit = false;
    for (auto & resource : resources) if (resource == "unittest") hit = true;
    evaluate (__LINE__, __func__, true, hit);
    
    database_imageresources.erase ("none-existing");
    resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 1, resources.size());

    database_imageresources.erase ("unittest");
    resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 0, resources.size());
}
/* Todo
  // Store Get Chapter
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    string usfm = database_usfmresources.getUsfm ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, "usfm", usfm);
    usfm = database_usfmresources.getUsfm ("bibledit", 2, 4);
    evaluate (__LINE__, __func__, "", usfm);
  }
  // Books
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    database_usfmresources.storeChapter ("bibledit", 3, 4, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 1, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 2, "usfm");
    vector <int> books = database_usfmresources.getBooks ("bibledit");
    evaluate (__LINE__, __func__, {1, 2, 3}, books);
  }
  // Chapters
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    database_usfmresources.storeChapter ("bibledit", 3, 4, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 1, "usfm");
    database_usfmresources.storeChapter ("bibledit", 1, 2, "usfm");
    vector <int> chapters = database_usfmresources.getChapters ("bibledit", 1);
    evaluate (__LINE__, __func__, {1, 2}, chapters);
    chapters = database_usfmresources.getChapters ("bibledit", 2);
    evaluate (__LINE__, __func__, {3}, chapters);
  }
  // Sizes
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    
    int size = database_usfmresources.getSize ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, 0, size);
    
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    size = database_usfmresources.getSize ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, 4, size);
    
    database_usfmresources.storeChapter ("bibledit", 2, 3, "chapter");
    size = database_usfmresources.getSize ("bibledit", 2, 3);
    evaluate (__LINE__, __func__, 7, size);
  }
  // Delete Book
  {
    refresh_sandbox (true);
    Database_UsfmResources database_usfmresources = Database_UsfmResources ();
    database_usfmresources.storeChapter ("bibledit", 2, 3, "usfm");
    vector <int> books = database_usfmresources.getBooks ("bibledit");
    evaluate (__LINE__, __func__, {2}, books);
    database_usfmresources.deleteBook ("bibledit", 2);
    books = database_usfmresources.getBooks ("bibledit");
    evaluate (__LINE__, __func__, {}, books);
  }
*/
}


#endif
