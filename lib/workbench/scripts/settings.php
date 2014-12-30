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
<h3>gettext("Edit workbench") $this->name</h3>
<form action="settings.php?name=$this->name" method="post">
<p><b>gettext("Top row")</b> gettext ("height"):</p>
<p><input type="text" name="height1" style="width: 18%" value="$this->height1" /></p>
</p>
<p>gettext("URLs"):</p>
<p>
  <input type="text" name="url11" style="width: 18%" value="$this->url11" />
  <input type="text" name="url12" style="width: 18%" value="$this->url12" />
  <input type="text" name="url13" style="width: 18%" value="$this->url13" />
  <input type="text" name="url14" style="width: 18%" value="$this->url14" />
  <input type="text" name="url15" style="width: 18%" value="$this->url15" />
</p>
<p>gettext("Widths"):</p>
<p>
  <input type="text" name="width11" style="width: 18%" value="$this->width11" />
  <input type="text" name="width12" style="width: 18%" value="$this->width12" />
  <input type="text" name="width13" style="width: 18%" value="$this->width13" />
  <input type="text" name="width14" style="width: 18%" value="$this->width14" />
  <input type="text" name="width15" style="width: 18%" value="$this->width15" />
</p>
<br>
<p><b>gettext("Center row")</b> gettext ("height"):</p>
<p><input type="text" name="height2" style="width: 18%" value="$this->height2" /></p>
<p>gettext("URLs"):</p>
<p>
  <input type="text" name="url21" style="width: 18%" value="$this->url21" />
  <input type="text" name="url22" style="width: 18%" value="$this->url22" />
  <input type="text" name="url23" style="width: 18%" value="$this->url23" />
  <input type="text" name="url24" style="width: 18%" value="$this->url24" />
  <input type="text" name="url25" style="width: 18%" value="$this->url25" />
</p>
<p>gettext("Widths"):</p>
<p>
  <input type="text" name="width21" style="width: 18%" value="$this->width21" />
  <input type="text" name="width22" style="width: 18%" value="$this->width22" />
  <input type="text" name="width23" style="width: 18%" value="$this->width23" />
  <input type="text" name="width24" style="width: 18%" value="$this->width24" />
  <input type="text" name="width25" style="width: 18%" value="$this->width25" />
</p>
<br>
<p><b>gettext("Bottom row")</b> gettext ("height"):</p>
<p><input type="text" name="height3" style="width: 18%" value="$this->height3" /></p>
<p>gettext("URLs"):</p>
<p>
  <input type="text" name="url31" style="width: 18%" value="$this->url31" />
  <input type="text" name="url32" style="width: 18%" value="$this->url32" />
  <input type="text" name="url33" style="width: 18%" value="$this->url33" />
  <input type="text" name="url34" style="width: 18%" value="$this->url34" />
  <input type="text" name="url35" style="width: 18%" value="$this->url35" />
</p>
<p>gettext("Widths"):</p>
<p>
  <input type="text" name="width31" style="width: 18%" value="$this->width31" />
  <input type="text" name="width32" style="width: 18%" value="$this->width32" />
  <input type="text" name="width33" style="width: 18%" value="$this->width33" />
  <input type="text" name="width34" style="width: 18%" value="$this->width34" />
  <input type="text" name="width35" style="width: 18%" value="$this->width35" />
</p>
<br>
<p><input type="submit" name="save" value=gettext("Save") /></p>
</form>
<h4>gettext("Samples")</h4>
<p>
  <a href="settings.php?name=$this->name&preset=1">gettext("Editor and Resources")</a>
  |
  <a href="settings.php?name=$this->name&preset=2">gettext("Editor and Notes")</a>
  |
  <a href="settings.php?name=$this->name&preset=3">gettext("Resources, Editor, Notes")</a>
  |
  <a href="settings.php?name=$this->name&preset=4">gettext("Editor and Consistency tool")</a>
</p>
<h4>gettext("Help")</h4>
<p>
  gettext("This page is for configuring the layout of a workbench.")
  gettext("A workbench enables you to display parts of Bibledit or websites side-by-side.")
</p>
<br>
<p>
  gettext("There are three rows:")
  gettext("A top row, a center row, and a bottom row.")
  gettext("Each row has five columns.")
  gettext("Thus there are 15 possible positions.")
</p>
<br>
<p>
  gettext("You can set the height of each of the three rows individually.")
  gettext("The height is set as a percentage of the total height of the browser window.")
  gettext("Or add px to the figure to set the height in pixels instead.")
</p>
<br>
<p>
  gettext("You can enter the URLs of internal Bibledit pages or external pages to display.")
  gettext("Enter their widths as a percentage of the total width of the browser window.")
  gettext("Or add px to set pixels.")
</p>
<br>
<p>
  gettext("The internal URL of a Bibledit function can be found by opening that function in the web browser, and then copying the last two bits from the URL in the address bar.")
  gettext("The Edit page would have this URL:")
  "edit/index.php".
  gettext("The Resources page is")
  "resource/index.php".
  gettext("The Notes page is")
  "notes/index.php".
</p>
<br>
<p>
  gettext("Some useful websites with resources that can be embedded are")
  "http://biblehub.com/interlinear",
  "http://unbound.biola.edu",
  "http://www.biblegateway.com/resources",
  "http://www.biblestudytools.com",
  "http://www.blueletterbible.org".
  gettext("There are many more of them.")
</p>
<br>
<p>
  gettext("To edit two Bibles side-by side, add two editors to the workbench.")
  gettext("Both will initially display the same Bible.")
  gettext("Switch the editors to the desired Bibles, and start editing.")
</p>
<br>
<p>
  gettext("The Notes editor has some preset selectors.")
  gettext("To display all the notes for a certain passage, enter the following URL:")
  "notes/index.php?presetselection=forverse".
  gettext("When this workbench will be opened, it will display all the notes that refer to the focused passage.")
  gettext("Other preset selectors for the Notes editor:")
</p>
<p>
  "notes/index.php?presetselection=assigned"
  -
  gettext("Display all notes assigned to you.")
</p>
<p>
  "notes/index.php?presetselection=subscribed"
  -
  gettext("Display all notes you are subscribed to.")
</p>
<p>
  "notes/index.php?presetselection=subscribeddayidle"
  -
  gettext("Display subscribed notes that have been idle for a day.")
</p>
<p>
  "notes/index.php?presetselection=subscribedweekidle"
  -
  gettext("Display subscribed notes that have been idle for a week.")
</p>
