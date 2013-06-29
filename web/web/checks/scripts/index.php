<h1><?php echo gettext ("Checks") ?></h1>
<p><a href="#help">[<?php echo gettext ("help") ?>]</a></p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<?php foreach ($this->ids as $offset => $id) { ?>
  <p><a href="index.php?approve=<?php echo $id ?>">
  ✔
  <a href="index.php?delete=<?php echo $id ?>">
  ✗
  </a><a href="index.php?goto=<?php echo $id ?>" target="_blank">
  <?php echo $this->data[$offset] ?>
  </a></p>
<?php } ?> 
<br>
<a id="help"></a>
<p><?php echo gettext ("The page displays a list of results of running the checks.") ?></p>
<p><?php echo gettext ("Click ✔ to approve and suppress the entry.") ?></p>
<p>
  <?php echo gettext ("Click ✗ to remove the entry for just now.") ?>
  <?php echo gettext ("It will be back next day, unless the text of the Bible is corrected.") ?>
</p>
<p><?php echo gettext ("Click an entry to open the chapter in the text editor.") ?></p>
<p><?php echo gettext ("The Manager should enable the checks.") ?></p>
<p><a href="suppress.php"><?php echo gettext ("Suppressed entries") ?></a></p>
