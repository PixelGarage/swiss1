<?php
/**
 * Contains the admin form of Swiss1 TV.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 10.05.17
 * Time: 22:34
 */

function swiss1_import_settings_form($form, &$form_state) {
  $form = array();

  //
  // run import
  $form['swiss1_action_set'] = array(
    '#type' => 'fieldset',
    '#title' => t('Run media import'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );
  $form['swiss1_action_set']['swiss1_last_run'] = array(
    '#markup' => t('Last media import: @date', array('@date' => date('d.m.Y H:i:s', variable_get('swiss1_cron_last_run', 0)))),
    '#prefix' => '<div>',
    '#suffix' => '</br></br></div>',
  );
  $form['swiss1_action_set']['swiss1_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Run media import'),
    '#submit' => array('swiss1_media_import'),
  );

  return system_settings_form($form);
}
