/*
 * ParseWebData_local.h
 *
 *  Created on: Jul 10, 2012
 *      Author: cyril
 */

#ifndef PARSEWEBDATA_LOCAL_H_
#define PARSEWEBDATA_LOCAL_H_

#include <string>
#include <map>
#include <list>

namespace ParseWebData {

/*!
 * Map string -> string
 */
typedef std::map<std::string, std::string> string_map;

/*!
 * List of strings
 */
typedef std::list<std::string> string_list;


string_list split(const std::string& s, const std::string& delim,
		bool keep_empty = true);
string_map map_pairs(const std::string& s, const std::string& elemDelim, const std::string& pairDelim);


}  // namespace ParseWebData



#endif /* PARSEWEBDATA_LOCAL_H_ */
