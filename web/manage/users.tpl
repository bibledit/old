<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    </meta>
    <title>{t}Users{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css">
    </link>
  </head>
  <body>
     {include file=../assets/header_full.tpl} 
    <h1>{t}Users{/t}</h1>
    <table>
      <thead>
        <tr>
          <td>{t}Username{/t}</td>
          <td></td>
          <td>{t}ID{/t}</td>
          <td>{t}Role{/t}</td>
          <td></td>
          <td>{t}Email{/t}</td>
        </tr>
      </thead>
      <tbody>
        {section name=offset loop=$usernames} 
        <tr>
          <td>{$usernames[offset]}</td>
          <td><a href="users.php?delete={$usernames[offset]}">[{t}delete{/t}]</a></td>
          <td>{$ids[offset]}</td>
          <td>{$levels[offset]}</td>
          <td><a href="users.php?user={$usernames[offset]}">[{t}change{/t}]</a></td>
          <td>{$emails[offset]}</td>
        </tr>
        {/section} 
      </tbody>
    </table>
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
