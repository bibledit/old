<h1><?php echo gettext ("Mail") ?></h1>

<p>
  <a href="<?php echo $this->link_inbox ?>" <?php if ($this->active_label == 'inbox') { ?>class="active"<?php } ?>>
  <?php echo gettext ("Inbox") ?></a>
  |
  <a href="<?php echo $this->link_trash ?>"<?php if ($this->active_label == 'trash') { ?>class="active"<?php } ?>>
  <?php echo gettext ("Trash") ?></a>
</p>

<?php if ($this->body != "") { ?>

  <p><?php echo $this->subject ?></p>
  <p>-</p>
  <p><?php echo $this->body ?></p>

<?php } else { ?>

  <table>
    <thead>
      <tr>
        <td><?php echo gettext ("Subject") ?></td>
        <td></td>
        <td><?php echo gettext ("Date") ?></td>
        <td></td>
        <td></td>
      </tr>
    </thead>
    <tbody>
      <?php foreach ($this->ids as $offset => $id) { ?>
      <tr>
        <td><a href="<?php echo $this->views[$offset] ?>"><?php echo $this->subjects[$offset] ?></a></td>
        <td>|</td>
        <td><?php echo $this->timestamps[$offset] ?></td>
        <td>|</td>
        <td><a href="<?php echo $this->deletes[$offset] ?>">[<?php echo gettext ("delete") ?>]</a></td>
      </tr>
      <?php } ?>
    </tbody>
  </table>

<?php } ?>
<p><?php echo gettext ("Mail older than 90 days will automatically be deleted.") ?></p>
