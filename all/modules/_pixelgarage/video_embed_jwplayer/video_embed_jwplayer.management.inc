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
 * secret set of the given JWPlayer account property, or false, if an error occurred.
 *
 * @param string $property_key
 *    The key of the JWPlayer account property to connect with. The returned BotrAPI
 *    class is instantiated with the credentials of the given property.
 *
 * @return bool|\BotrAPI
 */
function video_embed_jwplayer_get_instance($property_key) {
  $library = libraries_load('jwplayer_api');
  if ($library['loaded']) {
    $key = variable_get('video_embed_jwplayer_api_key_' . $property_key);
    $secret = variable_get('video_embed_jwplayer_api_secret_' . $property_key);

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
 * Gets the JWPlayer account property key holding the media with the given
 * media id.
 *
 * @param string $media_id
 *
 * @return string
 *    Returns the JWPlayer account property key of the account property that
 *    contains the media with the given media id.
 */
function video_embed_jwplayer_get_property_key($media_id) {
  $property_key = &drupal_static('video_embed_jwplayer_property_key_of_' . $media_id);
  if (!isset($property_key)) {
    if ($cache = cache_get('video_embed_jwplayer_property_key_of_' . $media_id)) {
      $property_key = $cache->data;
    }
    else {
      //
      // get account property key for the media id, allow others to override the default
      $property_key = 'default';
      $account_properties = _get_account_properties();
      foreach ($account_properties as $key => $property_name) {
        if ($jwplayer_api = video_embed_jwplayer_get_instance($key)) {
          $response = $jwplayer_api->call('/videos/list', array('video_keys_filter' => $media_id));
          if ($response['status'] == 'ok' && $response['total'] > 0) {
            $property_key = $key;
            break;
          }
        }
      }

      // add data to cache
      cache_set('video_embed_jwplayer_property_key_of_' . $media_id, $property_key, 'cache');
    }
  }


  return $property_key;
}


/**
 * Returns an array containing all available video conversions for a specific
 * video uploaded to a JWPlayer account property.
 *
 * @param string $media_id
 *    The video key.
 *
 * @return array
 *    Returns an associative array of all defined video conversions,
 *    where the key is the template id of the conversion and the value is the
 *    template name.
 */
function video_embed_jwplayer_get_available_video_conversions($media_id) {
  $video_conversions = &drupal_static('video_embed_jwplayer_conversions_' . $media_id);
  if (!isset($video_conversions)) {
    if ($cache = cache_get('video_embed_jwplayer_conversions_' . $media_id)) {
      $video_conversions = $cache->data;
    }
    else {
      //
      // get the video conversions via JWPlayer management api
      $video_conversions = array();
      $property_key = video_embed_jwplayer_get_property_key($media_id);
      if ($jwplayer_api = video_embed_jwplayer_get_instance($property_key)) {
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

      // add data to cache
      cache_set('video_embed_jwplayer_conversions_' . $media_id, $video_conversions, 'cache');
    }
  }
  return $video_conversions;
}


/**
 * Gets all defined players in the given JWPlayer account property.
 *
 * @param string $property_key
 *    The key of the JWPlayer account property, from which the player info is loaded.
 *
 * @return array Returns an associative array of all defined players,
 * Returns an associative array of all defined players,
 * where the key is the player id and the value is the player name.
 */
function video_embed_jwplayer_get_players($property_key) {
  $players = &drupal_static('video_embed_jwplayer_players_' . $property_key);
  if (!isset($players)) {
    //
    // get the players via JWPlayer management api
    $players = array();
    if ($jwplayer_api = video_embed_jwplayer_get_instance($property_key)) {
      $response = $jwplayer_api->call('/players/list');

      foreach ($response['players'] as $player) {
        if ($player['responsive']) {
          $players[$player['key']] = $player['name'];
        }
      }
    }
  }
  return $players;
}

