<h1><?php echo $this->book_name ?> <?php echo $this->chapter ?></h1>
<p><?php echo gettext ("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<p><?php echo gettext ("Book") ?>: <a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>"><?php echo $this->book_name ?></a></p>
<p><a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&deletechapter=<?php echo $this->chapter ?>"><?php echo gettext ("Delete this chapter") ?></a></p>
