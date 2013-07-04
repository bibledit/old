<h1><?php echo gettext ("Importing Bible data, step 2") ?></h1>
<p><?php echo gettext ("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<br>
<p><?php echo gettext ("The file has been uploaded successfully.") ?></p>
<p><?php echo gettext ("Filename") ?>: <?php echo $this->filename ?></p>
<h3><a href="import3.php?bible=<?php echo $this->bible ?>&file=<?php echo $this->filename ?>"><?php echo gettext ("Next") ?></a></h3>
