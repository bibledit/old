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
<p>
  gettext("Change History")
  |
  <a href="history.php?$this->back"> « </a>
  $this->start-$this->end/$this->count
  <a href="history.php?$this->forward"> » </a>
  |
  <a href="#filter">gettext("filter")</a>
  |
  <a href="#help">gettext("help")</a>
</p>
<br>
<?php for ($this->passageTexts as $offset => $passageText) {
  <?php if ($offset) {
  <br>
  <?php }
  <p>
    $this->passageTexts[$offset]
    |
    $this->authors[$offset]
    |
    $this->dates[$offset]
    |
    $this->bibles[$offset]
  </p>
  <table>
    <tr>
      <td>gettext("Old"):</td>
      <td>$this->oldTexts[$offset]</td>
    </tr>
    <tr>
      <td>gettext("Change"):</td>
      <td>$this->modifications[$offset]</td>
    </tr>
    <tr>
      <td>gettext("New"):</td>
      <td>$this->newTexts[$offset]</td>
    </tr>
  </table>
<?php }
<br>
<a id="filter"></a>
<p>
  <a href="history.php?$this->bible" title="gettext("Display entries for the entire Bible")">gettext ("Bible")</a>
  |
  <a href="history.php?$this->book" title="gettext("Display entries for the focused book")">gettext ("book")</a>
  |
  <a href="history.php?$this->chapter" title="gettext("Display entries for the focused chapter")">gettext ("chapter")</a>
  |
  <a href="history.php?$this->verse" title="gettext("Display entries for the focused verse")">gettext ("verse")</a>
</p>
<ul>
<?php for ($this->author_names as $offset => $author_name) {
  <li><a href="history.php?$this->author_queries[$offset]">$author_name</a></li>
<?php }
</ul>
<a id="help"></a>
<p>
  gettext("The Change History displays where and when changes in the Bible text were made by whom.")
  gettext("It displays 50 entries at a time.")
  gettext("It displays the most recent entries on top.")
  gettext("There is a pager for displaying older entries.")
</p>
<p>
  gettext("The page can display items for the current book only, or the current chapter, or the current verse.")
  gettext("It can also display items for one author only.")
</p>
<p>
  gettext("The items are kept indefinitely.")
</p>
<script type="text/javascript" src="history.js?config_logic_version ()"></script>
