<h1><?php echo gettext ("Styles") ?></h1>
<p><?php echo gettext ("This lists the currently available stylesheets.") ?> <a href="indexm.php?new="><?php echo gettext ("You can create a new stylesheet.") ?></a></p>
<?php foreach ($this->sheets as $sheet) { ?>
  <p><a href="sheetm.php?name=<?php echo $sheet ?>"><?php echo $sheet ?></a></p>
<?php } ?>
