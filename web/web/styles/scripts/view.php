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
<h1><?php echo gettext ("Style") ?></h1>

<table>

  <tr><td><?php echo gettext ("Stylesheet") ?></td><td><a href="sheetm.php?name=<?php echo $this->sheet ?>"><?php echo $this->sheet ?></a></td></tr>

  <tr><td><?php echo gettext ("Style") ?></td><td><?php echo $this->style ?></td></tr>

  <tr><td><?php echo gettext ("Name") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&name="><?php echo $this->name ?></a></td></tr>

  <tr><td><?php echo gettext ("Description") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&info="><?php echo $this->info ?></a></td></tr>

  <tr><td><?php echo gettext ("Category") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&category="><?php echo $this->category ?></a></td></tr>

  <tr><td><?php echo gettext ("This style") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&type="><?php echo $this->type_text ?></a></td></tr>

  <?php if ($this->subtype_text != "") { ?>
  <tr><td><?php echo gettext ("This one") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&subtype="><?php echo $this->subtype_text ?></a></td></tr>
  <?php } ?>

  <?php if ($this->fontsize_relevant) { ?>
  <tr><td><?php echo gettext ("Font size") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&fontsize="><?php echo $this->fontsize ?> <?php echo gettext ("points") ?></a></td></tr>
  <?php } ?>

  <?php if ($this->ibus_relevant) { ?>
  <tr><td><?php echo gettext ("Italic") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&italic="><?php echo $this->italic ?></a></td></tr>
  <tr><td><?php echo gettext ("Bold") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&bold="><?php echo $this->bold ?></a></td></tr>
  <tr><td><?php echo gettext ("Underline") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&underline"><?php echo $this->underline ?></a></td></tr>
  <tr><td><?php echo gettext ("Small caps") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&smallcaps="><?php echo $this->smallcaps ?></a></td></tr>
  <?php } ?>

  <?php if ($this->superscript_relevant) { ?>
  <tr><td><?php echo gettext ("Superscript") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&superscript={$superscript_toggle}"><?php echo $this->superscript_value ?></a></td></tr>
  <?php } ?>

  <?php if ($this->paragraph_treats_relevant) { ?>
  <tr><td><?php echo gettext ("Text alignment") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&alignment="><?php echo $this->justification ?></a></td></tr>
  <tr><td><?php echo gettext ("Space before") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&spacebefore="><?php echo $this->spacebefore ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Space after") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&spaceafter="><?php echo $this->spaceafter ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Left margin") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&leftmargin="><?php echo $this->leftmargin ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("Right margin") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&rightmargin="><?php echo $this->rightmargin ?> <?php echo gettext ("mm") ?></a></td></tr>
  <tr><td><?php echo gettext ("First line indent") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&firstlineindent="><?php echo $this->firstlineindent ?> <?php echo gettext ("mm") ?></a></td></tr>
  <?php } ?>

  <?php if ($this->columns_relevant) { ?>
  <tr><td><?php echo gettext ("Span columns") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&spancolumns={$spancolumns_toggle}"><?php echo $this->spancolumns ?></a></td></tr>
  <?php } ?>

  <?php if ($this->color_relevant) { ?>
  <tr><td><?php echo gettext ("Color") ?></td><td style="background-color:#<?php echo $this->color ?>"></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&color=">[<?php echo gettext ("edit") ?>]</a></td></tr>
  <?php } ?>

  <?php if ($this->print_relevant) { ?>
  <tr><td><?php echo gettext ("Print") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&print=<?php echo $this->print_toggle ?>"><?php echo $this->print ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userbool1_function != "") { ?>
  <tr><td><?php echo $this->userbool1_function ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userbool1={$userbool1_toggle}"><?php echo $this->userbool1_value ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userbool2_function != "") { ?>
  <tr><td><?php echo $this->userbool2_function ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userbool2={$userbool2_toggle}"><?php echo $this->userbool2_value ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userbool3_function != "") { ?>
  <tr><td><?php echo $this->userbool3_function ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userbool3={$userbool3_toggle}"><?php echo $this->userbool3_value ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userint1_notenumbering) { ?>
  <tr><td><?php echo gettext ("Note numbering") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&notenumbering="><?php echo $this->userint1 ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userint1_columnnumber) { ?>
  <tr><td><?php echo gettext ("Column number") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userint1="><?php echo $this->userint1 ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userint2_notenumberingrestart) { ?>
  <tr><td><?php echo gettext ("Restart the note numbering") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&notenumberingrestart="><?php echo $this->userint2 ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userint2_endnoteposition) { ?>
  <tr><td><?php echo gettext ("Position of endnote") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&endnoteposition="><?php echo $this->userint2 ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userstring1_numberingsequence) { ?>
  <tr><td><?php echo gettext ("User-defined note numbering sequence") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userstring1="><?php echo $this->userstring1 ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userstring1_wordlistaddition) { ?>
  <tr><td><?php echo gettext ("Addition after word list entry") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userstring1="><?php echo $this->userstring1 ?></a></td></tr>
  <?php } ?>

  <?php if ($this->userstring2_dumpendnotes) { ?>
  <tr><td><?php echo gettext ("Dump endnotes when encountering marker") ?></td><td><a href="view.php?sheet=<?php echo $this->sheet ?>&style=<?php echo $this->style ?>&userstring2="><?php echo $this->userstring2 ?></a></td></tr>
  <?php } ?>

</table>
