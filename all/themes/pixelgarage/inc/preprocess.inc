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
  $node  = $vars['node'];
  $img_path = drupal_get_path('theme', 'pixelgarage') . '/images/';

//
// shariff button definition
  libraries_load('shariff', 'naked');
  $vars['shariff_attrs'] = array(
    'data-services' => '["twitter","facebook","mail"]',
    'data-orientation' => "horizontal",
    'data-mail-url' => "mailto:",
    'data-mail-subject' => variable_get('shariff_mail_subject', t('')),
    'data-mail-body' => variable_get('shariff_mail_body', t('')),
    'data-lang' => "de",
  );

  //
  // get poster image, if none is set, set default image
  if (!empty($node->field_poster) && $poster = file_load($node->field_poster[LANGUAGE_NONE][0]['fid'])) {
    $vars['poster_url'] = file_create_url($poster->uri);
  }
  else {
    $vars['poster_url'] = file_create_url($img_path . 'poster_swiss1.jpg');
  }

  //
  // render video if it is playable, otherwise render poster image
  $validity_period_start = strtotime($node->field_validity_period[LANGUAGE_NONE][0]['value']);
  $validity_period_end = strtotime($node->field_validity_period[LANGUAGE_NONE][0]['value2']);
  $now = time();
  $vars['show_video'] = ($now >= $validity_period_start) && ($now < $validity_period_end) && isset($vars['content']['field_video_path']);



}
