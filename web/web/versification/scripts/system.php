<h1><?php echo gettext ("Versification system") ?> <?php echo $this->name ?></h1>

<p><?php echo gettext ("This shows how many verses each chapter of the Bible has according to this versification system, and how many chapters each book has. The data is given in two formats. First in human readable form, and then in xml format if you scroll further down the page.") ?></p>

<p><a href="index.php?delete=<?php echo $this->name ?>"><?php echo gettext ("Delete this versification") ?></a></p>

<table>
  <thead>
    <tr>
      <td><?php echo gettext ("Book") ?></td>
      <td><?php echo gettext ("Chapter") ?></td>
      <td><?php echo gettext ("Verses") ?></td>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($this->books as $offset => $book) { ?>
    <tr>
      <td><?php echo $book ?></td>
      <td><?php echo $this->chapters[$offset] ?></td>
      <td><?php echo $this->verses[$offset] ?></td>
    </tr>
    <?php } ?>
  </tbody>
</table>

<pre>
<?php echo $this->xml ?>
</pre>    

<p><?php echo gettext ("You can modify this versification system. To do that, copy the existing xml data from this page into an editor, make the changes, and paste the data back into the textbow below, and then submit it. It will overwrite whatever was there before.") ?></p>

<form action="{$php_self}?name=<?php echo $this->name ?>" name="form" method="post">
  <p><textarea name="data"></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></p>
</form>
