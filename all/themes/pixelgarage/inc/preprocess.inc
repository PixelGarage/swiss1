<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:23
 */

/**
 * Preprocess variables for the html template.
 */
function pixelgarage_preprocess_html(&$vars) {
  // make sure jQuery UI and effects is loaded for anonymous users
  drupal_add_library('system', 'ui');
  //drupal_add_library('system', 'effects');

}

/**
 * Override or insert variables for the page templates.
 */
function pixelgarage_preprocess_page(&$vars) {
  // hide titles on login forms
  pg_login_preprocess_page($vars);

  //$vars['logo'] = str_replace(array('.jpg', '.png'), '.svg', $vars['logo']);

}


/**
 * Implements hook_ds_pre_render_alter().
 *
 * @param $layout_render_array
 *   The render array
 * @param $context
 *   An array with the context that is being rendered. Available keys are
 *   - entity
 *   - entity_type
 *   - bundle
 *   - view_mode
 * @param array $vars
 *   All variables available for render. You can use this to add css classes.
 */
function pixelgarage_ds_pre_render_alter(&$layout_render_array, $context, &$vars) {
  // Check node type.
  if ($context['entity_type'] == 'node' && $context['bundle'] == 'videostream')  {
    $view_mode = $context['view_mode'];
    _preprocess_node_videostream($vars, $layout_render_array, $view_mode);
  }
}


function _preprocess_node_videostream(&$vars, &$layout_render_array, $view_mode) {

}
