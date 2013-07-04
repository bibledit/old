<h1><?php echo gettext ("Importing Bible data") ?></h1>
<p><?php echo gettext ("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<br>
<p><?php echo gettext ("This page assists you with importing USFM data into the Bible.") ?></p>
<br>
<p><?php echo gettext ("The configuration of the server on which Bibledit-Web runs imposes some limits on the size of the file to be uploaded. The size of the file should not exceed either of the two limits given below:") ?></p>
<p><?php echo gettext ("Maximum size of a file to be posted:") ?> <?php echo $this->post_max_size ?></p>
<p><?php echo gettext ("Maximum size of a file to be uploaded:") ?> <?php echo $this->upload_max_filesize ?></p>
<p><?php echo gettext ("The file to be uploaded can be a USFM file, or a compressed archive with USFM files.") ?></p>
<br>
<form enctype="multipart/form-data" action="import2.php?bible=<?php echo $this->bible ?>" method="POST">
  <p><?php echo gettext ("Choose the USFM file to be uploaded.") ?></p>
  <input name="data" type="file" />
  <p><?php echo gettext ("Then press the Upload button.") ?></p>
  <input type="submit" value="<?php echo gettext ("Upload") ?>" />
</form>
<p><?php echo gettext ("Uploading may take some time. It depends on the speed of your network, and on the size of the file.") ?></p>
<br>
<p><?php echo gettext ("If uploading fails, it may be because the file is too large, or the network too slow.") ?></p>
