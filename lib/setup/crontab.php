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


error_reporting (E_ALL);


require_once ("../bootstrap/bootstrap.php");


$messages = array ();


// The entries to go into cron.
$timerFolder = realpath ("../timer");
$timerSignature = "* * * * * cd $timerFolder; php index.php; cd";

$tasksFolder = realpath ("../tasks");
$tasksSignature = "* * * * * php $tasksFolder/run.php";


// Get current crontab contents.
$result = array ();
exec ("crontab -l 2>&1", $result, $exitcode);
// Errors may be caused by an empty crontab.
if ($exitcode != 0) {
  $result = array ();
}


// If any of the entries are not there yet, add them.
$timerIncluded = false;
$tasksIncluded = false;
foreach ($result as $line) {
  if ($line == $timerSignature) $timerIncluded = true;
  if ($line == $tasksSignature) $tasksIncluded = true;
}
if (!$timerIncluded || !$tasksIncluded) {
  if (!$timerIncluded) $result [] = $timerSignature;
  if (!$tasksIncluded) $result [] = $tasksSignature;
  $content = implode ("\n", $result) . "\n";
  $filename = tempnam (sys_get_temp_dir (), '');
  file_put_contents ($filename, $content);
  $result = array ();
  exec ("crontab $filename 2>&1", $result, $exitcode);
  if ($exitcode != 0) {
    $messages [] = "Crontab ran with an exitcode of $exitcode, while 0 would be expected. Therefore the scheduled task for Bibledit-Web may not have been set properly.";
    foreach ($result as $line) {
      $messages [] = "Error: " . $line;
    }
  }
  unlink ($filename);
}


if (count ($messages) == 0) {
  include ("../filter/url.php");
  Filter_Url::redirect ("step.php?referer=crontab");
  die;
}


// For Windows:
$webuser = get_current_user ();
// For Linux:
if (function_exists ("posix_getpwuid")) {
  $webuser = posix_getpwuid (posix_geteuid ());
  $webuser = $webuser ['name'];
}


?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Bibledit-Web Installation</title>
<link rel="stylesheet" href="stylesheet.css" type="text/css" />
</head>
<body>
<h1><img src="../assets/bibledit.png"> <a href="http://bibledit.org/">Bibledit-Web</a></h1>
<p>Bibledit-Web tried to configure the timer for running scheduled tasks through program <code>crontab</code>, but failed to do so.</p>
<p>More information is below.</p>
<ul>
<?php
foreach ($messages as $message) {
  echo "<li>$message</li>\n";
}
?>
</ul>
<p>May you ensure that the following two scheduled tasks are set properly in the <code>cron</code> scheduler for user <code><?php echo $webuser; ?></code>:</p>
<p><code><?php echo $timerSignature; ?></code></p>
<p><code><?php echo $tasksSignature; ?></code></p>
<p>If the <code>cron</code> scheduler does not work for user <code><?php echo $webuser; ?></code>, it is also an option to schedule the tasks as user <code>root</code>. Run <code>crontab -e</code> as user <code>root</code>, and enter the following line given above.</p>
<p>Your hosting provider will be able to advise also.</p>
<p><a href="crontab.php" class="button button-large">Retry</a> <a href="step.php?referer=crontab" class="button button-large">Carry on</a></p>
</body>
</html>
