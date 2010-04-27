<?php

// This file is no longer needed, just kept to see how uploading goes.

require("constants.php");


// Filename that contains the message identifier.
$filename_id = "$messagesdir/identifier";
// Get contents, suppress errors in case the file is not there.
@$thenames = file($filename_id);
// Get the identifier and increase it.
$identifier = $thenames[0];
$identifier++;
// Write the new identifier to file.
$fp_id = fopen($filename_id,'w');
fwrite($fp_id,"$identifier");
fclose($fp_id);


// The message file.
$identifier = $identifier + 1000000000;
$filename_message = "$messagesdir/"."$identifier"."."."bibledit";


if(move_uploaded_file($_FILES['uploaded']['tmp_name'], $filename_message)) {
  //echo "The message has been uploaded";
} else {
  echo "There was a problem uploading the message";
}


?> 
