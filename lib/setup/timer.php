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
require ("cron.php");
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
<p>Bibledit-Web needs a timer for running scheduled tasks through program <code>crontab</code>.</p>
<p>The following two scheduled tasks needs to be set in the <code>cron</code> scheduler with read and write access to user <code><?php echo $webuser; ?></code>:</p>
<p><code><?php echo $timerSignature; ?></code></p>
<p><code><?php echo $tasksSignature; ?></code></p>
<p>Some hosting providers prefer not to have scheduled tasks on their web server. Install it manually in such cases. Run <code>crontab -e</code> as user <code>root</code>, and enter the two lines given above.</p>
<p><a href="step.php?referer=crontab" class="button button-large">I have installed it manually, carry on</a> <a href="step.php?referer=timer" class="button button-large">Install it for me on the web server</a></p>
</body>
</html>
