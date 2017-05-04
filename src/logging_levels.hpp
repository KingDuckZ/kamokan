/* Copyright 2017, Michele Santullo
 * This file is part of "tawashi".
 *
 * "tawashi" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "tawashi" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "tawashi".  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "enum.h"
#include <spdlog/spdlog.h>

namespace tawashi {
	BETTER_ENUM(LoggingLevels, int,
		trace = spdlog::level::trace,
		debug = spdlog::level::debug,
		info = spdlog::level::info,
		warn = spdlog::level::warn,
		err = spdlog::level::err,
		critical = spdlog::level::critical,
		off = spdlog::level::off
	);
} //namespace tawashi
