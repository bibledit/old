<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
<div id="topcontainer">

<?php if ($this->display_topbar) { ?>

  <div id="topbar">
  
    <?php echo $this->mainmenu ?>
  
      <?php
      if ($this->user == "") {
        if ($this->logging_in != true) {
      ?>
          <ul id="usermenu" class="menu">
          <li><a href="<?php echo $this->header_path_modifier ?>session/login.php?request=<?php echo $this->request ?>"><?php echo Locale_Translate::_("Login") ?></a></li>
          </ul>
      <?php
        }
      } else {
        echo $this->usermenu;
  
        if ($this->level >= 2) {
          echo '<ul id="searchwrapper" class="menu">' . "\n";
          echo   '<form action="' . $this->header_path_modifier . 'search/search.php" method="get" name="search" id="search">' . "\n";
          echo   '  <input name="q" type="text" value="' . $this->search_query . '"/>' . "\n";
          echo   "  <input type=\"submit\" value=\"" . Locale_Translate::_("Search") . "\" onClick=\"this.value = '" . Locale_Translate::_ ("Searching") . "...'; return true;\" />\n";
          echo   "</form>\n";
          echo '</ul>' . "\n";
        }
      }
      ?>
  
    <div class="clearfix"></div>
  </div>
  
  <?php if ($this->display_navigator) { ?>
    <div id="versepickerwrapper"><?php echo $this->navigationHtml ?></div>
    <script type="text/javascript" src="../navigation/passage.js?<?php echo Config_Logic::version () ?>"></script>
    <?php echo $this->navigationCode; ?>
    <div class="clearfix"></div>
  <?php } ?>

<?php } ?>
</div>
<?php if ($this->display_topbar) { ?>
  <?php if ($this->display_navigator) { ?>
  <script type="text/javascript">var displayNavigator = true;</script>
  <?php } ?>
<?php } ?>
