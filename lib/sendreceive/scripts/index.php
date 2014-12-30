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
<h3><?php echo gettext("Send / Receive") ?></h3>

<?php if ($this->client) { ?> 

  <p class="success"><?php echo $this->successnotes ?></p>
  <p class="error"><?php echo $this->errornotes ?></p>
  <p class="error"><?php echo $this->demo ?></p>
  <p><?php echo gettext("Bibles and Consultation Notes.") ?></p>
  <p><?php echo gettext("Bibledit sends and receives the Bibles and Consultation Notes every night.") ?></p>
  <p><a href="?runsync="><?php echo gettext("Send and receive them now.") ?></a></p>
  <p><?php echo gettext("Repeat send and receive:") ?></p>
  <p>
    • 
    <?php if ($this->repeatsync == 2) { ?>⇉<?php } ?>
    <a href="?repeatsync=2"><?php echo gettext("Every five minutes") ?></a>
    <?php if ($this->repeatsync == 2) { ?>⇇<?php } ?>
  </p>
  <p>
    • 
    <?php if ($this->repeatsync == 1) { ?>⇉<?php } ?>
    <a href="?repeatsync=1"><?php echo gettext("Every hour") ?></a>
    <?php if ($this->repeatsync == 1) { ?>⇇<?php } ?>
  </p>
  <p>
    • 
    <?php if ($this->repeatsync == 0) { ?>⇉<?php } ?>
    <a href="?repeatsync=0"><?php echo gettext("Off") ?></a>
    <?php if ($this->repeatsync == 0) { ?>⇇<?php } ?>
  </p>
  <p>
    <?php echo gettext("It sends changes made here to the Bibledit server.") ?>
    <?php echo gettext("It receives updates of the Bibles and Consultation Notes from the Bibledit server.") ?>
  </p>

<?php } else { ?>

  <p class="success"><?php echo $this->successbible ?></p>
  <p class="error"><?php echo $this->errorbible ?></p>
  <p><?php echo gettext("Bible") ?>: <a href="?bible="><?php echo $this->bible ?></a>.</p>
  <p><?php echo gettext("Bibledit sends and receives this Bible every night.") ?></p>
  <p><a href="?runbible="><?php echo gettext("Send and receive it now.") ?></a></p>
  <p>
  <a href="?repeatbible=">
  <?php if ($this->repeatbible == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
  </a>
  <?php echo gettext("Send and receive it every five minutes.") ?>
  </p>
  <p>
    <?php echo gettext("It receives changes made by others in Bibledit-Gtk or other instances of Bibledit.") ?>
    <?php echo gettext("It sends changes made here to the server for use by other translators who use Bibledit-Gtk or Bibledit.") ?>
  </p>

<?php } ?>

<br>
<p><a href="../journal/index.php"><?php echo gettext("The Journal displays progress and diagnostic messages.") ?></a></p>
