<h1>{t}New password{/t}</h1>

<form action="password.php" name="form" method="post">

<table>

<p class="success">{$success_message}</p>

{if $success_message == ""}

<p>{t}If you lost the password, submit your username or email address, and a new password will be emailed to you.{/t}</p>

<tr>
<td align="right">{t}Username or email address{/t}</td>
<td><input type="text" name="user" maxlength="30" class="focus" /></td>
<td class="error">{$error_message}</td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value={t}Submit{/t} /></td>
</tr>

{/if}

</table>

</form>
