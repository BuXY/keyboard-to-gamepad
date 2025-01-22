#pragma once

#include <stddef.h>
#include <inttypes.h>

class MidiInput
{
public:
	enum class NoteEnum
	{
		C	= 0,
		Cs	= 1,
		D	= 2,
		Ds	= 3,
		E	= 4,
		F	= 5,
		Fs	= 6,
		G	= 7,
		Gs	= 8,
		A	= 9,
		As	= 10,
		B	= 11,
		Octave = 12,
	};

	struct NoteMapItem
	{
		size_t midiNote = 0;
		bool* targetBool = nullptr;

		NoteMapItem();
		NoteMapItem(const size_t i_midiNote, bool* i_targetBool);
	};

	void begin();
	void mapMidiNotesToBools(NoteMapItem i_noteMap[], const size_t i_count);
	void mapMidiNoteToBool(const size_t i_midiNote, bool* i_targetBool);
	uint8_t buildMidiNote(const uint8_t i_octave, const NoteEnum i_note) const;

	void handleNoteEvent(uint8_t i_channel, uint8_t i_note, uint8_t i_velocity, bool i_isNoteOn);

	// return: Pressed key count
	uint8_t updateInputs();

protected:
	static constexpr size_t noteMapSize = 256 * 2;
	static constexpr size_t modifierShiftAmount = noteMapSize / 2;

	bool* noteMap[noteMapSize];
	uint8_t pressedKnownKeyCount = 0;
};
