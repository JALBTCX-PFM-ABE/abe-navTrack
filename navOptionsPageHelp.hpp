
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

QString QCheckBoxText =
  navOptionsPage::tr ("Select this option to create a .nav file");

QString shipIDText =
  navOptionsPage::tr ("Select the survey platform");

QString navProcessText =
  navOptionsPage::tr ("Select the processing type used");

QString navAccessText =
  navOptionsPage::tr ("Select the classification of the survey data");

QString navDatumText =
  navOptionsPage::tr ("Select the datum used");

QString navSurvOpText =
  navOptionsPage::tr ("Enter the Survey Op");

QString navOutputFileText =
  navOptionsPage::tr ("This is the output file name.  If the name doesn't have a .nav extension one will be appended.  You may "
                 "use the <b>Browse</b> button to select an existing navTrack (.nav) file to be overwritten.");

QString output_file_browseText =
  navOptionsPage::tr ("Use this button to select an existing navTrack (.nav) file to overwrite."); 

QStringList processList =
  (QStringList() << "HSPS" << "INS" << "NPS" << "SBS - Seabeam hand-processed"
                 << "MNT/DGT - Digitized");

QStringList accessList =
  (QStringList() << "Unclassified - Unlimited" << "Unclassified - Restricted"
                 << "Confidential - Other" << "Confidential - Caesar"
                 << "Confidential - FDS" << "Secret - Other" << "Secret - Caesar"
                 << "Secret - FDS ");

QStringList datumList = 
  (QStringList() << "WGS-84" << "WGS-80" << "WGS-72" << "Unknown");
