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
      <?php if ($show_video): ?>
        <?php print render($content['field_video_path']); ?>
      <?php else: ?>
        <div class="video-poster"><img src="<?php print $poster_url; ?>"></div>
      <?php endif; ?>
      <div class="video-header">
        <div class="title-line-1"><?php print render($content['title']); ?></div>
      </div>
    </div>
    <div class="video-body">
      <div class="title-line-2">
        <span class="video-duration-category"><?php print $duration_category; ?></span>
        <span class="video-time-info"><?php print $time_info; ?></span>
      </div>
      <div class="title-line-3">
        <span class="video-season-episode"><?php print $season_episode; ?></span>
        <span class="video-language-age"><?php print $language_age; ?></span>
      </div>
      <?php print render($content['body']); ?>
      <div class="social-buttons">
        <div class="shariff" <?php print drupal_attributes($shariff_attrs); ?>></div>
      </div>
    </div>
    <div class="video-similar">
    </div>

    </<?php print $central_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
