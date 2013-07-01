<h1><?php echo gettext ("Clone repository") ?></h1>

<?php if ($this->object == "consultationnotes") { ?>
<p><?php echo gettext ("Consultation Notes") ?></p>
<?php } else { ?>
<p><?php echo gettext ("Bible") ?>: <?php echo $this->object ?></p>
<?php } ?>
<p><?php echo gettext ("Repository URL") ?>: <?php echo $this->url ?></p>
  
<p><?php echo gettext ("Bibledit-Web clones the remote repository, and optimizes the clone for your use.") ?></p>

<?php foreach ($this->contents as $line) {  ?>
  <p><code><?php echo $line ?></code></p>
<?php } ?>
