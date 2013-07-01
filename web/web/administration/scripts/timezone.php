<h1><?php echo gettext ("Site timezone") ?></h1>
<p><?php echo gettext ("This page deals with the default timezone for the site.") ?></p>
<p><?php echo gettext ("Visitors will use this default timezone.") ?></p>
<?php if ($this->timezone != "") { ?>
  <p><?php echo gettext ("Current timezone:") ?> <?php echo $this->timezone ?>.</p>
<?php } else { ?>
  <p><?php echo gettext ("The default timezone has not yet been set.") ?></p>
<?php } ?>
<p><a href="timezone.php?timezone="><?php echo gettext ("Change the timezone.") ?></a></p>
<p><?php echo gettext ("Those who have an account on this site can override this by setting their own preferred timezone.") ?></p>
