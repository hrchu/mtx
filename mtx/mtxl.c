/*  MTX -- SCSI Tape Attached Medium Changer Control Program

  Copyright 1997-1998 by Leonard N. Zubkoff <lnz@dandelion.com>

$Date$
$Revision$

  This file created Feb 2000 by Eric Lee Green <eric@estinc.com> from pieces
  extracted from mtx.c, plus some additional routines. 

  This program is free software; you may redistribute and/or modify it under
  the terms of the GNU General Public License Version 2 as published by the
  Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for complete details.
*/


/* FatalError: changed Feb. 2000 elg@estinc.com to eliminate a buffer
   overflow :-(. That could be important if mtxl is SUID for some reason. 
*/

#include "mtx.h"
#include "mtxl.h"

/* #define DEBUG_MODE_SENSE 1 */
/* #define DEBUG */
/* #define DEBUG_SCSI */

/* zap the following define when we finally add real import/export support */
#define IMPORT_EXPORT_HACK 1 /* for the moment, import/export == storage */

/* First, do some SCSI routines: */

/* the camlib is used on FreeBSD. */
#if HAVE_CAMLIB_H
#  include "scsi_freebsd.c"
#endif

/* the scsi_ctl interface is used on HP/UX. */
#if HAVE_SYS_SCSI_CTL_H
#  include "scsi_hpux.c"
#endif

/* the 'sg' interface is used on Linux. */
#if HAVE_SCSI_SG_H
#  include "scsi_linux.c"
#endif

/* The 'uscsi' interface is used on Solaris. */
#if HAVE_SYS_SCSI_IMPL_USCSI_H
#  include "scsi_sun.c"
#endif

/* The 'tm_buf' interface, as used on AIX. */
#ifdef HAVE_SYS_SCSI_H
#  include "scsi_aix.c"
#endif

/* The 'dslib' interface is used on SGI. */
#if HAVE_DSLIB_H
#  include "scsi_sgi.c"
#endif

/* Hmm, dunno what to do about Digital Unix at the moment. */
#ifdef DIGITAL_UNIX
#include "du/scsi.c"
#endif


#ifdef VMS
#include "[.vms]scsi.c"
#endif

extern char *argv0; /* something to let us do good error messages. */

/* Now for some low-level SCSI stuff: */

Inquiry_T *RequestInquiry(DEVICE_TYPE fd, RequestSense_T *RequestSense) {
  Inquiry_T *Inquiry;
  CDB_T CDB;

  Inquiry = (Inquiry_T *) xmalloc(sizeof(Inquiry_T));  
  
  CDB[0] = 0x12;		/* INQUIRY */
  CDB[1] = 0;			/* EVPD = 0 */
  CDB[2] = 0;			/* Page Code */
  CDB[3] = 0;			/* Reserved */
  CDB[4] = sizeof(Inquiry_T);	/* Allocation Length */
  CDB[5] = 0;			/* Control */

  /* set us a very short timeout, sigh... */
  SCSI_Set_Timeout(30); /* 30 seconds, sigh! */

  if (SCSI_ExecuteCommand(fd, Input, &CDB, 6,
			  Inquiry, sizeof(Inquiry_T), RequestSense) != 0)
    {
      free(Inquiry);
      return NULL;  /* sorry! */
    }
  return Inquiry;
}


int BigEndian16(unsigned char *BigEndianData)
{
  return (BigEndianData[0] << 8) + BigEndianData[1];
}


int BigEndian24(unsigned char *BigEndianData)
{
  return (BigEndianData[0] << 16) + (BigEndianData[1] << 8) + BigEndianData[2];
}





void FatalError(char *ErrorMessage, ...)
{
#define FORMAT_BUF_LEN 1024
  char FormatBuffer[FORMAT_BUF_LEN];
  char *SourcePointer;
  char *TargetPointer = FormatBuffer;
  int Character, LastCharacter = '\0';
  int numchars = 0;

  va_list ArgumentPointer;
  va_start(ArgumentPointer, ErrorMessage);
  /*  SourcePointer = "mtx: "; */
  sprintf(TargetPointer,"%s: ",argv0);
  numchars=strlen(TargetPointer);
  
  /* while ((Character = *SourcePointer++) != '\0') { */
  /*  *TargetPointer++ = Character;                   */
  /*  numchars++;                                     */
  /*   }                                              */
 
  
  while ((Character = *ErrorMessage++) != '\0')
    if (LastCharacter == '%')
      {
	if (Character == 'm')
	  {
	    SourcePointer = strerror(errno);
	    while ((Character = *SourcePointer++) != '\0') {
	      *TargetPointer++ = Character;
	      numchars++;
	      if (numchars==FORMAT_BUF_LEN-1) break;  
	    }
	    if (numchars==FORMAT_BUF_LEN-1) break;  /* break outer loop... */
	  }
	else
	  {
	    *TargetPointer++ = '%';
	    *TargetPointer++ = Character;
	    numchars++;
	    if (numchars==FORMAT_BUF_LEN-1) break;
	  }
	LastCharacter = '\0';
      }
    else
      {
	if (Character != '%') {
	  *TargetPointer++ = Character;
	  numchars++;
	  if (numchars==FORMAT_BUF_LEN-1) break;
	}
	LastCharacter = Character;
      }
  *TargetPointer = '\0';  /* works even if we had to break from above... */
  vfprintf(stderr, FormatBuffer, ArgumentPointer);
  va_end(ArgumentPointer);
#ifndef VMS
  exit(1);
#else
  sys$exit(VMS_ExitCode);
#endif
}

/* This is a really slow and stupid 'bzero' implementation'... */
void slow_bzero(char *buffer, int numchars) {
  while (numchars--) *buffer++ = 0;
}


/* malloc some memory while checking for out-of-memory conditions. */
void *xmalloc(size_t Size)
{
  void *Result = (void *) malloc(Size);
  if (Result == NULL)
    FatalError("cannot allocate %d bytes of memory\n", Size);
  return Result;
}

/* malloc some memory, zeroing it too: */
void *xzmalloc(size_t Size)
{
  void *Result=(void *)xmalloc(Size);
  
  slow_bzero(Result,Size);
  return Result;
}


int min(int x, int y)
{
  return (x < y ? x : y);
}


int max(int x, int y)
{
  return (x > y ? x : y);
}


/* Routine to inventory the library. Needed by, e.g., some Breece Hill
 * loaders. Sends an INITIALIZE_ELEMENT_STATUS command. This command
 * has no parameters, such as a range to scan :-(. 
 */

int Inventory(DEVICE_TYPE MediumChangerFD) {
  CDB_T CDB;
  RequestSense_T RequestSense;
 
  /* okay, now for the command: */
  CDB[0]=0x07; 
  CDB[1]=CDB[2]=CDB[3]=CDB[4]=CDB[5]=0;

  /* set us a very long timeout, sigh... */
  SCSI_Set_Timeout(30*60); /* 30 minutes, sigh! */
  
  if (SCSI_ExecuteCommand(MediumChangerFD,Input,&CDB,6,NULL,0,&RequestSense) != 0) {
    return -1;  /* could not do! */
  }
  return 0; /* did do! */
}

/* Routine to send an UNLOAD from the SSC spec to a device. This can be
 * used either to eject a tape (when sent to a tape drive), or to eject
 * a magzine (on some Seagate changers, when sent to LUN 1 ). 
 */

int Eject(DEVICE_TYPE fd) {
  CDB_T CDB;
  RequestSense_T RequestSense;
  /* okay, now for the command: */
  
  CDB[0]=0x1b;
  CDB[1]=CDB[2]=CDB[3]=CDB[4]=CDB[5]=0;
  
  if (SCSI_ExecuteCommand(fd,Input,&CDB,6,NULL,0,&RequestSense) != 0) {
    return -1;  /* could not do! */
  }
  return 0; /* did do! */
}  

/* Routine to read the Mode Sense Element Address Assignment Page */
/* We try to read the page. If we can't read the page, we return NULL.
 * Our caller really isn't too worried about why we could not read the
 * page, it will simply default to some kind of default values. 
 */ 
ElementModeSense_T *ReadAssignmentPage(DEVICE_TYPE MediumChangerFD) {
  CDB_T CDB;
  ElementModeSense_T *retval;  /* processed SCSI. */
  unsigned char input_buffer[136];
  ElementModeSensePage_T *sense_page; /* raw SCSI. */
  RequestSense_T RequestSense;       /* see what retval of raw SCSI was... */
  
  /* okay, now for the command: */
  CDB[0]=0x1a; /* Mode Sense(6) */
  CDB[1]=0; 
  CDB[2]=0x1d; /* Mode Sense Element Address Assignment Page */
  CDB[3]=0;
  CDB[4]=136; /* allocation_length... */
  CDB[5]=0;

  /* clear the data buffer: */
  slow_bzero((char *)&RequestSense,sizeof(RequestSense_T));
  slow_bzero((char *)input_buffer,sizeof(input_buffer));

  if (SCSI_ExecuteCommand(MediumChangerFD,Input,&CDB,6,
			  &input_buffer,sizeof(input_buffer),&RequestSense) != 0) {
#ifdef DEBUG_MODE_SENSE
    fprintf(stderr,"Could not execute mode sense...\n");
    fflush(stderr);
#endif
    return NULL; /* sorry, couldn't do it. */
  }

  /* Could do it, now build return value: */

#ifdef DEBUG_MODE_SENSE
  {
    int i;
    for (i=0;i<30;i+=3) {
      fprintf(stderr,"ib[%d]=%d ib[%d]=%d ib[%d]=%d\n",
	      i,input_buffer[i],i+1,input_buffer[i+1],
	      i+2,input_buffer[i+2]);
      /*  fprintf(stderr,"input_buffer[0]=%d input_buffer[3]=%d\n",
       *	  input_buffer[0], input_buffer[3]);
       */
    }
    fflush(stderr);
  }
#endif
  /* first, skip past: mode data header, and block descriptor header if any */
  sense_page=(ElementModeSensePage_T *)(input_buffer+4+input_buffer[3]);

#ifdef DEBUG_MODE_SENSE
  fprintf(stderr,"*sense_page=%x  %x\n",*((unsigned char *)sense_page),sense_page->PageCode);
  fflush(stderr);
#endif  

  retval=(ElementModeSense_T *) xzmalloc(sizeof(ElementModeSense_T));

  /* Remember that all SCSI values are big-endian: */
  retval->MediumTransportStart=(((int)sense_page->MediumTransportStartHi)<<8) +
    sense_page->MediumTransportStartLo;
  retval->NumMediumTransport=(((int)(sense_page->NumMediumTransportHi))<<8) +
    sense_page->NumMediumTransportLo;

  /* HACK! Some HP autochangers don't report NumMediumTransport right! */
  /* ARG! TAKE OUT THE %#@!# HACK! */
#ifdef STUPID_DUMB_IDIOTIC_HP_LOADER_HACK
  if (!retval->NumMediumTransport) {
    retval->NumMediumTransport=1;
  }
#endif

#ifdef DEBUG_MODE_SENSE
  fprintf(stderr,"rawNumStorage= %d %d    rawNumImportExport= %d %d\n",
	  sense_page->NumStorageHi,sense_page->NumStorageLo,
	  sense_page->NumImportExportHi, sense_page->NumImportExportLo);
  fprintf(stderr,"rawNumTransport=%d %d  rawNumDataTransfer=%d %d\n",
	  sense_page->NumMediumTransportHi,sense_page->NumMediumTransportLo,
	  sense_page->NumDataTransferHi,sense_page->NumDataTransferLo);
  fflush(stderr);
#endif

  retval->StorageStart=(((int)(sense_page->StorageStartHi))<<8) +
    sense_page->StorageStartLo;
  retval->NumStorage=(((int)(sense_page->NumStorageHi))<<8) + 
    sense_page->NumStorageLo;
  retval->ImportExportStart=(((int)(sense_page->ImportExportStartHi))<<8)+
    sense_page->ImportExportStartLo; 
  retval->NumImportExport=(((int)(sense_page->NumImportExportHi))<<8)+
    sense_page->NumImportExportLo;
  retval->DataTransferStart=(((int)(sense_page->DataTransferStartHi))<<8)+
    sense_page->DataTransferStartLo;
  retval->NumDataTransfer=(((int)(sense_page->NumDataTransferHi))<<8)+
    sense_page->NumDataTransferLo;

  /* allocate a couple spares 'cause some HP autochangers and maybe others
   * don't properly report the robotics arm(s) count here... 
   */
  retval->NumElements=retval->NumStorage+retval->NumImportExport+
    retval->NumDataTransfer+retval->NumMediumTransport;

  retval->MaxReadElementStatusData=(sizeof(ElementStatusDataHeader_T) +
				    4 * sizeof(ElementStatusPage_T) +
				    (retval->NumElements) *
				      sizeof(TransportElementDescriptor_T));


#ifdef IMPORT_EXPORT_HACK
  retval->NumStorage=retval->NumStorage+retval->NumImportExport;
#endif

#ifdef DEBUG_MODE_SENSE
  fprintf(stderr,"NumMediumTransport=%d\n",retval->NumMediumTransport);
  fprintf(stderr,"NumStorage=%d\n",retval->NumStorage);
  fprintf(stderr,"NumImportExport=%d\n",retval->NumImportExport);
  fprintf(stderr,"NumDataTransfer=%d\n",retval->NumDataTransfer);
  fprintf(stderr,"MaxReadElementStatusData=%d\n",retval->MaxReadElementStatusData);
  fprintf(stderr,"NumElements=%d\n",retval->NumElements);
  fflush(stderr);
#endif
  
  return retval;
}

static void FreeElementData(ElementStatus_T *data)
{
  free(data->DataTransferElementAddress);
  free(data->DataTransferElementSourceStorageElementNumber);
  free(data->DataTransferPrimaryVolumeTag);
  free(data->DataTransferAlternateVolumeTag);
  free(data->PrimaryVolumeTag);
  free(data->AlternateVolumeTag);
  free(data->StorageElementAddress);
  free(data->StorageElementIsImportExport);
  free(data->StorageElementFull);
  free(data->DataTransferElementFull);
}
  
       

/* allocate data  */

static ElementStatus_T *AllocateElementData(ElementModeSense_T *mode_sense) {
  ElementStatus_T *retval;

  retval=(ElementStatus_T *) xzmalloc(sizeof(ElementStatus_T));

  /* now for the invidual component arrays.... */

  retval->DataTransferElementAddress = (int *) xzmalloc(sizeof(int)*
                                    (mode_sense->NumDataTransfer + 1));
  retval->DataTransferElementSourceStorageElementNumber = 
    (int *) xzmalloc(sizeof(int)*(mode_sense->NumDataTransfer+1));
  retval->DataTransferPrimaryVolumeTag=
    (barcode *)xzmalloc(sizeof(barcode)*(mode_sense->NumDataTransfer+1));
  retval->DataTransferAlternateVolumeTag=
    (barcode *)xzmalloc(sizeof(barcode)*(mode_sense->NumDataTransfer+1));
  retval->PrimaryVolumeTag=
    (barcode *)xzmalloc(sizeof(barcode)*(mode_sense->NumStorage+1));
  retval->AlternateVolumeTag=
    (barcode *)xzmalloc(sizeof(barcode)*(mode_sense->NumStorage+1));
  retval->StorageElementAddress=
    (int *)xzmalloc(sizeof(int)*(mode_sense->NumStorage+1));
  retval->StorageElementIsImportExport=
    (boolean *)xzmalloc(sizeof(boolean)*(mode_sense->NumStorage+1));    
  retval->StorageElementFull=
    (boolean *)xzmalloc(sizeof(boolean)*(mode_sense->NumStorage+1));    
  retval->DataTransferElementFull=
    (boolean *)xzmalloc(sizeof(boolean)*(mode_sense->NumDataTransfer+1));
  
  return retval; /* sigh! */
}


void copy_barcode(unsigned char *src, unsigned char *dest) {
  int i;
  
  for (i=0; i < 37; i++) {
    *dest++ = *src++;
  }
  *dest=0; /* null-terminate, sigh. */ 
}


/* This #%!@# routine has more parameters than I can count! */
unsigned char *SendElementStatusRequest(DEVICE_TYPE MediumChangerFD,
					RequestSense_T *RequestSense,
					Inquiry_T *inquiry_info, 
					SCSI_Flags_T *flags,
					int ElementStart,
					int NumElements,
					int NumBytes
					) {

  CDB_T CDB;
  boolean is_attached = false;

  unsigned char *DataBuffer; /* size of data... */

#ifdef HAVE_GET_ID_LUN
  scsi_id_t *scsi_id;
#endif
  if ((inquiry_info->MChngr) && (inquiry_info->PeripheralDeviceType != MEDIUM_CHANGER_TYPE)) {
    is_attached=true;
  }
  if (flags->no_attached) { /* override, sigh */ 
    is_attached=false;
  }

  DataBuffer=(unsigned char *) xmalloc(NumBytes+1);

  slow_bzero((char *)RequestSense,sizeof(RequestSense_T));
#ifdef HAVE_GET_ID_LUN
  scsi_id = SCSI_GetIDLun(MediumChangerFD);
#endif


  CDB[0] = 0xB8;		/* READ ELEMENT STATUS */
  if (is_attached) {
    CDB[0] = 0xB4;  /* whoops, READ_ELEMENT_STATUS_ATTACHED! */ 
  }
#ifdef HAVE_GET_ID_LUN
  CDB[1] = (scsi_id->lun << 5) | ((flags->no_barcodes) ? 0 : 0x10) | flags->elementtype;  /* Lun + VolTag + Type code */
  free(scsi_id);
#else
  CDB[1] = ((flags->no_barcodes) ? 0 : 0x10) | flags->elementtype;		/* Element Type Code = 0, VolTag = 1 */
#endif
  CDB[2] = (ElementStart >> 8) & 0xff;	/* Starting Element Address MSB */
  CDB[3] = ElementStart & 0xff;		/* Starting Element Address LSB */


  CDB[4]= (NumElements >> 8) & 0xff;	  /* Number Of Elements MSB */
  CDB[5]= NumElements & 0xff ;        /* Number of elements LSB */

  /*  CDB[5]=127; */ /* test */
#ifdef DEBUG
  fprintf(stderr,"CDB[5]=%d\n" , CDB[5]);
#endif
  CDB[6] = 0;			/* Reserved */

  CDB[7]= (NumBytes >> 16) & 0xff; /* allocation length MSB */
  CDB[8]= (NumBytes >> 8) & 0xff;
  CDB[9]= NumBytes & 0xff;   /* allocation length LSB */

  CDB[10] = 0;			/* Reserved */
  CDB[11] = 0;			/* Control */

#ifdef DEBUG_BARCODE
  {
    int i;
    fprintf(stderr,"CDB= ");
    for (i=0;i<12;i++) {
      fprintf(stderr,"%x ",CDB[i]);
    }
    fprintf(stderr,"\n");
    fflush(stderr);
  }
#endif

  if (SCSI_ExecuteCommand(MediumChangerFD, Input, &CDB, 12,
			  DataBuffer,NumBytes, RequestSense) != 0){
    /* okay, first see if we have sense key of 'illegal request',
       additional sense code of '24', additional sense qualfier of 
       '0', and field in error of '4'. This means that we issued a request
       w/bar code reader and did not have one, thus must re-issue the request
       w/out barcode :-(.
    */
    
    /* Okay, most autochangers and tape libraries set a sense key here if
     * they do not have a bar code reader. For some reason, the ADIC DAT
     * uses a different sense key? Let's retry w/o bar codes for *ALL*
     * sense keys, sigh sigh sigh. 
     */
    
    if ( RequestSense->SenseKey > 1 ) {
      /* we issued a code requesting bar codes, there is no bar code reader? */
      /* clear out our sense buffer first... */
      slow_bzero((char *)RequestSense,sizeof(RequestSense_T));

      CDB[1] &= ~0x10; /* clear bar code flag! */

#ifdef DEBUG_BARCODE
      {
	int i;
	fprintf(stderr,"CDB= ");
	for (i=0;i<12;i++) {
	  fprintf(stderr,"%x ",CDB[i]);
	}
	fprintf(stderr,"\n");
	fflush(stderr);
      }
#endif
      
      if (SCSI_ExecuteCommand(MediumChangerFD, Input, &CDB, 12,
			      DataBuffer, NumBytes, RequestSense) != 0){
	free(DataBuffer);
	return NULL;
      }
    } else {
      free(DataBuffer);
      return NULL;
    }
  }

#ifdef DEBUG_BARCODE
  /* print a bunch of extra debug data :-(.  */
  PrintRequestSense(RequestSense); /* see what it sez :-(. */
  {
    int i;
    fprintf(stderr,"Data:");
    for (i=0;i<40;i++) {
      fprint(stderr,"%02x ",DataBuffer[i]);
    }
    fprintf(stderr,"\n");
  }
#endif  
  return DataBuffer; /* we succeeded! */
}

/******************* ParseElementStatus ***********************************/
/* This does the actual grunt work of parsing element status data. It fills
 * in appropriate pieces of its input data. It may be called multiple times
 * while we are gathering element status.
 */

static void ParseElementStatus(int *EmptyStorageElementAddress,
			       int *EmptyStorageElementCount,
			       unsigned char *DataBuffer,
			       ElementStatus_T *ElementStatus,
			       ElementModeSense_T *mode_sense

			       )   {
    
    unsigned char *DataPointer=DataBuffer;
    TransportElementDescriptor_T TEBuf;
    TransportElementDescriptor_T *TransportElementDescriptor;
    ElementStatusDataHeader_T *ElementStatusDataHeader;
    Element2StatusPage_T *ElementStatusPage;
    Element2StatusPage_T ESBuf;
    int ElementCount;
    int TransportElementDescriptorLength;
    int BytesAvailable; 
    
    ElementStatusDataHeader = (ElementStatusDataHeader_T *) DataBuffer;

    

  ElementStatusDataHeader = (ElementStatusDataHeader_T *) DataPointer;
  DataPointer += sizeof(ElementStatusDataHeader_T);
  ElementCount =
    BigEndian16(ElementStatusDataHeader->NumberOfElementsAvailable);
#ifdef DEBUG
      fprintf(stderr,"ElementCount=%d\n",ElementCount);
      fflush(stderr);
#endif
  while (ElementCount > 0)
    {
#ifdef DEBUG
      int got_element_num=0;
      
      fprintf(stderr,"Working on element # %d Element Count %d\n",got_element_num,ElementCount);
      got_element_num++;
#endif

      memcpy(&ESBuf, DataPointer, sizeof(ElementStatusPage_T));
      ElementStatusPage = &ESBuf;
      DataPointer += sizeof(ElementStatusPage_T);
      TransportElementDescriptorLength =
	BigEndian16(ElementStatusPage->ElementDescriptorLength);
      if (TransportElementDescriptorLength <
	  sizeof(TransportElementDescriptorShort_T)) {

	/* Foo, Storage Element Descriptors can be 4 bytes?! */
	if ( (ElementStatusPage->ElementTypeCode != MediumTransportElement &&
	      ElementStatusPage->ElementTypeCode != StorageElement &&
	      ElementStatusPage->ElementTypeCode != ImportExportElement ) ||
	     TransportElementDescriptorLength < 4) {
#ifdef DEBUG
	  fprintf(stderr,"boom! ElementTypeCode=%d\n",ElementStatusPage->ElementTypeCode);
#endif
	  FatalError("Transport Element Descriptor Length too short: %d\n",TransportElementDescriptorLength);
	} 
	
      }
#ifdef DEBUG
      fprintf(stderr,"Transport Element Descriptor Length=%d\n",TransportElementDescriptorLength);
#endif
      BytesAvailable =
	BigEndian24(ElementStatusPage->ByteCountOfDescriptorDataAvailable);
      while (BytesAvailable > 0)
	{
	  /* TransportElementDescriptor =
	     (TransportElementDescriptor_T *) DataPointer; */
          memcpy(&TEBuf, DataPointer, TransportElementDescriptorLength);
          TransportElementDescriptor = &TEBuf;

	  DataPointer += TransportElementDescriptorLength;
	  BytesAvailable -= TransportElementDescriptorLength;
	  ElementCount--;
	  switch (ElementStatusPage->ElementTypeCode)
	    {
	    case MediumTransportElement:
	      ElementStatus->TransportElementAddress = BigEndian16(TransportElementDescriptor->ElementAddress);
#ifdef DEBUG
	      fprintf(stderr,"TransportElementAddress=%d\n",ElementStatus->TransportElementAddress); 
#endif
	      break;
	      /* we treat ImportExport elements as if they were normal
	       * storage elements now, sigh...
	       */
	    case ImportExportElement:
	      ElementStatus->ImportExportCount++;
#ifdef DEBUG
	      fprintf(stderr,"ImportExportElement=%d\n",ElementStatus->StorageElementCount);
#endif
	      ElementStatus->StorageElementIsImportExport[ElementStatus->StorageElementCount] = 1;
	      /* WARNING: DO *NOT* PUT A 'break' STATEMENT HERE, it is supposed
	       * to run on into the case for a Storage Element! 
	       */
	    case StorageElement:
#ifdef DEBUG
	      fprintf(stderr,"StorageElementCount=%d  ElementAddress = %d ",ElementStatus->StorageElementCount,BigEndian16(TransportElementDescriptor->ElementAddress));
#endif
	      ElementStatus->StorageElementAddress[ElementStatus->StorageElementCount] =
		BigEndian16(TransportElementDescriptor->ElementAddress);
	      ElementStatus->StorageElementFull[ElementStatus->StorageElementCount] =
		TransportElementDescriptor->Full;
#ifdef DEBUG
	      fprintf(stderr,"TransportElement->Full = %d\n",TransportElementDescriptor->Full);
#endif
	      if (!TransportElementDescriptor->Full)
		{
		  EmptyStorageElementAddress[(*EmptyStorageElementCount)++] =
		    ElementStatus->StorageElementCount; /* slot idx. */
		    /*   ElementStatus->StorageElementAddress[ElementStatus->StorageElementCount]; */
		}
	      if ( (TransportElementDescriptorLength >  11) && 
		   (ElementStatusPage->VolBits & E2_AVOLTAG)) {
		copy_barcode(TransportElementDescriptor->AlternateVolumeTag,
			     ElementStatus->AlternateVolumeTag[ElementStatus->StorageElementCount]);
	      } else {
		ElementStatus->AlternateVolumeTag[ElementStatus->StorageElementCount][0]=0;  /* null string. */;
	      } 
	      if ( (TransportElementDescriptorLength > 11) && 
		   (ElementStatusPage->VolBits & E2_PVOLTAG)) {
		copy_barcode(TransportElementDescriptor->PrimaryVolumeTag,
			     ElementStatus->PrimaryVolumeTag[ElementStatus->StorageElementCount]);
	      } else {
		ElementStatus->PrimaryVolumeTag[ElementStatus->StorageElementCount][0]=0; /* null string. */
	      }
	      
	      ElementStatus->StorageElementCount++;
	      /* Note that the original mtx had no check here for 
		 buffer overflow, though some drives might mistakingly
		 do one... 
	      */ 
	      if (ElementStatus->StorageElementCount > mode_sense->NumStorage) {
		fprintf(stderr,"Warning:Too Many Storage Elements Reported (expected %d, now have %d\n",
			mode_sense->NumStorage,
			ElementStatus->StorageElementCount);
		fflush(stderr);
		return; /* we're done :-(. */
	      }


	      break;


	    case DataTransferElement:
	      /* tape drive not installed, go back to top of loop */

	      /* if (TransportElementDescriptor->Except) continue ; */

	      /* Note: This is for Exabyte tape libraries that improperly
		 report that they have a 2nd tape drive when they don't. We
		 could generalize this in an ideal world, but my attempt to
		 do so failed with dual-drive Exabyte tape libraries that
		 *DID* have the second drive. Sigh. 
	      */
	      /* No longer need this test due to code restructuring? */
	      /* if (TransportElementDescriptor->AdditionalSenseCode==0x83 && 
		  TransportElementDescriptor->AdditionalSenseCodeQualifier==0x04) 
		continue;
	      */ 
	      /* generalize it. Does it work? Let's try it! */
	      /* No, dammit, following does not work on dual-drive Exabyte
		 'cause if a tape is in the drive, it sets the AdditionalSense
		  code to something (sigh).
	      */
	      /* if (TransportElementDescriptor->AdditionalSenseCode!=0)
		continue;
	      */ 
	      

		  
	      ElementStatus->DataTransferElementAddress[ElementStatus->DataTransferElementCount] =
		BigEndian16(TransportElementDescriptor->ElementAddress);
	      ElementStatus->DataTransferElementFull[ElementStatus->DataTransferElementCount] = 
		TransportElementDescriptor->Full;
	      ElementStatus->DataTransferElementSourceStorageElementNumber[ElementStatus->DataTransferElementCount] =
		BigEndian16(TransportElementDescriptor
			    ->SourceStorageElementAddress);

	      if (ElementStatus->DataTransferElementCount >= mode_sense->NumDataTransfer) {
		FatalError("Too many Data Transfer Elements Reported\n");
	      }
	      if (ElementStatusPage->VolBits & E2_PVOLTAG) {
		copy_barcode(TransportElementDescriptor->PrimaryVolumeTag,
			     ElementStatus->DataTransferPrimaryVolumeTag[ElementStatus->DataTransferElementCount]);
	      } else {
		ElementStatus->DataTransferPrimaryVolumeTag[ElementStatus->DataTransferElementCount][0]=0; /* null string */
	      }
	      if (ElementStatusPage->VolBits & E2_AVOLTAG) {
		copy_barcode(TransportElementDescriptor->AlternateVolumeTag,
			     ElementStatus->DataTransferAlternateVolumeTag[ElementStatus->DataTransferElementCount]);
	      } else {
		ElementStatus->DataTransferAlternateVolumeTag[ElementStatus->DataTransferElementCount][0]=0; /* null string */
	      }
	      ElementStatus->DataTransferElementCount++;
	      /* 0 actually is a usable element address */
	      /* if (DataTransferElementAddress == 0) */
	      /*	FatalError( */
	      /*  "illegal Data Transfer Element Address %d reported\n", */
	      /* DataTransferElementAddress); */
	      break;
	    default:
	      FatalError("illegal Element Type Code %d reported\n",
			 ElementStatusPage->ElementTypeCode);
	    }
	}
    }
}

/********************* Real ReadElementStatus ********************* */

/*
 * We no longer do the funky trick to figure out ALLOCATION_LENGTH.
 * Instead, we use the SCSI Generic command rather than SEND_SCSI_COMMAND
 * under Linux, which gets around the @#%@ 4k buffer size in Linux. 
 * We still have the restriction that Linux cuts off the last two
 * bytes of the SENSE DATA (Q#@$%@#$^ Linux!). Which means that the
 * verbose widget won't work :-(. 
 
 * We now look for that "attached" bit in the inquiry_info to see whether
 * to use READ_ELEMENT_ATTACHED or plain old READ_ELEMENT. In addition, we
 * look at the device type in the inquiry_info to see whether it is a media
 * changer or tape device, and if it's a media changer device, we ignore the
 * attached bit (one beta tester found an old 4-tape DAT changer that set
 * the attached bit for both the tape device AND the media changer device). 

*/

ElementStatus_T *ReadElementStatus(DEVICE_TYPE MediumChangerFD, RequestSense_T *RequestSense, Inquiry_T *inquiry_info, SCSI_Flags_T *flags) {
  ElementStatus_T *ElementStatus;
  
  unsigned char *DataBuffer; /* size of data... */
  
  int EmptyStorageElementCount=0;
  int *EmptyStorageElementAddress; /* [MAX_STORAGE_ELEMENTS]; */
  
  int empty_idx=0;
  int invalid_sources=0;
  boolean is_attached = false;
  int i,j;
  
  ElementModeSense_T *mode_sense = NULL;
  
  if ((inquiry_info->MChngr) && (inquiry_info->PeripheralDeviceType != MEDIUM_CHANGER_TYPE)) {
    is_attached=true;
  }
  if (flags->no_attached) { /* override, sigh */ 
    is_attached=false;
  }
  if (!is_attached) {
    mode_sense=ReadAssignmentPage(MediumChangerFD);
  }
  if (!mode_sense) {
    mode_sense=(ElementModeSense_T *) xmalloc(sizeof(ElementModeSense_T));
    mode_sense->NumMediumTransport=MAX_TRANSPORT_ELEMENTS;
    mode_sense->NumStorage=MAX_STORAGE_ELEMENTS;
    mode_sense->NumDataTransfer=MAX_TRANSFER_ELEMENTS;
    mode_sense->MaxReadElementStatusData=  (sizeof(ElementStatusDataHeader_T)
					    + 3 * sizeof(ElementStatusPage_T)
					    + (MAX_STORAGE_ELEMENTS+MAX_TRANSFER_ELEMENTS+MAX_TRANSPORT_ELEMENTS)
					    * sizeof(TransportElementDescriptor_T));
    
    /* don't care about the others anyhow at the moment... */
  }
  
  ElementStatus=AllocateElementData(mode_sense);

  /* Now to initialize it (sigh).  */
  ElementStatus->StorageElementCount = 0;
  ElementStatus->DataTransferElementCount=0;
    
  
  /* first, allocate some empty storage stuff: Note that we pass this
   * down to ParseElementStatus (sigh!) 
   */
  
  EmptyStorageElementAddress=(int *)xzmalloc((mode_sense->NumStorage+1)*sizeof(int));
  for (i=0;i<mode_sense->NumStorage;i++) {
    EmptyStorageElementAddress[i]=-1;
  }
  
  /* Okay, now to send some requests for the various types of stuff: */

  /* -----------STORAGE ELEMENTS---------------- */
  /* Let's start with storage elements: */

  for (i=0;i<mode_sense->NumDataTransfer;i++) {
    /* initialize them to an illegal # so that we can fix later... */
    ElementStatus->DataTransferElementSourceStorageElementNumber[i] = -1; 
  }
  

  flags->elementtype=StorageElement; /* sigh! */
  DataBuffer=SendElementStatusRequest(MediumChangerFD,RequestSense,
				      inquiry_info,flags,
				      mode_sense->StorageStart,
				      /* adjust for import/export. */
				      mode_sense->NumStorage-mode_sense->NumImportExport,
				      mode_sense->MaxReadElementStatusData);
  if (!DataBuffer) {
    /* darn. Free up stuff and return. */
    /****FIXME**** do a free on element data! */
    FreeElementData(ElementStatus);
    return NULL; 
  } 

  ParseElementStatus(EmptyStorageElementAddress,&EmptyStorageElementCount,
		     DataBuffer,ElementStatus,mode_sense);

  free(DataBuffer); /* sigh! */

  /* --------------IMPORT/EXPORT--------------- */
  /* Next let's see if we need to do Import/Export: */
  if (mode_sense->NumImportExport > 0) {
    flags->elementtype=ImportExportElement;
    DataBuffer=SendElementStatusRequest(MediumChangerFD,RequestSense,
					inquiry_info,flags,
					mode_sense->ImportExportStart,
					mode_sense->NumImportExport,
					mode_sense->MaxReadElementStatusData);
    
    if (!DataBuffer) {
      /* darn. Free up stuff and return. */
      /****FIXME**** do a free on element data! */
      FreeElementData(ElementStatus);
      return NULL; 
    } 
    ParseElementStatus(EmptyStorageElementAddress,&EmptyStorageElementCount,
		     DataBuffer,ElementStatus,mode_sense);

  }
  


  /* ----------------- DRIVES ---------------------- */


  flags->elementtype=DataTransferElement; /* sigh! */
  DataBuffer=SendElementStatusRequest(MediumChangerFD,RequestSense,
				      inquiry_info,flags,
				      mode_sense->DataTransferStart,
				      mode_sense->NumDataTransfer,
				      mode_sense->MaxReadElementStatusData);
  if (!DataBuffer) {
    /* darn. Free up stuff and return. */
    /****FIXME**** do a free on element data! */
    FreeElementData(ElementStatus);
    return NULL; 
  } 

  ParseElementStatus(EmptyStorageElementAddress,&EmptyStorageElementCount,
		     DataBuffer,ElementStatus,mode_sense);

  free(DataBuffer); /* sigh! */


  /* ----------------- Robot Arm(s) -------------------------- */

    /* grr, damned brain dead HP doesn't report that it has any! */
  if (!mode_sense->NumMediumTransport) { 
    ElementStatus->TransportElementAddress=0; /* default it sensibly :-(. */
  } else {
     flags->elementtype=MediumTransportElement; /* sigh! */
     DataBuffer=SendElementStatusRequest(MediumChangerFD,RequestSense,
				      inquiry_info,flags,
				      mode_sense->MediumTransportStart,
				      1, /* only get 1, sigh. */
				      mode_sense->MaxReadElementStatusData);
     if (!DataBuffer) {
       /* darn. Free up stuff and return. */
       /****FIXME**** do a free on element data! */
       FreeElementData(ElementStatus);
       return NULL; 
     } 
   
     ParseElementStatus(EmptyStorageElementAddress,&EmptyStorageElementCount,
   		     DataBuffer,ElementStatus,mode_sense);

     free(DataBuffer); /* sigh! */
  }

  /*---------------------- Sanity Checking ------------------- */

  if (!ElementStatus->DataTransferElementCount)
    FatalError("no Data Transfer Element reported\n");
  if (ElementStatus->StorageElementCount == 0)
    FatalError("no Storage Elements reported\n");


  /* ---------------------- Reset SourceStorageElementNumbers ------- */

  /* okay, re-write the SourceStorageElementNumber code  *AGAIN*.
   * Pass1:
   *   Translate from raw element # to our translated # (if possible).
   * First, check the SourceStorageElementNumbers against the list of 
   * filled slots. If the slots indicated are empty, we accept that list as
   * valid. Otherwise decide the SourceStorageElementNumbers are invalid.
   *
   * Pass2:
   *
   * If we had some invalid (or unknown) SourceStorageElementNumbers
   * then we must search for free slots, and assign SourceStorageElementNumbers
   * to those free slots. We happen to already built a list of free
   * slots as part of the process of reading the storage element numbers
   * from the tape. So that's easy enough to do! 
   */

#ifdef DEBUG_TAPELIST
  fprintf(stderr,"empty slots: %d\n",EmptyStorageElementCount);
  if (EmptyStorageElementCount) {
     for (i=0; i<EmptyStorageElementCount; i++) {
         fprintf(stderr,"empty: %d\n",EmptyStorageElementAddress[i]);
     }
  }
#endif

  /* Okay, now we re-assign origin slots if the "real" origin slot
   * is obviously defective: 
   */
  /* pass one: */
  invalid_sources=0; /* no invalid sources yet! */
  for (i=0;i<ElementStatus->DataTransferElementCount;i++) {
    int elnum;
    int translated_elnum = -1;
    /* if we have an element, then ... */
    if (ElementStatus->DataTransferElementFull[i]) {
      elnum=ElementStatus->DataTransferElementSourceStorageElementNumber[i];
      /* if we have an element number, then ... */
      if (elnum >= 0) {
         /* Now to translate the elnum: */
	 for (j=0; j<ElementStatus->StorageElementCount; j++) {
	     if (elnum == ElementStatus->StorageElementAddress[j]) {
		 translated_elnum=j; 
	     }
	 }
      /* now see if the element # is already occupied: */
	if (ElementStatus->StorageElementFull[translated_elnum]) {
	  invalid_sources=1;
	  break; /* break out of the loop! */
	} else {
	   /* properly set the source... */
	ElementStatus->DataTransferElementSourceStorageElementNumber[i]=
		translated_elnum;
	}
		 
      } else {
	/* if element # was not >=0, then we had an invalid source anyhow! */
	invalid_sources=1;
      }
    }
  }

#ifdef DEBUG_TAPELIST
   fprintf(stderr,"invalid_sources=%d\n",invalid_sources);
#endif

  /* Pass2: */
  /* okay, we have invalid sources, so let's see what they should be: */
  /* Note: If EmptyStorageElementCount is < # of drives, the leftover
   * drives will be assigned a -1 (see the initialization loop for
   * EmptyStorageElementAddress above), which will be reported as "slot 0"
   * by the user interface. This is an invalid value, but more useful for us
   * to have than just crapping out here :-(. 
   */ 
  if (invalid_sources) {
    empty_idx=0;
    for (i=0;i<ElementStatus->DataTransferElementCount;i++) {
      if (ElementStatus->DataTransferElementFull[i]) {
#ifdef DEBUG_TAPELIST
        fprintf(stderr,"for drive %d, changing source %d to %d (empty slot #%d)\n",
	     i,
	     ElementStatus->DataTransferElementSourceStorageElementNumber[i],
             EmptyStorageElementAddress[empty_idx],
             empty_idx);
#endif
              
	ElementStatus->DataTransferElementSourceStorageElementNumber[i]=
	  EmptyStorageElementAddress[empty_idx++];
      }
    }
  }

  /* and done! */      
  free(mode_sense);
  free(EmptyStorageElementAddress);
  return ElementStatus;
  
}

/*************************************************************************/



/* Now the actual media movement routine! */
RequestSense_T *MoveMedium(DEVICE_TYPE MediumChangerFD, int SourceAddress,
		       int DestinationAddress, 
		       ElementStatus_T *ElementStatus,
		       Inquiry_T *inquiry_info, SCSI_Flags_T *flags)
{
  RequestSense_T *RequestSense = xmalloc(sizeof(RequestSense_T));
  CDB_T CDB;
  if ((inquiry_info->MChngr) && (inquiry_info->PeripheralDeviceType != MEDIUM_CHANGER_TYPE)) {  /* if using the ATTACHED API */
    CDB[0]=0xA7;   /* MOVE_MEDIUM_ATTACHED */
  } else {
    CDB[0] = 0xA5;		/* MOVE MEDIUM */
  }
  CDB[1] = 0;			/* Reserved */
  CDB[2] = (ElementStatus->TransportElementAddress >> 8) & 0xFF;  /* Transport Element Address MSB */
  CDB[3] = (ElementStatus->TransportElementAddress) & 0xff;   /* Transport Element Address LSB */
  CDB[4] = (SourceAddress >> 8) & 0xFF;	/* Source Address MSB */
  CDB[5] = SourceAddress & 0xFF; /* Source Address MSB */
  CDB[6] = (DestinationAddress >> 8) & 0xFF; /* Destination Address MSB */
  CDB[7] = DestinationAddress & 0xFF; /* Destination Address MSB */
  CDB[8] = 0;			/* Reserved */
  CDB[9] = 0;			/* Reserved */
  if (flags->invert) {
    CDB[10] = 1;			/* Reserved */
  } else {
    CDB[10] = 0;
  }
  /* eepos controls the tray for import/export elements, sometimes. */
  CDB[11] = 0 | (flags->eepos <<6);			/* Control */
  
  if (SCSI_ExecuteCommand(MediumChangerFD, Output, &CDB, 12,
			  NULL, 0, RequestSense) != 0) {
    return RequestSense;
  }
  free(RequestSense);
  return NULL; /* success! */
}

/* for Linux, this creates a way to do a short erase... the @#$%@ st.c
 * driver defaults to doing a long erase!
 */

RequestSense_T *Erase(DEVICE_TYPE MediumChangerFD) {
  RequestSense_T *RequestSense = xmalloc(sizeof(RequestSense_T));
  CDB_T CDB;

  CDB[0]=0x19;
  CDB[1]=0;  /* Short! */
  CDB[2]=CDB[3]=CDB[4]=CDB[5]=0;

  if (SCSI_ExecuteCommand(MediumChangerFD, Output, &CDB, 6,
			  NULL, 0, RequestSense) != 0) {
    return RequestSense;
  }
  free(RequestSense);
  return NULL;  /* Success! */
}  


#ifdef LONG_PRINT_REQUEST_SENSE

static char *sense_keys[] = {
  "No Sense",  /* 00 */
  "Recovered Error", /* 01 */
  "Not Ready",  /* 02 */
  "Medium Error",  /* 03 */
  "Hardware Error",  /* 04 */
  "Illegal Request",  /* 05 */
  "Unit Attention", /* 06 */
  "Data Protect",   /* 07 */
  "Blank Check", /* 08 */
  "0x09",   /* 09 */
  "0x0a",   /* 0a */
  "Aborted Command", /* 0b */
  "0x0c",  /* 0c */
  "Volume Overflow", /* 0d */
  "Miscompare",   /* 0e */
  "0x0f"  /* 0f */
};

static char Yes[]="yes";
static char No[]="no";

void PrintRequestSense(RequestSense_T *RequestSense)
{
  char *msg;

  fprintf(stderr, "mtx: Request Sense: Long Report=yes\n");
  fprintf(stderr, "mtx: Request Sense: Valid Residual=%s\n", RequestSense->Valid ? Yes : No);
  if (RequestSense->ErrorCode==0x70) { 
    msg = "Current" ;
  } else if (RequestSense->ErrorCode==0x71) { 
    msg = "Deferred" ;
  } else {
    msg = "Unknown?!" ;
  }
  fprintf(stderr, "mtx: Request Sense: Error Code=%0x (%s)\n",RequestSense->ErrorCode,msg);
  fprintf(stderr, "mtx: Request Sense: Sense Key=%s\n",sense_keys[RequestSense->SenseKey]);
  fprintf(stderr, "mtx: Request Sense: FileMark=%s\n", RequestSense->Filemark ? Yes : No);
  fprintf(stderr, "mtx: Request Sense: EOM=%s\n", RequestSense->EOM ? Yes : No);
  fprintf(stderr, "mtx: Request Sense: ILI=%s\n", RequestSense->ILI ? Yes : No);
  if (RequestSense->Valid) {
    fprintf(stderr, "mtx: Request Sense: Residual = %02X %02X %02X %02X\n",RequestSense->Information[0],RequestSense->Information[1],RequestSense->Information[2],RequestSense->Information[3]);
  } 
  fprintf(stderr,"mtx: Request Sense: Additional Sense Code = %02X\n", RequestSense->AdditionalSenseCode);
  fprintf(stderr,"mtx: Request Sense: Additional Sense Qualifier = %02X\n", RequestSense->AdditionalSenseCodeQualifier);
  if (RequestSense->SKSV) {
    fprintf(stderr,"mtx: Request Sense: Field in Error = %02X\n",RequestSense->BitPointer);
  }
  fprintf(stderr,"mtx: Request Sense: BPV=%s\n",RequestSense->BPV ? Yes : No);
  fprintf(stderr,"mtx: Request Sense: Error in CDB=%s\n",RequestSense->CommandData ? Yes : No);
  fprintf(stderr,"mtx: Request Sense: SKSV=%s\n",RequestSense->SKSV ? Yes : No);
  fflush(stderr);
  if (RequestSense->BPV || RequestSense -> SKSV) {
    fprintf(stderr,"mtx: Request Sense: Field Pointer = %02X %02X\n",RequestSense->FieldData[0],RequestSense->FieldData[1]);
  }
}


#else
void PrintRequestSense(RequestSense_T *RequestSense)
{
  int i;
  fprintf(stderr, "mtx: Request Sense: %02X",
	  ((unsigned char *) RequestSense)[0]);
  for (i = 1; i < sizeof(RequestSense_T); i++)
    fprintf(stderr, " %02X", ((unsigned char *) RequestSense)[i]);
  fprintf(stderr, "\n");
}

#endif

/* $Date$
 * $Log$
 * Revision 1.8  2001/06/25 23:06:22  elgreen
 * Readying this for 1.2.13 release
 *
 * Revision 1.7  2001/06/25 04:56:35  elgreen
 * Kai to the rescue *again* :-)
 *
 * Revision 1.6  2001/06/24 07:02:01  elgreen
 * Kai's fix was better than mine.
 *
 * Revision 1.5  2001/06/24 06:59:19  elgreen
 * Kai found bug in the barcode backoff.
 *
 * Revision 1.4  2001/06/15 18:56:54  elgreen
 * Arg, it doesn't help to check for 0 robot arms if you force it to 1!
 *
 * Revision 1.3  2001/06/15 14:26:09  elgreen
 * Fixed brain-dead case of HP loaders that report they have no robot arms.
 *
 * Revision 1.2  2001/06/09 17:26:26  elgreen
 * Added 'nobarcode' command to mtx (to skip the initial request asking for
 * barcodes for mtx status purposes).
 *
 * Revision 1.1.1.1  2001/06/05 17:10:25  elgreen
 * Initial import into SourceForge
 *
 * Revision 1.29  2001/05/01 01:39:23  eric
 * Remove the Exabyte special case code, which seemed to be barfing me :-(.
 *
 * Revision 1.28  2001/04/28 00:03:10  eric
 * update for 1.2.12.
 *
 * Revision 1.27  2001/04/18 19:27:39  eric
 * whoops, move ImportExport parser into the $#%!@ ImportExport test :-(.
 *
 * Revision 1.26  2001/04/18 18:55:44  eric
 * turn too many storage elements into a warning rather than a fatal error (sigh!)
 *
 * Revision 1.25  2001/04/18 18:46:54  eric
 * $%!@# "C" precedence rules.
 *
 * Revision 1.24  2001/04/18 17:58:04  eric
 * initialize EmptyStorageElementCount (sigh)
 *
 * Revision 1.23  2001/04/18 16:55:03  eric
 * hopefully get the initialization for element status straigtened out.
 *
 * Revision 1.22  2001/04/18 16:32:59  eric
 * Cleaned up all -Wall messages.
 *
 * Revision 1.21  2001/04/18 16:08:08  eric
 * after re-arranging & fixing bugs
 *
 * Revision 1.20  2001/04/17 21:28:43  eric
 * mtx 1.2.11
 *
 * Revision 1.19  2001/04/02 20:12:48  eric
 * checkin, mtx 1.2.11pre6
 *
 * Revision 1.18  2001/03/06 01:37:05  eric
 * Solaris changes
 *
 * Revision 1.17  2001/02/26 20:56:12  eric
 * added debugging, removed Makefile
 *
 * Revision 1.16  2001/02/19 23:22:28  eric
 * Add E2 bits
 *
 * Revision 1.15  2001/02/19 23:02:04  eric
 * Apply SPARC patch.
 *
 * Revision 1.14  2001/01/26 15:58:10  eric
 * Make work on solaris?
 *
 * Revision 1.13  2001/01/17 19:39:01  eric
 * Changed mtx.h and mtxl.c to use autoconf stuff to figure out which SCSI
 * routines to use (this way, if we ever port to an OS that has an identical
 * SCSI interface to one of the supported OS's, we don't have to do anything
 * special!).
 *
 * Revision 1.12  2001/01/17 16:36:26  eric
 * added date & revision strings as needed
 *
 * Revision 1.11  2000/11/30 20:35:18  eric
 * Added Erase command.
 *
 */