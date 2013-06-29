<h1><?php echo gettext ("Timezone") ?></h1>
<p><?php echo gettext ("Here you can set up your own timezone.") ?></p>
<p><?php echo gettext ("All dates and times will then be shown as relevant to your timezone.") ?></p>
<?php if ($this->timezone != "") { ?>
  <p><?php echo gettext ("Current timezone:") ?> <?php echo $this->timezone ?>.</p>
<?php } else { ?>
  <p><?php echo gettext ("Your timezone has not yet been set.") ?></p>
<?php } ?>
<p><a href="timezone.php?timezone="><?php echo gettext ("Change the timezone.") ?></a></p>
