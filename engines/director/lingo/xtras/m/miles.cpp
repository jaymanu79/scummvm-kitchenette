/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/file.h"
#include "common/system.h"

#include "audio/audiostream.h"
#include "audio/decoders/wave.h"
#include "audio/mixer.h"

#include "director/director.h"
#include "director/lingo/lingo.h"
#include "director/lingo/lingo-object.h"
#include "director/lingo/lingo-utils.h"
#include "director/cast.h"
#include "director/movie.h"
#include "director/sound.h"
#include "director/util.h"
#include "director/castmember/sound.h"
#include "director/lingo/xtras/m/miles.h"

/**************************************************
 *
 * USED IN:
 * Kitchenette (Le Lab, Windows)
 *
 * Sounds are played through the ScummVM mixer. MilesGetStatus returns
 * 4 while a sound is playing (the game waits on that value), 2 otherwise.
 *
 **************************************************/

/*
-- xtra Miles

--------------------------------------------------
--  The Miles Sound System Xtra - Version 3.6E  --
--  Copyright (C) 1991-97 RAD Game Tools, Inc.  --
--------------------------------------------------

new object

*MilesQuickPlay string, integer
*MilesQuickStream string, integer

*MilesStartup
*MilesReleaseHandles
*MilesReacquireHandles
*MilesShutdown
*MilesHookToSmacker
*MilesSetWaveOutput integer, integer, integer, integer
*MilesGetWaveDescription

MilesOpenWave object, string
MilesOpenWaveCastmember object, string, string
MilesOpenXMIDI object, string
MilesOpenStream object, string
MilesOpenCD object, integer
MilesOpenCopy object, string
MilesCopyFrom object

MilesPlay object
MilesPause object
MilesResume object
MilesClose object

MilesGetStatus object
MilesGetVolume object
MilesGetPosition object
MilesGetSpeed object
MilesGetLoopCount object
MilesGetPan object

MilesSetVolume object, integer
MilesSetPosition object, integer
MilesSetSpeed object, integer
MilesSetLoopCount object, integer
MilesSetPan object, integer

MilesGetCDTracks object
MilesPlayCDTrack object, integer
MilesPlayCDMS object, integer, integer
MilesGetCDStartTrackMS object, integer
MilesGetCDEndTrackMS object, integer
MilesEjectCD object
MilesRetractCD object
 */

namespace Director {

const char *const MilesXtra::xlibName = "Miles";
const XlibFileDesc MilesXtra::fileNames[] = {
	{ "mssx32",	nullptr },
	{ "Miles",	nullptr },
	{ nullptr,	nullptr },
};

static const MethodProto xlibMethods[] = {
	{ "new",	MilesXtra::m_new,	0, 0,	500 },
	{ "MilesOpenWave",	MilesXtra::m_MilesOpenWave,	1, 1,	500 },
	{ "MilesOpenWaveCastmember",	MilesXtra::m_MilesOpenWaveCastmember,	2, 2,	500 },
	{ "MilesOpenXMIDI",	MilesXtra::m_MilesOpenXMIDI,	1, 1,	500 },
	{ "MilesOpenStream",	MilesXtra::m_MilesOpenStream,	1, 1,	500 },
	{ "MilesOpenCD",	MilesXtra::m_MilesOpenCD,	1, 1,	500 },
	{ "MilesOpenCopy",	MilesXtra::m_MilesOpenCopy,	1, 1,	500 },
	{ "MilesCopyFrom",	MilesXtra::m_MilesCopyFrom,	0, 0,	500 },
	{ "MilesPlay",	MilesXtra::m_MilesPlay,	0, 0,	500 },
	{ "MilesPause",	MilesXtra::m_MilesPause,	0, 0,	500 },
	{ "MilesResume",	MilesXtra::m_MilesResume,	0, 0,	500 },
	{ "MilesClose",	MilesXtra::m_MilesClose,	0, 0,	500 },
	{ "MilesGetStatus",	MilesXtra::m_MilesGetStatus,	0, 0,	500 },
	{ "MilesGetVolume",	MilesXtra::m_MilesGetVolume,	0, 0,	500 },
	{ "MilesGetPosition",	MilesXtra::m_MilesGetPosition,	0, 0,	500 },
	{ "MilesGetSpeed",	MilesXtra::m_MilesGetSpeed,	0, 0,	500 },
	{ "MilesGetLoopCount",	MilesXtra::m_MilesGetLoopCount,	0, 0,	500 },
	{ "MilesGetPan",	MilesXtra::m_MilesGetPan,	0, 0,	500 },
	{ "MilesSetVolume",	MilesXtra::m_MilesSetVolume,	1, 1,	500 },
	{ "MilesSetPosition",	MilesXtra::m_MilesSetPosition,	1, 1,	500 },
	{ "MilesSetSpeed",	MilesXtra::m_MilesSetSpeed,	1, 1,	500 },
	{ "MilesSetLoopCount",	MilesXtra::m_MilesSetLoopCount,	1, 1,	500 },
	{ "MilesSetPan",	MilesXtra::m_MilesSetPan,	1, 1,	500 },
	{ "MilesGetCDTracks",	MilesXtra::m_MilesGetCDTracks,	0, 0,	500 },
	{ "MilesPlayCDTrack",	MilesXtra::m_MilesPlayCDTrack,	1, 1,	500 },
	{ "MilesPlayCDMS",	MilesXtra::m_MilesPlayCDMS,	2, 2,	500 },
	{ "MilesGetCDStartTrackMS",	MilesXtra::m_MilesGetCDStartTrackMS,	1, 1,	500 },
	{ "MilesGetCDEndTrackMS",	MilesXtra::m_MilesGetCDEndTrackMS,	1, 1,	500 },
	{ "MilesEjectCD",	MilesXtra::m_MilesEjectCD,	0, 0,	500 },
	{ "MilesRetractCD",	MilesXtra::m_MilesRetractCD,	0, 0,	500 },
	{ nullptr, nullptr, 0, 0, 0 }
};

static const BuiltinProto xlibBuiltins[] = {
	{ "MilesQuickPlay",	MilesXtra::m_MilesQuickPlay,	2, 2,	500, HBLTIN },
	{ "MilesQuickStream",	MilesXtra::m_MilesQuickStream,	2, 2,	500, HBLTIN },
	{ "MilesStartup",	MilesXtra::m_MilesStartup,	0, 0,	500, HBLTIN },
	{ "MilesReleaseHandles",	MilesXtra::m_MilesReleaseHandles,	0, 0,	500, HBLTIN },
	{ "MilesReacquireHandles",	MilesXtra::m_MilesReacquireHandles,	0, 0,	500, HBLTIN },
	{ "MilesShutdown",	MilesXtra::m_MilesShutdown,	0, 0,	500, HBLTIN },
	{ "MilesHookToSmacker",	MilesXtra::m_MilesHookToSmacker,	0, 0,	500, HBLTIN },
	{ "MilesSetWaveOutput",	MilesXtra::m_MilesSetWaveOutput,	4, 4,	500, HBLTIN },
	{ "MilesGetWaveDescription",	MilesXtra::m_MilesGetWaveDescription,	0, 0,	500, HBLTIN },
	{ nullptr, nullptr, 0, 0, 0, VOIDSYM }
};

MilesXtraObject::MilesXtraObject(ObjectType ObjectType) : Object<MilesXtraObject>("Miles") {
	_objType = ObjectType;
	_stream = nullptr;
	_volume = 127;
	_loopCount = 1;
}

MilesXtraObject::MilesXtraObject(const MilesXtraObject &obj) : Object<MilesXtraObject>(obj) {
	// Each new() gets its own sound slot
	_stream = nullptr;
	_volume = 127;
	_loopCount = 1;
}

MilesXtraObject::~MilesXtraObject() {
	closeSound();
}

void MilesXtraObject::closeSound() {
	g_system->getMixer()->stopHandle(_handle);
	delete _stream;
	_stream = nullptr;
}

bool MilesXtraObject::hasProp(const Common::String &propName) {
	return (propName == "name");
}

Datum MilesXtraObject::getProp(const Common::String &propName) {
	if (propName == "name")
		return Datum(MilesXtra::xlibName);
	warning("MilesXtra::getProp: unknown property '%s'", propName.c_str());
	return Datum();
}

void MilesXtra::open(ObjectType type, const Common::Path &path) {
	MilesXtraObject::initMethods(xlibMethods);
	MilesXtraObject *xobj = new MilesXtraObject(type);
	if (type == kXtraObj) {
		g_lingo->_openXtras.push_back(xlibName);
		g_lingo->_openXtraObjects.push_back(xobj);
	}
	g_lingo->exposeXObject(xlibName, xobj);
	g_lingo->initBuiltIns(xlibBuiltins);
}

void MilesXtra::close(ObjectType type) {
	MilesXtraObject::cleanupMethods();
	g_lingo->_globalvars[xlibName] = Datum();
}

void MilesXtra::m_new(int nargs) {
	g_lingo->printSTUBWithArglist("MilesXtra::m_new", nargs);
	g_lingo->dropStack(nargs);
	g_lingo->push(g_lingo->_state->me);
}

XOBJSTUB(MilesXtra::m_MilesOpenXMIDI, 1)
XOBJSTUB(MilesXtra::m_MilesOpenCD, 1)
XOBJSTUB(MilesXtra::m_MilesOpenCopy, 1)
XOBJSTUB(MilesXtra::m_MilesCopyFrom, 0)
XOBJSTUB(MilesXtra::m_MilesGetPosition, 0)
XOBJSTUB(MilesXtra::m_MilesGetSpeed, 22050)
XOBJSTUB(MilesXtra::m_MilesGetPan, 64)
XOBJSTUB(MilesXtra::m_MilesSetPosition, 0)
XOBJSTUB(MilesXtra::m_MilesSetSpeed, 0)
XOBJSTUB(MilesXtra::m_MilesSetPan, 0)
XOBJSTUB(MilesXtra::m_MilesGetCDTracks, 0)
XOBJSTUB(MilesXtra::m_MilesPlayCDTrack, 0)
XOBJSTUB(MilesXtra::m_MilesPlayCDMS, 0)
XOBJSTUB(MilesXtra::m_MilesGetCDStartTrackMS, 0)
XOBJSTUB(MilesXtra::m_MilesGetCDEndTrackMS, 0)
XOBJSTUB(MilesXtra::m_MilesEjectCD, 0)
XOBJSTUB(MilesXtra::m_MilesRetractCD, 0)
XOBJSTUB(MilesXtra::m_MilesQuickPlay, 0)
XOBJSTUB(MilesXtra::m_MilesQuickStream, 0)
XOBJSTUB(MilesXtra::m_MilesStartup, 1)
XOBJSTUB(MilesXtra::m_MilesReleaseHandles, 0)
XOBJSTUB(MilesXtra::m_MilesReacquireHandles, 0)
XOBJSTUB(MilesXtra::m_MilesShutdown, 0)
XOBJSTUB(MilesXtra::m_MilesHookToSmacker, 0)
XOBJSTUB(MilesXtra::m_MilesSetWaveOutput, 0)
XOBJSTUB(MilesXtra::m_MilesGetWaveDescription, "")

static MilesXtraObject *getMiles() {
	return static_cast<MilesXtraObject *>(g_lingo->_state->me.u.obj);
}

static void openWaveFile(int nargs) {
	MilesXtraObject *me = getMiles();
	Common::String fileName = g_lingo->pop().asString();
	g_lingo->dropStack(nargs - 1);
	me->closeSound();

	Common::Path path = findPath(fileName);
	Common::File *file = new Common::File();
	if (path.empty() || !file->open(path)) {
		warning("MilesXtra: cannot open '%s'", fileName.c_str());
		delete file;
		g_lingo->push(Datum(0));
		return;
	}
	me->_stream = Audio::makeWAVStream(file, DisposeAfterUse::YES);
	g_lingo->push(Datum(me->_stream ? 1 : 0));
}

void MilesXtra::m_MilesOpenWave(int nargs) {
	openWaveFile(nargs);
}

void MilesXtra::m_MilesOpenStream(int nargs) {
	openWaveFile(nargs);
}

void MilesXtra::m_MilesOpenWaveCastmember(int nargs) {
	MilesXtraObject *me = getMiles();
	Common::String memberName = g_lingo->pop().asString();
	Common::String castLibName = g_lingo->pop().asString();
	g_lingo->dropStack(nargs - 2);
	me->closeSound();

	Movie *movie = g_director->getCurrentMovie();
	int castLib = movie->getCastLibIDByName(castLibName);
	CastMemberID id = movie->getCastMemberIDByNameAndType(memberName, castLib, kCastSound);
	SoundCastMember *member = (SoundCastMember *)movie->getCastMember(id);
	if (!member || member->_type != kCastSound) {
		warning("MilesXtra: no sound member '%s' in castLib '%s'", memberName.c_str(), castLibName.c_str());
		g_lingo->push(Datum(0));
		return;
	}
	member->load();
	Audio::AudioStream *stream = member->_audio ? member->_audio->getAudioStream(false, false, DisposeAfterUse::YES) : nullptr;
	me->_stream = dynamic_cast<Audio::SeekableAudioStream *>(stream);
	if (!me->_stream)
		delete stream;
	g_lingo->push(Datum(me->_stream ? 1 : 0));
}

void MilesXtra::m_MilesPlay(int nargs) {
	MilesXtraObject *me = getMiles();
	g_lingo->dropStack(nargs);
	if (me->_stream) {
		Audio::Mixer *mixer = g_system->getMixer();
		mixer->stopHandle(me->_handle);
		me->_stream->rewind();
		// Miles: loop count 0 means forever
		Audio::AudioStream *playStream = me->_stream;
		DisposeAfterUse::Flag dispose = DisposeAfterUse::NO;
		if (me->_loopCount != 1) {
			playStream = new Audio::LoopingAudioStream(me->_stream, me->_loopCount, DisposeAfterUse::NO);
			dispose = DisposeAfterUse::YES;
		}
		mixer->playStream(Audio::Mixer::kSFXSoundType, &me->_handle, playStream, -1,
			CLIP(me->_volume * 2, 0, 255), 0, dispose);
	}
	g_lingo->push(Datum(0));
}

void MilesXtra::m_MilesPause(int nargs) {
	g_lingo->dropStack(nargs);
	g_system->getMixer()->pauseHandle(getMiles()->_handle, true);
	g_lingo->push(Datum(0));
}

void MilesXtra::m_MilesResume(int nargs) {
	g_lingo->dropStack(nargs);
	g_system->getMixer()->pauseHandle(getMiles()->_handle, false);
	g_lingo->push(Datum(0));
}

void MilesXtra::m_MilesClose(int nargs) {
	g_lingo->dropStack(nargs);
	getMiles()->closeSound();
	g_lingo->push(Datum(0));
}

void MilesXtra::m_MilesGetStatus(int nargs) {
	g_lingo->dropStack(nargs);
	// The game treats 4 as "playing"
	g_lingo->push(Datum(g_system->getMixer()->isSoundHandleActive(getMiles()->_handle) ? 4 : 2));
}

void MilesXtra::m_MilesGetVolume(int nargs) {
	g_lingo->dropStack(nargs);
	g_lingo->push(Datum(getMiles()->_volume));
}

void MilesXtra::m_MilesSetVolume(int nargs) {
	MilesXtraObject *me = getMiles();
	me->_volume = g_lingo->pop().asInt();
	g_lingo->dropStack(nargs - 1);
	g_system->getMixer()->setChannelVolume(me->_handle, CLIP(me->_volume * 2, 0, 255));
	g_lingo->push(Datum(0));
}

void MilesXtra::m_MilesGetLoopCount(int nargs) {
	g_lingo->dropStack(nargs);
	g_lingo->push(Datum(getMiles()->_loopCount));
}

void MilesXtra::m_MilesSetLoopCount(int nargs) {
	MilesXtraObject *me = getMiles();
	me->_loopCount = g_lingo->pop().asInt();
	g_lingo->dropStack(nargs - 1);
	g_lingo->push(Datum(0));
}

} // End of namespace Director
