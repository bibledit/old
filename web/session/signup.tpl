<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Signup{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
     <h1>{t}Signup{/t}</h1>

    <form action="signup" method="post">
    
      <table>
      
      <p class="error">{$error_message}</p>
    
      <tr>
      <td align="right">{t}Which username would you like to use?{/t}</td>
      <td><input type="text" name="user" maxlength="30" /></td>
      <td class="error">{$username_invalid_message}</td>
      </tr>
      
      <tr>
      <td align="right">{t}Which password?{/t}</td>
      <td><input type="password" name="pass" maxlength="30" /></td>
      <td class="error">{$password_invalid_message}</td>
      </tr>
      
      <tr>
      <td align="right">{t}Your email address?{/t}</td>
      <td><input type="text" name="mail" maxlength="256" /></td>
      <td class="error">{$email_invalid_message}</td>
      </tr>
      
      <tr>
      <td align="right">{$question}</td>
      <td><input type="text" name="answer" maxlength="40" /></td>
      <td class="error">{$answer_invalid_message}</td>
      </tr>
      
      <tr>
      <td align="right"></td>
      <td><input type="submit" name="submit" value={t}Signup{/t} /></td>
      </tr>
      
      </table>
      
      <input type="hidden" name="standard" maxlength="40" value={$standard} />
      
      <p>{t}This quotation may help you answer the question:{/t} {$passage}</p>
      
      
    </form>
    
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
