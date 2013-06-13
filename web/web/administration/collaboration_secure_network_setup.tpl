
  {include file=../assets/install_git.tpl} 
  {include file=../assets/install_ssh.tpl} 

  <h1>{t}Secure network repository setup{/t}</h1>
  {if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
  {else}
  <p>{t}Bible{/t}: {$object}</p>
  {/if}
  <p>{t}In this tutorial you are going to set up an secure repository on the network.{/t}</p>
  <p>{t}The tutorial uses a git repository on sourceforge.net.{/t}</p>
  <p>{t}Open an account on http://sourceforge.net. We will use the username "joe" in this tutorial.{/t}</p>
  <p>{t}Register a new project on your personal account page on sourceforge.net. Let's call this project "repo".{/t}</p>
  <p>{t}Enable project feature "Git" on this project. This provides the repository that we need.{/t}</p>
  <p>{t}The next thing is to try whether you can read the repository properly. To try that, open a terminal and issue the following command:{/t}</p>
  <code>git ls-remote ssh://joe@git.code.sf.net/p/repo/code</code>
  <p>{t}This command probably will ask for your password, and the first time it could ask you whether you want to continue connecting. But it should show no errors. If it completes without errors, then your secure repository works well.{/t}</p>

  <a name="url"></a>
  <h1>{t}Repository URL{/t}</h1>
  <p>{t}The repository that was created in this tutorial has the following URL:{/t}</p>
  <code>ssh://joe@git.code.sf.net/p/repo/code</code>
  <p>{t}Where it says "joe", you would need to fill in the username that can access the account on sourceforge.net. And where it has "repo" you would need to fill in the name of the project that you created on sourceforge.net.{/t}</p>
 <p>{t}Your own repository that you have created has a URL too, the one that will be used from now on. Please enter it below, and press the Submit button to save it.{/t}</p>
  <form action="collaboration_secure_network_setup.php?object={$object}#url" name="url" method="post">
    <p><input type="text" name="urlvalue" value="{$url}" /></p>
    <p><input type="submit" name="url" value={t}Submit{/t} /></p>
  </form>

  <a name="keys"></a>
  <h1>{t}Secure shell keys{/t}</h1>
  <p>{t}Once manual access to the secure repository works, the next step is to set up secure shell keys. These keys allow Bibledit-Web easy access to the secure repository, without the need to type the password each time.{/t}</p>
 
  <h3>{t}Generating the keys{/t}</h3>
  <p>{t}Here are the keys that can be used. Bibledit has generated or kept them for you.{/t}</p>
  <form action="collaboration_secure_network_setup.php?object={$object}#keys" name="keys" method="post">
    <table class="fullwidth">
      <tr>
        <td>{t}Private key{/t}</td>
        <td>{t}Public key{/t}</td>
      </tr>
      <tr>
        <td><textarea name="privatevalue">{$privatekey}</textarea></td>
        <td><textarea name="publicvalue">{$publickey}</textarea></td>
      </tr>
    </table>
    <input type="submit" name="keys" value={t}Update{/t} /></p>
  </form>
  <p>{t}Optionally, if you would like to use your own private and public keys, you can paste these above, and then press button Update above. Or if you empty the keys, and then press button Update, the keys will be regenerated.{/t}</p>

  <h3>{t}Enter the public key{/t}</h3>
  <p>{t}Go to your personal account at sourceforge.net. See above for this account.{/t}</p>
  <p>{t}On your personal account page at sourceforge.net, choose Services, then choose "Edit SSH Keys for Shell/CVS". Select all the text of the public key, see above, and copy it to the clipboard. Be sure it is the public key, not the private one. In the screen titled "SSH Keys", there is a place where it says "Authorized keys:". Paste the text of the public key into that space on the web page. After checking that everything looks fine, press button "Update". Make sure to enter only the public key, not the private key. Synchronizing the keys into the system has a 10 minute delay.{/t}</p>
  <p>{t}After the delay of 10 minutes has expired you can continue to the next page.{/t}</p>

  <h2><a href="collaboration_repo_read.php?object={$object}">{t}Next page{/t}</a></h2>

