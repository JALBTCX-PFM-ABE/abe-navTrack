
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

#include "navTrack.hpp"
#include "navTrackHelp.hpp"
#include "codes.h"

navTrack::navTrack (QWidget *parent): QWizard (parent, 0)
{
  void set_defaults (OPTIONS *options);

  QResource::registerResource ("/icons.rcc");


  //  Set the main icon

  setWindowIcon (QIcon (":/icons/navTrackWatermark.png"));

  set_defaults (&options);

  envin (&options);


  // Set the application font

  QApplication::setFont (options.font);


  //  Had to reset bool flags manually since the settings defaulted to last used

  options.isMakeTRK = 0;
  options.isMakeNAV = 0;


  //  Set the window size and location from the defaults

  this->resize (options.width, options.height);
  this->move (options.window_x, options.window_y);

  setWizardStyle (QWizard::ClassicStyle);

  setOption (HaveHelpButton, true);
  setOption (ExtendedWatermarkPixmap, false);

  connect (this, SIGNAL (helpRequested ()), this, SLOT (slotHelpClicked ()));

  setPage (0, new startPage (this, &options));
  setPage (1, new navOptionsPage (this, &options));
  setPage (2, new optionsPage (this, &options));
  setPage (3, new inputPage (this, &options, &inputFiles));
  setPage (4, new runPage (this, &progress, &trackList));

  setButtonText (QWizard::CustomButton1, tr("&Run"));
  setOption (QWizard::HaveCustomButton1, true);
  button (QWizard::CustomButton1)->setToolTip (tr ("Run the navTrack generation process"));
  button (QWizard::CustomButton1)->setWhatsThis (runText);
  connect (this, SIGNAL (customButtonClicked (int)), this, SLOT (slotCustomButtonClicked (int)));

  setStartId (0);
}



//Function initializes the wizard pages and where the custom button comes into play

void navTrack::initializePage (int id)
{
  button (QWizard::HelpButton)->setIcon (QIcon (":/icons/contextHelp.xpm"));
  button (QWizard::CustomButton1)->setEnabled (false);

  switch (id)
    {
    case 0:
      button (QWizard::NextButton)->setEnabled (true);
      break;

    case 1:
      break;

    case 2:
      break;
         
    case 3:
      break;

    case 4:
      button (QWizard::CustomButton1)->setEnabled (true);
      break;
    }
}



//Function is used to destroy pages when done

void navTrack::cleanupPage (int id)
{
  switch (id)
    {
    case 0:
      break;

    case 1:
      break;

    case 2:
      break;

    case 3:
      break;

    case 4:
      break;
    }
}



//Function acts as a toggle for the Help? button

void navTrack::slotHelpClicked ()
{
  QWhatsThis::enterWhatsThisMode ();
}



//Function occurs when the "Finish" button is clicked on the last page of the wizard
//This function will gather the field data and perform the .trk (and/or) .nav calculations

void navTrack::slotCustomButtonClicked (int id __attribute__ ((unused)))
{
  //If the user never selected to make either a .trk or .nav file then we can't do anything

  if(options.isMakeTRK == 0 && options.isMakeNAV == 0)
    {
      QMessageBox::critical (this, tr ("navTrack"), tr ("No files selected.  Program will now exit."));

      exit(1);
    }


  //Otherwise, grab the values and let's do this!

  options.heading[NAVTRACK_GSF] = field ("headingGSF").toDouble ();
  options.speed[NAVTRACK_GSF] = field ("speedGSF").toDouble ();
  options.heading[NAVTRACK_POS] = field ("headingPOS").toDouble ();
  options.speed[NAVTRACK_POS] = field ("speedPOS").toDouble ();

  QString outputFile = field ("outputFile").toString ();
  QString navOutputFile = field ("navOutputFile").toString();
  float maxTime = field("maxTimeForaPoint").toDouble();

  int32_t points = 0;
  int32_t track_points = 0;

  start_heading = 0.0;


  //  Use frame geometry to get the absolute x and y.

  QRect tmp = this->frameGeometry ();
  options.window_x = tmp.x ();
  options.window_y = tmp.y ();


  //  Use geometry to get the width and height.

  tmp = this->geometry ();
  options.width = tmp.width ();
  options.height = tmp.height ();


  //  Get the files from the QTextEdit box on the inputPage.

  QTextCursor inputCursor = inputFiles->textCursor ();

  inputCursor.setPosition (0);

  QStringList sort_files;

  sort_files.clear ();

  do
    {
      sort_files << inputCursor.block ().text ();
    } while (inputCursor.movePosition (QTextCursor::NextBlock));


  //  Sort so we can remove dupes.

  sort_files.sort ();


  //  Remove dupes and place into input_files.

  QString name, prev_name = "";
  input_files.clear ();

  for (int32_t i = 0 ; i < sort_files.size () ; i++)
    {
      name = sort_files.at (i);

      if (name != prev_name)
        {
          input_files.append (name);
          prev_name = name;
        }
    }

  button (QWizard::BackButton)->setEnabled (false);
  button (QWizard::CustomButton1)->setEnabled (false);
  QApplication::setOverrideCursor (Qt::WaitCursor);


  /******************************************************
        Big Block O' Variables

        filename             -  holds filename with max 512 characters
        *_handle             -  handle for file opens of specific file types
        ping                 -  keep track of # of BATHY PINGS during gsf reads
        eof                  -  end of file flag
        percent(s)           -  used to display percent complete to users
        data                 -  holds all the data drawn from an input file
        prev_data            -  holds the previous data allowing for heading, speed, and time change tests
        last_data            -  same as prev_data but in different locations
        gsf_id               -  used with GSF API
        gsf_rec              -  used with GSF API
        pos                  -  used with POS API
        cur                  -  used by Qt
        tmpString            -  as name implies, used to hold temporary strings
        gsfError             -  used with GSF API
        total_time           -  the time (in seconds) of a ping
        conv_time            -  the time (in seconds) of a ping with epoch 1960
        fileNum              -  keeps track of number of files
        navModeHolder        -  holds navMode and is used to secure actual navMode
        x_step,y_step        -  used as counts and loop increments
        goodRecord           -  used to verify whether there were any good records at end of loop
        navNAVREC_final_ndx  -  total number of files and the final array index used to store all the
                                nav4words before writing to the .nav file
        recType              -  used to hold whether this is a GSF or not record type
        FLAG_navRecSplit     -  flags whether a GSF file has a time navigation gap inside of itself or not

  *******************************************************/


  char                     filename[512], filename_ndx[512], remove_ndx[512];
  int32_t                  gsf_handle = -1, ping, eof, percent = 0, old_percent = -1;
  uint8_t                  first = NVTrue, got_data;
  INFO                     data, prev_data, swindow_data, last_data = {0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0};
  FILE                     *fp = NULL, *ofp = NULL, *nofp = NULL;
  gsfDataID                gsf_id;
  gsfRecords               gsf_rec;
  POS_OUTPUT_T             pos;
  QListWidgetItem          *cur;
  QString                  tmpString;
  extern int32_t           gsfError;
  time_t                   total_time;
  struct tm                *conv_time;
  float                    conv_secs;
  int32_t                  fileNum = 0, navModeHolder = 0, x_step, y_step, navNAVREC_final_ndx = 0, ndxHolder = 0;
  uint8_t                  goodRecord, recType = NVFalse, FLAG_navRecSplit = NVFalse;


  //Initialize navHEADREC_headRec if we need to

  if(options.isMakeNAV == 1)
    {
      init_HEADREC(&navHEADREC_headRec);
    }


  //Add the extension if needed, and check outputFiles availability

  //If user selected to make a .trk file

  if(options.isMakeTRK == 1)
    {
      if (!outputFile.endsWith (".trk")) outputFile.append (".trk");
        
      if ((ofp = fopen (outputFile.toLatin1 (), "w")) == NULL)
        {
          QMessageBox::critical (this, tr ("navTrack"), tr ("Unable to open .trk output file %1!").arg (outputFile));
          exit (-1);
        }
    }


  //If user selected to make a .nav file

  if(options.isMakeNAV == 1)
    {
      if (!navOutputFile.endsWith (".nav")) navOutputFile.append (".nav");


      //If we cannot open the .nav output file for binary writing

      if ((nofp = fopen (navOutputFile.toLatin1 (), "wb")) == NULL)
        {
          QMessageBox::critical (this, tr ("navTrack"), tr ("Unable to open .nav output file %1!").arg (outputFile));
          exit (-1);
        }


      //Else we can open the file and now must make the nav4word header

      else
        {
          QString temp = field("survOp").toString();

          strcpy(navHEADREC_headRec.survop, temp.toLatin1());


          //Having to sync the comboBox index with the gsf24word code

          navHEADREC_headRec.ship = options.ship_identity;


          //Syncing navProcess ComboBox index with old gsf24word indexes

          switch(field("navProcess").toInt())
            {
            case 0:
              navHEADREC_headRec.process = HSPS_CODE;
              break;
            case 1:
              navHEADREC_headRec.process = INS_CODE;
              break;
            case 2:
              navHEADREC_headRec.process = NPS_CODE;
              break;
            case 3:
              navHEADREC_headRec.process = SBS_CODE;
              break;
            case 4:
              navHEADREC_headRec.process = MNT_DGT_CODE;
              break;
            default:
              navHEADREC_headRec.process = HSPS_CODE;
              break;
            }


          //Syncing navAccess ComboBox index with old gsf24word indexes

          switch(field("navAccess").toInt())
            {
            case 0:
              navHEADREC_headRec.access = UNCLAS_UNLIMITED_CODE;
              break;
            case 1:
              navHEADREC_headRec.access = UNCLAS_RESTRICT_CODE;
              break;
            case 2:
              navHEADREC_headRec.access = CONFID_OTHER_CODE;
              break;
            case 3:
              navHEADREC_headRec.access = CONFID_CAESAR_CODE;
              break;
            case 4:
              navHEADREC_headRec.access = CONFID_FDS_CODE;
              break;
            case 5:
              navHEADREC_headRec.access = SECRET_OTHER_CODE;
              break;
            case 6:
              navHEADREC_headRec.access = SECRET_CAESAR_CODE;
              break;
            case 7:
              navHEADREC_headRec.access = SECRET_FDS_CODE;
              break;
            }


          //Syncing navDatum ComboBox index with old gsf24word indexes

          switch(field("navDatum").toInt())
            {
            case 0:
              navHEADREC_headRec.datum = WGS84_CODE;
              break;
            case 1:
              navHEADREC_headRec.datum = WGS80_CODE;
              break;
            case 2:
              navHEADREC_headRec.datum = WGS72_CODE;
              break;
            case 3:
              navHEADREC_headRec.datum = UNKNOWN_DATUM_CODE;
              break;
            }


          //Attempt to write the header to the binary .nav output file

          if(fwrite(&navHEADREC_headRec, sizeof(navHEADREC), 1, nofp) != 1)
            {
              QMessageBox::critical (this, tr ("navTrack"), tr ("Unable to write header to .nav file!"));
              exit(-1);      
            }
        }
    }


  //  Save the output directory.  It might have been input manually instead of browsed.

  if(options.isMakeTRK == 1) options.output_dir = QFileInfo (outputFile).absoluteDir ().absolutePath ();

  if(options.isMakeNAV == 1) options.navOutput_dir = QFileInfo (navOutputFile).absoluteDir ().absolutePath ();


  //  Save the options to the QSettings file.

  envout (&options);

  int32_t file_count = input_files.size ();

  if(options.isMakeNAV == 1)
    {
      //Set the final set index for the navNAVREC_Records variable

      navNAVREC_final_ndx = file_count;


      //Allocating file_count + 1 navNAVREC pointers in navNAVREC_Records

      if((navNAVREC_Records = (navNAVREC **) calloc(navNAVREC_final_ndx + 1, sizeof(navNAVREC))) == NULL)
        {
          printf("\nALLOCATION ERROR:  Unable to allocate for nav_NAVREC_Records\n");
          printf("Program will now terminate...\n\n");

          exit(-2);
        }
        
      for(x_step = 0; x_step < (navNAVREC_final_ndx + 1); x_step++)
        {
          if((navNAVREC_Records[x_step] = (navNAVREC *) calloc(1, sizeof(navNAVREC))) == NULL)
            {
              printf("\nALLOCATION ERROR:  Unable to allocate for nav_NAVREC_Records\n");
              printf("Program will now terminate...\n\n");

              exit(-2);
            }
        }


      //Initialize nav_NAVREC_Records

      for(x_step = 0; x_step < navNAVREC_final_ndx + 1; x_step++) init_NAVREC(navNAVREC_Records[x_step]);
    }


  //Time to loop through the input files for decimation and processing
  //
  //This is the BIG LOOP

  for (int32_t i = 0 ; i < file_count ; i++)
    {
      //Grab the file next on the list

      strcpy (filename, input_files.at (i).toLatin1 ());


      //Reset the percentage bar to 0 and write some text to the GUI window

      progress.fbar->reset ();
      progress.fbar->setRange (0, 100);
      tmpString = QString (tr ("Processing file %1 of %2 - %3").arg (i + 1).arg (file_count).arg (QFileInfo (filename).fileName ()));
      progress.fbox->setTitle (tmpString);
      qApp->processEvents ();

      progress.fbar->setWhatsThis (tr ("Progress of input file processing"));


      //Flag first to true and set pings to 0

      first = NVTrue;
      ping = 0;


      //If user selected to make a .trk file, write the small header

      if(options.isMakeTRK == 1) fprintf (ofp, "FILE %03d = %s\n", i, filename);


      //Flag got_data as false since we haven't got data yet

      got_data = NVFalse;


      //Assume we have a GSF but check for an alternative file type

      type = NAVTRACK_GSF;
      if (strstr (filename, ".pos") || strstr (filename, ".out") || strstr (filename, ".POS") || strstr (filename, ".OUT")) type = NAVTRACK_POS;


      //Switch / case the file type in question and begin decimation and file write procedures!

      switch (type)
        {
        case NAVTRACK_GSF:


          //NOTE:  Requires GSF_READONLY_INDEX and -not- GSF_READONLY in order to function correctly
          //

          if (!gsfOpen (filename, GSF_READONLY_INDEX, &gsf_handle))
            {
              strcpy(filename_ndx, filename);
              filename_ndx[strlen(filename) - 3] = 'n';
              strcpy(remove_ndx, "rm -f ");
              strcat(remove_ndx, filename_ndx);
        
              int32_t records = gsfGetNumberRecords (gsf_handle, GSF_NEXT_RECORD);


              //Default navMode is KFP_ISS60 -- same as gsf24word

              navModeHolder = KFP_ISS60;


              //Keep a flag saying we have a GSF record type and not a LIDAR one

              recType = NVTrue;


              //New file, reset the navRecSplit flag

              FLAG_navRecSplit = NVFalse;


              //Loop through ALL records

              for (int32_t j = 0 ; j < records ; j++)
                {
                  gsf_id.recordID = GSF_NEXT_RECORD;
                  gsf_id.record_number = j + 1;

                  if (gsfRead (gsf_handle, GSF_NEXT_RECORD, &gsf_id, &gsf_rec, NULL, 0) < 0) break;


                  //If we have a NAVIGATION RECORD

                  if (gsf_id.recordID == GSF_RECORD_HV_NAVIGATION_ERROR)
                    {
                      if (strcmp (gsf_rec.hv_nav_error.position_type, GSF_POS_TYPE_UNKN) == 0) navModeHolder = UNKNOWN_NAV_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_GPSU) == 0) navModeHolder = GPS_CA_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_PPSD) == 0) navModeHolder =  GSF_P_DIFF_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_PPSK) == 0) navModeHolder = GSF_P_KIN_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_PPSS) == 0) navModeHolder = GPS_P_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_SPSD) == 0) navModeHolder = GSF_S_DIFF_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_SPSK) == 0) navModeHolder = GSF_S_KIN_CODE; 
                      if (strcmp(gsf_rec.hv_nav_error.position_type,GSF_POS_TYPE_SPSS) == 0) navModeHolder = GPS_CA_CODE;
                      if (strcmp(gsf_rec.hv_nav_error.position_type, GSF_POS_TYPE_PPSG) == 0) navModeHolder = GSF_P_GYPSY_CODE; 
                      if (strcmp(gsf_rec.hv_nav_error.position_type, GSF_POS_TYPE_SPSG) == 0) navModeHolder = GSF_S_GYPSY_CODE; 
                    }


                  //Else if we have a BATHY PING

                  else if (gsf_id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING)
                    {
                      //Verify we have a good ping

                      if (!(gsf_rec.mb_ping.ping_flags & GSF_IGNORE_PING) && (gsf_rec.mb_ping.longitude != 0.0 && gsf_rec.mb_ping.latitude != 0.0) &&
                          fabs (gsf_rec.mb_ping.longitude) <= 180.0 && fabs (gsf_rec.mb_ping.latitude) <= 90.0 && (validateBeams(gsf_rec.mb_ping) > 0))
                        {
                          //Keep track of valid pings

                          ping++;


                          //We have a valid ping therefore we have data

                          got_data = NVTrue;


                          //Keep track of points (similiar to pings)

                          points++;


                          //Store all data needed from that particular ping record

                          data.f = i;
                          data.t = (double) gsf_rec.mb_ping.ping_time.tv_sec + (double) gsf_rec.mb_ping.ping_time.tv_nsec / 1000000000.0;
                          data.p = ping;
                          data.y = gsf_rec.mb_ping.latitude;
                          data.x = gsf_rec.mb_ping.longitude;
                          data.h = gsf_rec.mb_ping.heading;

                          if(options.isMakeNAV == 1)
                            {
                              //Allocate more space if needed

                              if(navNAVREC_Records[i]->numOfPings >= navNAVREC_Records[i]->numOfPingsAllocated)
                                {
                                  if(!check_mem_dataPings(navNAVREC_Records[i], 10000))
                                    {
                                      exit(-2);
                                    }
                                                                                                                                                
                                  printf("\n# of Pings Allocated is %d.  # of Pings is %d.\n\n", navNAVREC_Records[i]->numOfPingsAllocated,
                                         navNAVREC_Records[i]->numOfPings);
                                }
                            }


                          //Update our last data

                          last_data = data;


                          //If there is a minute time gap of pings in a single GSF file, then we flag it for splitting into sub-segments
                          //Sigh....5 minutes maybe?  15 minutes?  ....

                          if(!first && ((data.t - prev_data.t) >= 300.0)) FLAG_navRecSplit = NVTrue;


                          //If this is our first point/ping

                          if (first)
                            {
                              data.s = 0.0;


                              //If we selected to make a .nav file we will save the lat lon and navMode

                              if(options.isMakeNAV == 1)
                                {
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat = (int32_t) (data.y * 10000000);
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon = (int32_t) (data.x * 10000000);
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode = navModeHolder;
                                }


                              //Converting time in seconds to actual year, jday, hour, min, sec

                              total_time = (time_t) data.t;

                              conv_time = gmtime(&total_time);
                              conv_time->tm_yday++;
                              conv_secs = conv_time->tm_sec;

                                                                        
                              //  Save the last good heading since we want to compare it against the current heading.  Conversely, we want to compare
                              //  speeds using two adjacent points.

                              start_heading = data.h;

                              if(options.isMakeTRK == 1) fprintf (ofp, "+,%d,%d,%.9f,%.9f,%f,%f,%d,%d,%02d,%02d,%f\n", data.f, data.p, data.y, data.x, data.h, data.s,
                                                                  conv_time->tm_year + 1900, conv_time->tm_yday, conv_time->tm_hour, conv_time->tm_min, conv_secs);


                              //If we have selected to make a .nav file we now convert to epoch 1960

                              if(options.isMakeNAV == 1)
                                {
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].time =
                                    convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), &(conv_time->tm_hour),        
                                                      &(conv_time->tm_min), &conv_secs);

                                  navNAVREC_Records[i]->numOfPings = navNAVREC_Records[i]->numOfPings + 1;
                                }


                              //Set the sliding window to be the first point in the file at start

                              swindow_data = data;


                              //We no longer are the first point

                              first = NVFalse;


                              //Keep track of track points (for trackline .trk files)

                              track_points++;
                            }


                          //Else if we are NOT the first point we check for a change (heading, speed, time)

                          else if (changepos (&data, &prev_data, maxTime))
                            {
                              //If we selected to make a .nav file we will save the lat lon and navMode

                              if(options.isMakeNAV == 1)
                                {
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat = (int32_t) (data.y * 10000000);
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon = (int32_t) (data.x * 10000000);
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode = navModeHolder;

                                  if(FLAG_navRecSplit)
                                    {
                                      navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode *= -1;
                                      /*navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode ^ 0x80000000;*/
                                                                                        
                                      FLAG_navRecSplit = NVFalse;
                                    }
                                }


                              //Converting time in seconds to actual year, jday, hour, min, sec

                              total_time = (time_t) data.t;

                              conv_time = gmtime(&total_time);
                              conv_time->tm_yday++;
                              conv_secs = conv_time->tm_sec;


                              //  Save the last good heading since we want to compare it against the current heading.  Conversely, we want to compare
                              //  speeds using two adjacent points.

                              start_heading = data.h;

                              if(options.isMakeTRK == 1) fprintf (ofp, "+,%d,%d,%.9f,%.9f,%f,%f,%d,%d,%02d,%02d,%f\n", data.f, data.p, data.y, data.x, data.h, data.s,
                                                                  conv_time->tm_year + 1900, conv_time->tm_yday, conv_time->tm_hour, conv_time->tm_min, conv_secs);


                              //If we have selected to make a .nav file we now convert to epoch 1960

                              if(options.isMakeNAV == 1)
                                {
                                  navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].time =
                                    convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), &(conv_time->tm_hour),        
                                                      &(conv_time->tm_min), &conv_secs);

                                  navNAVREC_Records[i]->numOfPings = navNAVREC_Records[i]->numOfPings + 1;
                                }
                                        
                              swindow_data = data;
                              track_points++;
                            }


                          //Update what our previous data is

                          prev_data = data;


                          //Update our percent

                          percent = (int32_t) roundf (((float) (j) / records) * 100.0);
                          if (percent - old_percent >= 5 || old_percent > percent)
                            {
                              progress.fbar->setValue (percent);
                              old_percent = percent;

                              qApp->processEvents ();
                            }
                        } 
                    }


                  //This line is required for gsf API usage [gsfRead()]

                  records++;
                }


              ///////////////////////////////////////////////////////////////////////////
              /////////////////////////Issue 4 Resolved Found////////////////////////////
              ///////////////////////////////////////////////////////////////////////////


              //If we have gotten any data from the file

              if (got_data)
                {
                  //Converting time in seconds to actual year, jday, hour, min, sec

                  total_time = (time_t) data.t;

                  conv_time = gmtime(&total_time);
                  conv_time->tm_yday++;
                  conv_secs = conv_time->tm_sec;

                  if(options.isMakeTRK == 1) fprintf (ofp, "+,%d,%d,%.9f,%.9f,%f,%f,%d,%d,%02d,%02d,%f\n", data.f, data.p, data.y, data.x, data.h, data.s,
                                                      conv_time->tm_year + 1900, conv_time->tm_yday, conv_time->tm_hour, conv_time->tm_min, conv_secs);


                  //If we have selected to make a .nav file we now convert to epoch 1960

                  if(options.isMakeNAV == 1)
                    {
                      if((navNAVREC_Records[i]->numOfPings >= 1) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat != 
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].lat) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon !=
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].lon) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode !=
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].navMode) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].time !=
                          convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), 
                                            &(conv_time->tm_hour), &(conv_time->tm_min), &conv_secs)))
                        {
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat = (int32_t) (data.y * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon = (int32_t) (data.x * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode = navModeHolder;

                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].time =
                            convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), &(conv_time->tm_hour),        
                                              &(conv_time->tm_min), &conv_secs);

                          navNAVREC_Records[i]->numOfPings = navNAVREC_Records[i]->numOfPings + 1;
                          //fprintf(stderr, "Captured last point successfully for Records[%d]\n", i);
                        }
                    }

                  track_points++;
                }


              ///////////////////////////////////////////////////////////////////////////
              /////////////////////////Issue 4 Resolved Found////////////////////////////
              ///////////////////////////////////////////////////////////////////////////

              //Keep track of number of files we were able to open

              fileNum++;


              //Close this file - we are done with it

              gsfClose (gsf_handle);

              if (system(remove_ndx) == -1)
		{
		  fprintf (stderr, "Call to system error in file %s, function %s, line %d.  This should never happen!\n", __FILE__, __FUNCTION__, __LINE__);
		  exit (-1);
		}

              progress.fbar->setValue (100);
              qApp->processEvents ();
            }


          //Else we were not able to open the gsf file

          else
            {
              cur = new QListWidgetItem (QString (gsfStringError ()));
              trackList->addItem (cur);
              trackList->setCurrentItem (cur);
              trackList->scrollToItem (cur);


              //If we threw a unopenable error we need to close the gsf file

              if (gsfError != GSF_FOPEN_ERROR) gsfClose (gsf_handle);
            }

          break;


        case NAVTRACK_POS:

          if ((fp = open_pos_file (filename)) != NULL)
            {
              fseek (fp, 0, SEEK_END);
              eof = ftell (fp);
              fseek (fp, 0, SEEK_SET);


              //navModeHolder initialized here to nix warning with got_data usage

              navModeHolder = GSF_P_GYPSY_CODE;


              //  Set the progress bar.

              while (!pos_read_record (fp, &pos))
                {
                  ping++;

                  got_data = NVTrue;

                  points++;

                  data.f = i;
                  data.t = pos.gps_time;
                  data.p = ping;
                  data.y = pos.latitude * NV_RAD_TO_DEG;
                  data.x = pos.longitude * NV_RAD_TO_DEG;
                  data.h = (pos.platform_heading - pos.wander_angle) * NV_RAD_TO_DEG;
                  data.r = pos_get_timestamp(pos);      

                  navModeHolder = GSF_P_GYPSY_CODE;

                  if(options.isMakeNAV == 1)
                    {        
                      if(navNAVREC_Records[i]->numOfPings >= navNAVREC_Records[i]->numOfPingsAllocated)
                        {
                          if(!check_mem_dataPings(navNAVREC_Records[i], 10000))
                            {
                              exit(-2);
                            }
                                                                        
                          printf("\n# of Pings Allocated is %d.  # of Pings is %d.\n\n", navNAVREC_Records[i]->numOfPingsAllocated,
                                 navNAVREC_Records[i]->numOfPings);
                        }
                    }


                  /////////////////////////////////////////////////////////////////////////////////////////
                  //    if (track_points >= numOfPingsAllocated)
                  //      {
                  //        numOfPingsAllocated += 10000;
                  //                                                                        
                  //        if (realloc(dataPings, numOfPingsAllocated * sizeof(navDATAREC)) == NULL)
                  //          {
                  //            printf("Unable to allocate memory for ping storage sort.\n");
                  //            printf("Program will now terminate.\n");
                  //
                  //            exit(-2);
                  //          }
                  //                                                                        
                  //        printf("\n# of Pings Allocated is %d.  # of Pings is %d.\n\n", numOfPingsAllocated, track_points);
                  //      }
                  /////////////////////////////////////////////////////////////////////////////////////////
                                                
                  ///////////////////////////////////////////////////////////////////////////////
                  //      dataPings[track_points].lat = (int32_t) (data.y * 10000000);
                  //      dataPings[track_points].lon = (int32_t) (data.x * 10000000);
                  //      dataPings[track_points].navMode = 32; 
                  ///////////////////////////////////////////////////////////////////////////////


                  last_data = data;

                  if (first)
                    {
                      data.s = 0.0;


                      //If we selected to make a .nav file we will save the lat lon and navMode

                      if(options.isMakeNAV == 1)
                        {
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat = (int32_t) (data.y * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon = (int32_t) (data.x * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode = navModeHolder;
                        }


                      //Note that aircraft time uses data.r as input and NOT data.t
                      //(data.t is for GSF file types - data.r is for POS/SBET file types)

                      //Converting time in seconds to actual year, jday, hour, min, sec

                      total_time = (time_t) (data.r / 1000000.0);

                      conv_time = gmtime(&total_time);
                      conv_secs = conv_time->tm_sec;
                      conv_time->tm_yday++;


                      //  Save the last good heading since we want to compare it against the current heading.  Conversely, we want to compare
                      //  speeds using two adjacent points.

                      start_heading = data.h;

                      if(options.isMakeTRK == 1)
                        {                   
                          fprintf (ofp, "+,%d,%d,%.9f,%.9f,%f,%f,%d,%d,%02d,%02d,%f\n", data.f, data.p, data.y, data.x, data.h, data.s, 
                                   conv_time->tm_year + 1900, conv_time->tm_yday, conv_time->tm_hour, conv_time->tm_min, conv_secs);
                        }
                      if(options.isMakeNAV == 1)
                        {                        
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings++].time =
                            convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), &(conv_time->tm_hour),        
                                              &(conv_time->tm_min), &conv_secs);



                          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////                
                          //     //Converting again for 01/01/1960 epoch - this epoch is used by the .nav file format
                          //
                          //     dataPings[track_points].time = convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday),
                          //                                                      &(conv_time->tm_hour), &(conv_time->tm_min), &conv_secs);
                          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                        }
                      
                      first = NVFalse;

                      track_points++;
                    }
                  else if (changepos (&data, &prev_data, maxTime))
                    {
                      //If we selected to make a .nav file we will save the lat lon and navMode

                      if(options.isMakeNAV == 1)
                        {
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat = (int32_t) (data.y * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon = (int32_t) (data.x * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode = navModeHolder;
                        }


                      //Note that aircraft time uses data.r as input and NOT data.t
                      //(data.t is for GSF file types - data.r is for aircraft file types)

                      //Converting time in seconds to actual year, jday, hour, min, sec

                      total_time = (time_t) (data.r / 1000000.0);
                                                
                      conv_time = gmtime(&total_time);
                      conv_secs = conv_time->tm_sec;
                      conv_time->tm_yday++;


                      //  Save the last good heading since we want to compare it against the current heading.  Conversely, we want to compare
                      //  speeds using two adjacent points.

                      start_heading = data.h;

                      if(options.isMakeTRK == 1) fprintf (ofp, "+,%d,%d,%.9f,%.9f,%f,%f,%d,%d,%02d,%02d,%f\n", data.f, data.p, data.y, data.x, data.h, data.s, 
                                                          conv_time->tm_year + 1900, conv_time->tm_yday, conv_time->tm_hour, conv_time->tm_min, conv_secs);

                      if(options.isMakeNAV == 1)
                        {
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings++].time =
                            convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), &(conv_time->tm_hour),        
                                              &(conv_time->tm_min), &conv_secs);


                          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                          //      //Converting again for 01/01/1960 epoch - this epoch is used by the .nav file format
                          //      
                          //      dataPings[track_points].time = convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday),
                          //                                                       &(conv_time->tm_hour), &(conv_time->tm_min), &conv_secs);
                          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                        }
              
                      track_points++;
                    }

                  prev_data = data;


                  //  We don't want to eat up all of the free time on the system with a progress bar.

                  percent = (int32_t) roundf (((float) (ftell (fp)) / eof) * 100.0);

                  if (percent - old_percent >= 5 || old_percent > percent)
                    {
                      progress.fbar->setValue (percent);
                      old_percent = percent;

                      qApp->processEvents ();
                    }
                }


              if (got_data)
                {
                  //Note that aircraft time uses data.r as input and NOT data.t
                  //(data.t is for GSF file types - data.r is for aircraft file types)

                  //Converting time in seconds to actual year, jday, hour, min, sec

                  total_time = (time_t) (data.r / 1000000.0);
            
                  conv_time = gmtime(&total_time);
                  conv_secs = conv_time->tm_sec;
                  conv_time->tm_yday++;

                  if(options.isMakeTRK == 1) fprintf (ofp, "+,%d,%d,%.9f,%.9f,%f,%f,%d,%d,%02d,%02d,%f\n", data.f, data.p, data.y, data.x, data.h, data.s, 
                                                      conv_time->tm_year + 1900, conv_time->tm_yday, conv_time->tm_hour, conv_time->tm_min, conv_secs);

                  if(options.isMakeNAV == 1)
                    {
                      if((navNAVREC_Records[i]->numOfPings > 1) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat != 
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].lat) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon !=
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].lon) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode !=
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].navMode) &&
                         (navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings - 1].time !=
                          convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), 
                                            &(conv_time->tm_hour), &(conv_time->tm_min), &conv_secs)))
                        {
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lat = (int32_t) (data.y * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].lon = (int32_t) (data.x * 10000000);
                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].navMode = navModeHolder;

                          navNAVREC_Records[i]->dataPings[navNAVREC_Records[i]->numOfPings].time =
                            convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday), &(conv_time->tm_hour),        
                                              &(conv_time->tm_min), &conv_secs);

                          navNAVREC_Records[i]->numOfPings = navNAVREC_Records[i]->numOfPings + 1;
                          //fprintf(stderr, "Captured last point successfully for Records[%d]\n", i);
                        }
                    }


                  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  //      //Converting again for 01/01/1960 epoch - this epoch is used by the .nav file format
                  //
                  //      dataPings[track_points].time = convert_1960_time(&(conv_time->tm_year), &(conv_time->tm_yday),
                  //                                                       &(conv_time->tm_hour), &(conv_time->tm_min), &conv_secs);
                  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                  track_points++;
                }

              //printf("<{END OF FILE #%d}>\n", ++fileNum);

              fclose (fp);

              progress.fbar->setValue (100);
              qApp->processEvents ();
            }
                          
          break;

          //Switch End - File was either not opened or it was processed
        }


      cur = new QListWidgetItem (tr ("Completed processing file %1 of %2 - %3").arg (i + 1).arg (file_count).arg (QFileInfo (filename).fileName ()));
      trackList->addItem (cur);
      trackList->setCurrentItem (cur);
      trackList->scrollToItem (cur);


      //Increment ndxHolder

      ndxHolder++;

      //File for loop end - go to next file
    }


  //fprintf(stderr, "\nNum of Pings pre processing = %d\n", navNAVREC_Records[0]->numOfPings);
  //fprintf(stderr, "\nValidating NAVREC_Records[0 to %d]\n", navNAVREC_final_ndx);

        
  if(recType == NVTrue)
    {
      for(x_step = 0; x_step < navNAVREC_final_ndx; x_step++)
        {
          if(navNAVREC_Records[x_step]->numOfPings <= 1)
            {
              free_NAVREC(navNAVREC_Records[x_step]);
              free(navNAVREC_Records[x_step]);
              navNAVREC_Records[x_step] = NULL;
            }
        }


      //fprintf(stderr, "\nqsorting each file's worth of pings\n");

      for(x_step = 0; x_step < navNAVREC_final_ndx; x_step++)
        {
          if(navNAVREC_Records[x_step] != NULL)
            sortDataPings(navNAVREC_Records[x_step]);
        }


      //fprintf(stderr, "Sorting pings by dataPings[0]\n");

      sortNavRecs(navNAVREC_Records, navNAVREC_final_ndx);


      //fprintf(stderr, "Finalizing lines()\n");

      goodRecord = mergeNavRecs(navNAVREC_Records, navNAVREC_final_ndx);

        
      //fprintf(stderr, "Resorting after the big merger\n");

      for(x_step = 0; x_step < navNAVREC_final_ndx; x_step++)
        {
          if(navNAVREC_Records[x_step] != NULL)
            sortDataPings(navNAVREC_Records[x_step]);
        }


      //fprintf(stderr, "Creating the final set\n\n");

      createMasterRec(navNAVREC_Records, navNAVREC_final_ndx);


      //fprintf(stderr, "\nWriting the nav4word file\n");

      if(goodRecord)
        {
          for(y_step = 0; y_step < navNAVREC_Records[navNAVREC_final_ndx]->numOfPings; y_step++)
            {
              if(fwrite(&navNAVREC_Records[navNAVREC_final_ndx]->dataPings[y_step], sizeof(navDATAREC), 1, nofp) != 1)
                printf("Unable to write dataRec to .nav file!  Skipping ping record.\n");
            }
        }
      else
        {
          fprintf(stderr, "\nThere were no legitimate records to be written to the .nav file\n");
        }
    }
  else
    {
      for(y_step = 0; y_step < navNAVREC_Records[0]->numOfPings; y_step++)
        {
          if(fwrite(&navNAVREC_Records[0]->dataPings[y_step], sizeof(navDATAREC), 1, nofp) != 1)
            printf("Unable to write dataRec to .nav file!  Skipping ping record.\n");
        }
    }


  //fprintf(stderr, "\nTotal Allocated = %d", navNAVREC_Records[navNAVREC_final_ndx]->numOfPingsAllocated);
  //fprintf(stderr, "\nTotal Pings     = %d", navNAVREC_Records[navNAVREC_final_ndx]->numOfPings);

  //fprintf(stderr, "\nCLOSING FILES\n");        

  if(options.isMakeTRK == 1) fclose (ofp);
  if(options.isMakeNAV == 1) fclose (nofp);

  if(options.isMakeNAV == 1)
    {
      cur = new QListWidgetItem (tr ("\n\nDetermined %1 navTrack points from %2 positions.\n\n").arg (navNAVREC_Records[navNAVREC_final_ndx]->numOfPings).arg (points));

      trackList->addItem (cur);
      trackList->setCurrentItem (cur);
      trackList->scrollToItem (cur);
    }

  QApplication::restoreOverrideCursor ();
  button (QWizard::FinishButton)->setEnabled (true);
  fprintf(stderr, "END OF BIG FUNCTION\n");
}



//This function checks two points to discover if there is a heading change

int32_t navTrack::heading_change (INFO *data)
{
  double              heading1 = start_heading, heading2 = data->h;


  //  Check for northerly heading and adjust if necessary

  if (heading1 > (360 - options.heading[type]) && heading2 < options.heading[type]) heading2 += 360;
  if (heading2 > (360 - options.heading[type]) && heading1 < options.heading[type]) heading1 += 360;

  return (fabs (heading1 - heading2) >= options.heading[type]);
}



//This function checks two points to discover if there is a speed change

uint8_t navTrack::changepos (INFO *data, INFO *prev_data, float maxTime)
{
  double              dist;
  double              az;
  uint8_t             change;
        
  change = NVFalse;


  //  Heading change greater than options.heading[type].

  if (heading_change (data)) change = NVTrue;


  //  Speed greater than options.speed[type].

  else
    {
      invgp (NV_A0, NV_B0, prev_data->y, prev_data->x, data->y, data->x, &dist, &az);

      data->s = dist / (data->t - prev_data->t) * MPS_TO_KNOTS;

      if (data->s > options.speed[type]) change = NVTrue;
    }


  //  Time change greater than user defined time.

  if ((data->t - prev_data->t) > (maxTime * 60.0)) change = NVTrue;

  if ((data->t - prev_data->t) == 0.0) change = NVFalse;

  return (change);
}



//This function is used to help validate whether we have a good beam of pings or not
//It checks for any null beams

int32_t navTrack::validateBeams(gsfSwathBathyPing mb_ping)
{
  int32_t validNumOfBeams = 0;
  int32_t count;

  for(count = 0; count < mb_ping.number_beams; count++)
    {
      if(!(mb_ping.beam_flags[count] & NV_GSF_IGNORE_NULL_BEAM))
        validNumOfBeams++;
    }
        
  return(validNumOfBeams);
}
