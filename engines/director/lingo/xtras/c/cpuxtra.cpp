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

#include "common/system.h"

#include "director/director.h"
#include "director/lingo/lingo.h"
#include "director/lingo/lingo-object.h"
#include "director/lingo/lingo-utils.h"
#include "director/lingo/xtras/c/cpuxtra.h"

/**************************************************
 *
 * USED IN:
 * Kitchenette (Le Lab, Windows)
 *
 **************************************************/

/*
-- xtra Cpuxtra
new object me
-- CpuXtra --
* CpuId string str-- Global level cpuid method : returns cpu type, speed & features
 */

namespace Director {

const char *const CpuXtra::xlibName = "Cpuxtra";
const XlibFileDesc CpuXtra::fileNames[] = {
	{ "cpuxtra",	nullptr },
	{ nullptr,		nullptr },
};

static const MethodProto xlibMethods[] = {
	{ "new",	CpuXtra::m_new,	0, 0,	500 },
	{ nullptr, nullptr, 0, 0, 0 }
};

static const BuiltinProto xlibBuiltins[] = {
	{ "CpuId",	CpuXtra::m_CpuId,	1, 1,	500, HBLTIN },
	{ nullptr, nullptr, 0, 0, 0, VOIDSYM }
};

CpuXtraObject::CpuXtraObject(ObjectType ObjectType) : Object<CpuXtraObject>("Cpuxtra") {
	_objType = ObjectType;
}

bool CpuXtraObject::hasProp(const Common::String &propName) {
	return (propName == "name");
}

Datum CpuXtraObject::getProp(const Common::String &propName) {
	if (propName == "name")
		return Datum(CpuXtra::xlibName);
	warning("CpuXtra::getProp: unknown property '%s'", propName.c_str());
	return Datum();
}

void CpuXtra::open(ObjectType type, const Common::Path &path) {
	CpuXtraObject::initMethods(xlibMethods);
	CpuXtraObject *xobj = new CpuXtraObject(type);
	if (type == kXtraObj) {
		g_lingo->_openXtras.push_back(xlibName);
		g_lingo->_openXtraObjects.push_back(xobj);
	}
	g_lingo->exposeXObject(xlibName, xobj);
	g_lingo->initBuiltIns(xlibBuiltins);
}

void CpuXtra::close(ObjectType type) {
	CpuXtraObject::cleanupMethods();
	g_lingo->_globalvars[xlibName] = Datum();
}

void CpuXtra::m_new(int nargs) {
	g_lingo->printSTUBWithArglist("CpuXtra::m_new", nargs);
	g_lingo->dropStack(nargs);
	g_lingo->push(g_lingo->_state->me);
}

static void addProp(Datum &list, const char *name, int value) {
	Datum prop = Datum(Common::String(name));
	prop.type = SYMBOL;
	list.u.parr->arr.push_back(PCell(prop, Datum(value)));
}

void CpuXtra::m_CpuId(int nargs) {
	g_lingo->dropStack(nargs);

	// Returns [#CPU, #FPU, #SPEED, #MMX]: a Pentium MMX at 233 MHz passes any period check
	Datum result;
	result.type = PARRAY;
	result.u.parr = new PArray;
	addProp(result, "CPU", 5);
	addProp(result, "FPU", 1);
	addProp(result, "SPEED", 233);
	addProp(result, "MMX", 1);
	g_lingo->push(result);
}

} // End of namespace Director
