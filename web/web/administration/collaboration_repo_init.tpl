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
  <h1>{t}Repository initialization{/t}</h1>
  {if $url == ""}
  <p class="error">{t}No URL has been given for the repository. Please go back to enter the URL.{/t}</p>
  {else}
  <p>{t}Newer versions of git can clone an empty repository, but older versions only clone repositories that have been initialized with some data.{/t}</p>  
  <p>{t}Just to be sure that everything will work well, in this tutorial you are going to put some data into the repository, then try to clone it to see how it now goes.{/t}</p>
  <p>{t}You need to put something into the repository. It does not matter what, anything will do. Let's proceed and put an empty file into the repository. Issue the commands as given:{/t}</p>  
  <pre>
cd
mkdir repository
cd repository
git init
  </pre>
  <p>{t}It will say that an empty Git repository was initialized.{/t}</p>  
  <pre>
touch file
git add file
git commit -a -m init
  </pre>
  <p>{t}It will say that an initial commit was created.{/t}</p>  
  <pre>git push --all {$url}</pre>
  <p>{t}It will give some information, which shows that data was written to the repository. No errors should occur.{/t}</p>  
  <p>{t}Clean out the temporal directory:{/t}</p>  
  <pre>
cd
rm -rf repository
  </pre>
  <p>{t}Now try to clone the repository:{/t}</p>  
  <pre>git clone {$url}</pre>
  <p>{t}It should say that an empty Git repository was created, give some more information, and show that objects were received. It is does this, and there are no errors, then everything is fine. Your repository has been initialized.{/t}</p>  
  <h2><a href="collaboration_repo_read.php?object={$object}">{t}Next page{/t}</a></h2>
  {/if}
  {include file=../assets/footer_full.tpl} 
  </body>
</html>
