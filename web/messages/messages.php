<?php


// Outputs a message with information for the user.
// It takes a string or an array of strings.
function message_information ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo $msg;
    echo "<br>\n";
  }
  
}


// Outputs a message with a warning for the user.
// It takes a string or an array of strings.
function message_warning_header ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo "<h1> ", gettext ("Warning"), ": $msg</h1>\n";
  }
  
}


// Outputs a message with a warning for the user.
// It takes a string or an array of strings.
function message_warning ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo gettext ("Warning"), ": $msg<br>\n";
  }
  
}


?>
