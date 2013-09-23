/************************************************************************

  Demo for  TimeHarp 200   Software v5.0   T3R File Access

  Michael Wahl, PicoQuant GmbH, January 2003

  NEW: - Special records for external markers
       - Time Tag overflow is encoded in a special record

  Tested with the following compilers:

  - MinGW 2.0.0-3 (free compiler for Win 32 bit)
  - Lcc-win32 3.8 (free compiler for Win 32 bit)
  - DJGPP 2.03 (free compiler for DOS prot. mode 32 bit)
  - MS Visual C++ 4.0/5.0/6.0 (Win 32 bit)
  - Borland C++ 5.3 / C++Builder 3.0 (Win 32 bit)
  - gcc (x86 Linux 32 bit)
  - Turbo C 2.0 (DOS 16 bit)

  it should work with most others.

************************************************************************/

#include <stdio.h>
#include <string.h>

/*
The following structures are used to hold the TimeHarp file data
They reflect the file structure.
The data types used here to match the file structure are correct
for MSVC++. They may have to be changed for other compilers.
*/

typedef struct{ long int Start;
                long int Step;
				long int End;  } tParamStruct;

typedef struct{ long int MapTo;
				long int Show; } tCurveMapping;

/* The following represents the readable ASCII file header portion} */

struct {
	char Ident[16];
	char SoftwareVersion[6];
	char HardwareVersion[6];
	char FileTime[18];
	char CRLF[2];
	char CommentField[256]; } TxtHdr;

/* The following is binary header information */

struct {
	long int Channels;
	long int Curves;
	long int BitsPerChannel;
	long int RoutingChannels;
	long int NumberOfBoards;
	long int ActiveCurve;
	long int MeasMode;
	long int SubMode;
	long int RangeNo;
	long int Offset;			/* in ns */
	long int Tacq;				/* in ms */
	long int StopAt;
	long int StopOnOvfl;
	long int Restart;
	long int DispLinLog;
	long int DispTimeFrom;
	long int DispTimeTo;
	long int DispCountsFrom;
	long int DispCountsTo;
	tCurveMapping DispCurves[8];
	tParamStruct Params[3];
	long int RepeatMode;
	long int RepeatsPerCurve;
	long int RepeatTime;
	long int RepeatWaitTime;
	char ScriptName[20];} BinHdr;

struct {
	long int BoardSerial;
	long int CFDZeroCross;
	long int CFDDiscrMin;
	long int SyncLevel;
	long int CurveOffset;
	float Resolution;} BoardHdr;


struct {
	long int Globclock;
	long int Reserved1;
	long int Reserved2;
	long int Reserved3;
	long int Reserved4;
	long int Reserved5;
	long int Reserved6;
	long int SyncRate;
	long int TTTRCFDRate;
	long int TTTRStopAfter;
	long int TTTRStopReason;
	long int NoOfRecords;
	long int SpecialHeaderSize;} TTTRHdr;

/*The following data records will appear in the file NoOfRecords times*/

struct {
	unsigned TimeTag	:16;
	unsigned Channel	:12;
	unsigned Route		:2;
	unsigned Valid		:1;
	unsigned Reserved	:1; }  TTTRrecord;


int main(int argc, char* argv[])
{

 FILE *fpin,*fpout;
 long int result,ii;
 unsigned long counts=0, overflows=0;
 double ofltime=0, truetime=0;
 int mode=0;

 printf("\n");
 printf("\nTimeHarp 200 v5.0 TTTR file demo\n");

 if(argc<3)
 {
  printf("\nUsage: t3rdemo infile outfile [X]");
  printf("\ninfile is a binary TimeHarp 200 v.5.0 data file (*.t3r)");
  printf("\noutfile (ASCII) will contain: ");
  printf("\n X=0 photon records and header (default)");
  printf("\n X=1 photon records without header");
  printf("\n X=2 extended output incl. special records\n");
  goto ex;
 }

 if((fpin=fopen(argv[1],"rb"))==NULL)
 {printf("\ncannot open input file\n"); goto ex;}

 if((fpout=fopen(argv[2],"w"))==NULL)
  {printf("\ncannot open output file\n"); goto ex;}

 if(argc>3)
 {
	 if(strncmp(argv[3],"1",1)==0) mode=1;
	 if(strncmp(argv[3],"2",1)==0) mode=2;
 }
 printf("\nlisting mode: %d\n",mode);

 result = fread( &TxtHdr, 1, sizeof(TxtHdr) ,fpin);
 if (result!= sizeof(TxtHdr))
 {
   printf("\nerror reading input file, aborted.");
   goto close;
 }

 if(mode!=1)
 {
 fprintf(fpout,"Identifier       : %.*s\n",sizeof(TxtHdr.Ident),TxtHdr.Ident);
 fprintf(fpout,"Software Version : %.*s\n",sizeof(TxtHdr.SoftwareVersion),TxtHdr.SoftwareVersion);
 fprintf(fpout,"Hardware Version : %.*s\n",sizeof(TxtHdr.HardwareVersion),TxtHdr.HardwareVersion);
 fprintf(fpout,"Time of Creation : %.*s\n",sizeof(TxtHdr.FileTime),TxtHdr.FileTime);
 fprintf(fpout,"File Comment     : %.*s\n",sizeof(TxtHdr.CommentField),TxtHdr.CommentField);
 }

 if(strncmp(TxtHdr.SoftwareVersion,"5.0",3))
 {
	 printf("\nInput file version is %s. This program is for v5.0 only. Aborted.",
				TxtHdr.SoftwareVersion);
	 goto close;
 }

 result = fread( &BinHdr, 1, sizeof(BinHdr) ,fpin);
 if (result!= sizeof(BinHdr))
 {
   printf("\nerror reading input file, aborted.");
   goto close;
 }
 if(mode!=1)
 {
 fprintf(fpout,"No of Channels   : %ld\n",BinHdr.Channels);
 fprintf(fpout,"No of Curves     : %ld\n",BinHdr.Curves);
 fprintf(fpout,"Bits per Channel : %ld\n",BinHdr.BitsPerChannel);
 fprintf(fpout,"Routing Channels : %ld\n",BinHdr.RoutingChannels);
 fprintf(fpout,"No of Boards     : %ld\n",BinHdr.NumberOfBoards);
 fprintf(fpout,"Active Curve     : %ld\n",BinHdr.ActiveCurve);
 fprintf(fpout,"Measurement Mode : %ld\n",BinHdr.MeasMode);
 fprintf(fpout,"Measurem.SubMode : %ld\n",BinHdr.SubMode);
 fprintf(fpout,"Range No         : %ld\n",BinHdr.RangeNo);
 fprintf(fpout,"Offset           : %ld\n",BinHdr.Offset);
 fprintf(fpout,"AcquisitionTime  : %ld\n",BinHdr.Tacq);
 fprintf(fpout,"Stop at          : %ld\n",BinHdr.StopAt);
 fprintf(fpout,"Stop on Ovfl.    : %ld\n",BinHdr.StopOnOvfl);
 fprintf(fpout,"Restart          : %ld\n",BinHdr.Restart);
 fprintf(fpout,"DispLinLog       : %ld\n",BinHdr.DispLinLog);
 fprintf(fpout,"DispTimeAxisFrom : %ld\n",BinHdr.DispTimeFrom);
 fprintf(fpout,"DispTimeAxisTo   : %ld\n",BinHdr.DispTimeTo);
 fprintf(fpout,"DispCountAxisFrom: %ld\n",BinHdr.DispCountsFrom);
 fprintf(fpout,"DispCountAxisTo  : %ld\n",BinHdr.DispCountsTo);
 }

 if (BinHdr.NumberOfBoards != 1)
  {
    printf("\nerror: number of boards must be 1, aborted.");
    goto close;
  }

 result = fread( &BoardHdr, 1, sizeof(BoardHdr) ,fpin);
 if (result!= sizeof(BoardHdr))
 {
   printf("\nerror reading input file, aborted.");
   goto close;
 }

 if(mode!=1)
 {
 fprintf(fpout,"Board serial     : %ld\n",BoardHdr.BoardSerial);
 fprintf(fpout,"CFDZeroCross     : %ld\n",BoardHdr.CFDZeroCross);
 fprintf(fpout,"CFDDiscriminMin  : %ld\n",BoardHdr.CFDDiscrMin);
 fprintf(fpout,"SYNCLevel        : %ld\n",BoardHdr.SyncLevel);
 fprintf(fpout,"Curve Offset     : %ld\n",BoardHdr.CurveOffset);
 fprintf(fpout,"Resolution       : %f\n", BoardHdr.Resolution);
 }

 result = fread( &TTTRHdr, 1, sizeof(TTTRHdr) ,fpin);
 if (result!= sizeof(TTTRHdr))
  {
    printf("\nerror reading input file, aborted.");
    goto close;
  }

 if(mode!=1)
 {
 fprintf(fpout,"Glob Clock       : %ld\n",TTTRHdr.Globclock);
 fprintf(fpout,"Sync Rate        : %ld\n",TTTRHdr.SyncRate);
 fprintf(fpout,"Average CFD Rate : %ld\n",TTTRHdr.TTTRCFDRate);
 fprintf(fpout,"Stop After       : %ld\n",TTTRHdr.TTTRStopAfter);
 fprintf(fpout,"Stop Reason      : %ld\n",TTTRHdr.TTTRStopReason);
 fprintf(fpout,"No of Records    : %ld\n",TTTRHdr.NoOfRecords);
 fprintf(fpout,"Special Hdr Size : %ld\n\n",TTTRHdr.SpecialHeaderSize);
 }

 printf("\nNo of Records	 : %ld\n",TTTRHdr.NoOfRecords);

 /* v5.0: skip the special header (you may need to read it if you
    want to interpret an imaging file */
 fseek(fpin,TTTRHdr.SpecialHeaderSize*4,SEEK_CUR);

 /* Now read and interpret the TTTRrecords */

 if(mode!=1)
 {
  if(mode==2) fprintf(fpout," RawRec#   RawHex ");
  fprintf(fpout,"      #   timetag    time/s   channel  route\n\n");
 }

 for(ii=0; ii<TTTRHdr.NoOfRecords; ii++)
 {
	result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,fpin);
	if (result!= sizeof(TTTRrecord))
	{
		printf("\nerror reading input file, aborted.");
		goto close;
	}

	if(mode==2) fprintf(fpout,"%7lu  %08X ",ii,TTTRrecord);

	truetime = (ofltime + TTTRrecord.TimeTag) * TTTRHdr.Globclock * 1e-9; /* convert to seconds */

	if(TTTRrecord.Valid)
	{
		fprintf(fpout,"%7lu  %7u  %11.7lf  %5u   %2u",
			counts, TTTRrecord.TimeTag, truetime, TTTRrecord.Channel, TTTRrecord.Route);
		counts++;
	}
	else /* this means we have a 'special record' */
	{
		if(TTTRrecord.Channel & 0x800)  /* NEW v.5.0: Instead of te overflow bit we now evaluate this */
		{								/*	         AFTER! evaluating the valid record. */
			ofltime += 65536; /* unwrap the time tag overflow */
			overflows++;
		}
		if(mode==2)
		{
			fprintf(fpout,"         %7u  %11.7lf  ",TTTRrecord.TimeTag,truetime);
			if(TTTRrecord.Channel & 0x800)	fprintf(fpout," Overflow\n");
			if(TTTRrecord.Channel & 0x007)	fprintf(fpout," Marker=%1d\n",TTTRrecord.Channel & 0x007);
		}
	}
	if(TTTRrecord.Valid) fprintf(fpout,"\n");
 }

 printf("\noverflows=%1lu  counts=%1lu  timespan=%1.7lfs\n", overflows, counts, truetime);

close:
 fclose(fpin);
 fclose(fpout);
ex:
 printf("\npress return to exit");
 getchar();
 return(0);

}
