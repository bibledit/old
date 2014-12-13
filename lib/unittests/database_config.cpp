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


#include <unittests/database_config.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <flate/flate.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>


void test_database_config_general ()
{
  // Tests for Database_Config_General.
  evaluate (__LINE__, __func__, "Bible Translation", Database_Config_General::getSiteMailName ());

  string ref = "unittest";
  Database_Config_General::setSiteMailName (ref);
  evaluate (__LINE__, __func__, ref, Database_Config_General::getSiteMailName ());

  evaluate (__LINE__, __func__, "", Database_Config_General::getMailStorageSecurity ());
}


void test_database_config_bible ()
{
  // Tests for Database_Config_Bible.
    evaluate (__LINE__, __func__, "", Database_Config_Bible::getViewableByAllUsers ("testbible"));

    string ref = "1";
    Database_Config_Bible::setViewableByAllUsers ("testbible", ref);
    evaluate (__LINE__, __func__, ref, Database_Config_Bible::getViewableByAllUsers ("testbible"));
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
    request.session_logic ()->attemptLogin ("username", "password");

    // Testing setList, getList, plus add/removeUpdatedSetting.
    vector <string> empty;
    evaluate (__LINE__, __func__, empty, request.database_config_user ()->getUpdatedSettings ());

    vector <string> standard1;
    standard1.push_back ("one two three");
    standard1.push_back ("four five six");
    request.database_config_user ()->setUpdatedSettings (standard1);
    evaluate (__LINE__, __func__, standard1, request.database_config_user ()->getUpdatedSettings ());

    request.database_config_user ()->addUpdatedSetting ("seven eight nine");
    standard1.push_back ("seven eight nine");
    evaluate (__LINE__, __func__, standard1, request.database_config_user ()->getUpdatedSettings ());
    
    request.database_config_user ()->removeUpdatedSetting ("four five six");
    vector <string> standard2;
    standard2.push_back ("one two three");
    standard2.push_back ("seven eight nine");
    evaluate (__LINE__, __func__, standard2, request.database_config_user ()->getUpdatedSettings ());
    
    // Testing the Sprint month and its trim () function.
    // It should get today's month.
    int month = filter_string_date_numerical_month ();
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
    new_times.actime = filter_string_date_seconds_since_epoch () - (2 * 24 * 3600) - 10;
    new_times.modtime = filter_string_date_seconds_since_epoch () - (2 * 24 * 3600) - 10;
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

    evaluate (__LINE__, __func__, filter_string_date_numerical_year (), request.database_config_user ()->getSprintYear ());
    
    // Test getting a Bible that does not exist: It creates one.
    evaluate (__LINE__, __func__, "testBible", request.database_config_user ()->getBible ());
  }
}


