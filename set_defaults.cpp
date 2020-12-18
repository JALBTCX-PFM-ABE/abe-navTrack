
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

void set_defaults (OPTIONS *options)
{
  options->window_x = 0;
  options->window_y = 0;
  options->width = 800;
  options->height = 600;
  options->inputFilter = navTrack::tr ("GSF (*.d[0-9][0-9] *.gsf)");
  options->heading[NAVTRACK_GSF] = 2.0;
  options->speed[NAVTRACK_GSF] = 20.0;
  options->heading[NAVTRACK_POS] = 5.0;
  options->speed[NAVTRACK_POS] = 240.0;
  options->isMakeNAV = 0;
  options->isMakeTRK = 0;
  options->ship_identity = 0;
}
