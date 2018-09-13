#include <Audio.h>
#include "chords.h"
#include "rhythm.h"

AudioSynthKarplusStrong  string1;
AudioSynthKarplusStrong  string2;
AudioSynthKarplusStrong  string3;
AudioSynthKarplusStrong  string4;
AudioSynthKarplusStrong  string5;
AudioSynthKarplusStrong  string6;
AudioMixer4              mixer1;
AudioMixer4              mixer2;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(string1, 0, mixer1, 0);
AudioConnection          patchCord2(string2, 0, mixer1, 1);
AudioConnection          patchCord3(string3, 0, mixer1, 2);
AudioConnection          patchCord4(string4, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1,  0, mixer2, 0);
AudioConnection          patchCord6(string5, 0, mixer2, 1);
AudioConnection          patchCord7(string6, 0, mixer2, 2);
AudioConnection          patchCord8(mixer2,  0, i2s1,   0);
AudioConnection          patchCord9(mixer2,  0, i2s1,   1);
AudioControlSGTL5000     sgtl5000_1;
IntervalTimer            rhythmTimer;

byte pickDelay=5; int strumDelay=300; byte strumDir=0;
const int* chord=&majChord1[0][0]; byte chordTone=48; byte chordNote=0; float chordVelocity=1;
const int* rhythm=&rhythm1[0]; volatile int rhythmStep=-1;

void setup() {
  Serial1.begin(31250,SERIAL_8N1);
  AudioMemory(15);
  sgtl5000_1.enable(); sgtl5000_1.volume(0.6);
  mixer1.gain(0, 0.15); mixer1.gain(1, 0.15); mixer1.gain(2, 0.15);
  mixer1.gain(3, 0.15); mixer2.gain(1, 0.15); mixer2.gain(2, 0.15);
  delay(700); }

void loop() {
  byte MIDIin; static byte MIDIstatus=0; static byte MIDIchannel=0; static byte MIDIpara1=0; static byte MIDIpara2=0;
  if (Serial1.available() > 0) { MIDIin = Serial1.read();
    if ((MIDIin & 128) == 128) { MIDIstatus=MIDIin & 240; MIDIchannel=MIDIin & 15; MIDIpara1=255; MIDIpara2=255; } else {
    if (MIDIpara1==255) { MIDIpara1=MIDIin; } else if (MIDIpara2==255) { MIDIpara2=MIDIin;
	  if (MIDIstatus == 0x80) { MIDIsetNoteOff(MIDIchannel,MIDIpara1,MIDIpara2); }
	  if (MIDIstatus == 0x90) { MIDIsetNoteOn(MIDIchannel,MIDIpara1,MIDIpara2); }
	  if (MIDIstatus == 0xB0) { MIDIsetControl(MIDIchannel,MIDIpara1,MIDIpara2); }
	  if (MIDIstatus == 0xE0) { MIDIsetPitchbend(MIDIchannel,MIDIpara1+(128*MIDIpara2)); }
    MIDIpara1=255; MIDIpara2=255; } } } }

void MIDIsetNoteOn(byte channel, byte tone, float velocity) {
  chordVelocity=0.3+(velocity/127*0.7);
  if (tone>=36 and tone<72) { chordTone=tone+12; chordNote=chordTone%12; }
  if (tone==73) { if (chord==&majChord1[0][0]) { chord=&majChord2[0][0]; } else { chord=&majChord1[0][0]; } }
  if (tone==75) { if (chord==&minChord1[0][0]) { chord=&minChord2[0][0]; } else { chord=&minChord1[0][0]; } }
  if (tone==78) { doWaitRhythm(); strumDown(); strumDir=1; }
  if (tone==80) { doWaitRhythm(); strumUp(); strumDir=0; }
  if (tone==82) { doWaitRhythm(); if (strumDir==0) { strumDown(); strumDir=1; } else { strumUp(); strumDir=0; } }
  if (tone==85) { if (rhythmStep==-1) { rhythm=&rhythm1[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,1); } else { rhythmTimer.end(); rhythmStep=-1; } }
  if (tone==84) { rhythm=&rhythm1[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,strumDelay*1000); }
  if (tone==86) { rhythm=&rhythm2[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,strumDelay*1000); }
  if (tone==88) { rhythm=&rhythm3[0]; rhythmStep=0; rhythmTimer.begin(doRhythm,strumDelay*1000); }
  if (tone==72) { doWaitRhythm(); pick1(); }
  if (tone==74) { doWaitRhythm(); pick2(); }
  if (tone==76) { doWaitRhythm(); pick3(); }
  if (tone==77) { doWaitRhythm(); pick4(); }
  if (tone==79) { doWaitRhythm(); pick5(); }
  if (tone==81) { doWaitRhythm(); pick6(); } }

void MIDIsetNoteOff(byte channel, byte tone, byte velocity) { }

void MIDIsetControl(byte channel, byte control, byte value) {
  if (control==1) { strumDelay=50+((value)*3); }
  if (control==7) { pickDelay=value+1; } }

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
  if (*(rhythm+rhythmStep)==10) { rhythmStep++; rhythmTimer.begin(doRhythm,strumDelay*1000); }
  else if (*(rhythm+rhythmStep)==1) { rhythmStep++; pick1(); rhythmTimer.begin(doRhythm,pickDelay*1000); }
  else if (*(rhythm+rhythmStep)==2) { rhythmStep++; pick2(); rhythmTimer.begin(doRhythm,pickDelay*1000); }
  else if (*(rhythm+rhythmStep)==3) { rhythmStep++; pick3(); rhythmTimer.begin(doRhythm,pickDelay*1000); }
  else if (*(rhythm+rhythmStep)==4) { rhythmStep++; pick4(); rhythmTimer.begin(doRhythm,pickDelay*1000); }
  else if (*(rhythm+rhythmStep)==5) { rhythmStep++; pick5(); rhythmTimer.begin(doRhythm,pickDelay*1000); }
  else if (*(rhythm+rhythmStep)==6) { rhythmStep++; pick6(); rhythmTimer.begin(doRhythm,pickDelay*1000); } }

void doWaitRhythm() {
  if (rhythmStep>-1) { rhythmStep=0; rhythmTimer.begin(doRhythm,1000000); } }
