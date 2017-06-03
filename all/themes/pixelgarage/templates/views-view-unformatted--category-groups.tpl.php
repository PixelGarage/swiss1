<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php foreach ($rows as $id => $row): ?>
  <div<?php if ($classes_array[$id]) { print ' class="' . $classes_array[$id] .'"';  } ?>>
  <?php if ($subviews[$id]['show']): ?>
    <div class="group-header">
      <div class="group-header-line"></div>
      <?php print $row; ?>
    </div>
    <div class="group-body">
      <?php print $subviews[$id]['subview']; ?>
    </div>
  <?php endif; ?>
  </div>
<?php endforeach; ?>
