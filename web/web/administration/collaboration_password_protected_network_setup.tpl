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

  {include file=../assets/install_git.tpl} 

  <h1>{t}Password protected network repository setup{/t}</h1>
  {if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
  {else}
  <p>{t}Bible{/t}: {$object}</p>
  {/if}
  <p>{t}In this tutorial you are going to create a password protected git repository on the Internet.{/t}</p>
  <p>{t}You can host the repository on github.com. The tutorial assumes that user "joe" has an account on github.com. His password is "pazz". He has created a repository with the name "repo".{/t}</p>
  <p>{t}To test access to the new repository from another computer on the network, type:{/t}</p>
  <pre>git ls-remote https://joe:pazz@github.com/joe/repo.git</pre>
  <p>{t}If everything is fine, no errors will occur.{/t}</p>

  <a name="url"></a>
  <h1>{t}Repository URL{/t}</h1>
  <p>{t}The repository that was created in this tutorial has the following URL:{/t}</p>
  <pre>https://joe:pazz@github.com/joe/repo.git</pre>
  <p>{t}Your own repository that you have created has a URL too, the one that will be used from now on. Please enter it below, and press the Submit button to save it.{/t}</p>
  <form action="collaboration_password_protected_network_setup.php?object={$object}#url" name="url" method="post">
    <p><input type="text" name="urlvalue" value="{$url}" /></p>
    <p><input type="submit" name="url" value={t}Submit{/t} /></p>
  </form>

  <h2><a href="collaboration_repo_read.php?object={$object}">{t}Next page{/t}</a></h2>

