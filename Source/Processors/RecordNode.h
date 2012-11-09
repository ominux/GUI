/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2012 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __RECORDNODE_H_FB9B1CA7__
#define __RECORDNODE_H_FB9B1CA7__


#ifdef WIN32
#include <Windows.h>
#endif
#include "../../JuceLibraryCode/JuceHeader.h"
#include <stdio.h>
#include <map>

#include "GenericProcessor.h"

/**

  Receives inputs from all processors that want to save their data.
  Writes data to disk using fwrite.

  Receives a signal from the ControlPanel to begin recording.

  @see GenericProcessor, ControlPanel

*/

class RecordNode : public GenericProcessor,
                   public FilenameComponentListener
{
public:
	
	RecordNode();
	~RecordNode();

  /** Handle incoming data and decide which files and events to write to disk.
  */	
	void process(AudioSampleBuffer &buffer, MidiBuffer &eventBuffer, int& nSamples);

  /** Overrides implementation in GenericProcessor; used to change recording parameters
      on the fly.

      parameterIndex = 0: stop recording
      parameterIndex = 1: start recording
      parameterIndex = 2: 
            newValue = 0: turn off recording for current channel
            newValue = 1: turn on recording for current channel
  */
	void setParameter (int parameterIndex, float newValue);

  void addInputChannel(GenericProcessor* sourceNode, int chan);

	bool enable();
	bool disable();

  /** Called by the ControlPanel to determine the amount of space
      left in the current dataDirectory.
  */
	float getFreeSpace();

  /** Selects a channel relative to a particular processor with ID = id
  */
  void setChannel(int id, int chan);

  /** Turns recording on and off for a particular channel.

      Channel numbers are absolute (based on RecordNode channel mapping).
  */
  void setChannelStatus(int chan, bool status);

  /** Used to clear all connections prior to the start of acquisition.
  */
  void resetConnections();

  /** Overrides implementation by GenericProcessor.
  */
  bool isAudioOrRecordNode() {return true;}

  /** Callback to indicate when user has chosen a new data directory.
  */
  void filenameComponentChanged(FilenameComponent*);

  /** Creates a new data directory in the location specified by the fileNameComponent.
  */
  void createNewDirectory();
	
private:

  /** Keep the RecordNode informed of acquisition and record states. 
  */
	bool isRecording, isProcessing, signalFilesShouldClose;

  /** User-selectable directory for saving data files. Currently
      defaults to the user's home directory. 
  */
  File dataDirectory;

  /** Automatically generated folder for each recording session. 
  */
  File rootFolder;

  /** Holds data that has been converted from float to int16 before
      saving. 
  */
  int16* continuousDataIntegerBuffer;

  /** Holds data that has been converted from float to int16 before
      saving. 
  */
  float* continuousDataFloatBuffer;

  /** Integer timestamp saved for each buffer. 
  */ 
  int64 timestamp;

  /** Used to generate timestamps if none are given. 
  */ 
  Time timer;

  /** Holds information for a given channel to be recorded to 
      its own file.
  */ 
  struct Channel
  {
    int nodeId;
    int chan;
    String name;
    bool isRecording;
    String filename;
    FILE* file;
  };

  /** Closes all open files after recording has finished.
  */ 
  void closeAllFiles();

  /** Map of continuous channels. 
  */ 
  std::map<int, Channel> continuousChannels;

  /** Map of event channels. 
  */ 
  std::map<int, std::map<int,Channel> > eventChannels;

  /** Method for writing continuous buffers to disk. 
  */ 
  void writeContinuousBuffer(float* data, int nSamples, int channel);
  
  /** Method for writing event buffers to disk. 
  */ 
  void writeEventBuffer(MidiMessage& event, int node, int channel);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordNode);

};



#endif  // __RECORDNODE_H_FB9B1CA7__
