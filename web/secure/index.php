<?php
  session_start();
  require_once 'securesession.class.php';
  $ss = new SecureSession();
  $ss->check_browser = true;
  $ss->check_ip_blocks = 2;
  $ss->secure_word = 'SALT_';
  $ss->regenerate_id = true;
  if (!$ss->Check() || !isset($_SESSION['logged_in']) || !$_SESSION['logged_in'])
  {
    header('Location: login.php');
    die();
  }
?>
<html>
<head>
<title>SecureSession Sample</title>
</head>
<body>
You are successfully logged in!
</body>
</html>