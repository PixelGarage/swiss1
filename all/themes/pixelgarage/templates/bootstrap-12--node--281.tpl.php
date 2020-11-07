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
      <div class="follow-buttons">
        <!--div class="block-title">Folge uns</div-->
        <a href="https://facebook.com/swiss1.tv"><span class="fa fa-facebook-square"></span></a>
        <a href="https://instagram.com/swiss1.tv"><span class="fa fa-instagram"></span></a>
        <a href="mailto:hallo@swiss1.tv"><span class="fa fa-envelope-square"></span></a>
      </div>
      <?php print render($content['body']); ?>
  </<?php print $central_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
