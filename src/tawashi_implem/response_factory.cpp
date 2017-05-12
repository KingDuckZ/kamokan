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

#include "response_factory.hpp"
#include "settings_bag.hpp"
#include "cgi_env.hpp"
#include <functional>
#include <boost/container/flat_map.hpp>

namespace tawashi {
	namespace {
	} //unnamed namespace

	struct ResponseFactory::LocalData {
		Kakoune::SafePtr<SettingsBag> settings;
		boost::container::flat_map<std::string, ResponseMakerFunc> makers;
		ResponseMakerFunc jolly_maker;
		Kakoune::SafePtr<cgi::Env> cgi_env;
	};

	ResponseFactory::ResponseFactory (const Kakoune::SafePtr<SettingsBag>& parSettings, const Kakoune::SafePtr<cgi::Env>& parCgiEnv) :
		m_local_data(std::make_unique<LocalData>())
	{
		m_local_data->settings = parSettings;
		m_local_data->cgi_env = parCgiEnv;
	}

	ResponseFactory::~ResponseFactory() noexcept = default;

	std::unique_ptr<Response> ResponseFactory::make_response (const boost::string_ref& parName) {
		//spdlog::get("statuslog")->info("making response object for \"{}\"", parName);

		auto maker_it = m_local_data->makers.find(std::string(parName.data(), parName.size()));
		Kakoune::SafePtr<ResponseFactory> self(this);
		if (m_local_data->makers.end() != maker_it) {
			return maker_it->second(self, m_local_data->settings, m_local_data->cgi_env);
		}
		else if (m_local_data->jolly_maker) {
			return m_local_data->jolly_maker(self, m_local_data->settings, m_local_data->cgi_env);
		}
		else {
			assert(false);
			return std::unique_ptr<Response>();
		}
	}

	void ResponseFactory::register_maker (std::string&& parName, ResponseMakerFunc parMaker) {
		m_local_data->makers[std::move(parName)] = parMaker;
	}

	void ResponseFactory::register_jolly_maker (ResponseMakerFunc parMaker) {
		m_local_data->jolly_maker = parMaker;
	}
} //namespace tawashi
