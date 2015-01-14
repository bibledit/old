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
<h3>gettext("Consistency")</h3>
<p><a href="#help">[gettext("help")]</a></p>
<br>
<table>
<tr style="vertical-align: top;">
<td style="width:10%;">
  <p>gettext("Passages:")</p>
  <textarea id="passages"></textarea>
  <p>gettext("Translations:")</p>
  <textarea id="translations"></textarea>
</td>
<td style="width:1%;"></td>
<td>
  <div id="status"><img src="../pix/loading.gif"></div>
  <div id="texts"></div>
</td>
</tr>
</table>
<br>
<a id="help"></a>
<h4>gettext("Description")</h4>
<p>
  gettext("This assists with checking how consistent words have been translated in the Bible.")
</p>
<h4>gettext("Instructions")</h4>
<ol>
  <li>gettext("Enter book, chapter and verse in the 'Passages:' box.")</li>
  <p>gettext("System: The web page will load the verse text for the passages.")</p>
</ol>
<ol>
  <li>
    gettext("Enter key word text you want to search for in the 'Translations:' box.")
    gettext("One word per line.")
    </li>
  <p>gettext("System: The web page will mark the translations in the verse text.")</p>
</ol>
<h4>gettext("Examples")</h4>
<h5>gettext("Passages:")</h5>
<p>Exod. 37:4-5, 14-15, 27-28</p>
<p>Exod. 38:5-7</p>
<p>Exod. 39:35, 39</p>
<h5>gettext("Translation:")</h5>
<p>gettext("salvation")</p>
<p>gettext("justification")</p>
<p>gettext("redemption")</p>

<h4>gettext("Tips")</h4>
<p>gettext("Add Resources as extra aids for comparison"):
<?php for ($this->resources as $resource) {
  $resource
  <a href="?remove=$resource">[gettext("remove")]</a>
  |
<?php }
<a href="?add=">[gettext("add resource")]</a>
<p>gettext("Add the Consistency tool and the Bible editor in a Workbench for easier checking and editing.")
<script type="text/javascript" src="index.js?##VERSION##"></script>
<script type="text/javascript" src="../navigation/nav.js?##VERSION##"></script>
