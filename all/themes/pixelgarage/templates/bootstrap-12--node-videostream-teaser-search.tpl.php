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
  </div>
  <div class="row group-body">
    <div class="col-12 col-xs-6 col-sm-4 col-first">
      <div class="video-container">
        <div class="video-thumb-container"><img class="video-thumb" src="<?php print $image_url; ?>"></div>
        <?php if ($video_play_indicator): ?>
          <img src="<?php print $video_play_indicator; ?>" class="video-play-button">
        <?php endif; ?>
      </div>
      <div class="video-header">
        <div class="title-line-1"><?php print render($content['title']); ?></div>
      </div>
    </div>

    <div class="col-12 col-xs-6 col-sm-4">
      <div class="video-body">
        <?php print render($content['body']); ?>
      </div>
    </div>

    <div class="col-12 col-sm-4 col-last">
      <div class="video-details">
        <div class="video-time-info"><?php print $time_info; ?></div>
        <div class="video-duration-category"><?php print $duration_category; ?></div>
        <div class="video-season-episode"><?php print $season_episode; ?></div>
        <div class="video-language-age"><?php print $language_age; ?></div>
      </div>
    </div>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
