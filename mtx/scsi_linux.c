/* Copyright 1997, 1998 Leonard Zubkoff <lnz@dandelion.com>
   Changes in Feb 2000 Eric Green <eric@estinc.com>

$Date$
$Revision$

  This program is free software; you may redistribute and/or modify it under
  the terms of the GNU General Public License Version 2 as published by the
  Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for complete details.

*/

/* this is the SCSI commands for Linux. Note that <eric@estinc.com> changed 
 * it from using SCSI_IOCTL_SEND_COMMAND to using the SCSI generic interface.
 */

#ifndef HZ
#define HZ 100
#endif

/* These are copied out of BRU 16.1, with all the boolean masks changed
 * to our bitmasks.
*/
#define S_NO_SENSE(s) ((s)->SenseKey == 0x0)
#define S_RECOVERED_ERROR(s) ((s)->SenseKey == 0x1)

#define S_NOT_READY(s) ((s)->SenseKey == 0x2)
#define S_MEDIUM_ERROR(s) ((s)->SenseKey == 0x3)
#define S_HARDWARE_ERROR(s) ((s)->SenseKey == 0x4)
#define S_UNIT_ATTENTION(s) ((s)->SenseKey == 0x6)
#define S_BLANK_CHECK(s) ((s)->SenseKey == 0x8)
#define S_VOLUME_OVERFLOW(s) ((s)->SenseKey == 0xd)

#define DEFAULT_TIMEOUT 3*60  /* 3 minutes here */

/* Sigh, the T-10 SSC spec says all of the following is needed to
 * detect a short read while in variable block mode, and that even
 * though we got a BLANK_CHECK or MEDIUM_ERROR, it's still a valid read.
 */

#define HIT_FILEMARK(s) (S_NO_SENSE((s)) && (s)->Filemark && (s)->Valid)
#define SHORT_READ(s) (S_NO_SENSE((s)) && (s)->ILI && (s)->Valid &&  (s)->AdditionalSenseCode==0  && (s)->AdditionalSenseCodeQualifier==0)
#define HIT_EOD(s) (S_BLANK_CHECK((s)) && (s)->Valid)
#define HIT_EOP(s) (S_MEDIUM_ERROR((s)) && (s)->EOM && (s)->Valid)
#define HIT_EOM(s) ((s)->EOM && (s)->Valid)

#define STILL_A_VALID_READ(s) (HIT_FILEMARK(s) || SHORT_READ(s) || HIT_EOD(s) || HIT_EOP(s) || HIT_EOM(s))


#define SG_SCSI_DEFAULT_TIMEOUT HZ*60*5  /* 5 minutes? */

static int pack_id;

DEVICE_TYPE SCSI_OpenDevice(char *DeviceName)
{
  int timeout=SG_SCSI_DEFAULT_TIMEOUT;      /* 5 minutes */
  int DeviceFD = open(DeviceName, O_RDWR);
  if (DeviceFD < 0)
    FatalError("cannot open SCSI device '%s' - %m\n", DeviceName);

  if(ioctl(DeviceFD, SG_SET_TIMEOUT, &timeout)) {
    FatalError("failed to set sg timeout - %m\n");
  }
  pack_id=1;  /* used for SG v3 interface if possible. */
  return (DEVICE_TYPE) DeviceFD;
}

static int sg_timeout = SG_SCSI_DEFAULT_TIMEOUT ;

void SCSI_Set_Timeout(int secs) {
  sg_timeout=secs*HZ;
}
 
void SCSI_Default_Timeout(void) {
  sg_timeout=SG_SCSI_DEFAULT_TIMEOUT;
}

void SCSI_CloseDevice(char *DeviceName,
			     DEVICE_TYPE DeviceFD)
{
  if (close(DeviceFD) < 0)
    FatalError("cannot close SCSI device '%s' - %m\n", DeviceName);
}


/* Added by Eric Green <eric@estinc.com> to deal with burping
 * Seagate autoloader (hopefully!). 
 */
/* Get the SCSI ID and LUN... */
scsi_id_t *SCSI_GetIDLun(DEVICE_TYPE fd) {
  int status;
  scsi_id_t *retval;
  
  struct my_scsi_idlun {
    int word1;
    int word2;
  } idlun;

  status=ioctl(fd,SCSI_IOCTL_GET_IDLUN,&idlun);
  if (status) {
    return NULL; /* sorry! */
  }
  
  retval=(scsi_id_t *)xmalloc(sizeof(scsi_id_t));
  retval->id=idlun.word1 & 0xff;
  retval->lun=idlun.word1 >> 8 & 0xff;
#ifdef DEBUG
  fprintf(stderr,"SCSI:ID=%d LUN=%d\n",retval->id,retval->lun);
#endif
  return retval;
}


/* Changed January 2001 by Eric Green <eric@badtux.org> to 
 * use the Linux version 2.4 SCSI Generic facility if available.
 * Liberally cribbed code from Doug Gilbert's sg3 utils. 
 */

#ifdef SG_IO

#include "sg_err.h"  /* error stuff. */
#include "sg_err.c"  /* some of Doug Gilbert's routines */

/* Swiped these routines from Doug Gilbert, mucho modified. 
  -1 -> unrecoverable error, 0 -> successful, 1 -> recoverable (ENOMEM),
   2 -> try again */
int sg_write(int sg_fd, unsigned char *wrCmd, unsigned char * buff, int cmdlen, int buflen,
             int bs, int * diop, RequestSense_T *RequestSense)
{
    sg_io_hdr_t io_hdr;
    int res;

    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = cmdlen;
    io_hdr.cmdp = wrCmd;
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = buflen;
    io_hdr.dxferp = buff;
    io_hdr.mx_sb_len = sizeof(RequestSense_T);
    io_hdr.sbp = RequestSense;
    io_hdr.timeout = sg_timeout;
    io_hdr.pack_id = pack_id++;
    if (diop && *diop)
        io_hdr.flags |= SG_FLAG_DIRECT_IO;

    while (((res = write(sg_fd, &io_hdr, sizeof(io_hdr))) < 0) &&
           (EINTR == errno))
        ;
    if (res < 0) {
        if (ENOMEM == errno)
            return 1;
        perror("writing (wr) on sg device, error");
        return -1;
    }

    while (((res = read(sg_fd, &io_hdr, sizeof(io_hdr))) < 0) &&
           (EINTR == errno))
        ;
    if (res < 0) {
        perror("writing (rd) on sg device, error");
        return -1;
    }
    switch (sg_err_category3(&io_hdr)) {
  case SG_ERR_CAT_CLEAN:
        break;
    case SG_ERR_CAT_RECOVERED:
        fprintf(stderr, "Recovered error while writing\n");
        break;
    case SG_ERR_CAT_MEDIA_CHANGED:
        return 2;
    default:
        sg_chk_n_print3("writing", &io_hdr);
        return -1;
    }
    if (diop && *diop &&
        ((io_hdr.info & SG_INFO_DIRECT_IO_MASK) != SG_INFO_DIRECT_IO))
        *diop = 0;      /* flag that dio not done (completely) */
    return 0;
}

/* -1 -> unrecoverable error, 0 -> successful, 1 -> recoverable (ENOMEM),
   2 -> try again */
int sg_read(int sg_fd, unsigned char *rdCmd, unsigned char * buff, int cmdlen, int buflen,
            int bs, int * diop,  RequestSense_T *RequestSense)
{

    sg_io_hdr_t io_hdr;
    int res;

    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = cmdlen;
    io_hdr.cmdp = rdCmd;
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = buflen;
    io_hdr.dxferp = buff;
    io_hdr.mx_sb_len = sizeof(RequestSense_T);
    io_hdr.sbp = RequestSense;
    io_hdr.timeout = sg_timeout;
    io_hdr.pack_id = pack_id++;
    if (diop && *diop)
        io_hdr.flags |= SG_FLAG_DIRECT_IO;

    while (((res = write(sg_fd, &io_hdr, sizeof(io_hdr))) < 0) &&
           (EINTR == errno))
        ;
    if (res < 0) {
        if (ENOMEM == errno)
            return 1;
        perror("reading (wr) on sg device, error");
        return -1;
    }

    while (((res = read(sg_fd, &io_hdr, sizeof(io_hdr))) < 0) &&
           (EINTR == errno))
        ;
    if (res < 0) {
        perror("reading (rd) on sg device, error");
        return -1;
    }
    switch (sg_err_category3(&io_hdr)) {
    case SG_ERR_CAT_CLEAN:
        break;
    case SG_ERR_CAT_RECOVERED:
        fprintf(stderr, "Recovered error while reading\n");
        break;
    case SG_ERR_CAT_MEDIA_CHANGED:
        return 2;
    default:
        sg_chk_n_print3("reading", &io_hdr);
        return -1;
    }
    if (diop && *diop &&
        ((io_hdr.info & SG_INFO_DIRECT_IO_MASK) != SG_INFO_DIRECT_IO))
        *diop = 0;      /* flag that dio not done (completely) */

#if SG_DEBUG
    fprintf(stderr, "duration=%u ms\n", io_hdr.duration);
#endif
    return 0;
}

/* Use the new SG_IO structure */
int SCSI_ExecuteCommand(DEVICE_TYPE DeviceFD,
			       Direction_T Direction,
			       CDB_T *CDB,
			       int CDB_Length,
			       void *DataBuffer,
			       int DataBufferLength,
			RequestSense_T *RequestSense) {
  int res;
  if (Direction==Input) {
    res=sg_read((int)DeviceFD, (unsigned char *)CDB, (unsigned char *)DataBuffer,
		   CDB_Length, DataBufferLength,
		   0, NULL,  RequestSense);
  }  else {
    res= sg_write((int)DeviceFD, (unsigned char *)CDB, (unsigned char *)DataBuffer,
		   CDB_Length, DataBufferLength,
		   0, NULL,  RequestSense);
  }
  SCSI_Default_Timeout();  /* reset back to default timeout, sigh. */
}

#else
  

/* Changed February 2000 by Eric Green <eric@estinc.com> to 
 * use the SCSI generic interface rather than SCSI_IOCTL_SEND_COMMAND
 * so that we can get more than PAGE_SIZE data....
 *
 * Note that the SCSI generic interface abuses READ and WRITE calls to serve
 * the same purpose as IOCTL calls, i.e., for "writes", the contents of the
 * buffer that you send as the argument to the write() call are actually
 * altered to fill in result status and sense data (if needed). 
 * Also note that this brain-dead interface does not have any sort of 
 * provisions for expanding the sg_header struct in a backward-compatible
 * manner. This sucks. But sucks less than SCSI_IOCTL_SEND_COMMAND, sigh.
 */


#ifndef OLD_EXECUTE_COMMAND_STUFF

static void slow_memcopy(unsigned char *src, unsigned char *dest, int numbytes)
{
  while (numbytes--) {
    *dest++ = *src++;
  }
}


int SCSI_ExecuteCommand(DEVICE_TYPE DeviceFD,
			       Direction_T Direction,
			       CDB_T *CDB,
			       int CDB_Length,
			       void *DataBuffer,
			       int DataBufferLength,
			       RequestSense_T *RequestSense)
{

  unsigned char *Command=NULL;   /* the command data struct sent to them... */
  unsigned char *ResultBuf=NULL; /* the data we read in return...         */

  unsigned char *src;       /* for copying stuff, sigh. */
  unsigned char *dest;      /* for copy stuff, again, sigh. */

  int write_length = sizeof(struct sg_header)+CDB_Length;
  int i;                  /* a random index...          */
  int result;             /* the result of the write... */ 
  
  
  struct sg_header *Header; /* we actually point this into Command... */
  struct sg_header *ResultHeader; /* we point this into ResultBuf... */


  /* First, see if we need to set our SCSI timeout to something different */
  if (sg_timeout != SG_SCSI_DEFAULT_TIMEOUT) {
    /* if not default, set it: */
#ifdef DEBUG_TIMEOUT
    fprintf(stderr,"Setting timeout to %d\n", sg_timeout);
    fflush(stderr);
#endif
    if(ioctl(DeviceFD, SG_SET_TIMEOUT, &sg_timeout)) {
      FatalError("failed to set sg timeout - %m\n");
    }
  }

  if (Direction == Output) {  /* if we're writing, our length is longer... */
    write_length += DataBufferLength; 
  }
  
  /* allocate some memory... enough for the command plus the header +
   *  any other data that we may need here...
   */
  
  Command=(unsigned char *)xmalloc(write_length);
  Header = (struct sg_header *) Command;  /* make it point to start of buf */

  dest=Command; /* now to copy the CDB... from start of buffer,*/
  dest+= sizeof(struct sg_header); /* increment it past the header. */

  slow_memcopy((char *)CDB,dest,CDB_Length);

  /* if we are writing additional data, tack it on here! */
  if (Direction == Output) {
    dest += CDB_Length;
    slow_memcopy(DataBuffer,dest,DataBufferLength); /* copy to end of command */
  }
  /* Now to fill in the Header struct: */
  Header->reply_len=DataBufferLength+sizeof(struct sg_header);
#ifdef DEBUG
  fprintf(stderr,"sg:reply_len(sent)=%d\n",Header->reply_len);
#endif
  Header->twelve_byte = CDB_Length == 12; 
  Header->result = 0;
  Header->pack_len = write_length; /* # of bytes written... */
  Header->pack_id = 0;             /* not used              */
  Header->other_flags = 0;         /* not used.             */
  Header->sense_buffer[0]=0;      /* used? */

  /* Now to do the write... */
  result=write(DeviceFD,Command,write_length);
  
  /* Now to check the result :-(. */
  /* Note that we don't have any request sense here. So we have no
   * idea what's going on. 
   */ 
  if ( (result < 0) || (result != write_length) || Header->result ||
       Header->sense_buffer[0] ) {
#ifdef DEBUG_SCSI
    fprintf(stderr,"scsi:result=%d Header->result=%d Header->sense_buffer[0]=%d\n",
	    result,Header->result,Header->sense_buffer[0]);
#endif  
    /* we don't have any real sense data, sigh :-(. */
    if (Header->sense_buffer[0]) {
      /* well, I guess we DID have some! eep! copy the sense data! */
      slow_memcopy((char *)Header->sense_buffer,(char *)RequestSense,
		   sizeof(Header->sense_buffer));
    } else {
      dest=(unsigned char *)RequestSense;
      *dest=(unsigned char)Header->result; /* may chop, sigh... */
    }

    /* okay, now, we may or may not need to find a non-zero value to return.
     * For tape drives, we may get a BLANK_CHECK or MEDIUM_ERROR and find
     * that it's *STILL* a good read! Use the STILL_A_VALID_READ macro
     * that calls all those macros I cribbed from Richard. 
     */
    
    if (!STILL_A_VALID_READ(RequestSense)) {
      free(Command); /* zap memory leak, sigh */
      /* okay, find us a non-zero value to return :-(. */
      if (result) {
	return result;
      } else if (Header->result) {
	return Header->result;
      } else {
	return -1;  /* sigh */
      }
    } else {
      result=-1;
    }
  } else {
    result=0; /* we're okay! */
  }
    
  
  /* now to allocate the new block.... */
  ResultBuf=(unsigned char *)xmalloc(Header->reply_len);
  /* now to clear ResultBuf... */
  slow_bzero(ResultBuf,Header->reply_len); 

  ResultHeader=(struct sg_header *)ResultBuf;
  
  /* copy the original Header... */
  ResultHeader->result=0;
  ResultHeader->pack_id=0;
  ResultHeader->other_flags=0;
  ResultHeader->reply_len=Header->reply_len;
  ResultHeader->twelve_byte = CDB_Length == 12; 
  ResultHeader->pack_len = write_length; /* # of bytes written... */
  ResultHeader->sense_buffer[0]=0; /* whoops! Zero that! */
#ifdef DEBUG
  fprintf(stderr,"sg:Reading %d bytes from DeviceFD\n",Header->reply_len);
  fflush(stderr);
#endif
  result=read(DeviceFD,ResultBuf,Header->reply_len);
#ifdef DEBUG
  fprintf(stderr,"sg:result=%d ResultHeader->result=%d\n",
	  result,ResultHeader->result);
  fflush(stderr);
#endif
  /* New: added check to see if the result block is still all zeros! */
  if ( (result < 0) || (result != Header->reply_len) || ResultHeader->result ||
       ResultHeader->sense_buffer[0] ) {
#ifdef DEBUG
    fprintf(stderr,
	    "scsi: result=%d Header->reply_len=%d ResultHeader->result=%d ResultHeader->sense_buffer[0]=%d\n",
	    result,
	    Header->reply_len,
	    ResultHeader->result,
	    ResultHeader->sense_buffer[0]);
#endif
    /* eep! copy the sense data! */
    slow_memcopy((char *)ResultHeader->sense_buffer,(char *)RequestSense,
		 sizeof(ResultHeader->sense_buffer));
    /* sense data copied, now find us a non-zero value to return :-(. */
    /* NOTE: Some commands return sense data even though they validly
     * executed! We catch a few of those with the macro STILL_A_VALID_READ.
     */

    if (!STILL_A_VALID_READ(RequestSense)) {
      free(Command);
      if (result) {
	free(ResultBuf);
	return result;
      } else if (ResultHeader->result) {
	free(ResultBuf);
	return ResultHeader->result;
      } else {
	free(ResultBuf);
	return -1; /* sigh! */
      } 
    } else {
      result=-1; /* if it was a valid read, still have -1 result. */
    }
  } else {
    result=0;
  }
  
  /* See if we need to reset our SCSI timeout */
  if (sg_timeout != SG_SCSI_DEFAULT_TIMEOUT) {
    sg_timeout = SG_SCSI_DEFAULT_TIMEOUT; /* reset it back to default */
#ifdef DEBUG_TIMEOUT
    fprintf(stderr,"Setting timeout to %d\n", sg_timeout);
    fflush(stderr);
#endif
    /* if not default, set it: */
    if(ioctl(DeviceFD, SG_SET_TIMEOUT, &sg_timeout)) {
      FatalError("failed to set sg timeout - %m\n");
    }
  }
  
  
  /* now for the crowning moment: copying any result into the DataBuffer! */
  /* (but only if it were an input command and not an output command :-}  */
  if (Direction == Input) {
#ifdef DEBUG
    fprintf(stderr,"Header->reply_len=%d,ResultHeader->reply_len=%d\n",
	    Header->reply_len,ResultHeader->reply_len);
#endif
    src=ResultBuf+sizeof(struct sg_header);
    dest=DataBuffer;
    for (i=0;i< (ResultHeader->reply_len);i++) {
      if (i>=DataBufferLength) break;  /* eep! */
      *dest++=*src++;
    }
  }
  
  /* and return! */
  free(Command);    /* clean up memory leak... */
  free(ResultBuf);
  return result; /* good stuff ! */
}

#endif  
#endif   /* #ifdef  SG_IO    */

#ifdef OLD_EXECUTE_COMMAND_STUFF

int SCSI_ExecuteCommand(int DeviceFD,
			       Direction_T Direction,
			       CDB_T CDB,
			       int CDB_Length,
			       void *DataBuffer,
			       int DataBufferLength,
			       RequestSense_T *RequestSense)
{
  unsigned char *Command;
  int Zero = 0, Result;
  memset(RequestSense, 0, sizeof(RequestSense_T));
  switch (Direction)
    {
    case Input:
      Command = (unsigned char *)
	xmalloc(8 + max(DataBufferLength, sizeof(RequestSense_T)));
      memcpy(&Command[0], &Zero, 4);
      memcpy(&Command[4], &DataBufferLength, 4);
      memcpy(&Command[8], CDB, CDB_Length);
      break;
    case Output:
      Command = (unsigned char *)
	xmalloc(8 + max(CDB_Length + DataBufferLength, sizeof(RequestSense_T)));
      memcpy(&Command[0], &DataBufferLength, 4);
      memcpy(&Command[4], &Zero, 4);
      memcpy(&Command[8], CDB, CDB_Length);
      memcpy(&Command[8 + CDB_Length], DataBuffer, DataBufferLength);
      break;
    }
  Result = ioctl(DeviceFD, SCSI_IOCTL_SEND_COMMAND, Command);
  if (Result != 0)
    memcpy(RequestSense, &Command[8], sizeof(RequestSense_T));
  else if (Direction == Input)
    memcpy(DataBuffer, &Command[8], DataBufferLength);
  free(Command);
  return Result;
}

#endif

