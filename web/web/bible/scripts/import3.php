<h1><?php echo gettext ("Importing Bible data, step 3") ?></h1>
<p><?php echo gettext ("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<br>
<p><?php echo gettext ("The file or files were unpacked.") ?></p>
<p><?php echo gettext ("Location") ?>: <?php echo $this->location ?></p>
<h3><a href="import4.php?bible=<?php echo $this->bible ?>&location=<?php echo $this->location ?>"><?php echo gettext ("Next") ?></a></h3>
