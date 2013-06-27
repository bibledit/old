<h1>{t}Stylesheet{/t} {$name}</h1>

<p>{t}This shows the various styles contained in this stylesheet.{/t} [<a href="indexm.php?delete={$name}">{t}delete{/t}]</a></p>

<table>
  <thead>
    <tr>
      <td>{t}Style{/t}</td>
      <td>{t}Name{/t}</td>
      <td></td>
    </tr>
  </thead>
  <tbody>
    {section name=offset loop=$markers} 
    <tr>
      <td><a href="view.php?sheet={$name}&style={$markers[offset]}">{$markers[offset]}</a></td>
      <td>{$names[offset]}</td>
      <td>[<a href="sheetm.php?name={$name}&delete={$markers[offset]}">{t}delete{/t}]</a></td>
    </tr>
    {/section} 
  </tbody>
</table>

<p>{t}You can modify this stylesheet. To do that, make changes to the existing xml data in the textbox below, and then submit it. It will overwrite whatever was there before.{/t}</p>

<form action="{$php_self}?name={$name}" name="form" method="post">
  <p><textarea name="data">{$xml}</textarea></p>
  <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
</form>
