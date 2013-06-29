<h1><?php echo gettext ("Suppressed check results") ?></h1>
<p><a href="index.php">[<?php echo gettext ("back") ?>] <a href="#help">[<?php echo gettext ("help") ?>]</a></p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<?php foreach ($this->ids as $offset => $id) { ?>
  <p style="color:grey;">
  <a href="suppress.php?release=<?php echo $id ?>">
  ✗ 
  </a>
  <?php echo $this->data[$offset] ?>
  </p>
<?php } ?>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("The page lists the suppressed results of the checks.") ?>
</p>
<p>
  <?php echo gettext ("Click ✗ to remove the items from the suppressed list.") ?>
  <?php echo gettext ("It will be visible again in the list of check results.") ?>
</p>
