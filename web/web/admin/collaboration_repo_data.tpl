<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Collaboration{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
  {include file=../assets/header_full.tpl} 
  <h1>{t}Repository data{/t}</h1>
  <p>{t}You are now at a stage that the Bible data in Bibledit-Web, and the Bible data in the repository, needs to be synchronized. This means that some action needs to be taken to make these two sets of data the same. Not all the data in Bibledit-Web needs to be synchronized with the repository. It is only the data in the one Bible you are now dealing with.{/t}</p>
  <p>{t}Bible{/t}: {$bible}</p>

  <p>{t}There are two options:{/t}</p>
  <ol>
    <li><em>{t}Take the data in the repository.{/t}</em> {t}Taking the data in the repository means that the Bible in the repository is left as it is, and that a copy of that Bible overwrites your own Bible in Bibledit-Web. This option is used most of the time.{/t} <a href="xxx.php?bible={$bible}&directory={$directory}">{t}Take the Bible in the repository.{/t}</a></li>
    <li><em>{t}Take your own data.{/t}</em> {t}Taking your own data means that your Bible is taken as it is, and that this is copied to the repository, overwriting whatever is there in the repository now.{/t} <a href="xxx.php?bible={$bible}&directory={$directory}">{t}Take your Bible.{/t}</a></li>
  </ol> 
  {include file=../assets/footer_full.tpl} 
  </body>
</html>
