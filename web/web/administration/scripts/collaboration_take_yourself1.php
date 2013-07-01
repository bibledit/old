<h1><?php echo gettext ("Take local data") ?></h1>
<?php if ($this->object == "consultationnotes") { ?>
  <p><?php echo gettext ("Consultation Notes") ?></p>
<?php } else { ?>
  <p><?php echo gettext ("Bible") ?>: <?php echo $this->object ?></p>
<?php } ?>
<p><?php echo gettext ("Repository URL") ?>: <?php echo $this->url ?></p>
<p><?php echo gettext ("Your data is being pushed to the remote repository.") ?></p>
<?php foreach ($this->contents as $line) { ?>
  <p><code><?php echo $line ?></code></p>
<?php } ?>
