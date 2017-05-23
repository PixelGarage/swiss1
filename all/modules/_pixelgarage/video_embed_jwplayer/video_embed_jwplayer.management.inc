<?php
/**
 * This file contains access functionality to the JWPlayer Management API.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 09.05.17
 * Time: 20:19
 */
/**
 * Returns an instance of the BotrAPI class with the key and
 * secret set, or false, if an error occurred.
 *
 * @return bool| \BotrAPI
 */
function video_embed_jwplayer_get_instance() {
  $library = libraries_load('jwplayer_api');
  if ($library['loaded']) {
    $key = variable_get('video_embed_jwplayer_api_key');
    $secret = variable_get('video_embed_jwplayer_api_secret');
    if ($key && $secret) {
      $jwplayer_api = new \BotrAPI($key, $secret);
      return $jwplayer_api;
    }
    else {
      watchdog('JWPlayer API', 'The JWPlayer API credentials are not set (see JWPlayer settings form).', array(), WATCHDOG_ERROR);
      return false;
    }
  }

  watchdog('JWPlayer API', 'The JWPlayer API library is not loaded.', array(), WATCHDOG_ERROR);
  return false;
}

/**
 * Returns an array containing all available video transcoding templates (video quality
 * settings) in the JW Platform account.
 *
 * @return array
 *    Returns an associative array of all defined quality transcoding templates,
 *    where the key is the template id and the value is the template name.
 */
function video_embed_jwplayer_get_available_transcoding_templates() {
  $video_qualities = &drupal_static(__FUNCTION__);
  if (!isset($video_qualities)) {
    //
    // get the transcoding templates via JWPlayer management api
    $video_qualities = array();
    if ($jwplayer_api = video_embed_jwplayer_get_instance()) {
      $response = $jwplayer_api->call('/accounts/templates/list', array('order_by' => 'width:desc'));

      foreach ($response['templates'] as $template) {
        if ($template['default'] == 'video' && $template['width'] && $template['name'] != '1080p') {
          $video_qualities[$template['key']] = $template['name'];
        }
      }
    }
  }
  return $video_qualities;
}


/**
 * Returns an array containing all available video conversions for a specific
 * video uploaded to the JW Platform account.
 *
 * @param string $media_id
 *    The video key.
 *
 * @return array Returns an associative array of all defined video conversions,
 * Returns an associative array of all defined video conversions,
 * where the key is the template id of the conversion and the value is the
 * template name.
 */
function video_embed_jwplayer_get_available_video_conversions($media_id) {
  $video_conversions = &drupal_static($media_id);
  if (!isset($video_conversions)) {
    //
    // get the video conversions via JWPlayer management api
    $video_conversions = array();
    if ($jwplayer_api = video_embed_jwplayer_get_instance()) {
      $response = $jwplayer_api->call('/videos/conversions/list', array('video_key' => $media_id));
      if ($response['status'] != 'ok') return $video_conversions;

      foreach ($response['conversions'] as $conversion) {
        if ($conversion['mediatype'] == 'video' && $conversion['width'] && $conversion['template']['name'] != 'Original') {
          $template = $conversion['template'];
          $video_conversions[$template['key']] = array(
            'name' => $template['name'],
            'format' => $template['format']['key'],
          );
        }
      }
    }
  }
  return $video_conversions;
}


/**
 * Gets all defined players in the given JW platform account.
 *
 * @return array
 *    Returns an associative array of all defined players,
 *    where the key is the player id and the value is the player name.
 */
function video_embed_jwplayer_get_players() {
  $players = &drupal_static(__FUNCTION__);
  if (!isset($players)) {
    //
    // get the players via JWPlayer management api
    $players = array();
    if ($jwplayer_api = video_embed_jwplayer_get_instance()) {
      $response = $jwplayer_api->call('/players/list');

      foreach ($response['players'] as $player) {
        if ($player['responsive'] && $player['version'] == '7') {
          $players[$player['key']] = $player['name'];
        }
      }
    }
  }
  return $players;
}

