/*
 * ParseWebData_local.h
 *
 *  Created on: Jul 10, 2012
 *      Author: cyril
 */

#ifndef ParseWebData_H
#define ParseWebData_H

#include <string>
#include <map>

/*!
 * Parse Web Data namespace
 */
namespace ParseWebData {

/*!
 * Map string -> string
 */
typedef std::map<std::string, std::string> string_map;

/*!
 * Passed variable structure
 */
struct WebData {
	WebData() {
	}
	WebData(const std::string& _value) :
			value(_value) {
	}
	/*!
	 * Value of variable
	 */
	std::string value;
	/*!
	 * Additional attributes of variable
	 */
	string_map attributes;
};

/*!
 * Map Variable name to variable structure
 */
typedef std::map<std::string, WebData> WebDataMap;

/*!
 * Parse data from POST request
 * @param[in]  postData POST request to be parsed
 * @param[in]  contentType contents of Content-Type header
 * @param[out] postDataMap map of Variable->(Value+attribs)
 * @return success of parsing
 */
bool parse_post_data(const std::string& postData,
		const std::string& contentType, WebDataMap& postDataMap);

/*!
 * Parse data from GET request (query string)
 * @param[in]  getData
 * @param[out] postDataMap
 * @return success of parsing
 */
bool parse_get_data(const std::string& getData, WebDataMap& postDataMap);

} // namespace ParseWebData

#endif
