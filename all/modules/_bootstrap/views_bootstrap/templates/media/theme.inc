<?php

/**
 * @file
 * Preprocessors and helper functions for panel theming.
 */

/**
 * Implements hook_preprocess_HOOK().
 */
function template_preprocess_views_bootstrap_media_plugin_style(&$vars) {
  $view = &$vars['view'];

  $image_field = $vars['options']['image_field'];
  $heading_field = $vars['options']['heading_field'];
  $body_field = $vars['options']['body_field'];

  $vars['items'] = array();

  foreach (array_keys($vars['rows']) as $key) {
    $vars['items'][] = array(
      'image_field' => isset($view->field[$image_field]) ? $view->style_plugin->get_field($key, $image_field) : NULL,
      'heading_field' => isset($view->field[$heading_field]) ? $view->style_plugin->get_field($key, $heading_field) : NULL,
      'body_field' => isset($view->field[$body_field]) ? $view->style_plugin->get_field($key, $body_field) : NULL,
    );
  }
}
