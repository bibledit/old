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
<p>
  <?php echo Locale_Translate::_("Change History") ?>
  |
  <a href="history.php?<?php echo $this->back ?>"> « </a>
  <?php echo $this->start ?>-<?php echo $this->end ?>/<?php echo $this->count ?>
  <a href="history.php?<?php echo $this->forward ?>"> » </a>
  |
  <a href="#filter"><?php echo Locale_Translate::_("filter") ?></a>
  |
  <a href="#help"><?php echo Locale_Translate::_("help") ?></a>
</p>
<br>
<?php foreach ($this->passageTexts as $offset => $passageText) { ?>
  <?php if ($offset) { ?>
  <br>
  <?php } ?>
  <p>
    <?php echo $this->passageTexts[$offset] ?>
    |
    <?php echo $this->authors[$offset] ?>
    |
    <?php echo $this->dates[$offset] ?>
    |
    <?php echo $this->bibles[$offset] ?>
  </p>
  <table>
    <tr>
      <td><?php echo Locale_Translate::_("Old") ?>:</td>
      <td><?php echo $this->oldTexts[$offset] ?></td>
    </tr>
    <tr>
      <td><?php echo Locale_Translate::_("Change") ?>:</td>
      <td><?php echo $this->modifications[$offset] ?></td>
    </tr>
    <tr>
      <td><?php echo Locale_Translate::_("New") ?>:</td>
      <td><?php echo $this->newTexts[$offset] ?></td>
    </tr>
  </table>
<?php } ?>
<br>
<a id="filter"></a>
<p>
  <a href="history.php?<?php echo $this->bible ?>" title="<?php echo Locale_Translate::_("Display entries for the entire Bible") ?>"><?php echo Locale_Translate::_ ("Bible") ?></a>
  |
  <a href="history.php?<?php echo $this->book ?>" title="<?php echo Locale_Translate::_("Display entries for the focused book") ?>"><?php echo Locale_Translate::_ ("book") ?></a>
  |
  <a href="history.php?<?php echo $this->chapter ?>" title="<?php echo Locale_Translate::_("Display entries for the focused chapter") ?>"><?php echo Locale_Translate::_ ("chapter") ?></a>
  |
  <a href="history.php?<?php echo $this->verse ?>" title="<?php echo Locale_Translate::_("Display entries for the focused verse") ?>"><?php echo Locale_Translate::_ ("verse") ?></a>
</p>
<ul>
<?php foreach ($this->author_names as $offset => $author_name) { ?>
  <li><a href="history.php?<?php echo $this->author_queries[$offset] ?>"><?php echo $author_name ?></a></li>
<?php } ?>
</ul>
<a id="help"></a>
<p>
  <?php echo Locale_Translate::_("The Change History displays where and when changes in the Bible text were made by whom.") ?>
  <?php echo Locale_Translate::_("It displays 50 entries at a time.") ?>
  <?php echo Locale_Translate::_("It displays the most recent entries on top.") ?>
  <?php echo Locale_Translate::_("There is a pager for displaying older entries.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("The page can display items for the current book only, or the current chapter, or the current verse.") ?>
  <?php echo Locale_Translate::_("It can also display items for one author only.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("The items are kept indefinitely.") ?>
</p>
<script type="text/javascript" src="history.js?<?php echo Config_Logic::version () ?>"></script>
