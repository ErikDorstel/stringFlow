# stringFlow
Teensy 3.6 Guitar Synthesizer with MIDI Control

This project implements a guitar synthesizer using the following components:

Teensy 3.6 https://www.pjrc.com/store/teensy36.html

Teensy Audio Library https://www.pjrc.com/teensy/td_libs_Audio.html

Audio Adapter Board https://www.pjrc.com/store/teensy3_audio.html

You will find a schematic within the repository. The schematic contains additional parts for power supply and MIDI interface. The synthesizer uses a MIDI keyboard to control it. The following MIDI codes are used:

NoteOn 36 to 47:    set chord C3 to B3

NoteOn 48 to 50:    set chord C4 to B4

NoteOn 60 to 71:    set chord C5 to C6

NoteOn 73:          set major chord

NoteOn 75:          set minor chord

NoteOn 78:          strum down

NoteOn 80:          strum up

NoteOn 82:          strum down/up

NoteOn 85:          start/stop rhythmus 1

NoteOn 87:          start/stop rhythmus 2

NoteOn 72:          pick string 1

NoteOn 74:          pick string 2

NoteOn 76:          pick string 3

NoteOn 77:          pick string 4

NoteOn 79:          pick string 5

NoteOn 81:          pick string 6

controller 1:       set strum delay

controller 7:       set pick delay
