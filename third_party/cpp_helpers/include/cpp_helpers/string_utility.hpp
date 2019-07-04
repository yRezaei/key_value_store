#pragma once
#ifndef STRING_UTILITY_HPP
#define STRING_UTILITY_HPP

#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <regex>
#include <algorithm>
#include <cctype>

namespace str
{
	namespace internal_use
	{
		// change the function name to represnt the process 
		inline std::vector<std::pair<std::string::size_type, std::string::size_type>> split_string_by_chars(const std::string& str, char c1 = '{', char c2 = '}')
		{
			std::vector<std::pair<std::string::size_type, std::string::size_type>> result;
			for (auto i = 0u; i < str.size(); i++)
			{
				if (str[i] == c1)
				{
					auto id = str.find(c2, i);
					if (id != std::string::npos)
						result.emplace_back(std::make_pair(i, id));
				}

			}
			return result;
		}

		inline void insert_arg(std::vector<std::pair<std::string::size_type, std::string::size_type>>& pairs, std::uint32_t& str_id, std::uint32_t& pair_id, const std::string& str, std::stringstream& stream)
		{

		}

		template<typename T>
		inline void insert_arg(std::vector<std::pair<std::string::size_type, std::string::size_type>>& pairs, std::uint32_t& str_id, std::uint32_t& pair_id, const std::string& str, std::stringstream& stream, T first)
		{
			if (pair_id < pairs.size())
			{
				if (str_id < pairs[pair_id].first)
					stream << str.substr(str_id, pairs[pair_id].first - str_id) << first;
				else if (str_id == pairs[pair_id].first)
					stream << first;

				str_id = pairs[pair_id].second + 1;
				pair_id++;
			}
		}

		template<typename T, typename... Args>
		inline void insert_arg(std::vector<std::pair<std::string::size_type, std::string::size_type>>& pairs, std::uint32_t& str_id, std::uint32_t& pair_id, const std::string& str, std::stringstream& stream, T first, Args... args)
		{
			if (pair_id < pairs.size())
			{
				if (str_id < pairs[pair_id].first)
					stream << str.substr(str_id, pairs[pair_id].first - str_id) << first;
				else if (str_id == pairs[pair_id].first)
					stream << first;

				str_id = pairs[pair_id].second + 1;
				pair_id++;
			}

			insert_arg(pairs, str_id, pair_id, str, stream, args ...);
		}
	}

	template <typename... Args>
	inline std::string format(const std::string& str, Args ... args)
	{
		if (str.empty())
			return "";

		auto arggument_size = sizeof...(args);
		if (arggument_size == 0)
			return str;

		auto pairs = internal_use::split_string_by_chars(str);

		if (pairs.empty())
			return str;

		std::uint32_t str_id = 0;
		std::uint32_t pair_id = 0;

		std::stringstream stream;
		internal_use::insert_arg(pairs, str_id, pair_id, str, stream, args...);
		if (str_id < str.size())
			stream << str.substr(str_id);

		return stream.str();
	}

	inline std::vector<std::string> split(std::string const& source, std::string const& delims, bool allow_empty = true)
	{
		using iterator = std::regex_token_iterator<std::string::const_iterator>;
		std::vector<std::string> tokens;
		std::regex reg("[" + delims + "]");
		std::transform(
			iterator(source.begin(), source.end(), reg, -1),
			iterator(),
			std::back_inserter(tokens),
			[](auto m) { return std::string(m.first, m.second); }
		);

		if (!allow_empty) {
			auto new_end = std::remove_if(tokens.begin(), tokens.end(), [](std::string const& str) { return (str.empty() ? true : false); });
			tokens.erase(new_end, tokens.end());
		}

		return tokens;
	}

	inline std::string to_uppercase(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
		return str;
	}

	inline std::string to_lowercase(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		return str;
	}
}

#endif // !STRING_UTILITY_HPP
