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
<h3><span id="count"></span> gettext("Change Notifications")</h3>
<p>
  gettext("View")
  <a href="changes.php" <?php if ($this->filter == 0) echo 'class="active"'>gettext("all")</a>
  |
  <a href="changes.php?filter=personal" <?php if ($this->filter == 1) echo 'class="active"'>gettext("personal")</a>
  |
  <a href="changes.php?filter=team" <?php if ($this->filter == 2) echo 'class="active"'>gettext("team")</a>
  -
  <a href="#help">[gettext("help")]</a>
</p>
<br>
<?php for ($this->modifications as $offset => $modification) {
  <div id="entry$this->ids[$offset]">
    <a href="expand" id="expand$this->ids[$offset]"> ⊞ </a>
    <a href="remove" id="remove$this->ids[$offset]"> ✗ </a>
    $this->links[$offset]
    <span style="font-family:monospace;font-size:125%;">$this->categories[$offset]</span>
    $this->modifications[$offset]
  </div>
<?php }
<br>
<a id="help"></a>
<p>gettext("This page lists the change notifications.")</p>
<p>gettext("Use the up and down arrow keys to navigate through the entries.")</p>
<p>
  gettext("Press the delete key to remove the selected entry.")
  gettext("Or click the ✗.")
</p>
<p>
  gettext("Use the left and right arrow keys to expand or collapse the selected entry.")
  gettext("Or click the ⊞.")
</p>
<br>
<p>
  gettext("The expanded entry provides more information and options.")
  gettext("It displays all the Consultation Notes that refer to passage of the Change Notification.")
  gettext("If you are subscribed to a note, you can unsubscribe.")
  gettext("If a note is assigned to you, you can unassign it.")
  <?php if ($this->level >= 5) {
  gettext("You can delete a note.")
  <?php }
  gettext("You can create a new Consultation Notes with a query about the change in the text of the Bible.")
</p>
<br>
<p>
  gettext("The change notifications are for you only.")
  gettext("Your colleagues have their own lists.")
  gettext("Any changes in the text of the Bibles will be added to the list the next day.")
  gettext("This works if enabled in your personal notifications.")
</p>
<br>
<p>gettext("The change notifications fall into two categories:")</p>
<p>
  <large>☺</large>
  gettext("Changes marked with this symbol are your personal change proposals.")
</p>
<p>
  <large>♺</large>
  gettext("Changes marked with this symbol are changes in the team's Bible text.")
</p>
<p>
  gettext("Bible translation is a collabortive effort.")
  gettext("Changes you enter into the web-based Bible editor or on a Client may be overwritten by other team members.")
  gettext("The two categories of changes enable you to find out if your change proposal made its way into the team's text.")
</p>
<br>
<p>
  gettext('You can view "all" the change notifications.')
  gettext('You can also view only your "personal" change proposals, plus the changes of the team that could affect your change proposals.')
  gettext('Or you can view the change notifications of the "team".')
</p>
<br>
<p>
  <a href="?match">gettext("Automatically remove personal change proposals and their matching change notifications.")</a>
</p>
<script>
$this->script;
</script>
<script type="text/javascript" src="changes.js?config_logic_version ()"></script>
<script type="text/javascript" src="../navigation/nav.js?config_logic_version ()"></script>
<script type="text/javascript" src="../notes/click.js?config_logic_version ()"></script>
