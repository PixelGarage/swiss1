<?php

/**
 * @file
 * Definition of views_bootstrap_plugin_style.
 */

/**
 * Class to define a style plugin handler.
 */
class ViewsBootstrapAccordionPluginStyle extends views_plugin_style {

  /**
   * Definition.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['title_field'] = array('default' => NULL);
    $options['behavior'] = array('default' => 'closed');
    return $options;
  }

  /**
   * Options form.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    if (isset($form['grouping'])) {
      $options = array();
      foreach (element_children($form['grouping']) as $key => $value) {
        if (!empty($form['grouping'][$key]['field']['#options']) && is_array($form['grouping'][$key]['field']['#options'])) {
          $options = array_merge($options, $form['grouping'][$key]['field']['#options']);
        }
      }

      $form['title_field'] = array(
        '#type' => 'select',
        '#title' => t('Title field'),
        '#options' => $options,
        '#required' => TRUE,
        '#default_value' => $this->options['title_field'],
        '#description' => t('Select the field that will be used as the title.'),
      );
      $form['behavior'] = array(
        '#type' => 'radios',
        '#title' => t('Collapse Options'),
        '#options' => array(
          'closed' => t('All Items Closed'),
          'first' => t('First Item Open'),
          'all' => t('All Items Open'),
        ),
        '#required' => TRUE,
        '#description' => t('Default panel state for collapse behavior.'),
        '#default_value' => $this->options['behavior'],
      );
    }
  }

}
