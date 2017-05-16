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
        <img src="<?php print $poster_url; ?>" class="video-poster">
      <?php endif; ?>
    </div>
    <div class="video-header">

    </div>
    <div class="video-body">
      <?php print render($content['field_video_path']); ?>
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
