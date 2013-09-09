<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
?>
<h1><?php echo gettext ("Exports") ?></h1>
<p><?php echo gettext ("Bibledit-Web exports the Bibles every day at midnight.") ?></p>
<br>
<p><?php echo gettext ("The following Bibles will be exported") ?>:
<?php foreach ($this->bibles as $bible) { ?>
  <?php echo $bible ?>
  <a href="exports.php?removebible=<?php echo $bible ?>">[<?php echo gettext ("remove") ?>]</a>
  |
<?php } ?>
<a href="exports.php?addbible=">[<?php echo gettext ("add Bible") ?>]</a>
</p>
<br>
<p><a href="exports.php?generate="><?php echo gettext ("Do a fresh export now.") ?></a></p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<br>
<p><a href="../downloads/exports"><?php echo gettext ("View the exported data in the downloads section.") ?></a></p>
<br>
<p><?php echo gettext ("The exports use the following stylesheet:") ?> <?php echo $this->stylesheet ?> [<a href="exports.php?sheet="><?php echo gettext ("switch") ?></a>].</p>
<br>
<p>
<a href="exports.php?dropcapstoggle="><?php if ($this->dropcaps) { ?> ☑ <?php } else { ?> ☐ <?php } ?></a>
<?php echo gettext ("Use frames for OpenDocument format in case the chapter number gets put just before the first verse in drop caps.") ?>  
</p>
<br>
<p><?php echo gettext ("Page size and margins are relevant for export to a format that supports those.") ?></p>
<table>
  <tr><td><?php echo gettext ("Page width") ?></td><td><a href="exports.php?pagewidth="><?php echo $this->pagewidth ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Page height") ?></td><td><a href="exports.php?pageheight="><?php echo $this->pageheight ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Inner margin") ?></td><td><a href="exports.php?innermargin="><?php echo $this->innermargin ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Outer margin") ?></td><td><a href="exports.php?outermargin="><?php echo $this->outermargin ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Top margin") ?></td><td><a href="exports.php?topmargin="><?php echo $this->topmargin ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Bottom margin") ?></td><td><a href="exports.php?bottommargin="><?php echo $this->bottommargin ?> <?php echo gettext ("mm") ?></a></td></tr>
</table>
<br>
<p>
<a href="exports.php?dateinheadertoggle="><?php if ($this->dateinheader) { ?> ☑ <?php } else { ?> ☐ <?php } ?></a>
<?php echo gettext ("Display the current date in the running headers.") ?>
</p>
<br>
<p>
 <?php echo gettext ("It also exports to a file suitable for the Online Bible module creation process.") ?>
 <?php echo gettext ("For more information about how to create a module, see Online Bible, menu Help - Table Of Contents - How To - Create Your Own Bible Version.") ?>
</p>
<br>
<p>
 <?php echo gettext ("You can edit the configuration file used for exporting to a Sword module.") ?>
 <a href="http://www.crosswire.org/sword/develop/swordmodule" target="_blank"><?php echo gettext ("More information.") ?></a>
 <?php echo gettext ("Update the text below, and click button Save.") ?>
</p>
<p><?php echo gettext ("There are two special variables:") ?></p>
<p>&lt;bible&gt;: <?php echo gettext ("This will be replaced with the name of the Bible being exported.") ?></p>
<p>&lt;date&gt;: <?php echo gettext ("This will be replaced with the date of export.") ?></p>
<form action="exports.php" name="formswordconfig" method="post">
  <p><textarea name="swordconfig" class="fullwidth"><?php echo $this->swordconfig ?></textarea></p>
  <p><input type="submit" name="submitswordconfig" value=<?php echo gettext ("Save") ?> /> <a href="exports.php?resetswordconfig="><?php echo gettext ("Reset") ?></a></p>
</form>
