/**
 * @file constants
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_CONSTANTS_HPP
#define ART_CONSTANTS_HPP

#include <vector>

namespace art {

using partial_key_type = uint8_t;
using key_type = std::vector<partial_key_type>;

} // namespace art

#endif
