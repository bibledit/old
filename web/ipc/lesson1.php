<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
	"http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
<head>
<title>Contact Form</title>
<meta http-equiv="Content-Type" 
   content="text/html; charset=iso-8859-1">
</head>
<body>
<?php
   if ($_SERVER['REQUEST_METHOD'] != 'POST'){
      $me = $_SERVER['PHP_SELF'];
?>
   <form name="form1" method="post"
         action="<?php echo $me;?>">
      <table border="0" cellspacing="0" cellpadding="2">
         <tr>
            <td>Name:</td>
            <td><input type="text" name="Name"></td>
         </tr>
         <tr>
            <td>Subject</td>
            <td><input type="text" name="Subject"></td>
         </tr>
         <tr>
            <td valign="top">Message:</td>
            <td><textarea name="MsgBody"></textarea></td>
         </tr>
         <tr>
            <td>&nbsp;</td>
            <td><input type="submit" name="Submit"
               value="Send"></td>
         </tr>
      </table>
   </form>
<?php
   } else {
      error_reporting(0);
      $recipient = 'teus@localhost';
      $subject = stripslashes($_POST['Subject']);
      $from = stripslashes($_POST['Name']);
      $msg = "Message from: $from\n\n".stripslashes($_POST['MsgBody']);
      if (mail($recipient, $subject, $msg))
         echo nl2br("<b>Message Sent:</b>
         To: $recipient
         Subject: $subject
         Message:
         $msg");
      else
         echo "Message failed to send";
}
?>
</body>
</html>
