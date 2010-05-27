<h1>{t}Users{/t}</h1>
<p>{t}This shows all user accounts. You can delete users, or change users' roles.{/t}</p>
<table>
  <thead>
    <tr>
      <td>{t}Username{/t}</td>
      <td></td>
      <td>|</td>
      <td>{t}Role{/t}</td>
      <td></td>
      <td>|</td>
      <td>{t}Email{/t}</td>
    </tr>
  </thead>
  <tbody>
    {section name=offset loop=$usernames} 
    <tr>
      <td>{$usernames[offset]}</td>
      <td><a href="users.php?delete={$usernames[offset]}">[{t}delete{/t}]</a></td>
      <td>|</td>
      <td>{$levels[offset]}</td>
      <td><a href="users.php?user={$usernames[offset]}">[{t}change{/t}]</a></td>
      <td>|</td>
      <td>{$emails[offset]}</td>
    </tr>
    {/section} 
  </tbody>
</table>
<br>
<p>{t}Normally users create their own account. But you can also manually create new users on this site. The username that you assign will also be the password. For example, when you create username "joe", then the password will also be "joe".{/t}</p>
<p><a href="users.php?new=">{t}Create a new user{/t}</a></p>
