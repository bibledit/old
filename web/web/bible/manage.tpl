<h1>{t}Bibles{/t}</h1>
<p class="success">{$success_message}</p>
<p class="error">{$error_message}</p>
<p>{t}Available Bibles:{/t}</p>
<ul>
{section name=offset loop=$bibles}
  <li><a href="settings.php?bible={$bibles[offset]}">{$bibles[offset]}</a></li>
{/section}
</ul>
<p><a href="manage.php?new=">{t}Create a new empty Bible{/t}</a></p>
