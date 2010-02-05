<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" 
>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    </meta>
    <title>{t}Database{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css">
    </link>
  </head>
  <body>
     {include file=../assets/header_full.tpl} 
    <h1>
       {t}Database{/t} 
    </h1>
<p>Here you can see whether the database of the site is working well.</p>

<p>{t}Number of tables in the database before running the maintenance routine: {/t} {$tables_count_before}</p>

<p>{t}Running maintenance routine{/t}</p>

<p>{t}Number of tables in the database after the routine: {/t} {$tables_count_after}</p>

     {include file=../assets/footer_full.tpl} 
  </body>
</html>
