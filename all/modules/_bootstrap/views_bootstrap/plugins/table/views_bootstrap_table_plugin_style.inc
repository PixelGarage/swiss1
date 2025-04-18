<?php

/**
 * @file
 * Definition of views_bootstrap_plugin_style.
 */

/**
 * Class to define a style plugin handler.
 */
class ViewsBootstrapTablePluginStyle extends views_plugin_style_table {

  /**
   * Definition.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['bootstrap_styles'] = array('default' => array());
    $options['responsive'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Form.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['responsive'] = array(
      '#type' => 'checkbox',
      '#title' => t('Responsive'),
      '#default_value' => $this->options['responsive'],
      '#description' => t('To make a table scroll horizontally on small devices.'),
    );

    $form['bootstrap_styles'] = array(
      '#title' => t('Bootstrap styles'),
      '#type' => 'checkboxes',
      '#default_value' => $this->options['bootstrap_styles'],
      '#options' => array(
        'striped' => t('Striped'),
        'bordered' => t('Bordered'),
        'hover' => t('Hover'),
        'condensed' => t('Condensed'),
      ),
    );
  }

}
