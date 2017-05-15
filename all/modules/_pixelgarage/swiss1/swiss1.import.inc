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
  // get the schedule id, if available
  $new_entry = false;
  $json = json_encode($xml_entry);
  $entry_arr = json_decode($json, true);
  $schedule_id = isset($entry_arr['ScheduleReference']['AssetReconciliationId']) ? $entry_arr['ScheduleReference']['AssetReconciliationId'] : false;

  //
  // check if schedule entry has to be imported or updated (cached version differs)


  //
  // import/update xml entry into existing or new node
  $node = false;
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
  // schedule reference
  if ($schedule_id) {
    $node->field_schedule_id[LANGUAGE_NONE][0]['value'] = $schedule_id;
  }

  // Airing information
  $node->field_is_scheduled[LANGUAGE_NONE][0]['value'] = 0;
  if (isset($entry_arr['AiringInformation'])) {
    $airing_info = $entry_arr['AiringInformation'];

    if (isset($airing_info['ScheduledStartDateTime'])) {
      $start_time = str_replace('.', '-', $airing_info['ScheduledStartDateTime']);
      $start_timestamp = strtotime($start_time);
      $start_date = date('Y-m-d H:i:s', $start_timestamp);
      $node->field_broadcast_date[LANGUAGE_NONE][0]['value'] = $start_date;

      // set catchup period
      $catchup_period = isset($airing_info['CatchupPeriod']) ? ' +' . strtolower($airing_info['CatchupPeriod']) : false;
      if ($catchup_period) {
        $exp_timestamp = strtotime($start_time . $catchup_period);
        $exp_date = date('Y-m-d H:i:s', $exp_timestamp);
        $node->field_expiration_date[LANGUAGE_NONE][0]['value'] = $start_date;
        $node->field_expiration_date[LANGUAGE_NONE][0]['value2'] = $exp_date;
      }

      // set scheduled flag
      $node->field_is_scheduled[LANGUAGE_NONE][0]['value'] = 1;
    }

    if (isset($airing_info['ScheduledEndDateTime'])) {
      $end_time = str_replace('.', '-', $airing_info['ScheduledEndDateTime']);
      $end_timestamp = strtotime($end_time);
      $node->field_broadcast_date[LANGUAGE_NONE][0]['value2'] = date('Y-m-d H:i:s', $end_timestamp);
    }

    $node->field_duration[LANGUAGE_NONE][0]['value'] = isset($airing_info['Duration']) ? $airing_info['Duration'] : '';
    $featured = (isset($airing_info['Featured']) && $airing_info['Featured'] == 'true') ? 1 : 0;
    $node->field_featured[LANGUAGE_NONE][0]['value'] = $featured;

    //$run_type = isset($airing_info['RunType']) ? $airing_info['RunType'] : ''; // PRIMARY means catchUp period is set
  }

  //
  // EPG / Category
  if (isset($entry_arr['Description'])) {
    $epg = isset($entry_arr['Description']['Epg']) ? $entry_arr['Description']['Epg'] : false;
    if ($epg) {
      $node->title = isset($epg['Title']) ? $epg['Title'] : (isset($epg['Headline']) ? $epg['Headline'] : '');
      $synopsis = isset($epg['Synopsis']) ? $epg['Synopsis'] : '';
      $synopsis .= isset($epg['PresentedBy']) ? t('<br><br>Moderator: @moderator', array('@moderator' => $epg['PresentedBy'])) : '';
      $synopsis .= isset($epg['Interviewee']) ? t('<br><br>Interv:iew @interview', array('@interview' => $epg['Interviewee'])) : '';
      $node->body[LANGUAGE_NONE][0]['value'] = $synopsis;
      $node->field_video_language[LANGUAGE_NONE][0]['value'] = isset($epg['Language']) ? $epg['Language'] : '';
      $node->field_age_restriction[LANGUAGE_NONE][0]['value'] = isset($epg['AgeRestriction']) ? $epg['AgeRestriction'] : '';
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
      $node->field_expiration_date[LANGUAGE_NONE][0]['value'] = date('Y-m-d H:i:s', $from_timestamp);
    }
    if (isset($publish_info['ValidityDateTimeTo'])) {
      $valid_to = str_replace('.', '-', $publish_info['ValidityDateTimeTo']);
      $to_timestamp = strtotime($valid_to);
      $node->field_expiration_date[LANGUAGE_NONE][0]['value2'] = date('Y-m-d H:i:s', $to_timestamp);
    }

  }

  //
  // save/update node
  $node = node_submit($node);
  node_save($node);

  return $new_entry;
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

