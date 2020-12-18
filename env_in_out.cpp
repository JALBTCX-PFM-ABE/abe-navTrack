
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



#include "navTrack.hpp"


double settings_version = 1.00;


/*!
  These functions store and retrieve the program settings (environment) from a .ini file.  On both Linux and Windows
  the file will be called navTrack.ini and will be stored in a directory called ABE.config.  On Linux, the
  ABE.config directory will be stored in your $HOME directory.  On Windows, it will be stored in your $USERPROFILE
  folder.  If you make a change to the way a variable is used and you want to force the defaults to be restored just
  change the settings_version to a newer number (I've been using the program version number from version.hpp - which
  you should be updating EVERY time you make a change to the program!).  You don't need to change the
  settings_version though unless you want to force the program to go back to the defaults (which can annoy your
  users).  So, the settings_version won't always match the program version.
*/

void envin (OPTIONS *options)
{
  //  We need to get the font from the global settings.

#ifdef NVWIN3X
  QString ini_file2 = QString (getenv ("USERPROFILE")) + "/ABE.config/" + "globalABE.ini";
#else
  QString ini_file2 = QString (getenv ("HOME")) + "/ABE.config/" + "globalABE.ini";
#endif

  options->font = QApplication::font ();

  QSettings settings2 (ini_file2, QSettings::IniFormat);
  settings2.beginGroup ("globalABE");


  QString defaultFont = options->font.toString ();
  QString fontString = settings2.value (QString ("ABE map GUI font"), defaultFont).toString ();
  options->font.fromString (fontString);


  settings2.endGroup ();


  double saved_version = 0.0;
  QString string;


  //  Get the INI file name

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/navTrack.ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/navTrack.ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup ("navTrack");

  saved_version = settings.value (QString ("settings version"), saved_version).toDouble ();


  //  If the settings version has changed we need to leave the values at the new defaults since they may have changed.

  if (settings_version != saved_version) return;


  options->width = settings.value (QString ("width"), options->width).toInt ();
  options->height = settings.value (QString ("height"), options->height).toInt ();

  options->window_x = settings.value (QString ("x position"), options->window_x).toInt ();
  options->window_y = settings.value (QString ("y position"), options->window_y).toInt ();

  options->inputFilter = settings.value (QString ("input filter"), options->inputFilter).toString ();

  options->heading[NAVTRACK_GSF] = settings.value (QString ("GSF heading limit"), options->heading[NAVTRACK_GSF]).toDouble ();
  options->speed[NAVTRACK_GSF] = settings.value (QString ("GSF speed limit"), options->speed[NAVTRACK_GSF]).toDouble ();

  options->heading[NAVTRACK_POS] = settings.value (QString ("POS heading limit"), options->heading[NAVTRACK_POS]).toDouble ();
  options->speed[NAVTRACK_POS] = settings.value (QString ("POS speed limit"), options->speed[NAVTRACK_POS]).toDouble ();

  settings.endGroup ();
}


void envout (OPTIONS *options)
{
  //  Get the INI file name

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/navTrack.ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/navTrack.ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup ("navTrack");


  settings.setValue (QString ("settings version"), settings_version);


  settings.setValue (QString ("width"), options->width);
  settings.setValue (QString ("height"), options->height);

  settings.setValue (QString ("x position"), options->window_x);
  settings.setValue (QString ("y position"), options->window_y);

  settings.setValue (QString ("input filter"), options->inputFilter);

  settings.setValue (QString ("GSF heading limit"), options->heading[NAVTRACK_GSF]);
  settings.setValue (QString ("GSF speed limit"), options->speed[NAVTRACK_GSF]);

  settings.setValue (QString ("POS heading limit"), options->heading[NAVTRACK_POS]);
  settings.setValue (QString ("POS speed limit"), options->speed[NAVTRACK_POS]);

  settings.endGroup ();
}
