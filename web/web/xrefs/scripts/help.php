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
<h3><?php echo Locale_Translate::_("Place cross references") ?></h3>
<p><a href="index.php">[<?php echo Locale_Translate::_("back") ?></a>]</p>
<h4><?php echo Locale_Translate::_("Introduction") ?></h4>
<p>
  <?php echo Locale_Translate::_("The tool to place cross references enables you to easily and quickly place cross references in a Bible.") ?>
  <?php echo Locale_Translate::_("The tool has two main functions:") ?>
</p>
<ol>
<li>
  <?php echo Locale_Translate::_("Read the cross references from a source Bible, and place them in a target Bible.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Move cross references in the target Bible to their proper locations within a verse.") ?>
</li>
</ol>
<h4><?php echo Locale_Translate::_("Placing cross references") ?></h4>
<p>
  <?php echo Locale_Translate::_("Click the link to copy the cross references.") ?>
  <?php echo Locale_Translate::_("Bibledit reads the cross references from the entire focused chapter of the source Bible.") ?>
  <?php echo Locale_Translate::_("It interprets the book abbreviations.") ?>
  <?php echo Locale_Translate::_("It then translates those abbreviations to the abbreviations as used in the target Bible.") ?>
  <?php echo Locale_Translate::_("It then places the cross references in the entire focused chapter of the target Bible.") ?>
</p>
<h4><?php echo Locale_Translate::_("Moving cross references") ?></h4>
<p>
  <?php echo Locale_Translate::_("The page displays one verse a time.") ?>
  <?php echo Locale_Translate::_("Select the verses one by one, and move the cross references into place.") ?>
  <?php echo Locale_Translate::_("Everything can be done through the arrow keys on the keyboard.") ?>
</p>
<ul>
<li><?php echo Locale_Translate::_("Left arrow: Move the selected cross reference one word back.") ?></li>
<li><?php echo Locale_Translate::_("Right arrow: Move the selected cross reference one word forward.") ?></li>
<li><?php echo Locale_Translate::_("Up arrow: Select the previous cross reference.") ?></li>
<li><?php echo Locale_Translate::_("Down arrow: Select the next cross reference.") ?></li>
<li><?php echo Locale_Translate::_("Alt-Up arrow: Go to the next verse.") ?></li>
<li><?php echo Locale_Translate::_("Alt-Down arrow: Go to the previous verse.") ?></li>
</ul>
