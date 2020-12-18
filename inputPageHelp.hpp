
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

QString formatText = 
  inputPage::tr ("<br><br>The following file formats are accepted by the navTrack program:<br><br>"
                 "<ul>"
                 "<li>GSF (any extension but usually .dNN where NN represents a 2 digit number)</li>"
                 "<li>CHARTS POS/SBET (extension .pos, .out, .POS, .OUT)</li>"
                 "<ul>");

QString inputBrowseText = 
  inputPage::tr ("This button allows you to select multiple input files.  You may use this button as many times as "
                 "needed to define all of the desired input files.  This may also be used in conjunction with the "
                 "<b>Browse Directories</b> button.  All selected files are added to the <b>Input files</b> list.") + formatText;

QString dirBrowseText = 
  inputPage::tr ("This button allows you to select multiple input files by specifiying a top level directory.  Using "
                 "the <b>Directory file mask</b> field as the filter, all files below the selected directory will be "
                 "searched for matching files.  These files will be added to the <b>Input files</b> list.  This may "
                 "also be used in conjunction with the <b>Browse Files</b> button.  You may use this button as many "
                 "times as needed to define all of the desired input files.") + formatText;

QString fileMaskText = 
  inputPage::tr ("This is the file search filter for the <b>Browse Directories</b> button.  You may select a filter "
                "from the existing list or type in your own special filter.  This filter is used in searching "
                "directory trees when using the <b>Browse Directories</b> button.") + formatText;

QString inputFilesText = 
  inputPage::tr ("This is a list of all files that will be input to navTrack program.  You may edit the list or even "
                "add files by hand (although why anyone would do this is beyond me).  There is limited text editor "
                "functionality available for this list.  Right click to get a menu of functions including undo, redo, "
                "cut, paste, and others.  If you make a mistake and add duplicate files using the <b>Browse "
                "Directories</b> button or even the <b>Browse Files</b> button, fear not.  The input file list is sorted "
                "and duplicates are culled out before the navTrack generation process is started.") + formatText;
