
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


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

    *****************************************  IMPORTANT NOTE  **********************************/


/*********************************************************************************************

    This program is public domain software that was developed by the U.S. Naval Oceanographic
    Office.

    This is a work of the US Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the US Government.

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.

*********************************************************************************************/

#ifndef VERSION

#define VERSION     "PFM Software - navTrack V1.17 -  08/27/16"

#endif

/*! <pre>

  Version 1.00
  Kevin M Byrd
  09/06/2011

  First version.


  Version 1.01
  Jan C. Depner
  11/30/11

  Converted .xpm icons to .png icons.


  Version 1.02
  Kevin M Byrd
  01/18/2012

  Switched all nav code to use nav API.


  Version 1.03
  Kevin M Byrd
  06/12/2012

  Bug fixes.
  -Line merging and some more bad file recognitions
  -Finally got navTrack to recognize every ship CORRECTLY from shiplist.txt    (@_@) ugh
  -Changed the way got_data is used to retrieve that last point in a file


  Version 1.04
  Kevin M Byrd
  07/17/2012

  Bug fixes.
  -Various small fixes for files with a single ping


  Version 1.05
  Kevin M Byrd
  01/09/2013

  - Modified the nav4word library to get around the navMode = 0 issue
    which means this program needed to be rebuilt
  - Any checks made to test if navMode < 0 should now be checking if the signed bit is flagged
  - Left the navMode *= -1 code as the navlibrary fixes the 'negative zero' issue
  - I went through all programs touched by the 'negative zero' issue
    and rebuilt them so that the programs using the fixed library will
    have a 2013 date


  Version 1.07
  Kevin M Byrd
  03/22/2013

  - Switched data and prev_data to make (data - prev_data) >= 0
  - Changed 60 second gap checks to 300 second gap checks (5 minutes)


  Version 1.08
  Jan C. Depner (PFM Software)
  12/09/13

  Switched to using .ini file in $HOME (Linux) or $USERPROFILE (Windows) in the ABE.config directory.  Now
  the applications qsettings will not end up in unknown places like ~/.config/navo.navy.mil/blah_blah_blah on
  Linux or, in the registry (shudder) on Windows.


  Version 1.09
  Jan C. Depner (PFM Software)
  01/06/14

  Fixed an obvious screwup in inputPage.cpp where I was re-using the loop counters.
  Scoping in C++ saved my bacon but it was just too damn confusing.


  Version 1.10
  Jan C. Depner (PFM Software)
  03/14/14

  - Replaced all HMPS_ constants with the new NV_GSF_ constants.
  - Removed libhmpsgsf from mk file (it's gone).
  - Removed -lnav from mk file (it's been moved to nvutility).
  - Replaced *ALL* TAB characters in all of the *.cpp and *.hpp files.  TABs show up differently in every freaking editor known to man.
    That's why I try to avoid them.  What you think looks pretty in kate or gedit or eclipse may look like guano in any other editor.
    In XEmacs you can edit your ~/.xemacs/.custom.el file and add the following line:
    (setq-default indent-tabs-mode nil)
    If you want to replace pre-existing tabs with spaces in XEmacs just press <Replace>, enter ^q^i, press return, enter whatever number
    of spaces you want to use, press return, then press ! (also known as <Shift>1).
    If you're using some other editor you should search the interwebs for a way to insert spaces instead of tabs.


  Version 1.11
  Jan C. Depner (PFM Software)
  03/17/14

  Removed WLF support.  Top o' the mornin' to ye!
  IMPORTANT NOTE: From a cursory examination it appears that this program is broken for everything except GSF.  It also doesn't support
  CZMIL CSF format.  Please take a look at the trackLine program to see what should be going on.


  Version 1.12
  Jan C. Depner (PFM Software)
  05/07/14

  - Check status for return of "system" call (you should really use "remove" here instead of system).
  - Replaced RAD_TO_DEG with NV_RAD_TO_DEG.


  Version 1.13
  Jan C. Depner (PFM Software)
  06/13/14

  - Changed QFileDialog filters using the proper calls (so they work in both Qt4 and Qt5).


  Version 1.14
  Jan C. Depner (PFM Software)
  07/23/14

  - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
    inttypes.h sized data types (e.g. int64_t and uint32_t).


  Version 1.15
  Jan C. Depner (PFM Software)
  03/14/15

  - Since AHAB Hawkeye has switched to LAS format I have removed support for the old Hawkeye I binary format.


  Version 1.16
  Jan C. Depner (PFM Software)
  03/31/15

  - Fixed name filter for GSF files so that it wouldn't find files that don't end in .dNN where NN is numeric.
    This prevents us from seeing, for example, ESRI shape .dbf files in the GSF name list.


  Version 1.17
  Jan C. Depner (PFM Software)
  08/27/16

  - Now uses the same font as all other ABE GUI apps.  Font can only be changed in pfmView Preferences.

</pre>*/
