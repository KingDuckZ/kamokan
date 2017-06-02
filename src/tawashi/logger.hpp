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

#include <ostream>
#include <boost/utility/string_ref.hpp>
#include <cassert>
#include <ciso646>

namespace tawashi {
	class Logger {
	public:
		explicit Logger (std::ostream* parStream);
		~Logger() noexcept;

		template <typename... Args>
		void log (int parLevel, const boost::string_ref& parFormat, Args&&... parArgs);

	private:
		static const constexpr int LogLevels = 3;

		std::array<std::ostream*, LogLevels> m_outs;
	};

	template <typename... Args>
	void Logger::log (int parLevel, const boost::string_ref& parFormat, Args&&... parArgs) {
		assert(parLevel >= 0 and parLevel < LogLevels);
		if (nullptr == m_outs[parLevel])
			return;

		bool percentage_seq = false;
		for (auto chara : parFormat) {
			if (percentage_seq) {
			}
			else {
			}
		}
	}
} //namespace tawashi
