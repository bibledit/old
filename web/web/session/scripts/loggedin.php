<h1><?php echo gettext ("Welcome") ?></h1>
<p><?php echo gettext ("You have logged in.") ?></p>
<?php if ($this->mailcount > 0) { ?>
  <p><a href="../user/mail.php"><?php echo gettext ("There is mail for you.") ?></a></p>
<?php } ?>
