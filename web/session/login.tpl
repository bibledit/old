<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" 
>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    </meta>
    <title>{t}Login{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css">
    </link>
    <script type="text/javascript"> 
    </script> 
  </head>

  <body OnLoad="document.form.user.focus();">
    {include file=../assets/header_full.tpl} 
    <h1>{t}Login{/t}</h1>

<form action="login" name="form" method="post">

<table>

<p class="error">{$error_message}</p>

<tr>
<td align="right">{t}Username or email address{/t}</td>
<td><input type="text" name="user" maxlength="30" class="focus" /></td>
<td class="error">{$username_or_email_invalid_message}</td>
</tr>

<tr>
<td align="right">{t}Password{/t}</td>
<td><input type="password" name="pass" maxlength="30" /></td>
<td class="error">{$password_invalid_message}</td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value={t}Login{/t} /></td>
</tr>

</table>

<p>{t}Forgot password? Request the administrator for help.{/t}</p>

<p>{t}Not registered?{/t} <a href="signup">{t}Sign up!{/t}</a></p>
</form>

     {include file=../assets/footer_full.tpl} 
  </body>
</html>
