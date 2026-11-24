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

#include "base/plugins.h"

#include "engines/advancedDetector.h"

#include "common/config-manager.h"
#include "common/file.h"
#include "common/translation.h"

#include "backends/keymapper/action.h"
#include "backends/keymapper/keymap.h"

#include "director/director.h"

namespace Director {

DirectorGameGID DirectorEngine::getGameGID() const {
	return _gameDescription->gameGID;
}

const char *DirectorEngine::getGameId() const {
	return _gameDescription->desc.gameId;
}

Common::Platform DirectorEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

uint32 DirectorEngine::getGameFlags() const {
	return _gameDescription->desc.flags;
}

bool DirectorEngine::isDemo() const {
	return (bool)(_gameDescription->desc.flags & ADGF_DEMO);
}

uint16 DirectorEngine::getDescriptionVersion() const {
	return _gameDescription->version;
}

Common::Language DirectorEngine::getLanguage() const {
	return _gameDescription->desc.language;
}

const char *DirectorEngine::getExtra() {
	return _gameDescription->desc.extra;
}

bool DirectorEngine::hasFeature(EngineFeature f) const {
	return false;
		//(f == kSupportsReturnToLauncher);
}

static const ADExtraGuiOptionsMap optionsList[] = {
	{
		GAMEOPTION_GAMMA_CORRECTION,
		{
			_s("Enable gamma correction"),
			_s("Brighten the graphics to simulate a Macintosh monitor"),
			"gamma_correction",
			true,
			0,
			0
		}
	},
	{
		GAMEOPTION_TRUE_COLOR,
		{
			_s("Force true color (32bpp) mode"),
			_s("Use true color graphics mode (32 bits per pixel), even if the game is not designed for it"),
			"true_color",
			false,
			0,
			0
		}
	},
	AD_EXTRA_GUI_OPTIONS_TERMINATOR
};

} // End of Namespace Director

class DirectorMetaEngine : public AdvancedMetaEngine<Director::DirectorGameDescription> {
public:
	const char *getName() const override {
		return "director";
	}

	Common::Error createInstance(OSystem *syst, Engine **engine, const Director::DirectorGameDescription *desc) const override;
	virtual const ADExtraGuiOptionsMap *getAdvancedExtraGuiOptions() const override { return Director::optionsList; }
	Common::KeymapArray initKeymaps(const char *target) const override;
};

Common::KeymapArray DirectorMetaEngine::initKeymaps(const char *target) const {
	using namespace Common;

	KeymapArray keymaps = MetaEngine::initKeymaps(target);

	const String currDomain = ConfMan.getActiveDomainName();
	ConfMan.setActiveDomain(target);
	const String gameId = ConfMan.get("gameid");
	ConfMan.setActiveDomain(currDomain);

	if (gameId == "kitchenette") {
		// The game hardcodes the players' buzzers as the A, N and P keys.
		// Expose them as actions so they can be remapped to any key, mouse
		// button or gamepad button.
		Keymap *keymap = new Keymap(Keymap::kKeymapTypeGame, "kitchenette", _("Kitchenette buzzers"));
		static const struct {
			const char *id;
			const char *desc;
			KeyCode code;
			uint16 ascii;
			const char *key;
		} buzzers[] = {
			{ "BUZZER1", _s("Player 1 buzzer"), KEYCODE_a, 'a', "a" },
			{ "BUZZER2", _s("Player 2 buzzer"), KEYCODE_n, 'n', "n" },
			{ "BUZZER3", _s("Player 3 buzzer"), KEYCODE_p, 'p', "p" },
		};
		for (const auto &b : buzzers) {
			Action *act = new Action(b.id, _(b.desc));
			act->setKeyEvent(KeyState(b.code, b.ascii));
			act->addDefaultInputMapping(b.key);
			keymap->addAction(act);
		}
		keymaps.push_back(keymap);
	}

	return keymaps;
}

Common::Error DirectorMetaEngine::createInstance(OSystem *syst, Engine **engine, const Director::DirectorGameDescription *desc) const {
	*engine = new Director::DirectorEngine(syst,desc);
	return Common::kNoError;
}

#if PLUGIN_ENABLED_DYNAMIC(DIRECTOR)
	REGISTER_PLUGIN_DYNAMIC(DIRECTOR, PLUGIN_TYPE_ENGINE, DirectorMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(DIRECTOR, PLUGIN_TYPE_ENGINE, DirectorMetaEngine);
#endif
