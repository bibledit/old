<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
?>
<h2><?php echo gettext ("Edit resource") ?> "<?php echo $this->name ?>"</h2>
<h3><?php echo gettext ("Introduction") ?></h3>
<p>
  <?php echo gettext ("Bibledit-Web displays external resources to the user.") ?>
  <?php echo gettext ("Bibledit-Web attempts to extract the correct passage before passing it on to the user.") ?>
</p>
<p>
  <?php echo gettext ("This is how it works:") ?>
  <?php echo gettext ("Bibledit-Web notices the active passage, for example Genesis 1:1.") ?>
  <?php echo gettext ("Bibledit-Web then runs a script, and passes book, chapter and verse to it.") ?>
  <?php echo gettext ("The script fetches a page from the web.") ?>
  <?php echo gettext ("The script filters this web page.") ?>
  <?php echo gettext ("The script outputs the filtered content.") ?>
  <?php echo gettext ("Bibledit-Web captures the output of the script, and passes it to the user.") ?>
</p>
<p>
  <?php echo gettext ("This method of serving content saves the user a lot of bandwidth.") ?>
  <?php echo gettext ("Bibledit-Web strips a lot of stuff away, and only serves the essentials to the user.") ?>
</p>
<p>
  <?php echo gettext ("On this page you can edit and test the script.") ?>
</p>
<p><a href="admin.php">[<?php echo gettext ("back") ?>]</a></p>
<form action="edit.php?name=<?php echo $this->name ?>" name="name" method="post">
  <h3><?php echo gettext ("Command") ?></h3>
  <p>
    <?php echo gettext ("Bibledit-Web runs a separate process to fetch the external resource.") ?>
    <?php echo gettext ("This is for improved security.") ?>
    <?php echo gettext ("Enter the shell command to run the script.") ?>
    <?php echo gettext ("Samples:") ?>
  </p>
  <p>php script</p>
  <p>perl script</p>
  <p>python script</p>
  <p>bash script</p>
  <p>
    <?php echo gettext ("The string 'script' should always be there, because during execution Bibledit-Web replaces it with the filename of the script.") ?>
  </p>
  <input type="text" name="command" value="<?php echo $this->command ?>" />
  <h3><?php echo gettext ("Code") ?></h3>
  <p>
    <?php echo gettext ("Enter the code of the script manually or select a default template") ?>
    <select name="default">
      <?php foreach ($this->defaults as $default) { ?>
      <option value="<?php echo $default ?>"><?php echo $default ?></option>
      <?php } ?>
    </select>
    <?php echo gettext ("and save it.") ?>
  </p>
  <textarea name="code"><?php echo $this->code ?></textarea>
  <h3><?php echo gettext ("Save and test") ?></h3>
  <select name="book">
    <?php foreach ($this->books as $offset => $book) { ?>
    <option <?php if ($this->book == $book) echo 'selected="selected"' ?> value="<?php echo $book ?>"><?php echo $this->booknames [$offset] ?></option>
    <?php } ?>
  </select>
  <input type="text" name="chapter" value="<?php echo $this->chapter ?>" />
  :
  <input type="text" name="verse" value="<?php echo $this->verse ?>" />
  -
  <input type="submit" name="save" value=<?php echo gettext ("Submit") ?> />
</form>
<br>
<h3><?php echo gettext ("Result") ?></h3>
<p><?php echo gettext ("Output of running the script:") ?></p>
<textarea><?php echo $this->output ?></textarea>
<br>
<?php if ($this->posted) { ?>
<script type="text/javascript">
$(window).load (function () {
  $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 500);
});
</script>
<?php } ?>
