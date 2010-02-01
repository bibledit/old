<?php
  session_start();
  require_once 'securesession.class.php';
  $error = '';
  if (isset($_POST['uname']))
  {
    $uname = $_POST['uname'];
    $passwd = $_POST['passwd'];
    if ($uname == 'User' && $passwd == 'password')
    {
      $ss = new SecureSession();
      $ss->check_browser = true;
      $ss->check_ip_blocks = 2;
      $ss->secure_word = 'SALT_';
      $ss->regenerate_id = true;
      $ss->Open();
      $_SESSION['logged_in'] = true;
      header('Location: index.php');
      die();
    }
    else
    {
      $error = 'Incorrect username or password.';
    }
  }
?>

<html>
<head>
<title>SecureSession Sample</title>
</head>
<body>
<?php
  if (!empty($error))
  {
    echo $error;
  }
?>
<form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
Username: <input type="text" name="uname" />
Password: <input type="password" name="passwd" />
<input type="submit" value="Log In" />
</form>
</body>
</html>