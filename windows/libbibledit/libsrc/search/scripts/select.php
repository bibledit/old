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
<p><a href="index.php"><?php echo Locale_Translate::_("Search") ?></a></p>
<ol>
  <li><?php echo Locale_Translate::_("Enter the search term.") ?></li>
  <li><?php echo Locale_Translate::_("Press Enter.") ?></li>
</ol>
<p><a href="replace.php"><?php echo Locale_Translate::_("Replace") ?></a></p>
<ol>
  <li><?php echo Locale_Translate::_("Enter what to search for.") ?></li>
  <li><?php echo Locale_Translate::_("Enter what to replace with.") ?></li>
  <li><?php echo Locale_Translate::_("Press Enter.") ?></li>
  <li><?php echo Locale_Translate::_("Wait till the preview appears.") ?></li>
  <li>
    <?php echo Locale_Translate::_("Replace the text:") ?>
    <ul>
      <li><?php echo Locale_Translate::_("To replace in one passage, click ✔.") ?></li>
      <li><?php echo Locale_Translate::_("To remove a passage, click ✗.") ?></li>
      <li><?php echo Locale_Translate::_("To open a passage for edit, click that passage.") ?></li>
      <li><?php echo Locale_Translate::_("To replace all passages, click Apply all.") ?></li>
    </ul>
  </li>
</ol>
<p>
  <a href="search2.php">
  <?php echo Locale_Translate::_("Advanced search.") ?>
  <?php echo Locale_Translate::_("Search with more options.") ?>
  </a>
</p>
<p>
  <a href="replace2.php">
  <?php echo Locale_Translate::_("Advanced replace.") ?>
  <?php echo Locale_Translate::_("Replace with more options.") ?>
  </a>
</p>
<br>
<p><a href="similar.php"><?php echo Locale_Translate::_("Search the active Bible for verses similar to the focused verse.") ?></a></p>
<p><a href="strongs.php"><?php echo Locale_Translate::_("Search for verses with most Strong's numbers similar to the focused verse.") ?></a></p>
<p><a href="strong.php"><?php echo Locale_Translate::_("Search for all verses with a certain Strong's number.") ?></a></p>
<p><a href="originals.php"><?php echo Locale_Translate::_("Search for verses with most Hebrew or Greek words similar to the focused verse.") ?></a></p>
