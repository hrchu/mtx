/* Copyright 2001 Enhanced Software Technologies Inc.
 *   Released under terms of the GNU General Public License as
 * required by the license on 'mtxl.c'.
 * $Date: 2007-01-28 19:23:33 -0800 (Sun, 28 Jan 2007) $
 * $Revision: 125 $
 */

/* This is a generic SCSI device control program. It operates by
 * directly sending commands to the device.
 */

/*#define DEBUG_PARTITION */
/*#define DEBUG 1 */

/* 
   Commands:
         load -- Load medium
         unload -- Unload medium
         start -- Start device
         stop -- Stop device
         lock -- Lock medium
         unlock -- Unlock medium

 */

#include <stdio.h>
#include <string.h>

#include "mtx.h"
#include "mtxl.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

void Usage(void) {
  FatalError("Usage: scsieject -f <generic-device> <command> where <command> is:\n load | unload | start | stop | lock | unlock\n");
}

#define arg1 (arg[0])  /* for backward compatibility, sigh */
static int arg[4];  /* the argument for the command, sigh. */

/* the device handle we're operating upon, sigh. */
static char *device;  /* the text of the device thingy. */
static DEVICE_TYPE MediumChangerFD = (DEVICE_TYPE) -1;



static int S_load(void);
static int S_unload(void);
static int S_start(void);
static int S_stop(void);
static int S_lock(void);
static int S_unlock(void);

struct command_table_struct {
  int num_args;
  char *name;
  int (*command)(void);
} command_table[] = {
  { 0, "load", S_load },
  { 0, "unload", S_unload },
  { 0, "start", S_start },
  { 0, "stop", S_stop },
  { 0, "lock", S_lock },
  { 0, "unlock", S_unlock },
  { 0, NULL, NULL } /* terminate list */
};

char *argv0;


/* open_device() -- set the 'fh' variable.... */
void open_device(void) {

  if (MediumChangerFD != -1) {
    SCSI_CloseDevice("Unknown",MediumChangerFD);  /* close it, sigh...  new device now! */
  }

  MediumChangerFD = SCSI_OpenDevice(device);

}

static int get_arg(char *arg) {
  int retval=-1;

  if (*arg < '0' || *arg > '9') {
    return -1;  /* sorry! */
  }

  retval=atoi(arg);
  return retval;
}


/* we see if we've got a file open. If not, we open one :-(. Then
 * we execute the actual command. Or not :-(. 
 */ 
int execute_command(struct command_table_struct *command) {

  /* if the device is not already open, then open it from the 
   * environment.
   */
  if (MediumChangerFD == -1) {
    /* try to get it from STAPE or TAPE environment variable... */
    device=getenv("STAPE");
    if (device==NULL) {
      device=getenv("TAPE");
      if (device==NULL) {
	Usage();
      }
    }
    open_device();
  }


  /* okay, now to execute the command... */
  return command->command();
}


/* parse_args():
 *   Basically, we are parsing argv/argc. We can have multiple commands
 * on a line now, such as "unload 3 0 load 4 0" to unload one tape and
 * load in another tape into drive 0, and we execute these commands one
 * at a time as we come to them. If we don't have a -f at the start, we
 * barf. If we leave out a drive #, we default to drive 0 (the first drive
 * in the cabinet). 
 */ 

int parse_args(int argc,char **argv) {
  int i,cmd_tbl_idx,retval,arg_idx;
  struct command_table_struct *command;

  i=1;
  arg_idx=0;
  while (i<argc) {
    if (strcmp(argv[i],"-f") == 0) {
      i++;
      if (i>=argc) {
	Usage();
      }
      device=argv[i++];
      open_device(); /* open the device and do a status scan on it... */
    } else {
      cmd_tbl_idx=0;
      command=&command_table[0]; /* default to the first command... */
      command=&command_table[cmd_tbl_idx];
      while (command->name) {
	if (!strcmp(command->name,argv[i])) {
	  /* we have a match... */
	  break;
	}
	/* otherwise we don't have a match... */
	cmd_tbl_idx++;
	command=&command_table[cmd_tbl_idx];
      }
      /* if it's not a command, exit.... */
      if (!command->name) {
	Usage();
      }
      i++;  /* go to the next argument, if possible... */
      /* see if we need to gather arguments, though! */
      arg1=-1; /* default it to something */
      for (arg_idx=0;arg_idx < command->num_args ; arg_idx++) {
	if (i < argc) {
	  arg[arg_idx]=get_arg(argv[i]);
	  if (arg[arg_idx] !=  -1) {
	    i++; /* increment i over the next cmd. */
	  }
	} else {
	  arg[arg_idx]=0; /* default to 0 setmarks or whatever */
	} 
      }
      retval=execute_command(command);  /* execute_command handles 'stuff' */
      exit(retval);
    }
  }
  return 0; /* should never get here */
}

/* For tapes this is used to load a tape.
 * For CD/DVDs this is used to load a disc which is required by
 * some media changers.
 */

int S_load(void)
{
  int i = LoadUnload(MediumChangerFD, 1);

  if (i < 0) {
    fprintf(stderr,"scsieject: load failed\n");
    fflush(stderr);
  }

  return i;
}

/* This should eject a tape or magazine, depending upon the device sent
 * to.
 */
int S_unload(void)
{
  int i = LoadUnload(MediumChangerFD, 0);

  if (i < 0) {
    fprintf(stderr,"scsieject: unload failed\n");
    fflush(stderr);
  }

  return i;
}

int S_start(void)
{
  int i = StartStop(MediumChangerFD, 1);

  if (i < 0) {
    fprintf(stderr,"scsieject: start failed\n");
    fflush(stderr);
  }

  return i;
}

int S_stop(void)
{
  int i = StartStop(MediumChangerFD, 0);

  if (i < 0) {
    fprintf(stderr,"scsieject: stop failed\n");
    fflush(stderr);
  }

  return i;
}

int S_lock(void)
{
  int i = LockUnlock(MediumChangerFD, 1);

  if (i < 0) {
    fprintf(stderr,"scsieject: lock failed\n");
    fflush(stderr);
  }

  return i;
}

int S_unlock(void)
{
  int i = LockUnlock(MediumChangerFD, 0);

  if (i < 0) {
    fprintf(stderr,"scsieject: unlock failed\n");
    fflush(stderr);
  }

  return i;
}

/* See parse_args for the scoop. parse_args does all. */
int main(int argc, char **argv) {
  argv0=argv[0];
  parse_args(argc,argv);

  if (device) 
    SCSI_CloseDevice(device,MediumChangerFD);

  exit(0);
}
