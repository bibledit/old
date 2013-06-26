<table width="100%">
  <tr>
    <td>
<?php  
// Administration menu is only applicable to the site administrator.
if ($this->level >= 6) {
  echo '<a href="' . $this->header_path_modifier . 'administration/index.php">' . gettext ("Administration") . "</a>\n";
}
// Management menu is only applicable to the manager and higher roles.
if ($this->level >= 5) {
  echo '<a href="' . $this->header_path_modifier . 'manage/index.php">' . gettext ("Management") . "</a>\n";
}
// Translation menu is only applicable to the translator and higher roles.
if ($this->level >= 4) {
  echo '<a href="' . $this->header_path_modifier . 'translate/index.php">' . gettext ("Translation") . "</a>\n";
}
if ($this->level >= 3) {
  echo '<a href="' . $this->header_path_modifier . 'consultations/index.php">' . gettext ("Consultations") . "</a>\n";
}
if ($this->level >= 2) {
  echo '<a href="' . $this->header_path_modifier . 'members/index.php">' . gettext ("Members") . "</a>\n";
}
echo '<a href="' . $this->header_path_modifier . 'help/index.php">' . gettext ("Help") . "</a>\n";
?>
    </td>
    <td align="right">
<?php  
if ($this->user == "") {
  if ($this->logging_in != true) {
    echo '<a href="' . $this->header_path_modifier . 'session/login.php">' . gettext ("Login") . "</a>\n";
  }
} else {
  if ($this->level >= 2) {
    echo '<form action="' . $this->header_path_modifier . 'search/search.php" method="get" name="search" id="search">' . "\n";
    echo '  <input name="q" type="text" value="' . $this->query . '"/>' . "\n";
    echo "  <input type=\"submit\" value=\"" . gettext ("Search") . "\" onClick=\"this.value = '" . gettext ("Searching") . "...'; return true;\" />\n";
    echo "</form>\n";
  }
  echo '<a href="' . $this->header_path_modifier . 'user/index.php">' . $this->user . "</a>\n";
}
?>
    </td>
  </tr>
</table>
