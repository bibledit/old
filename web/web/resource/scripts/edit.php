<h2><?php echo gettext ("Edit resource") ?> "<?php echo $this->name ?>"</h2>
<p>
  <?php echo gettext ("Bibledit-Web displays external resources to the user.") ?>
  <?php echo gettext ("Bibledit-Web attempts to extract the correct passage before passing it on to the user.") ?>
</p>
<p>
  <?php echo gettext ("This is how it works:") ?>
  <?php echo gettext ("Bibledit-Web notices the active passage, for example Genesis 1:1.") ?>
  <?php echo gettext ("Bibledit-Web then runs a script, and passes book, chapter and verse to it.") ?>
  <?php echo gettext ("The script fetches a page from the web.") ?>
  <?php echo gettext ("The script filters this web page.") ?>
  <?php echo gettext ("The script outputs the filtered content.") ?>
  <?php echo gettext ("Bibledit-Web captures the output of the script, and passes it to the user.") ?>
</p>
<p>
  <?php echo gettext ("This method of serving content to the user saves the user a lot of bandwidth.") ?>
  <?php echo gettext ("Bibledit-Web strips a lot of stuff away, and only serves the essentials to the user.") ?>
</p>
<p>
  <?php echo gettext ("On this page you can edit the command and the script to run.") ?>
</p>
<br>
<p><a href="admin.php">[<?php echo gettext ("back") ?>]</a></p>
<br>
<form action="edit.php?name=<?php echo $this->name ?>" name="name" method="post">
  <p><?php echo gettext ("Enter the command to run the script:") ?></p>
  <input type="text" name="command" value="<?php echo $this->command ?>" />
  <p><?php echo gettext ("Enter the code of the script:") ?></p>
  <textarea name="code"><?php echo $this->code ?></textarea>
  <select name="book">
    <?php foreach ($this->books as $offset => $book) { ?>
    <option <?php if ($this->book == $book) echo 'selected="selected"' ?> value="<?php echo $book ?>"><?php echo $this->booknames [$offset] ?></option>
    <?php } ?>
  </select>
  <input type="text" name="chapter" value="<?php echo $this->chapter ?>" />
  :
  <input type="text" name="verse" value="<?php echo $this->verse ?>" />
  <p><input type="submit" name="save" value=<?php echo gettext ("Save") ?> /></p>
</form>
<br>
<p><?php echo gettext ("Output of running the script:") ?></p>
<textarea><?php echo $this->output ?></textarea>
<br>

<p>
  <?php echo gettext ("") ?>
</p>
