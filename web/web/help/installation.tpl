<h1>{t}Installation{/t}</h1>
<p>{t}Here are some guidelines as to how to install Bibledit-Web.{/t}</p>
<p><a href="07ubuntu1204.php">{t}Version 0.7 on Ubuntu 12.04.{/t}</a></p>
<p><a href="06ubuntu1110.php">{t}Version 0.6.1 on Ubuntu 11.10.{/t}</a></p>
<p><a href="05ubuntu1104.php">{t}Version 0.5 on Ubuntu 11.04.{/t}</a></p>
<h2>{t}Version being developed{/t}</h2>
<p>{t}In case you'd like to use the newest version, the one being developed, you can clone the source code from the git repository.{/t}</p>
<p><a href="http://sites.google.com/site/bibledit/community/development">{t}See the Development page for more details.{/t}</a></p>
<p>{t}Once you have cloned the source code, and have a local repository for the bibledit code, you can then copy directory web/web into your local web server, and get it up and running there. A web server is needed, PHP, and MySQL. If you have copied the web/web directory to, for example, http://localhost/bibledit, then to install the program and create all the databases, you need to run http://localhost/bibledit/setup.php once.{/t}</p>
<h2>{t}Login{/t}</h2>
<p>{t}To log into the application, use the default username "adminusername" and password "adminpassword". These only work if no Administrator's  account exist on the system. It is recommended to create an Administrator's account of your own choice. This will disable the default adminusername account.{/t}</p>
<h2>{t}Open installation{/t}</h2>
<p>{t}You can configure the Bibledit-Web installation to be completely open.{/t}</p>
<pre>./configure --with-open-installation=true</pre>
<p>
{t}It means that there is no need to login to the website.{/t} 
{t}The website appears to be logged in as user "admin" for any visitor.{/t} 
{t}This is useful for demo installations, or installations providing a free service to the the community.{/t}
</p>
