<h1>{t}Members{/t}</h1>
<p><a href="../downloads">{t}Downloads{/t}</a></p>
{foreach key=key item=item from=$plugins} 
  <p><a href={$key}>{$item}</a></p>
{/foreach} 

