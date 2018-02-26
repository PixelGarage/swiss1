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
<?php foreach ($categories as $key => $category): ?>
  <div class="views-row">
    <div class="group-header">
      <div class="group-header-line"></div>
      <?php print $category['category_name']; ?>
    </div>
    <div class="group-body">
      <?php print $category['subview']; ?>
      <?php if ($category['more']): ?>
        <div class="more-container">
          <a class="more-button" href="<?php print $category['more']; ?>"><?php print $more_text; ?></a>
        </div>
      <?php endif; ?>
    </div>
  </div>
<?php endforeach; ?>
