<?php

/**
 * @file
 * Definition of views_bootstrap_plugin_style.
 */

/**
 * Class to define a style plugin handler.
 */
class ViewsBootstrapTabPluginStyle extends views_plugin_style {

  /**
   * Definition.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['tab_field'] = array('default' => NULL);
    $options['tab_type'] = array('default' => 'tabs');
    $options['tab_position'] = array('default' => 'basic');
    $options['tab_fade'] = array('default' => FALSE);
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

      $form['tab_field'] = array(
        '#type' => 'select',
        '#title' => t('Tab field'),
        '#options' => $options,
        '#required' => TRUE,
        '#default_value' => $this->options['tab_field'],
        '#description' => t('Select the field that will be used as the tab.'),
      );

      $form['tab_type'] = array(
        '#type' => 'select',
        '#title' => t('Tab Type'),
        '#options' => array(
          'tabs' => t('Tabs'),
          'pills' => t('Pills'),
          'list' => t('List'),
        ),
        '#required' => TRUE,
        '#default_value' => $this->options['tab_type'],
      );

      $form['tab_position'] = array(
        '#type' => 'radios',
        '#title' => t('Position of tabs'),
        '#options' => array(
          'basic' => t('Tabs/pills on the top'),
          'left' => t('Tabs/pills on the left'),
          'right' => t('Tabs/pills on the right'),
          'below' => t('Tabs/pills on the bottom'),
          'justified' => t('Tabs/pills justified on the top'),
          'stacked' => t('Tabs/pills stacked'),
        ),
        '#required' => TRUE,
        '#default_value' => $this->options['tab_position'] ? $this->options['tab_position'] : 'basic',
      );

      $form['tab_fade'] = array(
        '#type' => 'checkbox',
        '#title' => t('Fade Effect'),
        '#default_value' => $this->options['tab_fade'],
        '#description' => t('Add a fade in effect when tabs clicked'),
      );
    }
  }

}
