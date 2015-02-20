<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
<h2>translate("Edit resource") "$this->name"</h2>
<h3>translate("Introduction")</h3>
<p>
  translate("Bibledit displays fragents of external web pages to the user.")
  translate("Bibledit extracts the correct bits from that web page.")
  translate("It passes that bit to the user.")
  translate("This is how it works:")
  translate("Bibledit notices the active passage, for example Genesis 1:1.")
  translate("Bibledit then calls a defined PHP function contained in a resource PHP script.")
  translate("It passes book, chapter and verse to the function.")
  translate("The function fetches a page from the web.")
  translate("The function filters this web page.")
  translate("The script returns the filtered content.")
  translate("Bibledit passes that to the user.")
  translate("This method of serving content saves the user a lot of bandwidth.")
  translate("Bibledit strips a lot of stuff away, and only serves the essentials to the user.")
</p>
<p>
  translate("On this page you can edit and test the script.")
</p>
<p><a href="admin">[translate("back")]</a></p>
<form action="edit?name=$this->name" name="name" method="post">
  <h3>translate("Code")</h3>
  <p>translate("Enter the code of the script.")</p>
  <textarea name="code">$this->code</textarea>
  <h3>translate("Save and test")</h3>
  <select name="book">
    <?php for ($this->books as $offset => $book) {
    <option <?php if ($this->book == $book) echo 'selected="selected"' value="$book">$this->booknames [$offset]</option>
    <?php }
  </select>
  <input type="text" name="chapter" value="$this->chapter" />
  :
  <input type="text" name="verse" value="$this->verse" />
  -
  <input type="submit" name="save" value=translate("Submit") />
</form>
<br>
<h3>translate("Result")</h3>
<p>translate("Output of running the script:")</p>
<textarea>$this->output</textarea>
<br>
<?php if ($this->posted) {
<script type="text/javascript">
$(window).load (function () {
  $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 500);
});
</script>
<?php }
<h3>translate("Notes")</h3>
<ul>
<li>
  translate("It is possible to make verse references in a Resource clickable in Bibledit while the Resource is being displayed.")
  translate("The code of the script should insert links as follows:")
  &lt;a class="navigate" href="19.90.2"&gt;Ps. 90:2&lt;/a&gt;.
  translate("Bibledit will take care of the rest when it displays the Resource.")
</li>
<li>
  translate("Any changes made to the standard resources that come with Bibledit will be overwritten when upgrading the software.")
</li>
<li>
  translate("The script will be saved to file")
  <code>$this->file;</code>.
  translate("The versification system and the verse mapping can be set in file")
  <code>$this->metafile;</code>.
  translate("The default resource files provide examples.")
</li>
</ul>
