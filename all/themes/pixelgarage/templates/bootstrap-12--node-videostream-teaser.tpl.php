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
      <div class="video-thumb-container"><img class="video-thumb" src="<?php print $image_url; ?>"></div>
      <?php if ($video_play_indicator): ?>
        <img class="video-play-button" src="<?php print $video_play_indicator; ?>">
      <?php endif; ?>
    </div>
    <div class="video-header">
      <div class="title-line-1"><?php print render($content['title']); ?></div>
    </div>
    <div class="video-body">
      <span class="video-duration-category"><?php print $duration_category; ?></span>
      <span class="video-time-info"><?php print $time_info; ?></span>
    </div>

    </<?php print $central_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
