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
  <div class="group-header">
    <div class="group-header-line"></div>
    <div class="broadcast-time"><?php print $broadcast_time; ?></div>
  </div>
  <div class="row group-body">
    <div class="col-12 col-xs-6 col-sm-4">
      <div class="video-container">
        <div class="video-thumb"><img src="<?php print $image_url; ?>"></div>
        <?php if ($video_play_indicator): ?>
          <img src="<?php print $video_play_indicator; ?>" class="video-play-button">
        <?php endif; ?>
      </div>
      <div class="video-header">
        <div class="title-line-1"><?php print render($content['title']); ?></div>
      </div>
      <div class="video-info">
        <span class="video-duration-category"><?php print $duration_category; ?></span>
        <span class="video-time-info"><?php print $time_info; ?></span>
      </div>
    </div>

    <div class="col-12 col-xs-6 col-sm-8">
      <div class="video-body">
        <?php print render($content['body']); ?>
      </div>
    </div>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
