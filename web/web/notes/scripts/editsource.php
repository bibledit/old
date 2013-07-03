<h1><?php echo gettext ("Edit Note Source") ?></h1>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p><?php echo gettext ("You can edit the source of a Consultation Note.") ?></p>
<p><?php echo gettext ("Each Consultation Notes has an identifier.") ?></p>
<p><?php echo gettext ("The identifier of a Consultation Note can be seen through the Actions link of that note.") ?></p>
<p><?php echo gettext ("Example:") ?></p>
<p><?php echo gettext ("Identifier:") ?> 943430030.</p>
<br>
<p><?php echo gettext ("Enter the identifier of the Consultation Note to be edited and press Submit.") ?></p>
<form action="editsource.php" name="noteid" method="post">
  <p>
  <input type="text" name="identifier" maxlength="15" value="<?php echo $this->identifier ?>" />
  <input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> />
  </p>
</form>
<br>
<form action="editsource.php?identifier=<?php echo $this->identifier ?>" name="notetext" method="post">
  <p><textarea name="data"><?php echo $this->data ?></textarea></p>
  <p><input type="submit" name="save" value=<?php echo gettext ("Save") ?> /></p>
</form>
