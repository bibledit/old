/*
 * ParseMultipartFormData.h
 *
 *  Created on: Jul 10, 2012
 *      Author: cyril
 */

#ifndef PARSEMULTIPARTFORMDATA_H_
#define PARSEMULTIPARTFORMDATA_H_

#include "ParseWebData_local.h"

namespace ParseWebData {
namespace ParseMultipartFormData {

bool parse_data(const std::string& data,
		const string_map& content_type, WebDataMap& dataMap);


}  // namespace ParseMultipartFormData
}  // namespace ParseWebData

#endif /* PARSEMULTIPARTFORMDATA_H_ */
