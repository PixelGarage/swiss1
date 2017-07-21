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
        <div class="video-poster-container"><img class="video-poster" src="<?php print $image_url; ?>"/></div>
        <?php if ($video_play_indicator): ?>
          <img class="video-play-button" src="<?php print $video_play_indicator; ?>">
        <?php endif; ?>
      </div>
    </<?php print $left_wrapper; ?>>
    <<?php print $right_wrapper; ?> class="col-sm-4 <?php print $right_classes; ?>">
      <div class="video-title"><?php print render($content['title']); ?></div>
      <div class="video-body">
        <?php print render($content['body']); ?>
        <span class="video-duration-category"><?php print $duration_category; ?></span>
      </div>
      <div class="video-broadcast-time">
        <div class="time-info"><?php print $time_info; ?></div>
      </div>
    </<?php print $right_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
