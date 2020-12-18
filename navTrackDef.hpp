
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

#ifndef __NAVTRACKDEF_H__
#define __NAVTRACKDEF_H__


#include <cerrno>
#include <unistd.h>
#include <sys/types.h>

#include <QtCore>
#include <QtGui>

#include "nvutility.h"
#include "nvutility.hpp"

#include "gsf.h"
#include "FilePOSOutput.h"

#include "version.hpp"


#define MPS_TO_KNOTS 1.943844924


//  File types.

#define NAVTRACK_GSF        0
#define NAVTRACK_POS        1


typedef struct
{
  double              x;      //  position longitude (dec deg)
  double              y;      //  position latitude (dec deg)
  double              h;      //  heading (degrees)
  double              s;      //  speed in nautical miles per hour
  double              t;      //  time in seconds from 1900
  int32_t             f;      //  input file # (starting from 0)
  int32_t             p;      //  ping record_number (starting with 1)
  int64_t             r;      //  time in microseconds from 1970
} INFO;

typedef struct
{
  QGroupBox           *fbox;
  QProgressBar        *fbar;
} RUN_PROGRESS;


typedef struct
{
  double              heading[4];
  double              speed[4];
  QString             inputFilter;
  QString             input_dir;
  QString             output_dir;
  QString             navOutput_dir;
  int32_t             window_x;
  int32_t             window_y;
  int32_t             width;
  int32_t             height;
  int32_t             isMakeNAV;
  int32_t             isMakeTRK;
  int32_t             ship_identity;
  QFont               font;                       //  Font used for all ABE GUI applications
} OPTIONS;


void envin (OPTIONS *options);
void envout (OPTIONS *options);


#endif
