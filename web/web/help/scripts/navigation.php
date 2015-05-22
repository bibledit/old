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
<h1><?php echo Locale_Translate::_("Navigation") ?></h1>
<p>
  <?php echo Locale_Translate::_("Several pages have a Passage Navigator on top.") ?>
  <?php echo Locale_Translate::_("It consists of the name of the book, the chapter, and the verse.") ?>
  <?php echo Locale_Translate::_("For example:") ?>
  <?php echo Locale_Translate::_("Matthew 1:1.") ?>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("The navigator enables you to go to another passage.") ?>
  <?php echo Locale_Translate::_("Click on the name of the book to navigate to another book.") ?>
  <?php echo Locale_Translate::_("In the same way you can click on the chapter and on the verse.") ?>
</p>
<br>
<p>
  <a href="navigation.php"><?php echo Locale_Translate::_("You can also use the keyboard to navigate.") ?></a>
  <?php echo Locale_Translate::_("Press Ctrl-G, enter the new passage, for example Mat 2 3, and press Enter.") ?>
  <?php echo Locale_Translate::_("You can also enter the name of the book without chapter and verse, for example, Genesis, and it will go to that book.") ?>
  <?php echo Locale_Translate::_("Or enter chapter and verse, for example 10 2, and it goes to that passage in the current book.") ?>
  <?php echo Locale_Translate::_("When entering just one number, for example 2, it will go to verse 2 in the current chapter.") ?>
  <?php echo Locale_Translate::_("Or enter Luke 10, to go to Luke chapter 10.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("Enter nothing or + to go to the next verse.") ?>
  <?php echo Locale_Translate::_("Enter - to go to the previous verse.") ?>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("Press Escape to cancel any navigation operation.") ?>
</p>
<br>
<p><?php echo Locale_Translate::_("The Navigator shows two arrows:") ?> ↶ <?php echo Locale_Translate::_ ("and") ?> ↷</p>
<p>↶ <?php echo Locale_Translate::_("goes back in the navigation history.") ?></p>
<p>↷ <?php echo Locale_Translate::_("goes forward in the navigation history.") ?></p>
<p><a href="navigation.php"><?php echo Locale_Translate::_("There are also keyboard shortcuts for it.") ?></a></p>
<br>
<p>
  <?php echo Locale_Translate::_("Some pages display one or more clickable passages.") ?>
  <?php echo Locale_Translate::_("Clicking them updates the Passage Navigator.") ?>
</p>
