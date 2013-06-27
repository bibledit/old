<h1>{t}Repository data{/t}</h1>
<p>{t}You are now at a stage that the data in Bibledit-Web, and the data in the repository, needs to be synchronized. This means that some action needs to be taken to make these two sets of data the same. Not all the data in Bibledit-Web needs to be synchronized with the repository. It is only the data in the one Bible or the Consultation Notes you are now dealing with.{/t}</p>
{if $object == "consultationnotes"}
<p>{t}Consultation Notes{/t}</p>
{else}
<p>{t}Bible{/t}: {$object}</p>
{/if}

<p>{t}There are two options:{/t}</p>
<ol>
  <li><em>{t}Take the data in the repository.{/t}</em> {t}Taking the data in the repository means that the data in the repository is left as it is, and that a copy of that data overwrites your own data in Bibledit-Web. This option is used most of the time.{/t} <a href="collaboration_take_repo.php?object={$object}">{t}Take the data in the repository.{/t}</a></li>
  <li><em>{t}Take your own data.{/t}</em> {t}Taking your own data means that your data is taken as it is, and that this is copied to the repository, overwriting whatever is there in the repository now.{/t} <a href="collaboration_take_yourself.php?object={$object}">{t}Take your data.{/t}</a></li>
</ol> 
