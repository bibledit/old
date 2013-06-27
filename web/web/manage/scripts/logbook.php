<h1><?php echo gettext ("Logbook") ?></h1>

<p>
<?php  
if (is_numeric ($this->previous)) {
  echo '<a href="logbook.php?page=' . $this->previous . '">««</a>' . "\n";
} else {
  echo '««' . "\n";
}
echo gettext ("Page") . " " . $this->page . "\n";
if (is_numeric ($this->next)) {
  echo '<a href="logbook.php?page=' . $this->next . '">»»</a>' . "\n";
} else {
  echo "»»" . "\n";
}
echo "|" . "\n";
echo '<a href="logbook.php?delete=">' . gettext ("clear") . '</a>' . "\n";
?>
</p>

<?php  
foreach ($this->timestamps as $offset => $timestamp) {
  echo '<p>' . $timestamp . " | " . $this->events[$offset] . "</p>\n";
}
?>
