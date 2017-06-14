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
  drupal_add_library('system', 'effects');

}

/**
 * Override or insert variables for the page templates.
 */
function pixelgarage_preprocess_page(&$vars) {
  // hide titles on login forms
  pg_login_preprocess_page($vars);

  //$vars['logo'] = str_replace(array('.jpg', '.png'), '.svg', $vars['logo']);

  // add search,close icon
  $path = drupal_get_path('theme', 'pixelgarage') . '/images/';
  $vars['search_icon'] = file_create_url($path . 'icon-search.svg');
  $vars['close_icon'] = file_create_url($path . 'icon-close.svg');

  // add background logo
  $vars['background_logo'] = file_create_url($path . 'swiss1_background_logo.png');
  //
  // get exposed search form, if available (to render it in modal dialog)
  $exposed_search_form_section = 'views_-exp-search_videos-search_page';
  $vars['exposed_search_form_modal_id'] = false;

  if (isset($vars['page']['navigation'][$exposed_search_form_section]) &&
    ($exposed_search_form = $vars['page']['navigation'][$exposed_search_form_section])) {
    $vars['exposed_search_form'] = $exposed_search_form;
    $vars['exposed_search_form_modal_id'] = 'modal-exposed-search-form';
    $vars['exposed_search_form_modal_title'] = t('Sendungen Suche');
  }
}


/* =============================================================================
 *      Preprocess Exposed search form
 * ========================================================================== */
/**
 * Adapts the exposed search form to show placeholders in search fields.
 *
 */
function pixelgarage_preprocess_views_exposed_form(&$vars) {
  //
  // add placeholders to search exposed form items
  $form = &$vars['form'];

  if ($form['#id'] == 'views-exposed-form-search-videos-search-page') {
    //
    // exposed form of video search view
    // set placeholder from description field
    foreach ($form['#info'] as $id => $info) {
      // add the description as placeholder to the widget
      if ($id != 'filter-combine') continue;

      // set placeholder
      $form[$info['value']]['#attributes']['placeholder'] = $info['description'];
      $form[$info['value']]['#printed'] = false;

      // render widget again
      $field = $form[$info['value']];
      $vars['widgets'][$id]->widget = drupal_render($field);
      $vars['widgets'][$id]->description = '';
    }

  }
  else if ($form['#id'] == 'views-exposed-form-program-page-program') {
    //
    // exposed form of program view
    // set prev / next date
    $vars['prev_date'] = $form['prev_day']['#value'];
    $vars['next_date'] = $form['next_day']['#value'];
  }

}


/* =============================================================================
 *      Preprocess category groups view (grouped by terms)
 * ========================================================================== */
/**
 * Adapts the category view.
 *
 */
function pixelgarage_preprocess_views_view_unformatted(&$vars) {
  $view = $vars['view'];
  if ($view->name == 'category_groups') {
    //
    // render subview masonry, only show subview if videostreams with given category are available
    $subviews = array();
    $display_id = $vars['is_front'] ? 'block_valid_broadcasts' : 'block_future_broadcasts';
    foreach ($view->result as $id => $term_obj) {
      $subview = views_embed_view('video_masonry', $display_id, $term_obj->tid);
      $row_count = substr_count($subview, 'node-videostream');
      $subviews[$id] = array(
        'show' => $row_count > 0,
        'subview' => $subview,
      );
    }
    $vars['subviews'] = $subviews;
  }

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
    _preprocess_node_videostream($vars, $view_mode);
  }
}


function _preprocess_node_videostream(&$vars, $view_mode) {
  $node  = $vars['node'];

//
// SHARIFF BUTTON DEFINITION
  global $base_url;

  libraries_load('shariff', 'naked');
  $vars['shariff_attrs'] = array(
    'data-services' => '["facebook","twitter","whatsapp","mail"]',
    'data-orientation' => "horizontal",
    'data-mail-url' => "mailto:",
    'data-mail-subject' => variable_get('shariff_mail_subject', t('')),
    'data-mail-body' => variable_get('shariff_mail_body', t('')),
    'data-lang' => "de",
    'data-url' => $base_url . '/node/' . $node->nid,
  );


  //
  // VIDEO / IMAGE DISPLAY
  // get poster image. if none is set, set default image
  $use_thumb = $view_mode == 'teaser' || $view_mode == 'teaser_search';
  $img_path = drupal_get_path('theme', 'pixelgarage') . '/images/';
  if ($use_thumb && !empty($node->field_thumb_path) && $thumb = file_load($node->field_thumb_path[LANGUAGE_NONE][0]['fid'])) {
    $thumb_uri = create_desaturated_image($thumb->uri);
    $vars['image_url'] = file_create_url($thumb_uri);
  }
  else if (!empty($node->field_poster) && $poster = file_load($node->field_poster[LANGUAGE_NONE][0]['fid'])) {
    $vars['image_url'] = file_create_url($poster->uri);
  }
  else {
    $default_img_path = $img_path . 'poster_swiss1.jpg';
    $default_uri = create_desaturated_image($default_img_path);
    $vars['image_url'] = file_create_url($default_uri);
  }

  //
  // render video if it is playable, otherwise render poster image
  $validity_period_start = strtotime($node->field_validity_period[LANGUAGE_NONE][0]['value']);
  $validity_period_end = strtotime($node->field_validity_period[LANGUAGE_NONE][0]['value2']);
  $now = time();
  $vars['show_video'] = ($now >= $validity_period_start) && ($now < $validity_period_end) && isset($vars['content']['field_video_path']);

  //
  // add video indicator image, if video is playable
  $vars['video_play_indicator'] = $vars['show_video'] ? file_create_url($img_path . 'icon-play.svg') : false;



  //
  // TIME INFORMATION
  // format time information
  $broadcast_starttime = !empty($node->field_broadcast_date) && isset($node->field_broadcast_date[LANGUAGE_NONE][0]['value']) ?
    strtotime($node->field_broadcast_date[LANGUAGE_NONE][0]['value']) : 0;
  $broadcast_endtime = !empty($node->field_broadcast_date) && isset($node->field_broadcast_date[LANGUAGE_NONE][0]['value2']) ?
    strtotime($node->field_broadcast_date[LANGUAGE_NONE][0]['value2']) : 0;
  // show full time info only for full view mode
  $broadcast_endtime = ($view_mode == 'full' || $view_mode == 'teaser_search') ? $broadcast_endtime : false;
  if ($date_str = _get_broadcast_date_string($broadcast_starttime, $broadcast_endtime)) {
    $vars['time_info'] = $date_str;
  }
  else if ($vars['show_video']) {
    // calculate validity duration in days
    $valid_period = ($validity_period_end - $now) / 86400;
    if ($valid_period > 365.0) {
      $vars['time_info'] = t('Mehr als ein Jahr');
    }
    else if ($valid_period > 31.0) {
      $vars['time_info'] = t('Mehr als ein Monat');
    }
    else if ($valid_period > 7.0) {
      $weeks = round($valid_period / 7.0);
      $vars['time_info'] = ($weeks > 1) ? t('Mehr als @weeks Wochen', array('@weeks' => $weeks)) : t('Mehr als eine Woche');
    }
    else if ($valid_period > 1.0) {
      $days = round($valid_period);
      $vars['time_info'] = ($days > 3) ? t('Noch @valid Tage', array('@valid' => $days)) :
        ($days > 1 ? t('Noch <span class="day-highlight">@valid</span> Tage', array('@valid' => $days)) : t('Noch <span class="day-one-highlight">1</span> Tag'));
    }
    else {
      $hours = round(($validity_period_end - $validity_period_start) / 3600);
      $vars['time_info'] = ($hours > 1) ? t('Noch @valid Stunden', array('@valid' => $hours)) : t('Noch @valid Stunde', array('@valid' => $hours));
    }
  }
  else {
    // validity period in the past or future
    $vars['time_info'] = t('Nicht verfügbar');
  }

  //
  // format broadcast start time
  $vars['broadcast_time'] = ($broadcast_starttime > 0) ? date('H:i', $broadcast_starttime): '';

  //
  // format duration / category
  if (!empty($node->field_category) && $tid = $node->field_category[LANGUAGE_NONE][0]['tid']) {
    $term = taxonomy_term_load($tid);
  }
  else {
    $term = taxonomy_get_term_by_name('Allgemein', 'kategorie');
  }
  $duration = !empty($node->field_duration) && $node->field_duration[LANGUAGE_NONE][0]['value'] ?
    $node->field_duration[LANGUAGE_NONE][0]['value'] : 0;
  $minutes = abs($duration / 60);
  $vars['duration_category'] = t('@mins min / @category', array('@mins' => $minutes, '@category' => $term->name));


  //
  // EPG INFORMATION
  // format category, language, age restriction and series
  $vars['language_age'] = '';
  $vars['season_episode'] = '';

  if (!empty($node->field_video_language) && $lang = $node->field_video_language[LANGUAGE_NONE][0]['value']) {
    $vars['language_age'] = $lang;
  }
  if (!empty($node->field_age_restriction) && $restr = $node->field_age_restriction[LANGUAGE_NONE][0]['value']) {
    $vars['language_age'] .= (!empty($vars['language_age']) ? ' | ' . $restr : $restr);
  }
  if (!empty($node->field_series) && $series = $node->field_series[LANGUAGE_NONE][0]['value']) {
    $vars['season_episode'] = $series;
  }
  if (!empty($node->field_season) && $season = $node->field_season[LANGUAGE_NONE][0]['value']) {
    $vars['season_episode'] .= (!empty($vars['season_episode']) ? ' - Saison ' . $season : t('Unbekannte Serie - Saison @season',array('@season' => $season)));
  }
  if (!empty($node->field_episode) && $episode = $node->field_episode[LANGUAGE_NONE][0]['value']) {
    $vars['season_episode'] .= (!empty($vars['season_episode']) ? ' - Episode ' . $episode : 'Episode ' . $episode);
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

/**
 * Helper function to create a desaturated image for the hover effect.
 */
function create_desaturated_image($image_uri) {
  $style_name = 'desaturate_25';
  $dest = image_style_path($style_name, $image_uri);
  if (!file_exists($dest)) {
    $style = image_style_load($style_name);
    if (isset($style['effects'])) {
      $success = image_style_create_derivative($style, $image_uri, $dest);
      return $success ? $dest : $image_uri;
    }
  }

  return $dest;
}
