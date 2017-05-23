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


/* =============================================================================
 *    Preprocessing of views
 * ========================================================================== */

function pixelgarage_preprocess_views_exposed_form(&$vars) {
  $test = $vars;
}


/* =============================================================================
 *    Preprocessing of video streams
 * ========================================================================== */

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
  // VIDEO DISPLAY
  // get poster image, if none is set, set default image
  $is_teaser = $view_mode == 'teaser' || $view_mode == 'teaser_program';
  $img_path = drupal_get_path('theme', 'pixelgarage') . '/images/';
  if ($is_teaser && !empty($node->field_thumb_path) && $thumb = file_load($node->field_thumb_path[LANGUAGE_NONE][0]['fid'])) {
    $vars['poster_url'] = file_create_url($thumb->uri);
  }
  else if (!empty($node->field_poster) && $poster = file_load($node->field_poster[LANGUAGE_NONE][0]['fid'])) {
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

  //
  // TIME INFORMATION
  // format time information
  $broadcast_starttime = !empty($node->field_broadcast_date) && $node->field_broadcast_date[LANGUAGE_NONE][0]['value'] ?
    strtotime($node->field_broadcast_date[LANGUAGE_NONE][0]['value']) : 0;
  $broadcast_endtime = !empty($node->field_broadcast_date) && $node->field_broadcast_date[LANGUAGE_NONE][0]['value2'] ?
    strtotime($node->field_broadcast_date[LANGUAGE_NONE][0]['value2']) : 0;
  if ($date_str = _get_broadcast_date_string($broadcast_starttime, $broadcast_endtime)) {
    $vars['time_info'] = $date_str;
  }
  else if ($vars['show_video']) {
    // calculate validity duration in days
    $valid_period = ($validity_period_end - $now) / 86400;
    if ($valid_period > 365.0) {
      $vars['time_info'] = t('Mehr als ein Jahr');
    }
    else if ($valid_period > 30.0) {
      $vars['time_info'] = t('Mehr als ein Monat');
    }
    else if ($valid_period > 1.0) {
      $days = round($valid_period);
      $vars['time_info'] = ($days > 1) ? t('Noch @valid Tage', array('@valid' => $days)) : t('Noch @valid Tag', array('@valid' => $days));
    }
    else {
      $hours = round(($validity_period_end - $validity_period_start) / 3600);
      $vars['time_info'] = ($hours > 1) ? t('Noch @valid Stunden', array('@valid' => $hours)) : t('Noch @valid Stunde', array('@valid' => $hours));
    }
  }
  else {
    // validity period in the past
    $vars['time_info'] = t('Nicht verfügbar');
  }

  //
  // format duration
  $duration = !empty($node->field_duration) && $node->field_duration[LANGUAGE_NONE][0]['value'] ?
    $node->field_duration[LANGUAGE_NONE][0]['value'] : 0;
  $minutes = abs($duration / 60);
  $vars['duration_minutes'] = t('@mins Min.', array('@mins' => $minutes));

  //
  // add video indicator image, if video is playable
  $vars['video_play_indicator'] = $vars['show_video'] ? file_create_url($img_path . 'icon-play.svg') : false;

  //
  // EPG INFORMATION
  // format category, language, age restriction and series
  $vars['category'] = '';
  $vars['language_age'] = '';
  $vars['season_episode'] = '';
  if (!empty($node->field_category) && $tid = $node->field_category[LANGUAGE_NONE][0]['tid']) {
    $term = taxonomy_term_load($tid);
    $vars['category'] = $term->name;
  }
  if (!empty($node->field_video_language) && $lang = $node->field_video_language[LANGUAGE_NONE][0]['value']) {
    $vars['language_age'] = $lang;
  }
  if (!empty($node->field_age_restriction) && $restr = $node->field_age_restriction[LANGUAGE_NONE][0]['value']) {
    $vars['language_age'] .= ($vars['language_age'] ? ' | ' . $restr : $restr);
  }
  if (!empty($node->field_season) && $season = $node->field_season[LANGUAGE_NONE][0]['value']) {
    $vars['season_episode'] = $season;
  }
  if (!empty($node->field_episode) && $episode = $node->field_episode[LANGUAGE_NONE][0]['value']) {
    $vars['season_episode'] .= ($vars['season_episode'] ? ' - Episode ' . $episode : 'Episode ' . $episode);
  }

}

/**
 * Helper function to format a broadcast date of a video.
 */
function _get_broadcast_date_string($starttime, $endtime = 0) {
  $now = time();
  $date = date('Y-m-d');
  $today_starttime = strtotime($date . '00:00:00');
  $today_endtime = strtotime($date . '23:59:59');
  $tomorrow_starttime = strtotime('+1 day', $today_starttime);
  $tomorrow_endtime = strtotime('+1 day', $today_endtime);

  if ($starttime > $tomorrow_endtime) {
    //
    // AFTER TOMORROW: display Mo, 12.10 12:45 - 13:30
    $str_date = date('D, d.m H:i', $starttime);
  }
  else if ($starttime >= $tomorrow_starttime && $starttime <= $tomorrow_endtime) {
    //
    // TOMORROW: display Morgen, 12:45 - 13:30
    $str_date = 'Morgen, ' . date('H:i', $starttime);
  }
  else if ($starttime > $now && $starttime <= $today_endtime) {
    //
    // TODOAY FUTURE: display Heute, 12:45 - 13:30
    $str_date = 'Heute, ' . date('H:i', $starttime);
  }
  else if ($starttime <= $now && $endtime > $now) {
    //
    // TODAY LIVE: display Endet um 13:30
    $end_time = date('H:i', $endtime);
    return t('Endet um @end', array('@end' => $end_time));
  }
  else {
    //
    // BROADCAST IN PAST: show validity duration
    return false;
  }

  if ($endtime) {
    $str_date .= date(' - H:i', $endtime);
  }

  return $str_date;
}
