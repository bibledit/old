<table width="100%">
  <tr>
    <td>
    {* Administration menu is only applicable to the site administrator *}
    {if $level >= 6}
      <a href="{$header_path_modifier}admin/index">{t}Administration{/t}</a>
    {/if}
    {* Management menu is only applicable to the manager and higher roles *}
    {if $level >= 5}
      <a href="{$header_path_modifier}manage/index">{t}Management{/t}</a>
    {/if}
    {* Poor Man's Crontab *}
    <img border="0" src="{$header_path_modifier}timer/index.php" width="0" height="0" />
    </td>
    <td align="right">
    {if $user == ""}
      { if $logging_in != true }
        <a href="{$header_path_modifier}session/login">{t}Login{/t}</a>
      {/if}
    {else}
        <a href="{$header_path_modifier}user">{$user}</a>
         |
        <a href="{$header_path_modifier}session/logout">{t}Logout{/t}</a>
    {/if}
    </td>
  </tr>
</table>
