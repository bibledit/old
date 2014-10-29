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
<p><a href="note.php?id=<?php echo $this->id ?>"><?php echo Locale_Translate::_("Go back to the note contents") ?></a></p>
<p><?php echo Locale_Translate::_("Summary") ?>: <?php echo $this->summary ?></p>
<p>
<?php if ($this->subscribed) { ?>
<?php echo Locale_Translate::_("You are subscribed to this note") ?> <a href="?id=<?php echo $this->id ?>&unsubscribe=">[<?php echo Locale_Translate::_ ("unsubscribe") ?>]</a>
<?php } else { ?>
<a href="?id=<?php echo $this->id ?>&subscribe="><?php echo Locale_Translate::_("Subscribe to this note") ?></a>
<?php } ?>
<p><?php echo Locale_Translate::_("The note has been assigned to") ?>:
<?php foreach ($this->assignees as $offset => $assignee) { ?>
  <?php echo $this->assignees[$offset] ?>
  <?php if ($this->level >= 5) { ?>
    <a href="?id=<?php echo $this->id ?>&unassign=<?php echo $this->assignees[$offset] ?>">[<?php echo Locale_Translate::_("unassign") ?>]</a>
    |
  <?php } ?>
<?php } ?>
<?php if ($this->level >= 5) { ?>
  <a href="assign-1.php?id=<?php echo $this->id ?>">[<?php echo Locale_Translate::_("add assignee") ?>]</a>
<?php } ?>
</p>
<?php if ($this->assignee) { ?>
  <p>
  <?php echo Locale_Translate::_("This note has been assigned to you for you to take action on.") ?>
  <a href="?id=<?php echo $this->id ?>&done="><?php echo Locale_Translate::_("I have done my part on it.") ?></a>
  </p>
<?php } ?>
<p>
<?php echo Locale_Translate::_("Status") ?>:
<?php if ($this->level >= 4) { ?>
  <a href="status-1.php?id=<?php echo $this->id ?>"><?php echo $this->status ?></a>
<?php } else { ?>
  <?php echo $this->status ?>
<?php } ?>
</p>
<p>
  <?php echo Locale_Translate::_("Verses") ?>: <?php echo $this->verses ?> |
  <a href="verses.php?id=<?php echo $this->id ?>">[<?php echo Locale_Translate::_("edit") ?>]</a>
</p>
  <p><?php echo Locale_Translate::_("Severity") ?>:
  <a href="severity-1.php?id=<?php echo $this->id ?>"><?php echo $this->severity ?></a></p>
<p>
</p>
<p>
<?php echo Locale_Translate::_("Bible") ?>:
<?php if ($this->bible != "") { ?>
  <?php echo $this->bible ?>
<?php } else { ?>
  <?php echo Locale_Translate::_("This is a general note, it does not apply to any specific Bible") ?>
<?php } ?>
  <a href="bible-1.php?id=<?php echo $this->id ?>">[<?php echo Locale_Translate::_("change") ?>]</a>
</p>
<p><?php echo Locale_Translate::_("Identifier") ?>: <?php echo $this->id ?></p>
</p>
<?php if ($this->level >= 5) { ?>
  <p>
  <?php if ($this->marked) { ?>
    <?php echo Locale_Translate::_("The note will be deleted after it expires
    ") ?>
    <a href="?id=<?php echo $this->id ?>&unmarkdel=">[<?php echo Locale_Translate::_("cancel") ?>]</a>
  <?php } else { ?>
    <a href="?id=<?php echo $this->id ?>&markdel="><?php echo Locale_Translate::_("Mark the note for deletion after a week") ?></a>
  <?php } ?>
  </p>
  <p><a href="?id=<?php echo $this->id ?>&delete="><?php echo Locale_Translate::_("Delete the note now") ?></a></p>
<?php } ?>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<script type="text/javascript" src="poll.js?<?php echo Config_Logic::version () ?>"></script>
