<h1>{t}Updating software, step 2{/t}</h1>
<p>{t}An attempt was made to get the file.{/t}</p>
<p>{t}URL{/t}: {$url}</p>
<p>{t}Length{/t}: {$length}</p>
<p>{t}File{/t}: {$tarball}</p>
{if $success == true}
  <p class="success">{t}The file looks good.{/t}</p>
  <h3><a href="update3.php?tarball={$tarball}&basename={$basename}">{t}Next{/t}</a></h3>
{else}
  <p class="error">{t}There is a problem with the file.{/t}</p>
  <p>{$errormessage}</p>
  <p><a href="update.php">{t}Try again{/t}</a></p>
{/if}
