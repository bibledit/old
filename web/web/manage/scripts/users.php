<h1><?php echo gettext ("Users") ?></h1>
<p><?php echo gettext ("This shows all user accounts. You can delete users, or change users' roles.") ?></p>
<table>
  <thead>
    <tr>
      <td><?php echo gettext ("Username") ?></td>
      <td>|</td>
      <td><?php echo gettext ("Role") ?></td>
      <td>|</td>
      <td><?php echo gettext ("Email") ?></td>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($this->usernames as $offset => $username) { ?>
    {section name=offset loop=$usernames} 
    <tr>
      <td><?php echo $username ?> <a href="users.php?delete=<?php echo $username ?>">[<?php echo gettext ("delete") ?>]</a></td>
      <td>|</td>
      <td><?php echo $this->levels[$offset] ?> <a href="users.php?user=<?php echo $username ?>">[<?php echo gettext ("change") ?>]</a></td>
      <td>|</td>
      <td><?php echo $this->emails[$offset] ?> <a href="users.php?username=<?php echo $username ?>">[<?php echo gettext ("change") ?>]</a></td>
    </tr>
    <?php } ?> 
  </tbody>
</table>
<br>
<p><?php echo gettext ('Normally users create their own account. You can also manually create new users on this site. The username that you assign will also be the password. For example, when you create username "joe", then the password will also be "joe".') ?></p>
<p><a href="users.php?new="><?php echo gettext ("Create a new user") ?></a></p>
