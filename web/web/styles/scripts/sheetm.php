<h1><?php echo gettext ("Stylesheet") ?> <?php echo $this->name ?></h1>

<p>
  <?php echo gettext ("This shows the various styles contained in this stylesheet.") ?>
  [<a href="indexm.php?delete=<?php echo $this->name ?>"><?php echo gettext ("delete") ?>]</a></p>

<table>
  <thead>
    <tr>
      <td><?php echo gettext ("Style") ?></td>
      <td><?php echo gettext ("Name") ?></td>
      <td></td>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($this->markers as $offset => $marker) { ?>
    <tr>
      <td><a href="view.php?sheet=<?php echo $this->name ?>&style=<?php echo $marker ?>"><?php echo $marker ?></a></td>
      <td><?php echo $this->names[$offset] ?></td>
      <td>[<a href="sheetm.php?name=<?php echo $this->name ?>&delete=<?php echo $marker ?>"><?php echo gettext ("delete") ?>]</a></td>
    </tr>
    <?php } ?>
  </tbody>
</table>

<p><?php echo gettext ("You can modify this stylesheet. To do that, make changes to the existing xml data in the textbox below, and then submit it. It will overwrite whatever was there before.") ?></p>

<form action="sheetm.php?name=<?php echo $this->name ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->xml ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></p>
</form>
