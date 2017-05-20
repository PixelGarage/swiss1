<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 10.05.17
 * Time: 11:50
 */

/**
 * Custom skins prefix.
 */
define('JWPLAYER_CUSTOM_SKINS_PREFIX', 'custom_');


/* -----------------------------------------------------------------
 *  Video embed field JWPlayer handler
 * ----------------------------------------------------------------- */

/**
 * Implements hook_video_embed_handler_info().
 */
function video_embed_jwplayer_video_embed_handler_info() {
  $handlers = array();
  $handlers['jwplayer'] = array(
    'title' => 'JWPlayer',
    'function' => 'video_embed_jwplayer_video_callback',
    'thumbnail_default' => drupal_get_path('module', 'video_embed_jwplayer') . '/img/default.jpg',
    'form' => 'video_embed_jwplayer_form_callback',
    'domains' => array(
      'content.jwplatform.com',
    ),
    'defaults' => array(
      'jwplayer_player' => '',
      'jwplayer_skin' => 'seven',
      'jwplayer_skin_active_color' => '',
      'jwplayer_skin_inactive_color' => '',
      'jwplayer_skin_background_color' => '',
      'jwplayer_url_signing' => false,
      'jwplayer_autostart' => false,
      'jwplayer_repeat' => false,
      'jwplayer_mute' => false,
      'jwplayer_controls' => true,
      'jwplayer_displaytitle' => true,
      'jwplayer_displaydescription' => true,
      'jwplayer_stretching' => 'uniform',
      'jwplayer_about_text' => '',
      'jwplayer_about_link' => '',
    ),
  );

  return $handlers;
}


/**
 * A forms api callback, returns the settings form for the provider
 *
 * @param array $defaults
 *    Default/current values for your provider, the currently saved settings
 *    with empty values filled with the defaults provided in info hook
 *
 * @return array
 *    Form as defined by forms api
 */
function video_embed_jwplayer_form_callback($defaults) {
  $form = array();
  $players = video_embed_jwplayer_get_players();
  $skins = video_embed_jwplayer_get_skins();
  $skin_defaults = $defaults['jwplayer_skinning'];
  $stretching = array(
    'uniform' => 'Uniform',
    'fill' => 'Fill',
    'exactfit' => 'Exact fit',
    'none' => 'None',
  );

  //
  // player
  $form['jwplayer_player'] = array(
    '#type' => 'select',
    '#title' => t('Cloud Players'),
    '#options' => $players,
    '#description' => t('Select one of the defined cloud players as video player.'),
    '#default_value' => $defaults['jwplayer_player'],
  );

  //
  // skinning
  $form['jwplayer_skinning'] = array(
    '#type' => 'fieldset',
    '#title' => t('JWPlayer skinning'),
    '#collapsible' => true,
    '#collapsed' => false,
    '#description' => t('Define the skin of the player. For custom skinning see: !ref', array(
      '!ref' => l('CSS skinning reference',
        'https://developer.jwplayer.com/jw-player/docs/developer-guide/customization/css-skinning/skins_reference',
        array(
          'attributes' => array('target' => '_blank'),
          'html' => true
        )
      ),
    )),
  );
  $form['jwplayer_skinning']['jwplayer_skin'] = array(
    '#type' => 'select',
    '#title' => t('Select a skin'),
    '#options' => $skins,
    '#description' => t('Select one of the available skins for the video player.'),
    '#default_value' => $skin_defaults['jwplayer_skin'],
    '#weight' => 10,
  );
  $form['jwplayer_skinning']['jwplayer_skin_active_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Override active color'),
    '#default_value' => $skin_defaults['jwplayer_skin_active_color'],
    '#attributes' => array('placeholder' => t('#hex or html color name')),
    '#weight' => 20,
  );
  $form['jwplayer_skinning']['jwplayer_skin_inactive_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Override inactive color'),
    '#default_value' => $skin_defaults['jwplayer_skin_inactive_color'],
    '#attributes' => array('placeholder' => t('#hex or html color name')),
    '#weight' => 30,
  );
  $form['jwplayer_skinning']['jwplayer_skin_background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Override background color'),
    '#default_value' => $skin_defaults['jwplayer_skin_background_color'],
    '#attributes' => array('placeholder' => t('#hex or html color name')),
    '#weight' => 40,
  );

  //
  // basic settings
  $form['jwplayer_url_signing'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use URL signing'),
    '#description' => t('Signing URL is a security feature that enables you to lock down public access to videos and/or players.'),
    '#default_value' => $defaults['jwplayer_url_signing'],
  );
  $form['jwplayer_autostart'] = array(
    '#type' => 'checkbox',
    '#title' => t('Autoplay video'),
    '#description' => t('Starts the video immediately.'),
    '#default_value' => $defaults['jwplayer_autostart'],
  );
  $form['jwplayer_mute'] = array(
    '#type' => 'checkbox',
    '#title' => t('Mute video'),
    '#description' => t('Mutes the video on play.'),
    '#default_value' => $defaults['jwplayer_mute'],
  );
  $form['jwplayer_repeat'] = array(
    '#type' => 'checkbox',
    '#title' => t('Repeat video'),
    '#description' => t('Repeats the video when the end is reached.'),
    '#default_value' => $defaults['jwplayer_repeat'],
  );

  //
  // appearence
  $form['jwplayer_controls'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display controls'),
    '#description' => t('Displays no video controls at all, if the flag is unset. Can be used for background videos.'),
    '#default_value' => $defaults['jwplayer_controls'],
  );
  $form['jwplayer_displaytitle'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display video title'),
    '#description' => t('Displays the video title in the top left corner when paused.'),
    '#default_value' => $defaults['jwplayer_displaytitle'],
  );
  $form['jwplayer_displaydescription'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display description'),
    '#description' => t('Displays the video description in the top left corner when paused.'),
    '#default_value' => $defaults['jwplayer_displaydescription'],
  );
  $form['jwplayer_stretching'] = array(
    '#type' => 'select',
    '#title' => t('Stretch Video'),
    '#options' => $stretching,
    '#description' => t('Stretching of video content to player size. Default: uniform'),
    '#default_value' => $defaults['jwplayer_stretching'],
  );
  $form['jwplayer_about_text'] = array(
    '#type' => 'textfield',
    '#title' => t('About menu text'),
    '#default_value' => $defaults['jwplayer_about_text'],
    '#attributes' => array('placeholder' => t('Text in right click menu')),
  );
  $form['jwplayer_about_link'] = array(
    '#type' => 'textfield',
    '#title' => t('About URL link'),
    '#default_value' => $defaults['jwplayer_about_link'],
    '#attributes' => array('placeholder' => t('Custom URL to link to on right click')),
  );

  return $form;
}


/**
 * Handler for JWPlayer videos.
 *
 * @param string $url
 *   The video URL set in the field.
 * @param array $settings
 *   The settings array of the style.
 *
 * @return array
 *   The video render array.
 */
function video_embed_jwplayer_video_callback($url, $settings) {
  //
  // get media id
  $output = array();
  $media_id = video_embed_jwplayer_get_media_id($url);
  if (!$media_id) {
    // We can't decode the URL - just return the URL as a link.
    $output['#markup'] = l($url, $url);
    return $output;
  }

  //
  // add player script in HEAD
  $player_url = JWPLAYER_CONTENT_BASE_PATH . JWPLAYER_CLOUD_PLAYER_PATH . $settings['jwplayer_player'] . '.js';
  drupal_add_js($player_url,
    array('type' => 'external', 'scope' => 'header', 'weight' => 1000)
  );

  //
  // get skin settings and add skn css to HEAD
  $skin_settings = $settings['jwplayer_skinning'];
  $selected_skin = $skin_settings['jwplayer_skin'];
  if (substr($selected_skin, 0, strlen(JWPLAYER_CUSTOM_SKINS_PREFIX)) === JWPLAYER_CUSTOM_SKINS_PREFIX) {
    //
    // add the custom css file to HEAD
    $selected_skin = str_replace(JWPLAYER_CUSTOM_SKINS_PREFIX, '', $selected_skin);
    $skin_file = drupal_get_path('module', 'video_embed_jwplayer') . '/css/' . $selected_skin . '.css';
    drupal_add_css($skin_file,
      array('type' => 'file', 'scope' => 'header')
    );
  }
  $active_color = !empty($skin_settings['jwplayer_skin_active_color']) ? ('", active: "' . $skin_settings['jwplayer_skin_active_color']) : '';
  $inactive_color = !empty($skin_settings['jwplayer_skin_active_color']) ? ('", inactive: "' . $skin_settings['jwplayer_skin_inactive_color']) : '';
  $background_color = !empty($skin_settings['jwplayer_skin_background_color']) ? ('", background: "' . $skin_settings['jwplayer_skin_background_color']) : '';

  //
  // define video embed
  $do_sign_url = $settings['jwplayer_url_signing'];
  $transcoding_templates = video_embed_jwplayer_get_available_transcoding_templates();
  $autoplay = $settings['jwplayer_autostart'] ? 'true' : 'false';
  $mute = $settings['jwplayer_mute'] ? 'true' : 'false';
  $repeat = $settings['jwplayer_repeat'] ? 'true' : 'false';
  $show_controls = $settings['jwplayer_controls'] ? 'true' : 'false';
  $display_title = $settings['jwplayer_displaytitle'] ? 'true' : 'false';
  $display_desc = $settings['jwplayer_displaydescription'] ? 'true' : 'false';
  $playlist = 'playlist: [{' .
    'image: "' . video_embed_jwplayer_get_media_url($media_id, 'jpg', $do_sign_url) . '", ' .
    'mediaid: "' . $media_id . '", ' .
    'sources: [{' .
    'file: "' . video_embed_jwplayer_get_media_url($media_id, 'mpd', $do_sign_url) . '"}, {' .
    'file: "' . video_embed_jwplayer_get_media_url($media_id, 'm3u8', $do_sign_url);
  foreach ($transcoding_templates as $key => $quality) {
    $playlist .= '"}, {file: "' . video_embed_jwplayer_get_media_url($media_id, 'mp4', $do_sign_url, $key) .
      '", label: "' . $quality;
  }
  $playlist .= '"}]}],';

  $video_embed = '<div id="jwplayer-video-embed"></div><script type="text/javaScript">' .
    'jwplayer("jwplayer-video-embed").setup({' .
    $playlist .
    'skin: { ' .
      'name: "' . $selected_skin .
      $active_color . $inactive_color . $background_color .
    '"}, ' .
    'autostart: ' . $autoplay . ', ' .
    'mute: ' . $mute . ', ' .
    'repeat: ' . $repeat . ', ' .
    'controls: ' . $show_controls . ', ' .
    'displaytitle: ' . $display_title . ', ' .
    'displaydescription: ' . $display_desc . ', ' .
    'stretching: "' . $settings['jwplayer_stretching'] . '", ' .
    'abouttext: "' . $settings['jwplayer_about_text'] . '", ' .
    'aboutlink: "' . $settings['jwplayer_about_link'] . '", ' .
    '});</script>';

  //
  // return render array
  $output['#markup'] = $video_embed;
  return $output;
}


/* -----------------------------------------------------------------
 *  Helper functions
 * ----------------------------------------------------------------- */

/**
 * Helper function to get the video media id from the input URL.
 *
 * @param string $url
 *   The video URL set in the field.
 *
 * @return string|bool
 *   The video media ID, or FALSE in case the ID can't be retrieved from the URL.
 */

function video_embed_jwplayer_get_media_id($url) {
  //
  // the url is in the form: http(s)://content.jwplatform.com/[TYPE/]MEDIA_ID[-TEMP_ID].EXT[?parameters]
  //
  // get file name
  $splits = explode('/', $url);
  $media_id = array_pop($splits);

  //
  // remove extension and signing parameters
  if ($media_id && $pos = strpos($media_id, '.')) {
    $media_id = substr($media_id, 0, $pos);
  }

  //
  // remove template id. if any
  if ($media_id && $pos = strpos($media_id, '-')) {
    $media_id = substr($media_id, 0, $pos);
  }

  return $media_id ? check_plain($media_id) : false;
}

/**
 * Generates a JWPlayer media URL. If $secret is set with the value from the
 * JWPlatform API Credentials, then the URL is signed.
 *
 * @param string $media_id
 *    The media id of the content for which the URL has to be created.
 * @param string $type
 *    The type (extension) of the media file. Supported are: mdp, m3u8, mp4, jpg
 * @param bool $do_sign
 *    Flag indicating if the media url has to be signed, e.g. the media can be
 *    limited to a given period. Default is false.
 * @param string $templ_param
 *    The key of a transcoding template, if type is mp4. Default is none.
 *    The width of a thumb image, if type is jpg. The following width (px) are available:
 *    '40', '120', '320', '480', '720', '1280', default is '1280'.
 * @param string $domain
 *    The masked domain name you want to use for signing the content, e.g.
 *    "https://video.mysite.com". Use this if you have setup DNS masking. Defaults to
 *    "//content.jwplatform.com/".
 *
 * @return string The signed url.
 * The signed url.
 */
function video_embed_jwplayer_get_media_url($media_id, $type, $do_sign = false, $templ_param = '', $domain = JWPLAYER_CONTENT_BASE_PATH) {
  //
  // create path according to the input parameters
  switch ($type) {
    case 'mdp':
    case 'm3u8':
      $path = JWPLAYER_MANIFEST_PATH . $media_id;
      break;
    case 'mp4':
      $path = JWPLAYER_VIDEO_PATH . $media_id;
      if (!empty($templ_param)) {
        $path .= '-' . $templ_param;
      }
      break;
    case 'jpg':
    case 'jpeg':
      $path = JWPLAYER_THUMB_PATH . $media_id;
      switch ($templ_param) {
        case '40':
        case '120':
        case '320':
        case '480':
        case '720':
        case '1280':
          $path .= '-' . $templ_param;
          break;
        default:
          $path .= '-1280';
      }
      $type = 'jpg';
      break;
    default:
      $path = $media_id;
  }
  $path .= '.' . $type;
  $media_url = $domain . $path;

  //
  // sign the URL, if a secret is available
  $secret = ($do_sign && !empty(variable_get('video_embed_jwplayer_api_secret'))) ? variable_get('video_embed_jwplayer_api_secret') : false;

  if ($secret) {
    //
    // set default expiration date
    $exp_timestamp = time() + 2 * 3600;

    //
    // allow others to alter expiration timestamp
    drupal_alter('video_embed_jwplayer_expiration_timestamp', $exp_timestamp, $media_id);

    //
    // create signed URL
    $signature = md5($path . ':' . $exp_timestamp . ':' . $secret);
    $media_url .= '?exp=' . $exp_timestamp . '&sig=' . $signature;
  }

  return $media_url;
};


function video_embed_jwplayer_get_skins () {
  $skins = array(
    'beelden' => 'Beelden',
    'bekle' => 'Bekle',
    'five' => 'Five',
    'glow' => 'Glow',
    'roundster' => 'Roundster',
    'seven' => 'Seven',
    'six' => 'Six',
    'stormtrooper' => 'Stormtrooper',
    'vapor' => 'Vapor',
  );

  //
  // scan css directory for custom skins
  $path = drupal_get_path('module', 'video_embed_jwplayer') . '/css';
  $files = scandir($path);
  if ($files) {
    $files = array_diff($files, array('.', '..'));
    foreach ($files as $file) {
      if (strpos($file, '.css') == (strlen($file) - 4)) {
        $skin = str_replace('.css', '', $file);
        $skins[JWPLAYER_CUSTOM_SKINS_PREFIX . $skin] = ucfirst($skin);
      }
    }
    asort($skins);
  }

  // return array sorted by value
  return $skins;
}
