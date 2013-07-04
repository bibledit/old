<h1><?php echo gettext ("Bible") ?> <?php echo $this->bible ?></h1>
<p>
  <?php echo gettext ("Available books") ?>:
  <?php foreach ($this->book_ids as $offset => $book_id) { ?>
    <a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book_ids[$offset] ?>"><?php echo $this->book_names[$offset] ?></a>
  <?php } ?>
</p>
<p><?php echo gettext ("Versification system") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>&versification="><?php echo $this->versification ?></a></p>
<p><a href="settings.php?bible=<?php echo $this->bible ?>&createbook="><?php echo gettext ("Create book") ?></a></p>
<p><a href="import_usfm.php?bible=<?php echo $this->bible ?>"><?php echo gettext ("Import USFM") ?></a></p>
<p><a href="import_bibleworks.php?bible=<?php echo $this->bible ?>"><?php echo gettext ("Import BibleWorks") ?></a></p>
<p><a href="manage.php?copy=<?php echo $this->bible ?>"><?php echo gettext ("Copy this Bible") ?></a></p>
<p><a href="manage.php?delete=<?php echo $this->bible ?>"><?php echo gettext ("Delete this Bible") ?></a></p>
