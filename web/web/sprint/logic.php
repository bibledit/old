<?php
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


require_once ("bootstrap/bootstrap.php");


class Sprint_Logic
{


  // This function runs the sprint burndown history logger for $bible.
  // If no $bible is passed, it will do all Bibles.
  // If $mail is true, it will mail the burndown chart to the subscribed users.
  // If $mail is false, it decides on its own whether to mail the chart to the users.
  static public function burndown ($bible, $mail)
  {
    if ($bible == "") $bible = "-";
    $mail = Filter_Bool::int ($mail);
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/burndown.php", $bible, $mail));
  }


  // This function creates a graphical burndown chart for sprint $year / $month.
  static public function createGraphicalBurndownChart ($bible, $year, $month)
  {
    // Number of days in the month for on the X-axis.
    $time = mktime (0, 0, 0, $month, 15, $year);
    $days_in_month = date ("t", $time);

    // Assemble history of this sprint.
    $database_sprint = Database_Sprint::getInstance ();
    $history = $database_sprint->getHistory ($bible, $year, $month);
    $data = array ();
    for ($day = 1; $day <= $days_in_month; $day++) {
      foreach ($history as $item) {
        if ($day == $item ['day']) {
          $tasks = $item ['tasks'];
          $complete = $item ['complete'];
          $tasks = $tasks * (100 - $complete) / 100;
          $tasks = intval ($tasks);
          $data [$day] = $tasks;
        }
      }
    }

    // Create burndown chart as an image.
    include_once ("../phpmygraph/phpMyGraph5.0.php");

    $cfg['title-visible'] = false;
    $cfg['width'] = 700;
    $cfg['height'] = 250;
    $cfg['zero-line-visible'] = false;
    $cfg['average-line-visible'] = false;
    $cfg['round-value-range'] = true;

    ob_start ();
    $graph = new phpMyGraph();
    $graph->parseVerticalLineGraph ($data, $cfg);
    $image = ob_get_contents ();
    ob_end_clean ();

    return $image;
  }


  // This function creates a text-based burndown chart for sprint $year / $month.
  static public function createTextBasedBurndownChart ($bible, $year, $month)
  {
    // Number of days in the month for on the X-axis.
    $time = mktime (0, 0, 0, $month, 15, $year);
    $days_in_month = date ("t", $time);

    // Assemble history of this sprint.
    $database_sprint = Database_Sprint::getInstance ();
    $history = $database_sprint->getHistory ($bible, $year, $month);
    $data = array ();
    for ($day = 1; $day <= $days_in_month; $day++) {
      if (Filter_Datetime::isBusinessDay ($year, $month, $day)) {
        $data [$day] = "";
        foreach ($history as $item) {
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
    $lines [] = '<table style="text-align:center;">';
    $lines [] = '<tr style="vertical-align: bottom;">';
    foreach ($data as $day => $tasks) {
      $text = str_repeat ("▓<br>", intval ($tasks));
      $lines [] = "<td>$text</td>";
    }
    $lines [] = "</tr>";

    // Write number of days along the x-axis.
    $lines [] = '<tr>';
    foreach ($data as $day => $tasks) {
      $lines [] = "<td style=\"width:1em\">$day</td>";
    }
    $lines [] = "</tr>";

    // Write "days" below the x-axis.
    $lines [] = '<tr>';
    $columncount = count ($data);
    $text = Locale_Translate::_("days");
    $lines [] = "<td colspan=\"$columncount\">$text</td>";
    $lines [] = "</tr>";

    $lines [] = "</table>";

    $chart = implode ("\n", $lines);
    return $chart;
  }


}


?>
