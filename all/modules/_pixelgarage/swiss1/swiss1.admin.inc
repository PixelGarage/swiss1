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
  $form['swiss1_settings']['swiss1_video_playable_start'] = array(
    '#type' => 'radios',
    '#title' => t('At which time should a video be playable?'),
    '#options' => $play_options,
    '#default_value' => variable_get('swiss1_video_playable_start') ? variable_get('swiss1_video_playable_start') : 'broadcast_end',
    '#description' => t('Defines the time when a broadcast video can be played on the Website. Default: at the end of its broadcast period.'),
  );
  $form['swiss1_settings']['swiss1_import_directory'] = array(
    '#type' => 'textfield',
    '#title' => t('Import Directory'),
    '#default_value' => variable_get('swiss1_import_directory') ? variable_get('swiss1_import_directory') : SWISS1_IMPORT_DEFAULT_DIRECTORY,
    '#description' => t('Defines the XML import directory relative to the Drupal root directory. All XML files in this directory are imported, if possible. Default: ../swiss1_media_import'),
  );


  return system_settings_form($form);
}
