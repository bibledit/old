<h1><?php echo gettext ("Test read access") ?></h1>

<?php if ($this->object == "consultationnotes") { ?>
<p><?php echo gettext ("Consultation Notes") ?></p>
<?php } else { ?>
<p><?php echo gettext ("Bible") ?>: <?php echo $this->object ?></p>
<?php } ?>
<p><?php echo gettext ("Repository URL") ?>: <?php echo $this->url ?></p>

<p><?php echo gettext ("Bibledit-Web attempted to read data from this repository. This is the result:") ?></p>  

<p><code><?php echo $this->read_access_result ?></code></p>

<p class="error"><?php echo $this->error_message ?></p>
<p class="success"><?php echo $this->success_message ?></p>

<p><?php echo gettext ("The shell command that was used is:") ?> <code><?php echo $this->command ?></code></p>  

<p><?php echo gettext ("If you'd like to retry, after updating the URL, press button Submit.") ?></p>
<form action="collaboration_repo_read.php?object=<?php echo $this->object ?>" name="url" method="post">
  <p><input type="text" name="urlvalue" value="<?php echo $this->url ?>" /></p>
  <p><input type="submit" name="url" value=<?php echo gettext ("Submit") ?> /></p>
</form>

<h2><a href="collaboration_repo_clone.php?object=<?php echo $this->object ?>"><?php echo gettext ("Next page") ?></a></h2>
