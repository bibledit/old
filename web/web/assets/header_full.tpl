<table width="100%">
  <tr>
    <td>
    {* Administration menu is only applicable to the site administrator *}
    {if $level >= 6}
      <a href="{$header_path_modifier}administration/index.php">{t}Administration{/t}</a>
    {/if}
    {* Management menu is only applicable to the manager and higher roles *}
    {if $level >= 5}
      <a href="{$header_path_modifier}manage/index.php">{t}Management{/t}</a>
    {/if}
    {* Translation menu is only applicable to the translator and higher roles *}
    {if $level >= 4}
      <a href="{$header_path_modifier}translate/index.php">{t}Translation{/t}</a>
    {/if}
    {if $level >= 3}
      <a href="{$header_path_modifier}consultations/index.php">{t}Consultations{/t}</a>
    {/if}
    {if $level >= 2}
      <a href="{$header_path_modifier}members/index.php">{t}Members{/t}</a>
    {/if}
    <a href="{$header_path_modifier}help/index.php">{t}Help{/t}</a>
    {* Poor Man's Crontab *}
    <img border="0" src="{$header_path_modifier}timer/index.php" width="0" height="0" />
    </td>
    <td align="right">
    {if $user == ""}
      { if $logging_in != true }
        <a href="{$header_path_modifier}session/login.php">{t}Login{/t}</a>
      {/if}
    {else}
        <a href="{$header_path_modifier}user/index.php">{$user}</a>
        |
        <a href="{$header_path_modifier}session/logout.php">{t}Logout{/t}</a>
    {/if}
    </td>
  </tr>
</table>
