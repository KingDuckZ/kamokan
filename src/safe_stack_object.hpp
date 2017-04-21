/*
	Copyright 2016, 2017 Michele "King_DuckZ" Santullo

	This file is part of MyCurry.

	MyCurry is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	MyCurry is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with MyCurry.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "kakoune/safe_ptr.hh"
#include <utility>

namespace curry {
	template <typename T>
	class SafeStackObject {
	public:
		typedef Kakoune::SafePtr<T> safe_ptr;

		SafeStackObject();
		SafeStackObject (SafeStackObject&& parOther);
		SafeStackObject (const SafeStackObject& parOther) = delete;
		template <typename... Args> explicit SafeStackObject (Args&&... parArgs);
		~SafeStackObject() noexcept = default;

		SafeStackObject& operator= (SafeStackObject&& parOther) = delete;
		SafeStackObject& operator= (const SafeStackObject& parOther) = delete;

		operator Kakoune::SafePtr<T>&();
		T& operator*();
		safe_ptr& operator->();

	private:
		T m_obj;
		safe_ptr m_obj_ptr;
	};

	template <typename T>
	SafeStackObject<T>::SafeStackObject() :
		m_obj(),
		m_obj_ptr(&m_obj)
	{
	}

	template <typename T>
	SafeStackObject<T>::SafeStackObject (SafeStackObject&& parOther) :
		m_obj(std::move(parOther.m_obj)),
		m_obj_ptr(&m_obj)
	{
	}

	template <typename T>
	template <typename... Args>
	SafeStackObject<T>::SafeStackObject (Args&&... parArgs) :
		m_obj(std::forward<Args>(parArgs)...),
		m_obj_ptr(&m_obj)
	{
	}

	//template <typename T>
	//SafeStackObject& SafeStackObject<T>::operator= (SafeStackObject&& parOther) {
	//	m_obj = std::move(parOther.m_obj);
	//	m_obj_ptr = std::move(parOther.m_obj_ptr);
	//	m_ob
	//}

	//template <typename T>
	//SafeStackObject& SafeStackObject<T>::operator= (const SafeStackObject& parOther) {
	//}

	template <typename T>
	SafeStackObject<T>::operator Kakoune::SafePtr<T>&() {
		return m_obj_ptr;
	}

	template <typename T>
	T& SafeStackObject<T>::operator*() {
		return *m_obj_ptr;
	}

	template <typename T>
	auto SafeStackObject<T>::operator->() -> safe_ptr& {
		return m_obj_ptr;
	}
} //namespace curry
