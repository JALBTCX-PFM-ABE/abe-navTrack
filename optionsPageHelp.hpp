
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

QString headingGSFText = 
  optionsPage::tr ("Enter the heading change limit, in degrees, for GSF files.  If the heading of the input data changes "
                   "by this amount then a new vertex will be added to the output navTrack file.  The normal default is 2.0 "
                   "degrees.");

QString speedGSFText = 
  optionsPage::tr ("Enter the speed change limit, in knots, for GSF files.  If the speed of the input data exceeds "
                   "this value then a new vertex will be added to the output navTrack file.  The normal default is 20.0 "
                   "knots.");

QString headingPOSText = 
  optionsPage::tr ("Enter the heading change limit, in degrees, for POS/SBET files.  If the heading of the input data changes "
                   "by this amount then a new vertex will be added to the output navTrack file.  The normal default is 5.0 "
                   "degrees.");

QString speedPOSText = 
  optionsPage::tr ("Enter the speed change limit, in knots, for POS/SBET files.  If the speed of the input data exceeds "
                   "this value then a new vertex will be added to the output navTrack file.  The normal default is 240.0 "
                   "knots.");

QString maxTimeForaPointText =
  optionsPage::tr ("Enter the max time alotted, when there is no change in navigation, before a new point is required "
                   "to be recorded.");

QString navCheckBoxText = 
  optionsPage::tr ("Select radio button to have program create a 4word .nav file after making the .trk file");
