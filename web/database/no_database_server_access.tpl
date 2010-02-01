<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
<title>{t}Database access{/t}</title>
<link rel="stylesheet" type="text/css" href="css/stylesheet.css"></link>
</head>

<body>
<h1>{t}Cannot access the database server{/t}</h1>

<p>{t}An error occurred while connecting to the MySQL database server. This is the error:{/t}</p>

<p><strong>{$database_error}</strong></p>


<ol>
   <li>
<p>{t}If the problem lies with the MySQL server host, then enter the correct value for the database_host in file db/credentials.php. For a local installation of Bibledit, this would normally be "localhost".{/t}</p> 
</li>
   <li> 
<p>{t}If the problem lies with the user, then enter the correct value for the database_user in the file mentioned. The default value set upon Bibledit installation is "root".{/t}</p> 
 </li>
   <li> 
<p>{t}If the problem lies with the password, then enter the correct value for the database_pass in that file. This value can also be set when configuring the Bibledit package. For example, to set the password to "mysecret", do this:{/t}</p> 
<pre>./configure  --with-mysql-root-password=mysecret</pre>
 </li>
   <li>
<p>{t}If the problem is that is can't connect to the local MySQL server through a given socket, then the database server may be down.{/t}</p> 
  </li>
 </ul>

</body>
</html>
