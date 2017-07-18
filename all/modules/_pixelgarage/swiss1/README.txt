-----------------------------------------------------------------------------
  SWISS1 TV Video Portal
-----------------------------------------------------------------------------
This module provides the functionality to manage the Swiss1 TV video portal.

It contains two integral parts:
1) The taylored JWPlayer providing all needed functionality (best streaming quality, catchup period per video,
   overriding of presets, custom behaviour (js), custom styling, ...).
   See module video_embed_jwplayer.

2) The video import via XML-files. All available broadcast videos are imported on a regular basis into the Swiss1 TV portal.
   There are two types of xml-files, that can be imported:

   a) The EPG (Electronic Programming Guide) file containing all broadcast entries of a single day. The broadcast entries
      contain only the basic information (no medias linked here) and are updated later with a specific xml files
      (VODRundown...).

   b) VODRundownxxx files containing exactly one scheduled video (VODEntry), which is part of the EPG and showed in the
      regular TV program. The VODEntry contains the complete information about a video broadcast, e.g. <AiringInformation>,
      <Description>, <ScheduleReference> and <Properties> containing the media links.

   See the structures/xsd/vodSample.xsd and the structures/xsd/linearRundownEPGSample.xsd for the schema definition
   of the XML files to be imported.


-----------------------------------------------------------------------------
  SWISS1 TV VOD Import
-----------------------------------------------------------------------------
The Media Import is performed via a CRON-Job, which runs on a regularly basis (each hour). All XML-Files found
in the configurable import directory (default is ../swiss1_media_import) are imported. The VOD import guarantees, that
each imported VODEntry has its own videostream node filled with the run-type and the calculated validity period,
the broadcast time, the linked media file, the thumbnail, the poster image and all the EPG information.

The import follows predefined rules:

1) Each VODEntry (scheduled or not scheduled) is imported into one VideoStream node.

2) The final set of information for a video can be delivered in several updates, meaning videostream nodes are updatable.
   A VideoStream node is uniquely identified with the schedule-id, the RunType and the broadcast-time.

3) If an EPG file is imported containing an already imported day, all existing VideoStream nodes of this day are deleted first.

4) For each VODEntry a specific validity period is calculated depending on the runType, the catchup-period and the
   end-of-license (see below). The beginning of the validity period can be configured to the broadcast start or end date.

5) Primary VODEntries outside of the Prime-time (configurable time period where the new content is broadcast) get a new
   runType assigned called PRIMARY_REPEAT. This allows to distinguish a primary broadcast and its repeating broadcasts.


-----------------------------------------------------------------------------
  Video Validity Period (Catch-up period versus license period)
-----------------------------------------------------------------------------
For each VODEntry a validity period is calculated, which defines the time span, in which the video can be played.

A contract for a video looks like this:
Portrait Michael Jackson
Lizenzdauer: 18 Monate
Catch-Up Rights: 7 Tage
Runs: 6

This means, during 18 month the video can be played 6 times, each with a catchup period of 7 days. Each run is
delivered as single VODEntry with run type PRIMARY. The importer updates the run type for all repeating runs with PRIMARY_REPEAT.
Repeating runs are broadcast outside the primary time period, which is configurable.

The validity period is calculated during the import as follows:

1) The start date is set according to the configuration settings of the importer, e.g. the broadcast start or end date.

2) The validity end date is defined by the minimum of the validity start date plus catchup period and the end-of-license date.

3) VODEntries of run-type RERUN get the catchup period from the related PRIMARY VODEntry.

4) VODEntries without schedule information has the validity period defined by the <PublishingInformation> element.


The validity period is controlled with several methods:

1) The video url is signed (JW Platform) with the validity end date, which invalidates a video url after this date.
   This allows to distribute the video url by social media without loosing control over legal issues.

2) The CMS loads the video only, if the request occurs in the validity period of a video. Otherwise only a thumbnail
   is displayed. This procedure guarantees, that a video is only available on the internet during its validity period.


