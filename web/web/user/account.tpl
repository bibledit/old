<h1>{t}Account{/t} {$username}</h1>

<form action="account.php" name="form" method="post">

<table>

<p class="error">{$error_message}</p>
{section name=offset loop=$success_messages} 
<p class="success">{$success_messages[offset]}</p>
{/section} 

{if $actions_taken == false}

<p>{t}You can change your password or your email address. Your current email address is{/t} {$email}.</p>

<tr>
<td align="right">{t}Current password{/t}</td>
<td><input type="text" name="currentpassword" maxlength="30" /></td>
<td class="error">{$current_password_invalid_message}</td>
</tr>

<tr>
<td align="right">{t}New password{/t}</td>
<td><input type="text" name="newpassword" maxlength="30" /></td>
<td class="error">{$new_password_invalid_message}</td>
</tr>

<tr>
<td align="right">{t}Again new password{/t}</td>
<td><input type="text" name="newpassword2" maxlength="30" /></td>
<td class="error">{$new_password2_invalid_message}</td>
</tr>

<tr>
<td align="right">{t}New email address{/t}</td>
<td><input type="text" name="newemail" maxlength="256" /></td>
<td class="error">{$new_email_invalid_message}</td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value={t}Submit{/t} /></td>
</tr>

{/if}

</table>

</form>
