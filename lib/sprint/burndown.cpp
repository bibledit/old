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


#include <sprint/burndown.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/date.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <locale/logic.h>
#include <database/sprint.h>
#include <database/logs.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <access/bible.h>


// This function runs the sprint burndown history logger for $bible.
// If no $bible is passed, it will do all Bibles.
// If $mail is true, it will mail the burndown chart to the subscribed users.
// If $mail is false, it decides on its own whether to mail the chart to the users.
void sprint_burndown (string bible, bool email)
{
  int localseconds = filter_date_local_seconds (filter_date_seconds_since_epoch ());
  int year = filter_date_numerical_year (localseconds);
  int month = filter_date_numerical_month (localseconds);
  int monthday = filter_date_numerical_month_day (localseconds); // 1 to 31.
  int weekday = filter_date_numerical_week_day (localseconds); // 0 (for Sunday) through 6 (for Saturday).
  int hour = filter_date_numerical_hour (localseconds);
  bool sprintstart = false;
  bool sprintfinish = false;
  

  // Every Friday at 2 PM (14:00h) it sends email about the sprint progress.
  if ((weekday == 5) && (hour == 14)) email = true;
  // On the first business day of the month, at 10 AM, send email about the start of the sprint.
  // Todo oif (filter_date_is_first_working_day_of_month ($monthday, $weekday) && ($hour == 10)) {
    email = true;
    sprintstart = true;
  // }
  // On the last business day of the month, at 2 PM (14:00h), send email about the end of the sprint.
  // Todo if (($monthday == filter_date_get_last_business_day_of_month ($year, $month)) && ($hour == 14)) {
    email = true;
    sprintfinish = true;
  //}
  
  
  // Determine what to do, or to quit.
  if (!email && !sprintstart && !sprintfinish) {
    if (hour != 1) return;
  }
  

  Database_Logs::log ("Updating Sprint information", Filter_Roles::manager ());

  
  /* Todo
  
  
  $database_config_bible = Database_Config_Bible::getInstance ();
  $database_config_user = Database_Config_User::getInstance ();
  $database_users = Database_Users::getInstance ();
  $database_mail = Database_Mail::getInstance ();
  $database_sprint = Database_Sprint::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  
  
  
  
  // Determine year / month / day of the current sprint.
  // If this script runs from midnight till early morning,
  // it applies to the day before.
  // If the script runs during the day, it applies to today.
  $year = date ("Y");
  $month = date ("n");
  $day = date ("j");
  $hour = date ("G");
  $time = mktime ($hour - 6, 0, 0, $month, $day, $year);
  $year = date ("Y", $time);
  $month = date ("n", $time);
  $day = date ("j", $time);
  
  
  $bibles = array ($bible);
  if ($bible == "") {
    $bibles = request->database_bibles()->getBibles ();
  }
  
  
  for ($bibles as $bible) {
    
    
    // Get the total number of tasks for this sprint,
    // and the average percentage of completion of them,
    // and store this information in the sprint history table.
    $ids = $database_sprint->getTasks ($bible, $year, $month);
    $percentages = array ();
    for ($ids as $id) {
      $percentages [] = $database_sprint->getComplete ($id);
    }
    $tasks = count ($ids);
    if ($tasks == 0) {
      $complete = 0;
    } else {
      $complete = array_sum ($percentages) / $tasks;
      $complete = intval ($complete);
    }
    $database_sprint->logHistory ($bible, $year, $month, $day, $tasks, $complete);
    
    
    // Send email if requested.
    if ($email) {
      if ($tasks) {
        // Only mail if the current sprint contains tasks.
        $categories = Database_Config_Bible::getSprintTaskCompletionCategories ($bible);
        $categories = explode ("\n", $categories);
        $category_count = count ($categories);
        $category_percentage = intval (100 / $category_count);
        $users = request->database_users ()->getUsers ();
        for ($users as $user) {
          if (!access_bible_write ($bible, $user)) continue;
          if (request->database_config_user()->getUserSprintProgressNotification ($user)) {
            
            $subject = translate("Team's progress in Sprint");
            if ($sprintstart) $subject = translate("Sprint has started");
            if ($sprintfinish) $subject = translate("Sprint has finished");
            $subject +=  " | " . $bible;
            
            $body = array ();
            
            $body [] = "<h3>" . translate("Sprint Planning and Team's Progress") . " | $bible</h3>";
            $body [] = "<table>";
            $tasks = $database_sprint->getTasks ($bible, $year, $month);
            for ($tasks as $id) {
              $body [] = "<tr>";
              $title = $database_sprint->getTitle ($id);
              $body [] = "<td>" . $title . "</td>";
              $complete = $database_sprint->getComplete ($id);
              $text = str_repeat ("▓", intval ($complete / $category_percentage)) . str_repeat ("▁", $category_count - intval ($complete / $category_percentage));
              $body [] = "<td>" . $text . "</td>";
              $body [] = "</tr>";
            }
            $body [] = "</table>";
            
            $body [] = "<h3>" . translate("Sprint Burndown Chart - Remaining Tasks") . "</h3>";
            $burndownchart = Sprint_Logic::createTextBasedBurndownChart ($bible, $year, $month);
            $body [] = "<p>$burndownchart</p>";
            
            if (count ($body) > 0) {
              $body = implode ("\n", $body);
              if (!client_logic_client_enabled ()) $database_mail->send ($user, $subject, $body);
            }
            
          }
        }
      } else {
        
        // Since there are no tasks, no mail will be sent: Make a logbook entry.
        Database_Logs::log ("No tasks in this Sprint: No email was sent");
      }
    }
  }
   */
}


// This function creates a text-based burndown chart for sprint $bible / $year / $month.
string sprint_create_burndown_chart (string bible, int year, int month) // Todo
{
  /* Todo
  // Number of days in the month for on the X-axis.
  $time = mktime (0, 0, 0, $month, 15, $year);
  $days_in_month = date ("t", $time);
  
  // Assemble history of this sprint.
  $database_sprint = Database_Sprint::getInstance ();
  $history = $database_sprint->getHistory ($bible, $year, $month);
  $data = array ();
  for ($day = 1; $day <= $days_in_month; $day++) {
    if (filter_date_is_business_day ($year, $month, $day)) {
      $data [$day] = "";
      for ($history as $item) {
        if ($day == $item ['day']) {
          $tasks = $item ['tasks'];
          $complete = $item ['complete'];
          $tasks = $tasks * (100 - $complete) / 100;
          $tasks = intval ($tasks);
          $data [$day] = $tasks;
        }
      }
    }
  }
  unset ($item);
  
  $lines = array ();
  lines.push_back ('<table style="text-align:center;">';
                   lines.push_back ('<tr style="vertical-align: bottom;">';
                                    for ($data as $day => $tasks) {
                                      $text = str_repeat ("▓<br>", intval ($tasks));
                                      lines.push_back ("<td>$text</td>";
                                                       }
                                                       lines.push_back ("</tr>";
                                                                        
                                                                        // Write number of days along the x-axis.
                                                                        lines.push_back ('<tr>';
                                                                                         for ($data as $day => $tasks) {
                                                                                           lines.push_back ("<td style=\"width:1em\">$day</td>";
                                                                                                            }
                                                                                                            lines.push_back ("</tr>";
                                                                                                                             
                                                                                                                             // Write "days" below the x-axis.
                                                                                                                             lines.push_back ('<tr>';
                                                                                                                                              $columncount = count ($data);
                                                                                                                                              $text = translate("days");
                                                                                                                                              lines.push_back ("<td colspan=\"$columncount\">$text</td>";
                                                                                                                                                               lines.push_back ("</tr>";
                                                                                                                                                                                
                                                                                                                                                                                lines.push_back ("</table>";
                                                                                                                                                                                                 
                                                                                                                                                                                                 $chart = implode ("\n", $lines);
                                                                                                                                                                                                 return $chart;
*/
}

