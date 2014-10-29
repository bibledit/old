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
?>
<h1><?php echo Locale_Translate::_("Exports") ?></h1>

<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>

<p>
  <?php echo Locale_Translate::_("Bible") ?>: 
  <a href="?bible=" title="<?php echo Locale_Translate::_("Select another Bible") ?>"><?php echo $this->bible ?></a>
  <a href="../exports" target="_blank" title="<?php echo Locale_Translate::_("View the exported data") ?>">[<?php echo Locale_Translate::_ ("view") ?>]</a>
  <a href="?remove=" title="<?php echo Locale_Translate::_("Remove the exported data") ?>">[<?php echo Locale_Translate::_ ("remove") ?>]</a>
</p>

<br>
<hr>

<p>
  <a href="?webtoggle=">
  <?php if ($this->web == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to Web during the night.") ?>
  <a href="?webnow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>

<br>
<hr>

<p>
  <a href="?htmltoggle=">
  <?php if ($this->html == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to Html during the night.") ?>
  <a href="?htmlnow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>

<br>
<hr>

<p>
  <a href="?usfmtoggle=">
  <?php if ($this->usfm == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to full USFM during the night.") ?>
  <a href="?usfmnow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>
<br>
<p>
  <a href="?usfmsecuretoggle=">
  <?php if ($this->usfmsecure == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Secure the export with the password.") ?>
</p>

<br>
<hr>

<p>
  <a href="?texttoggle=">
  <?php if ($this->text == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to basic USFM and text during the night.") ?>
  <a href="?textnow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>

<br>
<hr>

<p>
  <a href="?odttoggle=">
  <?php if ($this->odt == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to OpenDocument during the night.") ?>
  <a href="?odtnow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>
<br>
<p>
<a href="exports.php?dropcapstoggle="><?php if ($this->dropcaps) { ?> ☑ <?php } else { ?> ☐ <?php } ?></a>
<?php echo Locale_Translate::_("Use frames for OpenDocument format in case the chapter number gets put just before the first verse in drop caps.") ?>
</p>
<br>
<p><?php echo Locale_Translate::_("Page size and margins for export to OpenDocument:") ?></p>
<table>
  <tr><td><?php echo Locale_Translate::_("Page width") ?></td><td><a href="exports.php?pagewidth="><?php echo $this->pagewidth ?> <?php echo Locale_Translate::_ ("mm") ?></a></td></tr>
  <tr><td><?php echo Locale_Translate::_("Page height") ?></td><td><a href="exports.php?pageheight="><?php echo $this->pageheight ?> <?php echo Locale_Translate::_ ("mm") ?></a></td></tr>
  <tr><td><?php echo Locale_Translate::_("Inner margin") ?></td><td><a href="exports.php?innermargin="><?php echo $this->innermargin ?> <?php echo Locale_Translate::_ ("mm") ?></a></td></tr>
  <tr><td><?php echo Locale_Translate::_("Outer margin") ?></td><td><a href="exports.php?outermargin="><?php echo $this->outermargin ?> <?php echo Locale_Translate::_ ("mm") ?></a></td></tr>
  <tr><td><?php echo Locale_Translate::_("Top margin") ?></td><td><a href="exports.php?topmargin="><?php echo $this->topmargin ?> <?php echo Locale_Translate::_ ("mm") ?></a></td></tr>
  <tr><td><?php echo Locale_Translate::_("Bottom margin") ?></td><td><a href="exports.php?bottommargin="><?php echo $this->bottommargin ?> <?php echo Locale_Translate::_ ("mm") ?></a></td></tr>
</table>
<br>
<p>
<a href="exports.php?dateinheadertoggle="><?php if ($this->dateinheader) { ?> ☑ <?php } else { ?> ☐ <?php } ?></a>
<?php echo Locale_Translate::_("Display the current date in the running headers.") ?>
</p>
<br>
<p>
  <a href="?odtsecuretoggle=">
  <?php if ($this->odtsecure == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Secure the export with the password.") ?>
</p>

<br>
<hr>

<p>
  <a href="?infotoggle=">
  <?php if ($this->info == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Generate info document during the night.") ?>
  <a href="?infonow="><?php echo Locale_Translate::_("Generate now.") ?></a>
</p>

<br>
<hr>

<p>
  <a href="?eswordtoggle=">
  <?php if ($this->esword == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to e-Sword during the night.") ?>
  <a href="?eswordnow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>

<br>
<hr>

<p>
  <a href="?onlinebibletoggle=">
  <?php if ($this->onlinebible == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo Locale_Translate::_("Export to Online Bible during the night.") ?>
  <a href="?onlinebiblenow="><?php echo Locale_Translate::_("Export now.") ?></a>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("The exported file is suitable for the Online Bible module creation process.") ?>
  <?php echo Locale_Translate::_("The help within the Online Bible provides more information about creating a module.") ?>
</p>

<br>
<hr>

<p><?php echo Locale_Translate::_("The exports use the following stylesheet:") ?> <a href="?sheet="><?php echo $this->stylesheet ?></a>.</p>

<br>

<form action="exports.php" name="passwordform" method="post">
  <p>
    <?php echo Locale_Translate::_("Password to secure the exports:") ?>
    <input type="text" name="passwordentry" maxlength="256" value="<?php echo $this->password ?>" />
    <input type="submit" name="passwordsubmit" value="<?php echo Locale_Translate::_("Save") ?>" />
  </p>
</form>

<hr>

<p><a href="http://freely-given.org/Software/BibleDropBox" target="_blank"><?php echo Locale_Translate::_("The Bible Drop Box assists with exporting to even more formats.") ?></a></p>
