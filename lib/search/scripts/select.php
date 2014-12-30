<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
<p><a href="index.php"><?php echo gettext("Search") ?></a></p>
<ol>
  <li><?php echo gettext("Enter the search term.") ?></li>
  <li><?php echo gettext("Press Enter.") ?></li>
</ol>
<p><a href="replace.php"><?php echo gettext("Replace") ?></a></p>
<ol>
  <li><?php echo gettext("Enter what to search for.") ?></li>
  <li><?php echo gettext("Enter what to replace with.") ?></li>
  <li><?php echo gettext("Press Enter.") ?></li>
  <li><?php echo gettext("Wait till the preview appears.") ?></li>
  <li>
    <?php echo gettext("Replace the text:") ?>
    <ul>
      <li><?php echo gettext("To replace in one passage, click ✔.") ?></li>
      <li><?php echo gettext("To remove a passage, click ✗.") ?></li>
      <li><?php echo gettext("To open a passage for edit, click that passage.") ?></li>
      <li><?php echo gettext("To replace all passages, click Apply all.") ?></li>
    </ul>
  </li>
</ol>
<p>
  <a href="search2.php">
  <?php echo gettext("Advanced search.") ?>
  <?php echo gettext("Search with more options.") ?>
  </a>
</p>
<p>
  <a href="replace2.php">
  <?php echo gettext("Advanced replace.") ?>
  <?php echo gettext("Replace with more options.") ?>
  </a>
</p>
<br>
<p><a href="similar.php"><?php echo gettext("Search the active Bible for verses similar to the focused verse.") ?></a></p>
<p><a href="strongs.php"><?php echo gettext("Search for verses with most Strong's numbers similar to the focused verse.") ?></a></p>
<p><a href="strong.php"><?php echo gettext("Search for all verses with a certain Strong's number.") ?></a></p>
<p><a href="originals.php"><?php echo gettext("Search for verses with most Hebrew or Greek words similar to the focused verse.") ?></a></p>
