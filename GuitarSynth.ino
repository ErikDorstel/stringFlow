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
AudioConnection          patchCord5(mixer1, 0, mixer2, 0);
AudioConnection          patchCord6(string5, 0, mixer2, 1);
AudioConnection          patchCord7(string6, 0, mixer2, 2);
AudioConnection          patchCord8(mixer2, 0, i2s1, 0);
AudioConnection          patchCord9(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;
IntervalTimer myTimer;

byte pick_delay=5; volatile byte strum_dir=0; byte chord_tone=48; byte chord_note=0; float chord_velocity=1; const int* chord=&maj_chord1[0][0];
const int* rhythm=&rhythm1[0]; volatile int rhythm_step=-1; int strum_delay=400;

void setup() {
  Serial1.begin(31250,SERIAL_8N1);
  AudioMemory(15);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  mixer1.gain(0, 0.15);
  mixer1.gain(1, 0.15);
  mixer1.gain(2, 0.15);
  mixer1.gain(3, 0.15);
  mixer2.gain(1, 0.15);
  mixer2.gain(2, 0.15);
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
  chord_velocity=0.3+(velocity/127*0.7);
  if (tone>=36 and tone<72) { chord_tone=tone+12; chord_note=chord_tone%12; }
  if (tone==73) { if (chord==&maj_chord1[0][0]) { chord=&maj_chord2[0][0]; } else { chord=&maj_chord1[0][0]; } }
  if (tone==75) { if (chord==&min_chord1[0][0]) { chord=&min_chord2[0][0]; } else { chord=&min_chord1[0][0]; } }
  if (tone==78) { strumDown(); strum_dir=1; }
  if (tone==80) { strumUp(); strum_dir=0; }
  if (tone==82) { if (strum_dir==0) { strumDown(); strum_dir=1; } else { strumUp(); strum_dir=0; } }
  if (tone==85) { if (rhythm_step==-1) { rhythm=&rhythm1[0]; rhythm_step=0; myTimer.begin(doRhythmus,1); } else { myTimer.end(); rhythm_step=-1; } }
  if (tone==87) { if (rhythm_step==-1) { rhythm=&rhythm2[0]; rhythm_step=0; myTimer.begin(doRhythmus,1); } else { myTimer.end(); rhythm_step=-1; } }
  if (tone==72) { pick1(); }
  if (tone==74) { pick2(); }
  if (tone==76) { pick3(); }
  if (tone==77) { pick4(); }
  if (tone==79) { pick5(); }
  if (tone==81) { pick6(); } }

void MIDIsetNoteOff(byte channel, byte tone, byte velocity) { }

void MIDIsetControl(byte channel, byte control, byte value) {
  if (control==1) { strum_delay=50+((value)*3); }
  if (control==7) { pick_delay=value+1; } }

void MIDIsetPitchbend(byte channel, word pitch) { }

void strumDown() {
  pick1(); delay(pick_delay); pick2(); delay(pick_delay); pick3(); delay(pick_delay);
  pick4(); delay(pick_delay); pick5(); delay(pick_delay); pick6(); delay(pick_delay); }

void strumUp() {
  pick6(); delay(pick_delay); pick5(); delay(pick_delay); pick4(); delay(pick_delay);
  pick3(); delay(pick_delay); pick2(); delay(pick_delay); pick1(); delay(pick_delay); }
  
void pick1() { if (*(chord+(chord_note*6)+0)<255) { string1.noteOn((pow(2,(float(chord_tone+*(chord+(chord_note*6)+0))-69)/12))*440,chord_velocity); } }
void pick2() { if (*(chord+(chord_note*6)+1)<255) { string2.noteOn((pow(2,(float(chord_tone+*(chord+(chord_note*6)+1))-69)/12))*440,chord_velocity); } }
void pick3() { if (*(chord+(chord_note*6)+2)<255) { string3.noteOn((pow(2,(float(chord_tone+*(chord+(chord_note*6)+2))-69)/12))*440,chord_velocity); } }
void pick4() { if (*(chord+(chord_note*6)+3)<255) { string4.noteOn((pow(2,(float(chord_tone+*(chord+(chord_note*6)+3))-69)/12))*440,chord_velocity); } }
void pick5() { if (*(chord+(chord_note*6)+4)<255) { string5.noteOn((pow(2,(float(chord_tone+*(chord+(chord_note*6)+4))-69)/12))*440,chord_velocity); } }
void pick6() { if (*(chord+(chord_note*6)+5)<255) { string6.noteOn((pow(2,(float(chord_tone+*(chord+(chord_note*6)+5))-69)/12))*440,chord_velocity); } }

void doRhythmus() {
  if (*(rhythm+rhythm_step)==0) { rhythm_step=0; }
  if (*(rhythm+rhythm_step)==10) { rhythm_step++; myTimer.begin(doRhythmus,strum_delay*1000); }
  else if (*(rhythm+rhythm_step)==1) { rhythm_step++; pick1(); myTimer.begin(doRhythmus,pick_delay*1000); }
  else if (*(rhythm+rhythm_step)==2) { rhythm_step++; pick2(); myTimer.begin(doRhythmus,pick_delay*1000); }
  else if (*(rhythm+rhythm_step)==3) { rhythm_step++; pick3(); myTimer.begin(doRhythmus,pick_delay*1000); }
  else if (*(rhythm+rhythm_step)==4) { rhythm_step++; pick4(); myTimer.begin(doRhythmus,pick_delay*1000); }
  else if (*(rhythm+rhythm_step)==5) { rhythm_step++; pick5(); myTimer.begin(doRhythmus,pick_delay*1000); }
  else if (*(rhythm+rhythm_step)==6) { rhythm_step++; pick6(); myTimer.begin(doRhythmus,pick_delay*1000); } }
