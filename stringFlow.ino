#include <Audio.h>
#include "chords.h"
#include "rhythm.h"
#include "ADT.h"

IntervalTimer rhythmTimer;
byte pickDelay=5, strumDir=0; int strumDelay=300;
const int* chord=&majChord1[0][0]; byte chordTone=48, chordNote=0; float chordVelocity=1;
const int* rhythm=&rhythm1[0]; volatile int rhythmStep=-1;

void setup() {
  Serial1.begin(31250,SERIAL_8N1);
  AudioMemory(15);
  sgtl5000_1.enable(); sgtl5000_1.volume(0.6);
  mixer1.gain(0, 0.15); mixer1.gain(1, 0.15); mixer1.gain(2, 0.15); mixer1.gain(3, 0.15);
  mixer2.gain(0, 1); mixer2.gain(1, 0.15); mixer2.gain(2, 0.15); mixer2.gain(3, 0);
  delay(1000); }

void loop() {
  byte MIDIin; static byte MIDIstatus=0, MIDIchannel=0, MIDIpara1=0, MIDIpara2=0;
  if (Serial1.available() > 0) { MIDIin = Serial1.read();
    if ((MIDIin & 128) == 128) { MIDIstatus=MIDIin & 240; MIDIchannel=MIDIin & 15; MIDIpara1=255; MIDIpara2=255; } else {
    if (MIDIpara1==255) { MIDIpara1=MIDIin; } else if (MIDIpara2==255) { MIDIpara2=MIDIin;
	  if (MIDIstatus == 0x80) { MIDIsetNoteOff(MIDIchannel,MIDIpara1,MIDIpara2); } else
	  if (MIDIstatus == 0x90) { MIDIsetNoteOn(MIDIchannel,MIDIpara1,MIDIpara2); } else
	  if (MIDIstatus == 0xB0) { MIDIsetControl(MIDIchannel,MIDIpara1,MIDIpara2); } else
	  if (MIDIstatus == 0xE0) { MIDIsetPitchbend(MIDIchannel,MIDIpara1+(128*MIDIpara2)); }
    MIDIpara1=255; MIDIpara2=255; } } } }

void MIDIsetNoteOn(byte channel, byte tone, float velocity) {
  chordVelocity=0.3+(velocity/127*0.7);
  if (tone>=36 and tone<72) { chordTone=tone+12; chordNote=chordTone%12; }
  switch (tone) {
    case 73: if (chord==&majChord1[0][0]) { chord=&majChord2[0][0]; } else { chord=&majChord1[0][0]; } break;
    case 75: if (chord==&minChord1[0][0]) { chord=&minChord2[0][0]; } else { chord=&minChord1[0][0]; } break;
    case 78: doWaitRhythm(); strumDown(); strumDir=1; break;
    case 80: doWaitRhythm(); strumUp(); strumDir=0; break;
    case 82: doWaitRhythm(); if (strumDir==0) { strumDown(); strumDir=1; } else { strumUp(); strumDir=0; } break;
    case 85: if (rhythmStep==-1) { rhythm=&rhythm1[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,1); } else { rhythmTimer.end(); rhythmStep=-1; } break;
    case 84: rhythm=&rhythm1[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,strumDelay*1000); break;
    case 86: rhythm=&rhythm2[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,strumDelay*1000); break;
    case 88: rhythm=&rhythm3[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,strumDelay*1000); break;
    case 72: doWaitRhythm(); pick1(); break;
    case 74: doWaitRhythm(); pick2(); break;
    case 76: doWaitRhythm(); pick3(); break;
    case 77: doWaitRhythm(); pick4(); break;
    case 79: doWaitRhythm(); pick5(); break;
    case 81: doWaitRhythm(); pick6(); break; } }

void MIDIsetNoteOff(byte channel, byte tone, byte velocity) { }

void MIDIsetControl(byte channel, byte control, byte value) {
  switch (control) {
    case 1: strumDelay=50+((value)*3); break;
    case 7: pickDelay=value+1; break; } }

void MIDIsetPitchbend(byte channel, word pitch) { }

void strumDown() {
  pick1(); delay(pickDelay); pick2(); delay(pickDelay); pick3(); delay(pickDelay);
  pick4(); delay(pickDelay); pick5(); delay(pickDelay); pick6(); delay(pickDelay); }

void strumUp() {
  pick6(); delay(pickDelay); pick5(); delay(pickDelay); pick4(); delay(pickDelay);
  pick3(); delay(pickDelay); pick2(); delay(pickDelay); pick1(); delay(pickDelay); }
  
void pick1() { if (*(chord+(chordNote*6)+0)<255) { string1.noteOn((pow(2,(float(chordTone+*(chord+(chordNote*6)+0))-69)/12))*440,chordVelocity); } }
void pick2() { if (*(chord+(chordNote*6)+1)<255) { string2.noteOn((pow(2,(float(chordTone+*(chord+(chordNote*6)+1))-69)/12))*440,chordVelocity); } }
void pick3() { if (*(chord+(chordNote*6)+2)<255) { string3.noteOn((pow(2,(float(chordTone+*(chord+(chordNote*6)+2))-69)/12))*440,chordVelocity); } }
void pick4() { if (*(chord+(chordNote*6)+3)<255) { string4.noteOn((pow(2,(float(chordTone+*(chord+(chordNote*6)+3))-69)/12))*440,chordVelocity); } }
void pick5() { if (*(chord+(chordNote*6)+4)<255) { string5.noteOn((pow(2,(float(chordTone+*(chord+(chordNote*6)+4))-69)/12))*440,chordVelocity); } }
void pick6() { if (*(chord+(chordNote*6)+5)<255) { string6.noteOn((pow(2,(float(chordTone+*(chord+(chordNote*6)+5))-69)/12))*440,chordVelocity); } }

void doRhythm() {
  if (*(rhythm+rhythmStep)==0) { rhythmStep=0; }
  if (*(rhythm+rhythmStep)==10) { rhythmStep++; rhythmTimer.begin(doRhythm,strumDelay*1000); } else
  if (*(rhythm+rhythmStep)==1) { rhythmStep++; pick1(); rhythmTimer.begin(doRhythm,pickDelay*1000); } else
  if (*(rhythm+rhythmStep)==2) { rhythmStep++; pick2(); rhythmTimer.begin(doRhythm,pickDelay*1000); } else
  if (*(rhythm+rhythmStep)==3) { rhythmStep++; pick3(); rhythmTimer.begin(doRhythm,pickDelay*1000); } else
  if (*(rhythm+rhythmStep)==4) { rhythmStep++; pick4(); rhythmTimer.begin(doRhythm,pickDelay*1000); } else
  if (*(rhythm+rhythmStep)==5) { rhythmStep++; pick5(); rhythmTimer.begin(doRhythm,pickDelay*1000); } else
  if (*(rhythm+rhythmStep)==6) { rhythmStep++; pick6(); rhythmTimer.begin(doRhythm,pickDelay*1000); } }

void doWaitRhythm() {
  if (rhythmStep>-1) { rhythmStep=0; rhythmTimer.begin(doRhythm,1000000); } }
