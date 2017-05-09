<?php
/**
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
    }
  }

  watchdog('JWPlayer API', 'The JWPlayer API library is not loaded.', array(), WATCHDOG_ERROR);
  return FALSE;
}

/**
 * Creates an array with all available video transcoding templates (quality
 * settings) in your account on the JW Platform.
 *
 * @return array
 *    Returns an associative array of available quality transcoding templates (key is template id).
 */
function video_embed_jwplayer_get_available_transcoding_templates() {
  $qualities = &drupal_static(__FUNCTION__);
  if (!isset($qualities)) {
    //
    // get the transcoding templates via JWPlayer management api
    $qualities = array(
      'adaptive' => 'Adaptive',
      'NzT8l9sJ' => '720p',
      '0Pdg6jyA' => '406p',
      'CKaEk4PM' => '270p',
      'WMO2e8la' => '180p',
    );
  }
  return $qualities;
}

