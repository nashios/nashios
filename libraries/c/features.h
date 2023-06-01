#pragma once

#undef __USE_FILE_OFFSET64

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define __USE_FILE_OFFSET64 1
#endif
