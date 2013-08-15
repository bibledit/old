<h1><?php echo gettext ("Send/Receive") ?></h1>
<p><?php echo gettext ("Bibledit-Web sends and receives the Bibles every day at midnight.") ?></p>
<p><?php echo gettext ("It receives changes made by others, and it sends changes made here to the server for use by other translators.") ?></p>
<br>
<p class="success"><?php echo $this->success ?></p>
<p><a href="?run="><?php echo gettext ("Send and receive Bibles now.") ?></a></p>
<p>
<a href="?togglerepeat=">
<?php if ($this->repeat == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Send and receive every five minutes.") ?>
</p>
<p><a href="../manage/logbook.php"><?php echo gettext ("See the Logbook for details.") ?></a></p>


