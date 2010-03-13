<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Versification system{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Versification system{/t} {$name}</h1>
    
    <p>{t}This shows how many verses each chapter of the Bible has according to this versification system, and how many chapters each book has. The data is given in two formats. First in human readable form, and then in xml format if you scroll further down the page.{/t}</p>
    
    <p><a href="index.php?delete={$name}">{t}Delete this versification{/t}</a></p>

    <table>
      <thead>
        <tr>
          <td>{t}Book{/t}</td>
          <td>{t}Chapter{/t}</td>
          <td>{t}Verses{/t}</td>
        </tr>
      </thead>
      <tbody>
        {section name=offset loop=$books} 
        <tr>
          <td>{$books[offset]}</td>
          <td>{$chapters[offset]}</td>
          <td>{$verses[offset]}</td>
        </tr>
        {/section} 
      </tbody>
    </table>

<pre>
{$xml}
</pre>    

    <p>{t}You can modify this versification system. To do that, copy the existing xml data from this page into an editor, make the changes, and paste the data back into the textbow below, and then submit it. It will overwrite whatever was there before.{/t}</p>

    <form action="{$php_self}?name={$name}" name="form" method="post">
      <p><textarea name="data"></textarea></p>
      <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
    </form>

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
