<p>
<?php  
  echo gettext ("Bible") . "\n";
  echo '<a href="' . $this->caller . '?session=' . $this->session . '&switchbible=">' . $this->bible . "</a>\n";
  echo "|\n";
  echo gettext ("Go to") . "\n";
  if (is_numeric ($this->previous_book)) {
    echo '<a href="' . $this->caller . '?session=' . $this->session . '&switchbook=' . $this->previous_book . '&switchchapter=1&switchverse=1">««</a>' . "\n";
  } else {
    echo "««\n";
  }
  echo '<a href="' . $this->caller . '?session=' . $this->session . '&switchbook=">' . $this->book_name . "</a>\n";
  if (is_numeric ($this->next_book)) {
    echo '<a href="' . $this->caller . '?session=' . $this->session . '&switchbook=' . $this->next_book . '&switchchapter=1&switchverse=1">»»</a>' . "\n";
  } else {
    echo "»»\n";
  }
  if (is_numeric ($this->previous_chapter)) {
    echo '<a href="' . $this->caller . '?session=' . $this->session . '&book=' . $this->book . '&switchchapter=' . $this->previous_chapter . '&switchverse=1">««</a>' . "\n";
  } else {
    echo "««\n";
  }
  echo '<a href="' . $this->caller . '?session=' . $this->session . '&book=' . $this->book . '&switchchapter=">' . gettext ("chapter") . ' ' . $this->chapter . "</a>\n";
  if (is_numeric ($this->next_chapter)) {
    echo '<a href="' . $this->caller . '?session=' . $this->session . '&book=' . $this->book . '&switchchapter=' . $this->next_chapter . '&switchverse=1">»»</a>' . "\n";
  } else {
    echo "»»\n";
  }
  if (is_numeric ($this->previous_verse)) {
    echo '<a href="' . $this->caller . '?session=' . $this->session . '&book=' . $this->book . '&switchverse=' . $this->previous_verse . '">««</a>' . "\n";
  } else {
    echo "««\n";
  }
  echo '<a href="' . $this->caller . '?session=' . $this->session . '&book=' . $this->book . '&switchverse=">' . gettext ("verse") . ' ' . $this->verse . "</a>\n";
  if (is_numeric ($this->next_verse)) {
    echo '<a href="' . $this->caller . '?session=' . $this->session . '&book=' . $this->book . '&switchverse=' . $this->next_verse . '">»»</a>' . "\n";
  } else {
    echo "»»\n";
  }
  echo "|\n";
  echo '<a href="' . $this->caller . '?session=' . $this->session . '">↻</a>' . "\n";
?>
</p>
