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
  Video Validity Period
-----------------------------------------------------------------------------
Each VODEntry has a validity period, which is the period, in which the video can be played. This period is calculated
during the import and is controlled by the JW video player, that prevents a video to be played after its expiration date.
Additionally a cron job periodically deletes all video streams with an expired validity.

Scheduled video:        The validity period is defined by the broadcast start time and the catchup period.
Single video on demand: The validity period is defined by the <PublishingInformation> element.
