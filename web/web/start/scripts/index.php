<?php foreach ($this->menu as $category => $menu) { ?>
  <p>
  <?php foreach ($menu as $url => $text) { ?>
    <a href="?category=<?php echo $category ?>&url=<?php echo $url ?>"><?php echo $text ?></a>
  <?php } ?>
  </p>
<?php } ?>
