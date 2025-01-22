#include "MidiInput.h"

#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Instance in main.cpp
extern MidiInput midiInput;

MidiInput::NoteMapItem::NoteMapItem()
{
}
MidiInput::NoteMapItem::NoteMapItem(const size_t i_midiNote, bool* i_targetBool)
	: midiNote(i_midiNote)
	, targetBool(i_targetBool)
{
}

uint8_t MidiInput::buildMidiNote(const uint8_t i_octave, const NoteEnum i_note) const
{
	return i_octave * static_cast<uint8_t>(NoteEnum::Octave) + static_cast<uint8_t>(i_note);
}

void handleNoteOn(uint8_t i_channel, uint8_t i_note, uint8_t i_velocity)
{
	midiInput.handleNoteEvent(i_channel, i_note, i_velocity, true);
}

void handleNoteOff(uint8_t i_channel, uint8_t i_note, uint8_t i_velocity)
{
	midiInput.handleNoteEvent(i_channel, i_note, i_velocity, false);
}

void MidiInput::handleNoteEvent(uint8_t i_channel, uint8_t i_note, uint8_t i_velocity, bool i_isNoteOn)
{
	// Find defined key in map
	bool* keyPointer = noteMap[i_note];
	if (keyPointer != nullptr)
	{
		// Update pressed key count LEDs
		if (*keyPointer != i_isNoteOn)
		{
			pressedKnownKeyCount += (i_isNoteOn ? 1 : -1);
		}

		// Update key state
		*keyPointer = i_isNoteOn;
	}
}

void MidiInput::begin()
{
	MIDI.setHandleNoteOn(handleNoteOn);
	MIDI.setHandleNoteOff(handleNoteOff);
	MIDI.begin(MIDI_CHANNEL_OMNI);
	MIDI.turnThruOff();

	// Clear notemap
	for (size_t i = 0; i < noteMapSize; ++i)
	{
		noteMap[i] = nullptr;
	}
}

void MidiInput::mapMidiNotesToBools(MidiInput::NoteMapItem i_noteMap[], const size_t i_count)
{
	for (size_t i = 0; i < i_count; ++i)
	{
		NoteMapItem& item = i_noteMap[i];
		mapMidiNoteToBool(item.midiNote, item.targetBool);
	}
}

void MidiInput::mapMidiNoteToBool(const size_t i_midiNote, bool* i_targetBool)
{
	noteMap[i_midiNote] = i_targetBool;
}

uint8_t MidiInput::updateInputs()
{
	while (MIDI.read())
	{
		; // deliberate
	}

	return pressedKnownKeyCount;
}
