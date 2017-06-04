<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:22
 */

/**
 * Alters the menu entries.
 * @param $items
 */
function pixelgarage_menu_alter(&$items) {
  // hides local task tabs
  pg_login_menu_alter($items);
}

/* =============================================================================
 *    Proximity alter hooks implementation
 * ========================================================================== */

/**
 * Alters the proximity item load parameter array.
 *
 * The load parameter array defines a specific url parameter for each proximity item.
 * This parameter is added at the end of the request url and must be unique and URL conform.
 * The unique parameter defines, which item content has to be loaded from
 * the server (see next api function).
 *
 * @param $container_index      int     Index of proximity container (if more than one container exists in one page).
 * @param $view                 array   The view.
 * @param $ajax_load_params     array   Array of ajax load parameters to be altered, one for each proximity item (row)
 *                                      retrieved by the view. Default is the views row index.
 */
function pixelgarage_proximity_load_params_alter($container_index, $view, &$ajax_load_params) {
  // Return the node id as ajax parameter for each item.
  foreach ($view->result as $id => $item) {
    $ajax_load_params[$id] = $item->nid;
  }
}

/**
 * Implements the item specific content as render array or html string.
 * The input parameter $param contains the unique load parameter of the requested item.
 *
 * @param $container_index      int     Index of proximity container (if more than one container exists in one page).
 * @param $param                string  The item specific load parameter (see also hook_proximity_ajax_load_params_alter).
 * @param $render_item          mixed   The rendered content to be returned to the client. The $render_item should be
 *                                      replaced either by a string (rendered html content), a render array or an integer (error code).
 */
function pixelgarage_proximity_render_item_alter($container_index, $param, &$render_item) {
  // return the render array for the specific node, if available
  if ($node= node_load($param)) {
    $view_mode = 'full';
    if (property_exists($node, 'ds_switch') && !empty($node->ds_switch)) {
      // take an alternate view mode set by the ds switch
      $view_mode = $node->ds_switch;
    }
    $render_item = node_view($node, $view_mode);
  }
}


/* =============================================================================
 *    Alteration of program views exposed filter
 * ========================================================================== */

function pixelgarage_form_alter(&$form, &$form_state, $form_id) {
  //
  // manage custom filter of program view
  if($form_id == 'views_exposed_form' && $form_state['view']->name == 'program') {
    //
    // alter form
    $options = _get_date_select_options();
    hide($form['field_broadcast_date_value']);
    $form['field_broadcast_date_select'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => '0 days',
    );

    //
    //execute manage_custom_filters submit function in first step on $form['#submit'] array
    array_unshift($form['#submit'], 'manage_custom_filters');
  }
}

function manage_custom_filters(&$form, &$form_state) {
  $now = strtotime('today');
  $start_timestamp = strtotime($form_state['values']['field_broadcast_date_select'], $now);
  $end_timestamp = strtotime('+1 day', $start_timestamp);
  $selected_start_date = date('Y-m-d H:i:s', $start_timestamp);
  $selected_end_date = date('Y-m-d H:i:s', $end_timestamp);

  // set filter values in exposed hidden field
  $form_state['values']['field_broadcast_date_value']['min'] = $selected_start_date;
  $form_state['values']['field_broadcast_date_value']['max'] = $selected_end_date;

  // calc prev / next day and store them in form (hidden)
  $prev_day = strtotime('-1 day', $start_timestamp);
  $prev_date = date('D, d.m.Y', $prev_day);
  $prev_date = str_replace(array('Mon','Tue', 'Wed', 'Thu','Fri','Sat','Sun'), array('Mo','Di','Mi','Do','Fr','Sa','So'), $prev_date);
  $form['prev_day'] = array (
    '#type' => 'hidden',
    '#value' => $prev_date,
  );

  $next_day = strtotime('+1 day', $start_timestamp);
  $next_date = date('D, d.m.Y', $next_day);
  $next_date = str_replace(array('Mon','Tue', 'Wed', 'Thu','Fri','Sat','Sun'), array('Mo','Di','Mi','Do','Fr','Sa','So'), $next_date);
  $form['next_day'] = array (
    '#type' => 'hidden',
    '#value' => $next_date,
  );

}


function _get_date_select_options() {
  $options = array();
  $now = strtotime('today');

  // create select options
  for ($i = -7; $i <= 10; $i++) {
    $rel_time = $i . ' days';
    $timestamp = strtotime($rel_time, $now);
    switch ($i) {
      case -1:
        $options[$rel_time] = date('\G\e\s\t\e\r\n\,   d.m.Y', $timestamp);
        break;
      case 0:
        $options[$rel_time] = date('\H\e\u\t\e,   d.m.Y', $timestamp);
        break;
      case 1:
        $options[$rel_time] = date('\M\o\r\g\e\n,   d.m.Y', $timestamp);
        break;
      default:
        $date = date('D,   d.m.Y', $timestamp);
        $date = str_replace(array('Mon','Tue', 'Wed', 'Thu','Fri','Sat','Sun'), array('Mo','Di','Mi','Do','Fr','Sa','So'), $date);
        $options[$rel_time] = $date;
    }
  }
  return $options;
}
