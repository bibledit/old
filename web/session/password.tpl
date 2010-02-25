<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Password{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>

  <body onload="document.form.user.focus();">
    {include file=../assets/header_full.tpl} 
    <h1>{t}New password{/t}</h1>

    <form action="password.php" name="form" method="post">
    
    <table>
    
    <p class="success">{$success_message}</p>

    {if $success_message == ""}

    <p>{t}If you lost the password, submit your username or email address, and a new password will be emailed to you.{/t}</p>
    
    <tr>
    <td align="right">{t}Username or email address{/t}</td>
    <td><input type="text" name="user" maxlength="30" class="focus" /></td>
    <td class="error">{$error_message}</td>
    </tr>
    
    <tr>
    <td align="right"></td>
    <td><input type="submit" name="submit" value={t}Submit{/t} /></td>
    </tr>
    
    {/if}

    </table>
    
    </form>

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
