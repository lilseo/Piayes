////
////  midiKeyboardStateFNs.cpp
////  Piayes
////
////  Created by Mat Wiesman on 3/22/17.
////
////
//
//#include "midiKeyboardStateFNs.hpp"
//
//void MidiKeyboardState::processNextMidiBuffer (MidiBuffer& buffer,
//                                               const int startSample,
//                                               const int numSamples,
//                                               const bool injectIndirectEvents,
//                                               int chordValue)
//{
//    MidiBuffer::Iterator i (buffer);
//    MidiMessage message;
//    int time;
//    
//    const ScopedLock sl (lock);
//    
//    while (i.getNextEvent (message, time))
//        processNextMidiEvent (message);
//    
//    if (injectIndirectEvents)
//    {
//        MidiBuffer::Iterator i2 (eventsToAdd);
//        const int firstEventToAdd = eventsToAdd.getFirstEventTime();
//        const double scaleFactor = numSamples / (double) (eventsToAdd.getLastEventTime() + 1 - firstEventToAdd);
//        
//        while (i2.getNextEvent (message, time))
//        {
//            if (chordValue == 1) {
//                // value 1 is major chords
//                MidiMessage m3(message);
//                m3.setNoteNumber((int) message.getNoteNumber() + 4); // the major third
//                MidiMessage m5(message);
//                m5.setNoteNumber((int) message.getNoteNumber() + 7); // the perfect fifth
//                const int pos = jlimit (0, numSamples - 1, roundToInt ((time - firstEventToAdd) * scaleFactor));
//                buffer.addEvent (message, startSample + pos);
//                buffer.addEvent (m3, startSample + pos);
//                buffer.addEvent (m5, startSample + pos);
//            }
//            else if (chordValue == 2) {
//                // value 2 is minor chord
//                MidiMessage m3(message);
//                m3.setNoteNumber((int) message.getNoteNumber() + 3); // the minor third
//                MidiMessage m5(message);
//                m5.setNoteNumber((int) message.getNoteNumber() + 7); // the perfect fifth
//                const int pos = jlimit (0, numSamples - 1, roundToInt ((time - firstEventToAdd) * scaleFactor));
//                buffer.addEvent (message, startSample + pos);
//                buffer.addEvent (m3, startSample + pos);
//                buffer.addEvent (m5, startSample + pos);
//            }
//            else {
//                // default to normal
//                const int pos = jlimit (0, numSamples - 1, roundToInt ((time - firstEventToAdd) * scaleFactor));
//                buffer.addEvent (message, startSample + pos);
//            }
//            //std::cout << "testing" << std::endl;
//        }
//    }
//    
//    eventsToAdd.clear();
//}
