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
<h3><span id="count"></span> <?php echo gettext("Change Notifications") ?></h3>
<p>
  <?php echo gettext("View") ?>
  <a href="changes.php" <?php if ($this->filter == 0) echo 'class="active"' ?>><?php echo gettext("all") ?></a>
  |
  <a href="changes.php?filter=personal" <?php if ($this->filter == 1) echo 'class="active"' ?>><?php echo gettext("personal") ?></a>
  |
  <a href="changes.php?filter=team" <?php if ($this->filter == 2) echo 'class="active"' ?>><?php echo gettext("team") ?></a>
  -
  <a href="#help">[<?php echo gettext("help") ?>]</a>
</p>
<br>
<?php for ($this->modifications as $offset => $modification) { ?>
  <div id="entry<?php echo $this->ids[$offset] ?>">
    <a href="expand" id="expand<?php echo $this->ids[$offset] ?>"> ⊞ </a>
    <a href="remove" id="remove<?php echo $this->ids[$offset] ?>"> ✗ </a>
    <?php echo $this->links[$offset] ?>
    <span style="font-family:monospace;font-size:125%;"><?php echo $this->categories[$offset] ?></span>
    <?php echo $this->modifications[$offset] ?>
  </div>
<?php } ?>
<br>
<a id="help"></a>
<p><?php echo gettext("This page lists the change notifications.") ?></p>
<p><?php echo gettext("Use the up and down arrow keys to navigate through the entries.") ?></p>
<p>
  <?php echo gettext("Press the delete key to remove the selected entry.") ?>
  <?php echo gettext("Or click the ✗.") ?>
</p>
<p>
  <?php echo gettext("Use the left and right arrow keys to expand or collapse the selected entry.") ?>
  <?php echo gettext("Or click the ⊞.") ?>
</p>
<br>
<p>
  <?php echo gettext("The expanded entry provides more information and options.") ?>
  <?php echo gettext("It displays all the Consultation Notes that refer to passage of the Change Notification.") ?>
  <?php echo gettext("If you are subscribed to a note, you can unsubscribe.") ?>
  <?php echo gettext("If a note is assigned to you, you can unassign it.") ?>
  <?php if ($this->level >= 5) { ?>
  <?php echo gettext("You can delete a note.") ?>
  <?php } ?>
  <?php echo gettext("You can create a new Consultation Notes with a query about the change in the text of the Bible.") ?>
</p>
<br>
<p>
  <?php echo gettext("The change notifications are for you only.") ?>
  <?php echo gettext("Your colleagues have their own lists.") ?>
  <?php echo gettext("Any changes in the text of the Bibles will be added to the list the next day.") ?>
  <?php echo gettext("This works if enabled in your personal notifications.") ?>
</p>
<br>
<p><?php echo gettext("The change notifications fall into two categories:") ?></p>
<p>
  <large>☺</large>
  <?php echo gettext("Changes marked with this symbol are your personal change proposals.") ?>
</p>
<p>
  <large>♺</large>
  <?php echo gettext("Changes marked with this symbol are changes in the team's Bible text.") ?>
</p>
<p>
  <?php echo gettext("Bible translation is a collabortive effort.") ?>
  <?php echo gettext("Changes you enter into the web-based Bible editor or on a Client may be overwritten by other team members.") ?>
  <?php echo gettext("The two categories of changes enable you to find out if your change proposal made its way into the team's text.") ?>
</p>
<br>
<p>
  <?php echo gettext('You can view "all" the change notifications.') ?>
  <?php echo gettext('You can also view only your "personal" change proposals, plus the changes of the team that could affect your change proposals.') ?>
  <?php echo gettext('Or you can view the change notifications of the "team".') ?>
</p>
<br>
<p>
  <a href="?match"><?php echo gettext("Automatically remove personal change proposals and their matching change notifications.") ?></a>
</p>
<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="changes.js?<?php echo config_logic_version () ?>"></script>
<script type="text/javascript" src="../navigation/nav.js?<?php echo config_logic_version () ?>"></script>
<script type="text/javascript" src="../notes/click.js?<?php echo config_logic_version () ?>"></script>
