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
 * Define run types of VODEntry
 */
define('SWISS1_RUNTYPE_PRIMARY', 'PRIMARY');
define('SWISS1_RUNTYPE_RERUN', 'RERUN');


/* -----------------------------------------------------------------
 *  Media cleanup
 * ----------------------------------------------------------------- */

/**
 * Deletes all expired video streams according to the imported video entry settings.
 *
 * A VideoStream can be created for multiple runs. The date "End of license"
 * indicates, if the VideoSttream and its linked media files can be deleted after
 * the validity period or not.
 */
function swiss1_delete_expired_media() {
  //
  // delete all video streams with an expiration date in the past (catch-up rights)
  $now = date('Y-m-d H:i:s'); // format matters for comparison to work
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'videostream')
    ->propertyCondition('status', 1)
    ->fieldCondition('field_end_of_license', 'value', $now, '<');
  $result = $query->execute();

  if (isset($result['node']) && !empty($result['node'])) {
    node_delete_multiple(array_keys($result['node']));
  }
}


/* -----------------------------------------------------------------
 *  Media importer
 * ----------------------------------------------------------------- */

/**
 * Imports all new XML media files. Processed files are deleted.
 */
function swiss1_media_import () {
  //
  // Swiss1 TV media import
  $import_counter = array(
    '@files' => 0,
    '@created' => 0,
    '@updated' => 0,
    '@failed' => 0,
  );

  try {
    //
    // import all xml files in the dedicated directory
    $import_dir = variable_get('swiss1_import_directory') ?
      variable_get('swiss1_import_directory') : SWISS1_IMPORT_DEFAULT_DIRECTORY;
    $files = scandir($import_dir);
    if ($files) {
      sort($files);
      foreach ($files as $file) {
        $file_path = $import_dir . '/' . $file;
        if (is_file($file_path) && mime_content_type($file_path) == 'application/xml') {
          // import file
          _import_xml_file($file_path, $import_counter);
          $import_counter['@files']++;

          // delete file
          unlink($file_path);
        }
      }
    }

  } catch (Exception $ex) {
    watchdog('Swiss1 Importer',$ex->getMessage(), array(), WATCHDOG_ERROR);
    drupal_set_message($ex->getMessage(), 'error');
  }

  // display feedback about import, if any
  if ($import_counter) {
    $importMessage = t('Swiss1 TV media import - @files files imported: @created videos created, @updated videos updated, @failed videos failed.');
    if($import_counter['@failed']) {
      drupal_set_message(t($importMessage, $import_counter), 'warning');
      watchdog('Swiss1 Importer', $importMessage, $import_counter, WATCHDOG_WARNING);
    }
    else {
      drupal_set_message(t($importMessage, $import_counter));
      watchdog('Swiss1 Importer', $importMessage, $import_counter, WATCHDOG_INFO);

    }
  }

  //
  // update last run variable
  variable_set('swiss1_last_media_import', REQUEST_TIME);
}

/**
 * Imports an XML file into the CMS database.
 *
 * @param string $filepath
 *    The filepath of the xml file to be imported.
 * @param array  $import_counter
 *    Result array with numbers of created and updated VODEntries.
 *
 * @throws \Exception
 */
function _import_xml_file($filepath, &$import_counter) {
  //
  // read xml file
  $xml = simplexml_load_file($filepath);

  switch($xml->getName()) {
    case 'OnlineCMSSchedule':
      // file contains EPG entries
      $xpath = '//Entry';
      break;
    case 'VODEntries':
      // file contains VOD entries
      $xpath = '//VODEntry';
      break;
    default:
      // no valid xml file
      $msg = t('The xml file does not conform to the Swiss1 XML document type (see XSD files for XML definition).');
      throw new Exception($msg);
  }

  //
  // TODO: Delete all entries of a day schedule, if schedule already exists (day id?)

  //
  // select the video entries in the file and import it
  $message = '';
  $xml_entries = $xml->xpath($xpath);
  foreach ($xml_entries as $xml_entry) {
    $success = _parse_entry($xml_entry, $import_counter);
    if (is_string($success)) {
      $message .= $success . "\n";
    }
  }
  if (!empty($message)) {
    $warning = t('Import of file "@filepath" with warnings: @warn', array(
      '@filepath' => $filepath,
      '@warn' => $message,
    ));
    watchdog('Swiss1 Importer', $warning, array(), WATCHDOG_WARNING);
    drupal_set_message($warning, 'warning');
  }

}


/**
 * Parses an XML entry and create or update a node of type VideoStream.
 *
 * @param object $xml_entry
 *    An object of class SimpleXMLElement containing the data of a VODEntry.
 *
 * @param array  $import_counter
 *    An array holding the result of the xml import of VODEntries.
 *
 * @return bool|string   Returns true, if import of entry was successful, or an err_msg in case of missing data.
 */
function _parse_entry($xml_entry, &$import_counter) {
  //
  // convert xml entry into associative array
  $updated = false;
  $json = json_encode($xml_entry);
  $arr_entry = json_decode($json, true);

  //
  // import/update xml entry into existing or new node
  // only scheduled VODEntries can be updated, single VODEntries are always created (new)
  $node = false;
  $schedule_id = !empty($arr_entry['ScheduleReference']['AssetReconciliationId']) ? $arr_entry['ScheduleReference']['AssetReconciliationId'] : false;
  if ($schedule_id) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'videostream')
      ->fieldCondition('field_schedule_id', 'value', $schedule_id)
      ->range(0, 1);
    $result = $query->execute();
    if (isset($result['node'])) {
      // Update existing
      $node = node_load(key($result['node']));
      $node->status = 1;
      $updated = true;
    }
  }

  // create new node, if it not already exists
  if (!$node) {
    $node = new stdClass();
    $node->type = 'videostream';
    $node->language = LANGUAGE_NONE;
    $node->uid = 1;
    $node->status = 1;
    node_object_prepare($node);
  }

  //
  // update all VideoStream fields
  //
  // schedule reference
  $node->field_schedule_id[LANGUAGE_NONE][0]['value'] = $schedule_id ? $schedule_id : '';

  // Airing information
  // Only for scheduled entries in the future
  if (isset($arr_entry['AiringInformation'])) {
    $airing_info = $arr_entry['AiringInformation'];

    // get broadcast start time
    if (!empty($airing_info['ScheduledStartDateTime'])) {
      // replace . with - to guarantee the correct parsing in strtotime: dash means European date format d-m-Y
      $start_time = str_replace('.', '-', $airing_info['ScheduledStartDateTime']);
      $start_timestamp = strtotime($start_time);
      $start_date = date('Y-m-d H:i:s', $start_timestamp);
      $node->field_broadcast_date[LANGUAGE_NONE][0]['value'] = $start_date;
    }
    else {
      $msg = t('No start date time has been defined in VDOEntry with schedule id = @id.', array('@id' => $schedule_id));
      $import_counter['@failed']++;
      return $msg;
    }

    // get broadcast end time (must be > start time)
    if (!empty($airing_info['ScheduledEndDateTime'])) {
      // replace . with - to guarantee the correct parsing in strtotime: dash means European date format d-m-Y
      $end_time = str_replace('.', '-', $airing_info['ScheduledEndDateTime']);
      $end_timestamp = strtotime($end_time);
      if ($end_timestamp > $start_timestamp) {
        $end_date = date('Y-m-d H:i:s', $end_timestamp);
        $node->field_broadcast_date[LANGUAGE_NONE][0]['value2'] = $end_date;
      }
      else {
        $msg = t('End date time set before start date time for VDOEntry with schedule id = @id.', array('@id' => $schedule_id));
        $import_counter['@failed']++;
        return $msg;
      }
    }
    else {
      $msg = t('No end date time has been defined in VDOEntry with schedule id = @id.', array('@id' => $schedule_id));
      $import_counter['@failed']++;
      return $msg;
    }

    // set end of license date
    $eol_timestamp = false;
    if (!empty($airing_info['EndOfLicense'])) {
      // replace . with - to guarantee the correct parsing in strtotime: dash means European date format d-m-Y
      $eol_time = str_replace('.', '-', $airing_info['EndOfLicense']);
      $eol_timestamp = strtotime($eol_time);
      $node->field_end_of_license[LANGUAGE_NONE][0]['value'] = date('Y-m-d H:i:s', $eol_timestamp);
    }


    // Set validity period
    // REMARK: VODEntries with a validity end time in the past will be deleted during the next cleanup run
    // get catchup period
    $catchup_period = !empty($airing_info['CatchupPeriod']) ? _get_catchup_period($airing_info['CatchupPeriod']) : 'now';
    $run_type = !empty($airing_info['RunType']) ? strtoupper($airing_info['RunType']) : 'UNDEFINED';
    $playable_on_broadcast_start = variable_get('swiss1_video_playable_start') == 'broadcast_start';

    // validity start date
    if ($playable_on_broadcast_start) {
      // set validity period start to broadcast start time
      $node->field_validity_period[LANGUAGE_NONE][0]['value'] = $start_date;
      $validity_start_timestamp = $start_timestamp;
    }
    else {
      // set validity period start to broadcast end time
      $node->field_validity_period[LANGUAGE_NONE][0]['value'] = $end_date;
      $validity_start_timestamp = $end_timestamp;
    }

    // validity end date
    switch ($run_type) {
      case SWISS1_RUNTYPE_PRIMARY:
        // set expiration date from catchup period
        $exp_timestamp = strtotime($catchup_period, $validity_start_timestamp);
        $exp_timestamp = ($eol_timestamp && $eol_timestamp < $exp_timestamp) ? $eol_timestamp : $exp_timestamp;
        $exp_date = date('Y-m-d H:i:s', $exp_timestamp);
        break;
      case SWISS1_RUNTYPE_RERUN:
        // get catchup period from PRIMARY schedule
        //$primary_schedule_id = $arr_entry['PrimaryScheduleId']); // TODO: get primary schedule id from VODEntry
        //$exp_date = _get_validity_end_date($primary_schedule_id);
        //break;
      default:
        // set catchup period to 25 years, if no run type is given
        $exp_timestamp = $eol_timestamp ? $eol_timestamp : strtotime('+25 years', $validity_start_timestamp);
        $exp_date = date('Y-m-d H:i:s', $exp_timestamp);
        break;
    }
    $node->field_validity_period[LANGUAGE_NONE][0]['value2'] = $exp_date;

    // get duration and feature flag
    $node->field_duration[LANGUAGE_NONE][0]['value'] = !empty($airing_info['Duration']) ? $airing_info['Duration'] : '';
    $featured = (!empty($airing_info['Featured']) && $airing_info['Featured'] == 'true') ? 1 : 0;
    $node->field_featured[LANGUAGE_NONE][0]['value'] = $featured;
  }

  //
  // Publishing Information (only for entries not scheduled in program)
  // Default period is [now, now + 25 years]
  if (isset($arr_entry['PublishingInformation'])) {
    $publish_info = $arr_entry['PublishingInformation'];

    if (!empty($publish_info['ValidityDateTimeFrom'])) {
      $valid_from = str_replace('.', '-', $publish_info['ValidityDateTimeFrom']);
      $from_timestamp = strtotime($valid_from);
      $node->field_validity_period[LANGUAGE_NONE][0]['value'] = date('Y-m-d H:i:s', $from_timestamp);
    }
    else {
      // set now as validity period start
      $node->field_validity_period[LANGUAGE_NONE][0]['value'] = date('Y-m-d H:i:s');
    }
    if (!empty($publish_info['ValidityDateTimeTo'])) {
      $valid_to = str_replace('.', '-', $publish_info['ValidityDateTimeTo']);
      $to_timestamp = strtotime($valid_to);
      $node->field_validity_period[LANGUAGE_NONE][0]['value2'] = date('Y-m-d H:i:s', $to_timestamp);
    }
    else {
      // set period to 25 years
      $timestamp = strtotime('+25 years');
      $node->field_validity_period[LANGUAGE_NONE][0]['value2'] = date('Y-m-d H:i:s', $timestamp);
    }

  }

  //
  // EPG infos / Category (can be empty)
  if (isset($arr_entry['Description'])) {
    $epg = isset($arr_entry['Description']['Epg']) ? $arr_entry['Description']['Epg'] : false;
    if ($epg) {
      $node->title = !empty($epg['Title']) ? $epg['Title'] : (!empty($epg['Headline']) ? $epg['Headline'] : t('Kein Titel'));
      $synopsis = !empty($epg['Synopsis']) ? $epg['Synopsis'] : t('<p>Keine Information zur Sendung verfügbar.</p>');
      $synopsis .= !empty($epg['PresentedBy']) ? t('<p>Moderator: @moderator</p>', array('@moderator' => $epg['PresentedBy'])) : '';
      $synopsis .= !empty($epg['Interviewee']) ? t('<p>Gast: @guest</p>', array('@guest' => $epg['Interviewee'])) : '';
      $node->body[LANGUAGE_NONE][0]['value'] = $synopsis;
      $node->body[LANGUAGE_NONE][0]['format'] = 'full_html';
      $node->field_video_language[LANGUAGE_NONE][0]['value'] = !empty($epg['Language']) ? $epg['Language'] : t('Deutsch');
      $node->field_age_restriction[LANGUAGE_NONE][0]['value'] = !empty($epg['AgeRestriction']) ? $epg['AgeRestriction'] : t('6+');
      $node->field_series[LANGUAGE_NONE][0]['value'] = !empty($epg['Series']) ? $epg['Series'] : '';
      $node->field_season[LANGUAGE_NONE][0]['value'] = !empty($epg['Season']) ? $epg['Season'] : '';
      $node->field_episode[LANGUAGE_NONE][0]['value'] = !empty($epg['EpisodeNumber']) ? $epg['EpisodeNumber'] : '';
      $node->field_category[LANGUAGE_NONE][0]['tid'] =
        !empty($epg['Genre']) ? _get_category_key($epg['Genre']) : _get_category_key();

      //$buzzword = !empty($epg['Buzzword']) ? $epg['Buzzword'] : '';
      //$keyword = !empty($epg['Keyword']) ? $epg['Keyword'] : '';
    }
  }

  //
  // Properties
  // retrieve path information, if any otherwise try to create path with  media id.
  if (isset($arr_entry['Properties'])) {
    $properties = $arr_entry['Properties'];

    // set video stream and media ID, if available
    $media_id = '';
    $video_url = !empty($properties['StreamingLink']) ? $properties['StreamingLink'] : '';
    if (!empty($video_url) && module_exists('video_embed_jwplayer')) {
      $media_id = video_embed_jwplayer_get_media_id($video_url);
      $node->field_media_id[LANGUAGE_NONE][0]['value'] = $media_id ? $media_id : '';
      $video_url = $media_id ? JWPLAYER_CONTENT_BASE_PATH . $media_id : $video_url;
    }
    $node->field_video_path[LANGUAGE_NONE][0]['video_url'] = $video_url;

    // set thumbnails, if any
    $thumb_path = !empty($properties['ThumbnailFTPPathFilename']) ? $properties['ThumbnailFTPPathFilename'] : false;
    $file_obj = _get_managed_image_obj($thumb_path, $media_id);
    $node->field_thumb_path[LANGUAGE_NONE][0] = $file_obj ? (array)$file_obj : '';

    // set poster, if any
    $poster_path = !empty($properties['PosterframeFTPPathFilename']) ? $properties['PosterframeFTPPathFilename'] : false;
    $file_obj = _get_managed_image_obj($poster_path, $media_id);
    $node->field_poster[LANGUAGE_NONE][0] = $file_obj ? (array)$file_obj : '';

  }

  //
  // save/update node
  if ($updated) {
    $import_counter['@updated']++;
  }
  else {
    $import_counter['@created']++;
  }

  $node = node_submit($node);
  node_save($node);
  return true;
}


/* -----------------------------------------------------------------
 *  Helper functions
 * ----------------------------------------------------------------- */

/**
 * Get the validity period end date of the node with
 * the given schedule id.
 *
 * @return bool
 */
function _get_validity_end_date($primary_schedule_id) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'videostream')
    ->fieldCondition('field_schedule_id', 'value', $primary_schedule_id)
    ->range(0, 1);
  $result = $query->execute();
  if (isset($result['node'])) {
    // Update existing
    $node = node_load(key($result['node']));
    return $node->field_validity_period[LANGUAGE_NONE][0]['value2'];
  }
  return false;
}

/**
 * Transforms the given catchup period into a relative time format.
 *
 * @param $catchup_period
 */
function _get_catchup_period($catchup_period) {
  $catchup_period = strtolower($catchup_period);
  if (strpos($catchup_period, 'tage') !== false) {
    $catchup_period = str_replace('tage', 'days', $catchup_period);
  }
  else if (strpos($catchup_period, 'tag') !== false) {
    $catchup_period = str_replace('tag', 'day', $catchup_period);
  }
  else if (strpos($catchup_period, 'stunden') !== false) {
    $catchup_period = str_replace('stunden', 'hours', $catchup_period);
  }
  else if (strpos($catchup_period, 'stunde') !== false) {
    $catchup_period = str_replace('stunde', 'hour', $catchup_period);
  }

  // check prefix
  if (strpos($catchup_period, '+') === false) {
    $catchup_period = '+' . $catchup_period;
  }

  return $catchup_period;
}

/**
 * Gets the Category term key of the given term name.
 */
function _get_category_key($term_name = '') {
  $term = !empty($term_name) ? taxonomy_get_term_by_name($term_name, 'kategorie') : false;

  if ($term) {
    return key($term);
  }
  else {
    // create the category term
    return _create_new_category($term_name);
  }
}

/**
 * @param $term_name
 *
 * @return mixed
 */
function _create_new_category($term_name) {
  // get kategorie vocabulary id
  $vid = null;
  foreach(taxonomy_get_vocabularies() as $vocabulary) {
    if (strtolower($vocabulary->name) == 'kategorie') {
      $vid = $vocabulary->vid;
      break;
    }
  }

  // create the new category term in kategorie vocabulary
  if (!empty($vid) && $term_name) {
    $term = new stdClass();
    $term->name = $term_name;
    $term->vid = $vid;
    taxonomy_term_save($term);
    return $term->tid;
  }

  // return default category tid
  $default = taxonomy_get_term_by_name('Allgemein', 'kategorie');
  return key($default);
}

/**
 * Returns the managed image object for the given file path, or false if no path is available.
 *
 */
function _get_managed_image_obj($img_path, $media_id = '') {
  //
  // get impoprt directory path
  $img_dir = '/images';
  $import_dir = variable_get('swiss1_import_directory') ?
    variable_get('swiss1_import_directory') : SWISS1_IMPORT_DEFAULT_DIRECTORY;

  if ($img_path) {
    //
    // check, if FTP path is given
    $len = strlen($img_dir);
    if (substr($img_path, 0, $len) === $img_dir) {
      //
      // copy image on FTP Server to folder public://images
      $img_path = $import_dir . $img_path;
      if ($img_uri = file_unmanaged_copy($img_path, 'public://images', FILE_EXISTS_REPLACE)) {
        //
        // check if image already exists in db
        if ($file_obj = managed_file_exists($img_uri)) {
          return $file_obj;
        }

        //
        // create file object
        $file_obj = new stdClass();
        $file_obj->fid = NULL;
        $file_obj->uri = $img_uri;
        $file_obj->filename = drupal_basename($img_uri);
        $file_obj->filemime = file_get_mimetype($img_uri);
        $file_obj->status   = FILE_STATUS_PERMANENT;
        $file_obj->uid      = 1;
        $file_obj->display  = 1;
        $file_obj = file_save($file_obj);
        return $file_obj;
      }
      else {
        // file could not be copied
        return false;
      }
    }
    else if (strpos($img_path, 'http') !== false) {
      // check for remote file path
      $img_uri = $img_path;
    }
    else {
      // no valid image path
      return false;
    }
  }
  else if ($media_id && module_exists('video_embed_jwplayer')) {
      //
      // no image path is given, create one if possible (JWPlayer thumbs path)
      $img_uri = video_embed_jwplayer_get_media_url($media_id, 'jpg', true, '1280');
  }
  else {
    // no valid image path
    return false;
  }

  // Create managed file from remote file. If a managed file with the destination name exists
  // then its database entry will be updated
  $file_obj = system_retrieve_file($img_uri, 'public://images', true,FILE_EXISTS_REPLACE);

  return $file_obj;

}

/**
 * Returns the file object, if file with uri is already imported, otherwise false.
 */
function managed_file_exists($uri) {
  $fid = db_select('file_managed', 'f')
    ->fields('f', array('fid'))
    ->condition('uri', $uri)
    ->execute()
    ->fetchField();

  if ($fid) {
    // File already imported, load file
    return file_load($fid);
  }
  return false;
}
