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


$inputData = "";
if (isset($_POST['submit'])) {
  $inputData = $_POST['data'];
  $inputData = trim ($inputData);
}
$outputData = $inputData;


?>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
  <title>BibleWorks ⇨ eSword</title>
  <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
</head>
<body>
<h2>BibleWorks ⇨ eSword</h2>
<p><a href="index.php">Go back</a></p>
<h3>Introduction</h3>
<p>This page converts a text suitable for the BibleWorks Version Database Compiler to text suitable for importing into eSword.</p>
<p>The text file may have Strong codifications, italics, cross references and footnotes.</p>
<p>The <i>italics</i> are coded between square brackets: [and] means <i>and</i>.</p>
<h3>Step 1: Supply input text</h3>
<p>Paste some BibleWorks text into the text box, and then press the Submit button below.</p>
<form action="bibleworks2esword.php" name="form" method="post">
  <p><textarea name="data"><?php echo $inputData; ?></textarea></p>
  <p><input type="submit" name="submit" value="Submit" /></p>
</form>
<h3>Step 2: Use output text</h3>
<p>Select the converted text from the text box below and copy it to the clipboard.</p>
<p><textarea name="outputdata"><?php echo $outputData; ?></textarea></p>


<?php
?>


</body>
</html>
