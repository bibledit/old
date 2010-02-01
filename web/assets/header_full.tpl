<div align="right">
{ if $user == "" }
  { if $logging_in != true }
    <a href="{$header_path_modifier}session/login">{t}Login{/t}</a>
  {/if}
{else}
    <a href="{$header_path_modifier}session/user">{$user}</a>
     |
    <a href="{$header_path_modifier}session/logout">{t}Logout{/t}</a>
{/if}
</div>
