<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Logbook{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Logbook{/t}</h1>

    <table>
      <thead>
        <tr>
          <td><a href="logbook.php?delete=all">[{t}delete all{/t}]</a> {t}Timestamp{/t}</td>
          <td></td>
          <td>{t}Event{/t}</td>
        </tr>
      </thead>
      <tbody>
        {section name=offset loop=$timestamps} 
        <tr>
          <td>{$timestamps[offset]}</td>
          <td>|</td>
          <td>{$events[offset]}</td>
        </tr>
        {/section} 
      </tbody>
    </table>
    


    {include file=../assets/footer_full.tpl} 
  </body>
</html>
