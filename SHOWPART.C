/**************************************************************************
                           S H O W P A R T

  Show partition record data for 80 (first) drive.  Thanks to Steve
  Behman for early version.

  Works equally by compiling with Microsoft or Borland:

    cl showpart.c
  or
    bcc showpart.c

  7/4/93        Dave Hillman
                CIS:      74045,1127
                Internet: dxh50@juts.ccc.amdahl.com

**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <process.h>
#include <bios.h>

typedef unsigned char Byte;
typedef unsigned long Ulong;
typedef struct
{
  Byte boot_ind;        // boot indicator.  0 or 0x80 if active
  Byte beg_cchr[3];     // Beginning of partition Track sector & head
  Byte system_ind;      // 0x06 is BIG DOS primary
                        // 0x05 is Extended DOS
                        // 0x07 is NT - NTFS
                        // 0x0a is OS/2 Boot manager
  Byte end_cchr[3];     // Ending of partition Track, sector and head
  Ulong rel_sector;     // Starting sector number
  Ulong num_sector;     // Length of partition in sectors
} PartitionRecord;

typedef struct
{
  Byte bootStuff[ 0x1be ];
  PartitionRecord par[ 4 ];
  Byte endStuff[2];
} BootRec;

union REGS in;
struct SREGS seg;

#define AX in.x.ax
#define BX in.x.bx
#define ES seg.es
#define CX in.x.cx
#define DX in.x.dx

Byte buf[512];

/**************************************************************************
                               M A I N
**************************************************************************/

void main()
{
  int i;
  BootRec *rec = (BootRec *)buf;
  PartitionRecord *par = rec->par;
  Byte far *p = ( Byte far *)buf;
  int drive = 0;

  AX = 0x0201;                          /* read 1 sector. this will be
                                           0x0301 to write */
  BX = FP_OFF( p );                     /* into the record */
  ES = FP_SEG( p );
  CX = 0x0001;                          /* to/from track 0, sector 1 */
  DX = 0x0080 + drive;                  /* in drive 80 (81 is the
                                           second) */
  int86x( 0x13, &in, &in, &seg );

  /* display the data to be sure we got it right */
  for (i = 0; i < 4; i++)
  {
    printf( " %d:%d %02x %02x\n",
        drive, i,
        par[i].boot_ind,
        par[i].system_ind );
  }

  /*
   * Do other work here such as writing the buffer to disk so it can
   * be modified and written later. Or modified and written back to
   * the boot sector.
   */

} /* end main */

