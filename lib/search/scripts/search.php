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
<div style="float:left; width:50%;">
<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey"><?php echo $this->noteCount ?> <?php echo Locale_Translate::_("notes") ?></font></p>
<?php foreach ($this->noteUrls as $offset => $noteUrl) { ?>
  <p style="margin-top: 0.75em; margin-bottom: 0em"><a href="<?php echo $noteUrl ?>"><?php echo $this->noteTitles[$offset] ?></a></p>
  <?php echo $this->noteExcerpts[$offset] ?>
<?php } ?>
</div>
<div style="float:right; width:50%; ">
<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey"><?php echo $this->textCount ?> <?php echo Locale_Translate::_("verses") ?></font></p>
<?php foreach ($this->textLinks as $offset => $textLink) { ?>
  <p style="margin-top: 0.75em; margin-bottom: 0em"><?php echo $textLink ?></p>
  <?php echo $this->textExcerpts[$offset] ?>
<?php } ?>
</div>
