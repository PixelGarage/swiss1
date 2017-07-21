<?php
/**
 * Contains the admin form of Swiss1 TV.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 10.05.17
 * Time: 22:34
 */

/**
 * Default Import directory for XML files
 */
define('SWISS1_IMPORT_DEFAULT_DIRECTORY', '../swiss1_media_import');


/**
 * Settings form to configure Swiss1 TV file import.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function swiss1_import_settings_form($form, &$form_state) {
  $form = array();

  //
  // run import
  $form['swiss1_action_set'] = array(
    '#type' => 'fieldset',
    '#title' => t('Run media import'),
    '#collapsible' => false,
    '#collapsed' => false,
    '#weight' => 0,
  );
  $form['swiss1_action_set']['swiss1_last_run'] = array(
    '#markup' => t('Last media import: @date', array('@date' => date('d.m.Y H:i:s', variable_get('swiss1_last_media_import', 0)))),
    '#prefix' => '<div>',
    '#suffix' => '</br></br></div>',
  );
  $form['swiss1_action_set']['swiss1_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Run media import'),
    '#submit' => array('swiss1_media_import'),
  );

  //
  // Swiss1 settings
  $play_options = array(
    'broadcast_start' => t('Broadcast start'),
    'broadcast_end' => t('Broadcast end'),
  );
  $form['swiss1_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure Swiss1 TV import'),
    '#collapsible' => true,
    '#collapsed' => false,
    '#weight' => 10,
  );
  $form['swiss1_settings']['swiss1_import_directory'] = array(
    '#type' => 'textfield',
    '#title' => t('Import Directory'),
    '#default_value' => variable_get('swiss1_import_directory') ? variable_get('swiss1_import_directory') : SWISS1_IMPORT_DEFAULT_DIRECTORY,
    '#description' => t('Defines the XML import directory relative to the Drupal root directory. All XML and image files in this directory are imported. Default: ../swiss1_media_import'),
    '#weight' => 0,
  );
  $form['swiss1_settings']['swiss1_video_playable_start'] = array(
    '#type' => 'radios',
    '#title' => t('At which time should a video be playable?'),
    '#options' => $play_options,
    '#default_value' => variable_get('swiss1_video_playable_start') ? variable_get('swiss1_video_playable_start') : 'broadcast_end',
    '#description' => t('Defines the time when a broadcast video can be played on the Website. Default: at the end of its broadcast period.'),
    '#weight' => 5,
  );
  $form['swiss1_settings']['swiss1_import_catchup_period'] = array(
    '#type' => 'textfield',
    '#title' => t('Default catchup period'),
    '#size' => 20,
    '#default_value' => variable_get('swiss1_import_catchup_period') ? variable_get('swiss1_import_catchup_period') : '+7 days',
    '#description' => t('Defines the default catchup period of a broadcast. Default is 7 days. Remark: Define period as relative date, e.g. 1 day, 4 days, 30 minutes etc.'),
    '#weight' => 10,
  );
  $day_start_options = array(
    '00:00' => t('Midnight'),
    '01:00' => t('Ein Uhr morgens'),
    '02:00' => t('Zwei Uhr morgens'),
    '03:00' => t('Drei Uhr morgens'),
    '04:00' => t('Vier Uhr morgens'),
    '12:00' => t('Zwölf Uhr mittags'),
    '13:00' => t('Ein Uhr nachmittags'),
    '14:00' => t('Zwei Uhr nachmittags'),
    '15:00' => t('Drei Uhr nachmittags'),
    '16:00' => t('Vier Uhr nachmittags'),
    '17:00' => t('Fünf Uhr nachmittags'),
    '18:00' => t('Sechs Uhr abends'),
    '19:00' => t('Sieben Uhr abends'),
    '20:00' => t('Acht Uhr abends'),
  );
  $form['swiss1_settings']['swiss1_video_day_start_time'] = array(
    '#type' => 'select',
    '#title' => t('Start time of a broadcast day'),
    '#options' => $day_start_options,
    '#default_value' => variable_get('swiss1_video_day_start_time') ? variable_get('swiss1_video_day_start_time') : '02:00',
    '#description' => t("Defines the start time of a day on the Program page. This setting has no influence on the imported data, only on the Program page display. Default: Two o'clock in the morning."),
    '#weight' => 15,
  );
  $form['swiss1_settings']['swiss1_primary_period'] = array(
    '#type' => 'fieldset',
    '#title' => t('Prime-time broadcast period per day'),
    '#collapsible' => true,
    '#collapsed' => false,
    '#weight' => 20,
  );
  $form['swiss1_settings']['swiss1_primary_period']['swiss1_primary_period_starttime'] = array(
    '#type' => 'textfield',
    '#title' => t('Prime-time start time'),
    '#size' => 20,
    '#default_value' => variable_get('swiss1_primary_period_starttime') ? variable_get('swiss1_primary_period_starttime') : '18:45',
    '#description' => t('Defines the start time of the daily prime-time broadcast period. Default is 18:45. Remark: Define the start time without a date, e.g. 18:45.'),
    '#weight' => 0,
  );
  $form['swiss1_settings']['swiss1_primary_period']['swiss1_primary_period_endtime'] = array(
    '#type' => 'textfield',
    '#title' => t('Prime-time end time'),
    '#size' => 20,
    '#default_value' => variable_get('swiss1_primary_period_endtime') ? variable_get('swiss1_primary_period_endtime') : '21:45',
    '#description' => t('Defines the end time of the daily prime-time broadcast period. Default is 21:45.'),
    '#weight' => 1,
  );


  // LIVE Streaming
  $form['swiss1_live_streaming'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure Swiss1 TV live streaming'),
    '#collapsible' => true,
    '#collapsed' => false,
    '#weight' => 25,
  );
  $form['swiss1_live_streaming']['swiss1_live_streaming_starttime'] = array(
    '#type' => 'textfield',
    '#title' => t('Live streaming player start time'),
    '#size' => 20,
    '#default_value' => variable_get('swiss1_live_streaming_starttime') ? variable_get('swiss1_live_streaming_starttime') : '-1 hour',
    '#description' => t('Defines the time relative to the live streaming start, when the streaming player is shown. Default is -1 hour. Remark: Define period as relative date, e.g. -1 hour, -30 minutes etc.'),
    '#weight' => 10,
  );
  $form['swiss1_live_streaming']['swiss1_live_streaming_endtime'] = array(
    '#type' => 'textfield',
    '#title' => t('Live streaming player end time'),
    '#size' => 20,
    '#default_value' => variable_get('swiss1_live_streaming_endtime') ? variable_get('swiss1_live_streaming_endtime') : '+1 hour',
    '#description' => t('Defines the time relative to the live streaming end, when the streaming player is hidden. Default is +1 hour. Remark: Define period as relative date, e.g. +1 hour, +30 minutes etc.'),
    '#weight' => 20,
  );


  return system_settings_form($form);
}
