-----------------------------------------------------------------------------
  SWISS1 TV Video Portal
-----------------------------------------------------------------------------
This module provides the functionality to manage the Swiss1 TV video portal.

It contains two integral parts:
1) The taylored JWPlayer providing all needed functionality (best streaming quality, catchup period per video,
   overriding of presets, custom behaviour (js), custom styling, ...).
   See module video_embed_jwplayer.

2) The video import via XML-files. All videos are imported on a regular basis into the Swiss1 TV portal. There are
   two types of videos:

   a) The scheduled video, which is part of the EPG and showed in the regular TV program. It contains a <AiringInformation>
      element providing the broadcast information and a <ScheduleReference> element with a schedule_id.

   b) A single video item, which is NOT part of the EPG. It contains a <PublishingInformation> element defining the
      validity period of the video, but no <AiringInformation> and <ScheduleReference>.

   c) See the structures/xsd/vodSample.xsd and the structures/xsd/linearRundownEPGSample.xsd for the schema definition
      of the XML files to be imported.


-----------------------------------------------------------------------------
  SWISS1 TV Video Import
-----------------------------------------------------------------------------
Each VODEntry is imported into one VideoStream node.

The full set of information for a video can be delivered in several updates, e.g. first only the broadcast information and
the EPG information are delivered, and then in a second update the media information and catchup period is delivered.

The XML files contain all already delivered data, not only the new data, meaning each VODEntry contains all currently
available data about a video.


-----------------------------------------------------------------------------
  Video Validity Period (Catch-up period versus license period)
-----------------------------------------------------------------------------
Each VODEntry has a validity period, which is the period, in which the video can be played.

A contract for a video looks like this:
Portrait Michael Jackson
Lizenzdauer: 18 Monate
Catch-Up Rights: 7 Tage
Runs: 6

This means, during 18 month the video can be played 6 times, each with a catchup period of 7 days.
The CMS handles each run as a seperate video with its own catchup period, so the validity period for each video is 7 days,
after that the video is deleted by a cron job.

This validity period is calculated during the import and is controlled with several methods:

1) The video url is signed (JW Platform), which prevents a video to be played after its expiration date. This allows
    to distribute the video url by social media without loosing control over legal issues.

2) The CMS loads the video only, if the request occurs in the validity period. Otherwise only a thumbnail is displayed.
    Thid procedure guarantees, that a video is only available during its validity period.


Scheduled video:        The validity period is defined by the broadcast start time and the catchup period.
Single video on demand: The validity period is defined by the <PublishingInformation> element.
