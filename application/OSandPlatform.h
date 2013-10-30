#ifndef _OSandPlatform_INCLUDED
#define _OSandPlatform_INCLUDED

/// \addtogroup lowlevel
/// @{

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <strings.h>

#ifdef __cplusplus
extern "C" {
#endif

//Chip driver status return values
typedef int32_t ChipDriverStatus_t;
#define SUCCESS 0
#define FAILURE 1
#define SATURATED 101

#ifndef RELEASE
#define ASSERT(EXPR) assert(EXPR)
#else
#define ASSERT(EXPR)
#endif

#ifdef __cplusplus
}
#endif



#endif //OSandPlatform_INCLUDED

/// @}
