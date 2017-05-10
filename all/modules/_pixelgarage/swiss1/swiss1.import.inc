<?php
/**
 * Contains the Swiss1 TV media files and meta data import.
 * The implementation of the importer is based on a XML file interface.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 10.05.17
 * Time: 22:26
 */

/**
 * Import directory for XML files
 */
define('SWISS1_IMPORT_DIRECTORY', '../media_import');


/**
 * Imports all new XML media files. Processed files are deleted.
 */
function swiss1_media_import () {
  //
  // Swiss1 TV media import
  $result = null;

  try {
    //
    // import all xml files in the dedicated directory
    $files = scandir(SWISS1_IMPORT_DIRECTORY);
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

  } catch (Exception $ex) {
    watchdog('Swiss1 Importer',$ex->getMessage(), array(), WATCHDOG_ERROR);
    drupal_set_message($ex->getMessage(), 'error');
  }

  // display feedback about import, if any
  if ($result) {
    $importMessage = 'Swiss1 TV media import - @new video streams created, @updates video streams updated.';
    drupal_set_message(t($importMessage, $result));
    watchdog('HSO Importer', $importMessage, $result, WATCHDOG_INFO);
  }
}
