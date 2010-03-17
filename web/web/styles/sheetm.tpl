<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Stylesheet{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Stylesheet{/t} {$name}</h1>
    
    <p>{t}This shows the various styles contained in this stylesheet.{/t} [<a href="indexm.php?delete={$name}">{t}delete{/t}]</a></p>
    
    <table>
      <thead>
        <tr>
          <td>{t}Style{/t}</td>
          <td>{t}Name{/t}</td>
          <td></td>
        </tr>
      </thead>
      <tbody>
        {section name=offset loop=$markers} 
        <tr>
          <td><a href="view.php?sheet={$name}&style={$markers[offset]}">{$markers[offset]}</a></td>
          <td>{$names[offset]}</td>
          <td>[<a href="sheetm.php?name={$name}&delete={$markers[offset]}">{t}delete{/t}]</a></td>
        </tr>
        {/section} 
      </tbody>
    </table>

    <p>{t}You can modify this stylesheet. To do that, make changes to the existing xml data in the textbox below, and then submit it. It will overwrite whatever was there before.{/t}</p>

    <form action="{$php_self}?name={$name}" name="form" method="post">
      <p><textarea name="data">{$xml}</textarea></p>
      <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
    </form>

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
