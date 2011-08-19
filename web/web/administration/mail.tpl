<h1>{t}Mail{/t}</h1>
<p>{t}This page deals with setting up the email system.{/t}</p>
<p>{t}Bibledit-Web sends out email. It also collects email every five minutes and acts upon these emails.{/t}</p>
<p>{t}It needs an external email account for sending and receiving email.{/t}</p>
<p>{t}The help information on this page assumes that we use a Google mail account. You will have your own email account dedicated to Bibledit-Web.{/t}</p>
<p>{t}The email address is bibletranslation@gmail.com, and the password is pazzwrd.{/t}</p>



<a name="name"></a>
<h2>{t}Site name and email address{/t}</h2>
<form action="mail.php#name" name="email" method="post">
  <p>{t}Our own sample values are given below. Yours will be different.{/t}</p>
  <p>{t}Name: Bible Translation.{/t}</p>
  <p>{t}Email address: bibletranslation@gmail.com.{/t}</p>
  <p>{t}To switch the mailer off, leave the fields empty.{/t}</p>
  <h4>{t}Enter the name and the email address, and press Submit.{/t}</h4>
  <table>
    <tr>
      <td align="right">{t}Name{/t}</td>
      <td><input type="text" name="sitename" maxlength="50" value="{$sitename}" /></td>
    </tr>
    <tr>
      <td align="right">{t}Email address{/t}</td>
      <td><input type="text" name="sitemail" maxlength="50" value="{$sitemail}" /></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="submit" name="email" value={t}Submit{/t} /></td>
      <td class="error">{$site_name_error}</td>
      <td class="success">{$site_name_success}</td>
    </tr>
  </table>
</form>

<a name="retrieve"></a>
<h2>{t}Retrieving mail{/t}</h2>
<form action="mail.php#retrieve" name="retrieve" method="post">
  <p>{t}Our own sample values are given below. Yours will be different.{/t}</p>
  <p>{t}POP3 Host: pop.gmail.com.{/t}</p>
  <p>{t}Username: bibletranslation@gmail.com.{/t}</p>
  <p>{t}Password: pazzwrd.{/t}</p>
  <p>{t}Security: SSL.{/t}</p>
  <p>{t}Port: Leave empty. The right port will be chosen automatically.{/t}</p>
  <h4>{t}Enter the account details, and press Submit.{/t}</h4>
  <p>{t}When you submit the values, Bibledit will attempt to contact this email account and tell you how many messages there are in the Inbox. If something goes wrong, the message given may be cryptic at times.{/t}</p>
  <table>
    <tr>
      <td align="right">POP3 Host</td>
      <td><input type="text" name="storagehost" maxlength="50" value="{$storagehost}" /></td>
    </tr>
    <tr>
      <td align="right">{t}Username{/t}</td>
      <td><input type="text" name="storageusername" maxlength="50" value="{$storageusername}" /></td>
    </tr>
    <tr>
      <td align="right">{t}Password{/t}</td>
      <td><input type="text" name="storagepassword" maxlength="50" value="{$storagepassword}" /></td>
    </tr>
    <tr>
      <td align="right">Security</td>
      <td>
        <select name="storagesecurity">
        <option value="NONE" >None</option>
        <option value="SSL" {if $storagesecurity == "SSL"} selected="selected" {/if}>SSL</option>
        <option value="TLS" {if $storagesecurity == "TLS"} selected="selected" {/if}>TLS</option>
      </td>
    </tr>
    <tr>
      <td align="right">Port</td>
      <td><input type="text" name="storageport" maxlength="10" value="{$storageport}" /></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="submit" name="retrieve" value={t}Submit{/t} onClick="this.value = '{t}Please wait{/t}'; return true;" /></td>
      <td class="error">{$storage_error}</td>
      <td class="success">{$storage_success}</td>
    </tr>
  </table>
</form>

<a name="send"></a>
<h2>{t}Sending mail{/t}</h2>
<form action="mail.php#send" name="send" method="post">
  <p>{t}Our own sample values are given below. Yours will be different.{/t}</p>
  <p>{t}SMTP Host: smtp.gmail.com. If it is left empty, it will use the default sendmail transport.{/t}</p>
  <p>{t}Authentication: LOGIN.{/t}</p>
  <p>{t}Username: bibletranslation@gmail.com.{/t}</p>
  <p>{t}Password: pazzwrd.{/t}</p>
  <p>{t}Security: SSL.{/t}</p>
  <p>{t}Port: 465.{/t}</p>
  <p>{t}If the SMTP server does not need authenticaton, just leave the fields for username and password empty.{/t}</p>
  <h4>{t}Enter the account details, and press Submit.{/t}</h4>
  <p>{t}When you submit new values, Bibledit will attempt to send an email the site's email address. If something goes wrong, the message given may be cryptic at times. Please enter the correct details for this to work.{/t}</p>
  <table>
    <tr>
      <td align="right">SMTP Host</td>
      <td><input type="text" name="sendhost" maxlength="50" value="{$sendhost}" /></td>
    </tr>
    <tr>
      <td align="right">Authentication</td>
      <td>
        <select name="sendauthentication">
        <option value="None" >None</option>
        <option value="Plain" {if $sendauthentication == "Plain"} selected="selected" {/if}>PLAIN</option>
        <option value="Login" {if $sendauthentication == "Login"} selected="selected" {/if}>LOGIN</option>
        <option value="Crammd5" {if $sendauthentication == "Crammd5"} selected="selected" {/if}>CRAM-MD5</option>
      </td>
    </tr>
    <tr>
      <td align="right">{t}Username{/t}</td>
      <td><input type="text" name="sendusername" maxlength="50" value="{$sendusername}" /></td>
    </tr>
    <tr>
      <td align="right">{t}Password{/t}</td>
      <td><input type="text" name="sendpassword" maxlength="50" value="{$sendpassword}" /></td>
    </tr>
    <tr>
      <td align="right">Security</td>
      <td>
        <select name="sendsecurity">
        <option value="NONE" >None</option>
        <option value="SSL" {if $sendsecurity == "SSL"} selected="selected" {/if}>SSL</option>
        <option value="TLS" {if $sendsecurity == "TLS"} selected="selected" {/if}>TLS</option>
      </td>
    </tr>
    <tr>
      <td align="right">Port</td>
      <td><input type="text" name="sendport" maxlength="10" value="{$sendport}" /></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="submit" name="send" value={t}Submit{/t} onClick="this.value = '{t}Please wait{/t}'; return true;" /></td>
      <td class="error">{$send_error}</td>
      <td class="success">{$send_success}</td>
    </tr>
  </table>
</form>
