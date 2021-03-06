/* Copyright 2017, Michele Santullo
 * This file is part of "kamokan".
 *
 * "kamokan" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "kamokan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "kamokan".  If not, see <http://www.gnu.org/licenses/>.
 */

#include "response_factory.hpp"
#include "settings_bag.hpp"
#include "cgi_env.hpp"
#include <functional>
#include <boost/container/flat_map.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <array>

namespace kamokan {
	namespace {
	} //unnamed namespace

	struct ResponseFactory::LocalData {
		Kakoune::SafePtr<SettingsBag> settings;
		boost::container::flat_map<std::string, ResponseMakerFunc> makers_get;
		boost::container::flat_map<std::string, ResponseMakerFunc> makers_post;
		std::array<ResponseMakerFunc, tawashi::RequestMethodType::_size()> jolly_makers;
		Kakoune::SafePtr<tawashi::cgi::Env> cgi_env;
		std::chrono::time_point<std::chrono::steady_clock> time0;
	};

	ResponseFactory::ResponseFactory (const Kakoune::SafePtr<SettingsBag>& parSettings, const Kakoune::SafePtr<tawashi::cgi::Env>& parCgiEnv) :
		m_local_data(std::make_unique<LocalData>())
	{
		m_local_data->time0 = std::chrono::steady_clock::now();
		m_local_data->settings = parSettings;
		m_local_data->cgi_env = parCgiEnv;
		std::fill(m_local_data->jolly_makers.begin(), m_local_data->jolly_makers.end(), nullptr);
	}

	ResponseFactory::~ResponseFactory() noexcept = default;

	std::unique_ptr<Response> ResponseFactory::make_response (const boost::string_view& parName, tawashi::RequestMethodType parReqType) {
		std::string name(parName.data(), parName.size());
		spdlog::get("statuslog")->info(
			"making response object for \"{}\" method {}",
			name,
			parReqType._to_string()
		);

		const auto& makers = (
			static_cast<tawashi::RequestMethodType>(tawashi::RequestMethodType::POST) == parReqType ?
					m_local_data->makers_post : m_local_data->makers_get
			);

		auto maker_it = makers.find(name);
		if (makers.end() != maker_it) {
			return make_response(maker_it->second);
		}
		else if (m_local_data->jolly_makers[parReqType]) {
			spdlog::get("statuslog")->info("no exact match found for \"{}\", assuming it's a pastie's token", name);
			return make_response(m_local_data->jolly_makers[parReqType]);
		}
		else {
			spdlog::get("statuslog")->critical("no exact match found for \"{}\" with method {} and no jolly maker given, this should not happen", name, parReqType._to_string());
			return std::unique_ptr<Response>();
		}
	}

	std::unique_ptr<Response> ResponseFactory::make_response (ResponseMakerFunc parMaker) const {
		auto retval = parMaker(m_local_data->settings, m_local_data->cgi_env);
		assert(retval);
		retval->set_app_start_time(m_local_data->time0);
		return retval;
	}

	void ResponseFactory::register_maker (std::string&& parName, ResponseMakerFunc parMaker) {
		m_local_data->makers_get[parName] = parMaker;
		m_local_data->makers_post[std::move(parName)] = parMaker;
	}

	void ResponseFactory::register_maker (std::string&& parName, tawashi::RequestMethodType parReqType, ResponseMakerFunc parMaker) {
		switch (parReqType) {
		case tawashi::RequestMethodType::GET:
			m_local_data->makers_get[std::move(parName)] = parMaker;
			break;
		case tawashi::RequestMethodType::POST:
			m_local_data->makers_post[std::move(parName)] = parMaker;
			break;
		};
	}

	void ResponseFactory::register_jolly_maker (ResponseMakerFunc parMaker, tawashi::RequestMethodType parReqType) {
		m_local_data->jolly_makers[parReqType] = parMaker;
	}

	void ResponseFactory::set_app_start_time (const std::chrono::time_point<std::chrono::steady_clock>& parTime) {
		assert(parTime <= m_local_data->time0);
		m_local_data->time0 = parTime;
	}
} //namespace kamokan
