<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 08.05.17
 * Time: 19:10
 */

/**
 * Hook allowing to override the default expiration time set to 2 hours after
 * the video playback has started. The timestamp format is a UNIX timestamp,
 * e.g. number of seconds since 1.1.1970.
 * The expiration timestamp is used in signed media URLs to prevent a video of
 * being played beyond the given timestamp.
 *
 * @param int   $exp_timestamp
 *    The timestamp to be altered. Default is time() + 2*3600.
 * @param $media_id
 *    The media-id of the video to be expired.
 *
 * @return mixed
 *
 */
function hook_video_embed_jwplayer_expiration_timestamp_alter(&$exp_timestamp, $media_id) {
  //
  // Example: get the expiration timestamp from a the node holding the video
  // and its media id.
  $timestamp = time() + 2 * 3600;
  return $timestamp;
}
