<h1><?php echo gettext ("Changes") ?></h1>
<p>
  <a href="#help">[<?php echo gettext ("help") ?>]</a>
  <?php echo gettext ("Displaying") ?> <?php echo $this->displayedChangesCount ?> <?php echo gettext ("of") ?> <?php echo $this->totalChangesCount ?></p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<table>
  <?php foreach ($this->modifications as $offset => $modification) { ?>
  <tr>
    <td><a href="changes.php?approve=<?php echo $this->ids[$offset] ?>"> ✔ </a></td>
    <td><a href="changes.php?goto=<?php echo $this->ids[$offset] ?>" target="_blank"><?php echo $this->passages[$offset] ?></a></td>
    <td><a href="change.php?id=<?php echo $this->ids[$offset] ?>"><?php echo $this->modifications[$offset] ?></a></td>
    <td><?php echo $this->yourNotesCount[$offset] ?></td>
  </tr>
  <?php } ?>
</table>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("This page lists the changes in the text of the Bibles.") ?>
</p>
<p>
  <?php echo gettext ("Click ✔ to approve a change.") ?>
  <?php echo gettext ("The change will disappear.") ?>
  <a href="changes.php?approvepage="><?php echo gettext ("Or approve all changes above.") ?></a>  
  
</p>
<p>
  <?php echo gettext ("Click the passage to open the Text Editor for the chapter.") ?>
</p>
<p>
  <?php echo gettext ("Click the changed text to display the Change Details.") ?>
</p>
<p>
  <?php echo gettext ("Any changes in the text of the Bibles will be added to the list the next day.") ?>
</p>
<p>
  <?php echo gettext ("This works if enabled in your personal notifications.") ?>
</p>
<p>
  <?php echo gettext ("There is a number at the right of each change.") ?>
</p>
<p>
  <?php echo gettext ("It indicates the number of notes you are subscribed to or that have been assigned to you.") ?>
</p>
<p>
  <?php echo gettext ("The change notifications are for you only.") ?>
  <?php echo gettext ("Your colleagues have their own lists.") ?>
</p>
