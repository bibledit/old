<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Export{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Export{/t}</h1>
    <p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
    <p>
      {t}This shows the whole Bible exported into Unified Standard Format Markers data.{/t}
      <a href="../usfm/usfm.htm">{t}See the Unified Standard Format Markers User Reference for more information about this format.{/t}</a>
    </p>
    <p><textarea>{$data}</textarea></p>
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
