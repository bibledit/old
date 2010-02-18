<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Account{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>

  <body onload="document.form.currentpassword.focus();">
    {include file=../assets/header_full.tpl} 
    <h1>{t}Account{/t} {$username}</h1>

    <form action="account" name="form" method="post">
    
    <table>

    <p class="error">{$error_message}</p>
    {section name=offset loop=$success_messages} 
    <p class="success">{$success_messages[offset]}</p>
    {/section} 


    
    <p>{t}You can change your password or your email address.{/t}</p>
    
    <tr>
    <td align="right">{t}Current password{/t}</td>
    <td><input type="text" name="currentpassword" maxlength="30" /></td>
    <td class="error">{$current_password_invalid_message}</td>
    </tr>
    
    <tr>
    <td align="right">{t}New password{/t}</td>
    <td><input type="text" name="newpassword" maxlength="30" /></td>
    <td class="error">{$new_password_invalid_message}</td>
    </tr>

    <tr>
    <td align="right">{t}Again new password{/t}</td>
    <td><input type="text" name="newpassword2" maxlength="30" /></td>
    <td class="error">{$new_password2_invalid_message}</td>
    </tr>

    <tr>
    <td align="right">{t}New email address{/t}</td>
    <td><input type="text" name="newemail" maxlength="256" /></td>
    <td class="error">{$new_email_invalid_message}</td>
    </tr>

    <tr>
    <td align="right"></td>
    <td><input type="submit" name="submit" value={t}Submit{/t} /></td>
    </tr>
    
    </table>
    
    </form>

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
