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
<h3><?php echo Locale_Translate::_("Consistency") ?></h3>
<p><a href="#help">[<?php echo Locale_Translate::_("help") ?>]</a></p>
<br>
<table>
<tr style="vertical-align: top;">
<td style="width:10%;">
  <p><?php echo Locale_Translate::_("Passages:") ?></p>
  <textarea id="passages"></textarea>
  <p><?php echo Locale_Translate::_("Translations:") ?></p>
  <textarea id="translations"></textarea>
</td>
<td style="width:1%;"></td>
<td>
  <div id="status"><img src="../pix/loading.gif"></div>
  <div id="texts"></div>
</td>
</tr>
</table>
<br>
<a id="help"></a>
<h4><?php echo Locale_Translate::_("Description") ?></h4>
<p>
  <?php echo Locale_Translate::_("This assists with checking how consistent words have been translated in the Bible.") ?>
</p>
<h4><?php echo Locale_Translate::_("Instructions") ?></h4>
<ol>
  <li><?php echo Locale_Translate::_("Enter book, chapter and verse in the 'Passages:' box.") ?></li>
  <p><?php echo Locale_Translate::_("System: The web page will load the verse text for the passages.") ?></p>
</ol>
<ol>
  <li>
    <?php echo Locale_Translate::_("Enter key word text you want to search for in the 'Translations:' box.") ?>
    <?php echo Locale_Translate::_("One word per line.") ?>
    </li>
  <p><?php echo Locale_Translate::_("System: The web page will mark the translations in the verse text.") ?></p>
</ol>
<h4><?php echo Locale_Translate::_("Examples") ?></h4>
<h5><?php echo Locale_Translate::_("Passages:") ?></h5>
<p>Exod. 37:4-5, 14-15, 27-28</p>
<p>Exod. 38:5-7</p>
<p>Exod. 39:35, 39</p>
<h5><?php echo Locale_Translate::_("Translation:") ?></h5>
<p><?php echo Locale_Translate::_("salvation") ?></p>
<p><?php echo Locale_Translate::_("justification") ?></p>
<p><?php echo Locale_Translate::_("redemption") ?></p>

<h4><?php echo Locale_Translate::_("Tips") ?></h4>
<p><?php echo Locale_Translate::_("Add Resources as extra aids for comparison") ?>:
<?php foreach ($this->resources as $resource) { ?>
  <?php echo $resource ?>
  <a href="?remove=<?php echo $resource ?>">[<?php echo Locale_Translate::_("remove") ?>]</a>
  |
<?php } ?>
<a href="?add=">[<?php echo Locale_Translate::_("add resource") ?>]</a>
<p><?php echo Locale_Translate::_("Add the Consistency tool and the Bible editor in a Workbench for easier checking and editing.") ?>
<script type="text/javascript" src="index.js?<?php echo Config_Logic::version () ?>"></script>
<script type="text/javascript" src="../navigation/nav.js?<?php echo Config_Logic::version () ?>"></script>
