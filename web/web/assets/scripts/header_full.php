<table width="100%">
<tr>
<td>
<a href="../start/index.php"><?php echo gettext ("Start") ?></a>
</td>
<td align="right">
<?php  
if ($this->user == "") {
  if ($this->logging_in != true) {
?>
    <a href="<?php echo $this->header_path_modifier ?>session/login.php?request=<?php echo $this->request ?>"><?php echo gettext ("Login") ?></a>
<?php  
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
