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


$filename = realpath ("..") . "/.htaccess";
$errorpage = $_SERVER["HTTP_REFERER"];
$errorpage = dirname (dirname ($errorpage));
$errorpage = "$errorpage/index/index.php";
$errordocument = "ErrorDocument 404 $errorpage";


$htaccess = <<<EOD
<IfModule env_module>
  SetEnv htaccessWorking true
</IfModule>

$errordocument

<IfModule mod_php5.c>
  php_value upload_max_filesize 512M
  php_value post_max_size 512M
  php_value memory_limit 512M
</IfModule>

EOD;


@$success = file_put_contents ($filename, $htaccess);
if ($success === false) {
  $message = error_get_last ();
  $message = $message ['message'];
  $messages [] = $message;
}


if (count ($messages) == 0) {
  include ("../filter/url.php");
  Filter_Url::redirect ("step.php?referer=apache");
  die;
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
<p>Bibledit-Web tried to configure the web server, but failed to do so.</p>
<p>More information is below.</p>
<ul>
<?php
foreach ($messages as $message) {
  echo "<li>$message</li>\n";
}
?>
</ul>
<p>Your hosting provider will be able to advise what to do with regard to the problems mentioned.</p>
<p><a href="apache.php" class="button button-large">Retry</a></p>
</body>
</html>
