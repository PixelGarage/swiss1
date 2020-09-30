<?php

/**
 * @file
 * Administrative page code for the CookiePro module.
 */

/**
 * Administrative settings.
 *
 * @return array

 */
function cookiepro_header_settings($form, &$form_state) {
  $header_section = variable_get('cookiepro_header_settings');


  $form['cookiepro_header']['description'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Getting Started with CookiePro'),
    '#description'   => t('<p><br> The CookiePro module requires a CookiePro account. To sign up for a free or paid account, visit <a href="https://www.cookiepro.com/pricing/?referral=DRUPMOD" target="_blank">CookiePro.com</a> and select the edition that fits your business needs.</br>

Once you have access to your CookiePro account, follow the steps below or check out the <a href="https://www.cookiepro.com/help/technical-implementation/" target="_blank">Getting Started</a> guide to get CookiePro up and running on your Drupal website.</p></br>
<p><strong>Cookie Banner & Preference Center</strong></p>

                                  <p>1. Scan your website and review your cookies categories</p>
                                   <p>2. Style and configure your banner and preference center</p>
                                   <p>3. Block cookies using a tag manager and/or JS Rewrite</p>
                                   <p>4. Copy and paste the Main Cookies Script Tag below</p>
                                   <p>5. Save the configuration to publish the cookie banner</p></br>

<p>Your banner is now viewable on your Drupal website! Detailed step-by-step instructions and best practices are available in the <a href="https://community.cookiepro.com" target="_blank">CookiePro Community</a>.</p>

                                   '),
    '#size' => 0,
  );


  $form['cookiepro_header']['scripts'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Insert the Main Script Tag from CookiePro here. Remove the comments at the start and end of the script and then save the configuration.'),
    '#default_value' => isset($header_section['scripts']) ? $header_section['scripts'] : '',
    '#rows'          => 8,
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => 'Save  Configuration',
  );

  $form['cookiepro_header']['footer'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Additional Information'),
    '#description'   => t('<p>In addition to your cookie banner and preference center, CookiePro automatically generates the following scripts that can be added to your Cookie or Privacy Notice page.</p>


                                  <p>1. The Cookie Settings script inserts a button on your site to enable visitors to access their cookie preferences at any time</p>
                                   <p>2. The Cookie List script inserts a detailed list of cookies, including descriptions and categories they are assigned to</p></br>
                            <p><strong> Disclaimer </strong></p>

<p>This module allows you to publish CookiePro’s cookie banner and preference center on your Drupal website. Use of this module does not, by itself, ensure compliance with legal requirements related to cookies.</p>

                                '),
    '#size' => 0,
  );
  return $form;
}

/**
 * Submit handler().
 */
function cookiepro_header_settings_submit($form, &$form_state) {
  $header['scripts'] = $form_state['values']['scripts'];
  variable_set('cookiepro_header_settings', $header);
}
