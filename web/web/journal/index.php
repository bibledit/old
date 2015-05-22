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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$database_logs = Database_Logs::getInstance();
$database_config_general = Database_Config_General::getInstance ();


$session_logic = Session_Logic::getInstance ();
$userLevel = $session_logic->currentLevel ();


$timezone = $database_config_general->getTimezone ();
if ($timezone) date_default_timezone_set ($timezone);


// Deal with AJAX call for a possible new journal entry.
@$filename = $_GET['filename'];
if (isset ($filename)) {
  $result = $database_logs->getNext ($filename);
  if ($result) {
    $output = array ();
    $output [] = $result [0];
    $line = $result [1];
    $entryLevel = (integer) $line;
    if ($entryLevel <= $userLevel) 
    {
      $output [] = render_journal_entry ($line);
    };
    echo implode ("\n", $output);
  }
  die;
}


// Check the watchdog for the minutely tasks.
$message = "";
$watchdog = dirname (__DIR__) . "/config/watchdog";
if (file_exists ($watchdog)) {
  $current_time = time ();
  $watchdog_time = filemtime ($watchdog);
  $time = abs ($current_time - $watchdog_time);
  if ($time > 180) {
    $message = Locale_Translate::_("The minutely jobs have not run lately.");
  }
} else {
  $message = Locale_Translate::_("The minutely jobs do not run.");
}
if ($message) $database_logs->log (Locale_Translate::_("Warning") . ": " . $message);
unset ($message);
unset ($watchdog);


$header = new Assets_Header (Locale_Translate::_("Journal"));
$header->run ();


$view = new Assets_View (__FILE__);


$day = 0;
$today = strtotime ("today");
$date = $today - ($day * 86400);


$data = $database_logs->get ($day);
$filename = $data [0];
$entries = $data [1];


// Show no more than so many entries.
$limit = 1000;
$count = count ($entries);
if ($count > $limit) {
  $shorter = array ();
  for ($i = $count - $limit; $i < $count; $i++) {
    $shorter [] = $entries [$i];
  }
  $entries = $shorter;
  unset ($shorter);
}


$lines = array ();
foreach ($entries as $entry) {
  $entryLevel = (integer) $entry;
  if ($entryLevel > $userLevel) continue;
  $lines [] = render_journal_entry ($entry);
}
$view->view->lines = $lines;


// Pass the filename of the most recent entry to javascript for use by the AJAX calls for getting subsequent journal entries.
$script = <<<EOD
var filename = $filename;
EOD;
$view->view->script = $script;


$view->render ("index.php");
Assets_Page::footer ();


function render_journal_entry ($entry)
{
  $entry = substr ($entry, 2);
  $seconds = (integer) $entry;
  $entry = substr ($entry, 10);
  $timestamp = date ('g:i:s a', $seconds);
  $event = Filter_Html::sanitize ($entry);
  return "$timestamp | $event";
}


?>
