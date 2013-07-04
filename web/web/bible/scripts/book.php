<h1><?php echo $this->book_name ?></h1>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo gettext ("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<p><?php echo gettext ("Chapters:") ?>
<?php foreach ($this->chapters as $offset => $chapter) { ?>
  <a href="chapter.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&chapter=<?php echo $this->chapters[$offset] ?>"><?php echo $this->chapters[$offset] ?></a>
<?php } ?>
</p>
<p><a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&createchapter="><?php echo gettext ("Create chapter") ?></a></p>
<p><a href="settings.php?bible=<?php echo $this->bible ?>&deletebook=<?php echo $this->book ?>"><?php echo gettext ("Delete this book") ?></a></p>
