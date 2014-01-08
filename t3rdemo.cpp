#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>

using namespace std;

/*
The following structures are used to hold the TimeHarp file data
They reflect the file structure.
*/

struct tParamStruct{
    long Start;
    long Step;
    long End;
};

struct tCurveMapping{
    long MapTo;
    long Show;
};

// The following represents the readable ASCII file header portion}
struct TxtHdr{
    array<char, 16> Ident;
    array<char, 6> SoftwareVersion;
    array<char, 6> HardwareVersion;
    array<char, 18> FileTime;
    array<char, 2> CRLF;
    array<char, 256> CommentField;

};

// The following is binary header information
struct BinHdr {
    long Channels;
    long Curves;
    long BitsPerChannel;
    long RoutingChannels;
    long NumberOfBoards;
    long ActiveCurve;
    long MeasMode;
    long SubModes;
    long RangeNo;
    long Offset;
    long Tecq;
    long StopAt;
    long StopOnOvfl;
    long Restart;
    long DispLinLog;
    long DispTimeFrom;
    long DispTimeTo;
    long DispCountsTo;
    array<tCurveMapping, 8> DispCurves;
    array<tParamStruct, 3> Params;
    long RepeatMode;
    long RepeatsPerCurve;
    long RepeatTime;
    long RepeatWaitTime;
    array<char, 20> ScriptName;
};

struct BoardHdr{
    long BoardSerial;
    long CFDZeroCross;
    long CFDDiscrMin;
    long SyncLevel;
    long CurveOffset;
    float Resolution;
};

struct TTTRHdr{
    long Globclock;
    long Reserved1;
    long Reserved2;
    long Reserved3;
    long Reserved4;
    long Reserved5;
    long Reserved6;
    long SyncRate;
    long TTTRCFDRate;
    long TTTRStopAfter;
    long TTTRStopReason;
    long NoOfRecords;
    long SpecialHeaderSize;
};

//The following data records will appear in the file NoOfRecords times

struct TTTRrecord{
    unsigned TimeTag:16;
    unsigned Channel:12;
    unsigned Route:2;
    unsigned Valid:1;
    unsigned Reserved:1;
};

//enum class test{};

const void close(FILE &fpin, FILE &fpout) {
    fclose(fpin);
    fclose(fpout);
}

int main(int argc, char* argv[])
{
    //vector<int> a{1,2,3,4,5};
    FILE *fpin;
    FILE *fpout;
    long result;
    long ii;
    unsigned long counts=0;
    unsigned long overflows=0;
    double ofltime=0;
    double truetime=0;
    int mode = 0;

    cout << endl;
    cout << endl;
    cout << "TimeHarp 200 version C++11 TTTR file demo" << endl;
    if(argc < 3) {
        cout << endl;
        cout << "Usage: t3rdemo infile outfile [x]" << endl;
        cout << "infile is a binary TimeHarp 200 version C++11 data file (*.t3r)" << endl;
        cout << "outfile (ASCII) will contain: "<< endl;
        cout << " X=0 photon records and hearder (default)." << endl;
        cout << " X=1 photon records without header." << endl;
        cout << " X=2 extended output include special records."<<endl;
        close(fpin, fpout);
        return 0;
    }



    for (auto i: a)
        cout << i << endl;
    cout << "Hello World!" << endl;
    return 0;
}

