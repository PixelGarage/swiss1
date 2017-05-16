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
define('SWISS1_IMPORT_DIRECTORY', '../swiss1_media_import');

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
 * A VideoStream can be created for multiple runs. A flag delete_after_expiration
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
    ->fieldCondition('field_validity_period', 'value2', $now, '<');
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
  $result = false;

  try {
    //
    // import all xml files in the dedicated directory
    $files = scandir(SWISS1_IMPORT_DIRECTORY);
    sort($files);
    if ($files) {
      $files = array_diff($files, array('.', '..'));
      foreach ($files as $file) {
        $file_path = SWISS1_IMPORT_DIRECTORY . '/' . $file;
        if (is_file($file_path)) {
          // import file
          $result = _import_xml_file($file_path);

          // delete file
          //unlink($file);
        }
      }
    }

  } catch (Exception $ex) {
    watchdog('Swiss1 Importer',$ex->getMessage(), array(), WATCHDOG_ERROR);
    drupal_set_message($ex->getMessage(), 'error');
  }

  // display feedback about import, if any
  if ($result) {
    $importMessage = 'Swiss1 TV media import - @created video streams created, @updated video streams updated.';
    drupal_set_message(t($importMessage, $result));
    watchdog('HSO Importer', $importMessage, $result, WATCHDOG_INFO);
  }

  //
  // update last run variable
  variable_set('swiss1_last_media_import', REQUEST_TIME);
}

/**
 * Imports an XML file into the CMS database.
 *
 * @param $file
 */
function _import_xml_file($file) {
  $result = array(
    '@created' => 0,
    '@updated' => 0,
  );

  //
  // read xml file
  $xml = simplexml_load_file($file);

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
      // no valid file
      $xpath = false;
  }

  //
  // select the video entries in the file and import it
  $xml_entries = $xml->xpath($xpath);
  foreach ($xml_entries as $xml_entry) {
    $new_entry = _parse_entry($xml_entry);
    if ($new_entry) {
      $result['@created']++;
    }
    else {
      $result['@updated']++;
    }
  }

  // return import result
  return $result;
}


/**
 * Parses an XML entry and create or update a node of type VideoStream.
 *
 * @param $xml_entry
 *
 * @return bool
 *  Returns true, if a new node has been created, false otherwise (existing node update)
 */
function _parse_entry($xml_entry) {
  //
  // convert xml entry into associative array
  $new_entry = false;
  $json = json_encode($xml_entry);
  $entry_arr = json_decode($json, true);

  //
  // import/update xml entry into existing or new node
  $node = false;
  $schedule_id = isset($entry_arr['ScheduleReference']['AssetReconciliationId']) ? $entry_arr['ScheduleReference']['AssetReconciliationId'] : false;
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
    }
  }

  // create new node, if it not exists
  if (!$node) {
    $node = new stdClass();
    $node->type = 'videostream';
    $node->language = LANGUAGE_NONE;
    $node->uid = 1;
    $node->status = 1;
    node_object_prepare($node);
    $new_entry = true;
  }

  //
  // update all VideoStream fields
  //
  // schedule reference
  $node->field_schedule_id[LANGUAGE_NONE][0]['value'] = $schedule_id ? $schedule_id : '';

  // Airing information (only for scheduled entries)
  if (isset($entry_arr['AiringInformation'])) {
    $airing_info = $entry_arr['AiringInformation'];
    $run_type = isset($airing_info['RunType']) ? strtoupper($airing_info['RunType']) : SWISS1_RUNTYPE_PRIMARY; // PRIMARY means catchUp period is set
    $playable_on_broadcast_start = variable_get('swiss1_video_playable_start') == 'broadcast_start';

    // get broadcast start time
    if (isset($airing_info['ScheduledStartDateTime'])) {
      $start_time = str_replace('.', '-', $airing_info['ScheduledStartDateTime']);
      $start_timestamp = strtotime($start_time);
      $start_date = date('Y-m-d H:i:s', $start_timestamp);
      $node->field_broadcast_date[LANGUAGE_NONE][0]['value'] = $start_date;
    }
    else {
      $msg = t('No schedule start date time has been defined in VDOEntry with schedule id = @id.', array('@id' => $schedule_id));
      throw new Exception($msg);
    }

    // get broadcast end time
    if (isset($airing_info['ScheduledEndDateTime'])) {
      $end_time = str_replace('.', '-', $airing_info['ScheduledEndDateTime']);
      $end_timestamp = strtotime($end_time);
      $end_date = date('Y-m-d H:i:s', $end_timestamp);
      $node->field_broadcast_date[LANGUAGE_NONE][0]['value2'] = $end_date;
    }
    else {
      $msg = t('No schedule end date time has been defined in VDOEntry with schedule id = @id.', array('@id' => $schedule_id));
      throw new Exception($msg);
    }

    // set validity period start and end date
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

    switch ($run_type) {
      case SWISS1_RUNTYPE_PRIMARY:
        $catchup_period = isset($airing_info['CatchupPeriod']) ? ' +' . strtolower($airing_info['CatchupPeriod']) : '+1 second';
        $exp_timestamp = strtotime($catchup_period, $validity_start_timestamp);
        $exp_date = date('Y-m-d H:i:s', $exp_timestamp);
        break;
      case SWISS1_RUNTYPE_RERUN:
        //$primary_schedule_id = ''; // TODO: get primary schedule id from VODEntry
        //$exp_date = _get_validity_end_date($primary_schedule_id);
        //break;
      default:
      $exp_date = $node->field_validity_period[LANGUAGE_NONE][0]['value'];
        break;
    }
    $node->field_validity_period[LANGUAGE_NONE][0]['value2'] = $exp_date;

    // get duration and feature flag
    $node->field_duration[LANGUAGE_NONE][0]['value'] = isset($airing_info['Duration']) ? $airing_info['Duration'] : '';
    $featured = (isset($airing_info['Featured']) && $airing_info['Featured'] == 'true') ? 1 : 0;
    $node->field_featured[LANGUAGE_NONE][0]['value'] = $featured;
  }

  //
  // EPG / Category
  if (isset($entry_arr['Description'])) {
    $epg = isset($entry_arr['Description']['Epg']) ? $entry_arr['Description']['Epg'] : false;
    if ($epg) {
      $node->title = isset($epg['Title']) ? $epg['Title'] : (isset($epg['Headline']) ? $epg['Headline'] : t('Kein Titel'));
      $synopsis = isset($epg['Synopsis']) ? $epg['Synopsis'] : t('Keine Information zur Sendung verfügbar');
      $synopsis .= isset($epg['PresentedBy']) ? t('<br><br>Moderator: @moderator', array('@moderator' => $epg['PresentedBy'])) : '';
      $synopsis .= isset($epg['Interviewee']) ? t('<br>Gast: @guest', array('@guest' => $epg['Interviewee'])) : '';
      $node->body[LANGUAGE_NONE][0]['value'] = $synopsis;
      $node->field_video_language[LANGUAGE_NONE][0]['value'] = isset($epg['Language']) ? $epg['Language'] : t('Deutsch');
      $node->field_age_restriction[LANGUAGE_NONE][0]['value'] = isset($epg['AgeRestriction']) ? $epg['AgeRestriction'] : t('Für Kinder geeignet');
      $node->field_series[LANGUAGE_NONE][0]['value'] = isset($epg['Series']) ? $epg['Series'] : '';
      $node->field_season[LANGUAGE_NONE][0]['value'] = isset($epg['Season']) ? $epg['Season'] : '';
      $node->field_episode[LANGUAGE_NONE][0]['value'] = isset($epg['EpisodeNumber']) ? $epg['EpisodeNumber'] : '';
      //$buzzword = isset($epg['Buzzword']) ? $epg['Buzzword'] : '';
      //$keyword = isset($epg['Keyword']) ? $epg['Keyword'] : '';
    }
    $node->field_category[LANGUAGE_NONE][0]['tid'] =
      isset($entry_arr['Description']['Category']) ? _get_category_key($entry_arr['Description']['Category']) : null;
  }

  //
  // Properties
  if (isset($entry_arr['Properties'])) {
    $properties = $entry_arr['Properties'];

    // set media id
    //$node->field_media_id[LANGUAGE_NONE][0]['value'] = $properties['MediaId'];

    // set thumbnail
    $thumb_path = isset($properties['ThumbnailFTPPathFilename']) ? $properties['ThumbnailFTPPathFilename'] : false;
    if ($thumb_path) {
      $file_obj = system_retrieve_file($thumb_path, 'public://images', true,FILE_EXISTS_REPLACE);
      $node->field_thumb_path[LANGUAGE_NONE][0] = $file_obj;
    }

    // set poster
    $poster = isset($properties['PosterframeFTPPathFilename']) ? $properties['PosterframeFTPPathFilename'] : false;
    if ($poster) {
      $file_obj = system_retrieve_file($poster, 'public://images', true,FILE_EXISTS_REPLACE);
      $node->field_poster[LANGUAGE_NONE][0] = $file_obj;
    }
    // set video stream
    $video_url = isset($properties['StreamingLink']) ? $properties['StreamingLink'] : '';
    $node->field_video_path[LANGUAGE_NONE][0]['video_url'] = $video_url;
  }

  //
  // Publishing Information (only for not scheduled entries)
  if (isset($entry_arr['PublishingInformation'])) {
    $publish_info = $entry_arr['PublishingInformation'];

    if (isset($publish_info['ValidityDateTimeFrom'])) {
      $valid_from = str_replace('.', '-', $publish_info['ValidityDateTimeFrom']);
      $from_timestamp = strtotime($valid_from);
      $node->field_validity_period[LANGUAGE_NONE][0]['value'] = date('Y-m-d H:i:s', $from_timestamp);
    }
    else {
      // set now as validity period start
      $node->field_validity_period[LANGUAGE_NONE][0]['value'] = date('Y-m-d H:i:s');
    }
    if (isset($publish_info['ValidityDateTimeTo'])) {
      $valid_to = str_replace('.', '-', $publish_info['ValidityDateTimeTo']);
      $to_timestamp = strtotime($valid_to);
      $node->field_validity_period[LANGUAGE_NONE][0]['value2'] = date('Y-m-d H:i:s', $to_timestamp);
    }
    else {
      $timestamp = strtotime('+25 years');
      $node->field_validity_period[LANGUAGE_NONE][0]['value2'] = date('Y-m-d H:i:s', $timestamp);
    }

  }

  //
  // save/update node
  $node = node_submit($node);
  node_save($node);

  return $new_entry;
}

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
 * Gets the Category term key of the given term name.
 */
function _get_category_key($term_name) {
  $key = null;
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'taxonomy_term')
    ->entityCondition('bundle', 'kategorie')
    ->propertyCondition('name', 'value', $term_name)
    ->range(0, 1);

  $result = $query->execute();
  if ($result && $result['taxonomy_term']) {
    $key = key($result['taxonomy_term']);
  }
  // return mapping
  return $key;
}

