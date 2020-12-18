
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

/* codes.h */

#define ENDIAN_CODE            0x00010203


#define UNKNOWN_PROCESS_CODE   0
#define MNT_DGT_CODE           1
#define SBS_CODE               6
#define NPS_CODE               7
#define INS_CODE               8
#define HSPS_CODE               9
#define SECRET_OSP_CODE        1
#define SECRET_FDS_CODE        2
#define SECRET_CAESAR_CODE     3
#define SECRET_OTHER_CODE      4
#define CONFID_FDS_CODE        6
#define CONFID_CAESAR_CODE     7
#define CONFID_OTHER_CODE      8
#define UNCLAS_RESTRICT_CODE   9
#define UNCLAS_UNLIMITED_CODE  10
#define UNKNOWN_DATUM_CODE     1
#define WGS72_CODE             4
#define WGS80_CODE             5
#define WGS84_CODE             6
#define UNKNOWN_NAV_CODE       0
#define INTERPOLATED_NAV_CODE  8
#define DIGITIZED_NAV_CODE     16
#define GPS_P_CODE             17 /* GSF PPSS */
#define GPS_CA_CODE            18 /* GSF SPSS */
#define LORAN_CODE             19
#define SATFIX_CODE            20
#define SATDR_CODE             21
#define RENAV_CODE             22
#define ADJNAV_CODE            23
#define GPS_HYDRO              24
#define DIFF_GPS_HYDRO         25
#define RANGE_HYDRO            26
#define KFP_ISS60              27
#define GSF_P_DIFF_CODE        28 /* GSF PPSD */
#define GSF_P_KIN_CODE         29 /* GSF PPSK */
#define GSF_S_DIFF_CODE        30 /* GSF SPSD */
#define GSF_S_KIN_CODE         31 /* GSF SPSK */
#define GSF_P_GYPSY_CODE         32 /* GSF PPSG*/
#define GSF_S_GYPSY_CODE         33 /* GSF SPSG*/
