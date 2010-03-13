<html>
<body>
<?php


require("constants.php");


// GET the variables.
$channel = stripslashes($_GET["channel"]);
$subject = stripslashes($_GET["subject"]);
$message = stripslashes($_GET["message"]);

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
$filename_message = "messages/"."$identifier"."."."$channel";


// Write the message file.
$file_pointer = fopen($filename_message,'w');
flock($file_pointer, LOCK_EX);
fwrite($file_pointer,"$subject");
fwrite($file_pointer,"\n");
fwrite($file_pointer,"$message");
fwrite($file_pointer,"\n");
flock($file_pointer,LOCK_UN);
fclose($file_pointer);


?>
</body>
</html>
