<?php
/**
 * @file
 * Bootstrap 12 template for Display Suite.
 */
?>


<<?php print $layout_wrapper; print $layout_attributes; ?> class="<?php print $classes; ?>">
  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>
  <div class="row">
    <<?php print $central_wrapper; ?> class="col-sm-12 <?php print $central_classes; ?>">

    <div class="video-container">
      <img src="<?php print $poster_url; ?>" class="video-poster">
    </div>
    <div class="video-header">
      <div class="title-line-1">
        <span class="video-duration"><?php print $duration_minutes; ?></span>
      </div>
    </div>
    <div class="video-body">
      <div class="title-line-2"><?php print render($content['title']); ?></div>
      <div class="title-line-3">
        <span class="video-category"><?php print $category; ?></span>
        <span class="video-time-info"><?php print $time_info; ?></span>
      </div>
    </div>

    </<?php print $central_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
