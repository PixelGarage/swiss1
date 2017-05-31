<?php
/**
 * @file
 * Bootstrap 8-4 template for Display Suite.
 */
?>


<<?php print $layout_wrapper; print $layout_attributes; ?> class="<?php print $classes; ?>">
  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>
  <div class="row">
    <<?php print $left_wrapper; ?> class="col-sm-8 <?php print $left_classes; ?>">
      <div class="video-container">
        <img src="<?php print $poster_url; ?>" class="video-poster">
      </div>
    </<?php print $left_wrapper; ?>>
    <<?php print $right_wrapper; ?> class="col-sm-4 <?php print $right_classes; ?>">
      <div class="video-title"><?php print render($content['title']); ?></div>
      <div class="video-body">
        <?php print render($content['body']); ?>
        <span class="video-category"><?php print $category; ?></span>
      </div>
      <div class="social-buttons">
        <div class="shariff" <?php print drupal_attributes($shariff_attrs); ?>></div>
      </div>
    </<?php print $right_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
