/* Copyright 2001 Enhanced Software Technologies Inc.
 *   Written by Eric Lee Green <eric@estinc.com>
 *
 *$Date$
 *$Revision$

  This program is free software; you may redistribute and/or modify it under
  the terms of the GNU General Public License Version 2 as published by the
  Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for complete details.

*/

/* These are the SCSI commands for AIX. The syntax for AIX is:
 *
 *  /dev/scsi<n>/<id>.<lun>
 *
 * where <n> is the number of the scsi adapter (0..n) and
 * <id> and <lun> are the SCSI ID and LUN of the device you wish to
 * talk to. 
 *
 * AIX has a very flexible SCSI subsystem, but it is somewhat
 * clumsy to use. 
 */

/* we do very nasty thing here -- we operate upon device name! */
DEVICE_TYPE SCSI_OpenDevice(char *DeviceName) {
  /* okay, we must first parse out the ID and LUN: */
  char *rptr;
  struct tm_device_type *retval = (struct tm_device_type *) malloc(sizeof(struct tm_device_type));
  int id,lun,filenum,idlun;

  if (retval==NULL) {
    fprintf(stderr,"%s: Allocation error in SCSI_OpenDevice for %s. Exiting.\n",argv0,DeviceName);
    fflush(stderr);
    exit(1);
  }

  rptr=strrchr(DeviceName,'/');
  
  if (!rptr) {
    fprintf(stderr,"%s: Illegal device name '%s'. Exiting.\n",argv0,DeviceName);
    fflush(stderr);
    exit(1);
  }
  
  *rptr++=0;

  if (sscanf(rptr,"%d.%d",&id,&lun) < 2) {
    /* whoops, we did not get 2 items:  */ 
    fprintf(stderr,"%s: Illegal device name '%s/%s'. Exiting.\n",argv0,DeviceName,rptr);
    fflush(stderr);
    exit(1);
  }

  /* Okay, now to try to open the DeviceName */
  if ((filenum=open(DeviceName,0))<0) {
    fprintf(stderr,"%s: Illegal device name '%s/%s'. Exiting.\n",argv0,DeviceName,rptr);
    perror(argv0);
    fflush(stderr);
    exit(1);
  }

  retval->filenum=filenum;
  retval->id=id;
  retval->lun=lun;
  retval->DeviceName=DeviceName;
  return (DEVICE_TYPE) retval;
}

#define MTX_HZ 1000 
#define MTX_DEFAULT_SCSI_TIMEOUT 60*5*MTX_HZ /* 5 minutes! */
static int mtx_default_timeout = MTX_DEFAULT_SCSI_TIMEOUT ;
void SCSI_Set_Timeout(int sec) {
  mtx_default_timeout=sec*MTX_HZ;
}

void SCSI_Default_Timeout() {
  mtx_default_timeout=MTX_DEFAULT_SCSI_TIMEOUT;
}

/* Okay , now *DO IT!* */
int SCSI_ExecuteCommand(DEVICE_TYPE DeviceFD,
			Direction_T Direction,
			CDB_T *CDB,
			int CDB_Length,
			void *DataBuffer,
			int DataBufferLength,
			RequestSense_T *RequestSense) {

  int id,lun;
  struct tm_device_type *fd=(struct tm_device_type *) DeviceFD;
  struct devinfo info;
  struct sc_buf sb; /* the sc_buf struct needed to commicate w/adapter. */


  id=fd->id;
  lun=fd->lun;

  /* okay, first of all, make sure we're not asking for a bigger
   * operation than we are allowed to ask for, by going to the driver
   * with IOCINFO. 
   */
  if (ioctl(filenum,IOCINFO,&info)) {
    fprintf(stderr,"%s: Could not get info for %s. Exiting.\n",argv0,fd->DeviceName);
    exit(1);
  }
    
  /* Now check the max_transfer: */
  if ((int)info.scsi.max_transfer < DataBufferLength) {
    fprintf(stderr,"%s: SCSI transfer too large. %d requested, %d allowed.\n",argv0,DataBufferLength,(int)info.scsi.max_transfer);
    fflush(stderr);
    exit(1);
  }
  
  /* okay, we have our open file, we have the other stuff: Now initialize
     a transaction with that ID/LUN:
  */

  if (ioctl(filenum, SCIOSTART,IDLUN(id,lun))) {
    fprintf(stderr,"%s: Could not start SCSI transaction with %s/%d.%d. Exiting.\n",argv0,fd->DeviceName,id,lun);
    fflush(stderr);
    exit(1);
  }
  
  /* okay, now to decide command: */
  
  


  /*... have finished command...*/
  
  /* when done w/command, get rid of the connection: */  
  if (ioctl(filenum,SCIOSTOP,IDLUN(id,lun))) {
    fprintf(stderr,"%s: Could not stop SCSI transaction with %s/%d.%d. Exiting.\n",argv0,fd->DeviceName,id,lun);
    fflush(stderr);
    exit(1);
  }
     
  
