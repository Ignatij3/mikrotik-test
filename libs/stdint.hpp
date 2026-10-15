#ifndef MIKROTIK_IOT_TEST_LIBS_STDINT_HPP
#define MIKROTIK_IOT_TEST_LIBS_STDINT_HPP

// signed integer types

typedef __INT8_TYPE__  int8;
typedef __INT16_TYPE__ int16;
typedef __INT32_TYPE__ int32;
typedef __INT64_TYPE__ int64;

constexpr int8  int8_MAX  = __INT8_MAX__;
constexpr int8  int8_MIN  = (-__INT8_MAX__ - 1);
constexpr int16 int16_MAX = __INT16_MAX__;
constexpr int16 int16_MIN = (-__INT16_MAX__ - 1);
constexpr int32 int32_MAX = __INT32_MAX__;
constexpr int32 int32_MIN = (-__INT32_MAX__ - 1);
constexpr int64 int64_MAX = __INT64_MAX__;
constexpr int64 int64_MIN = (-__INT64_MAX__ - 1);

// unsigned integer types

typedef __UINT8_TYPE__  uint8;
typedef __UINT16_TYPE__ uint16;
typedef __UINT32_TYPE__ uint32;
typedef __UINT64_TYPE__ uint64;

constexpr uint8  uint8_MAX  = __UINT8_MAX__;
constexpr uint16 uint16_MAX = __UINT16_MAX__;
constexpr uint32 uint32_MAX = __UINT32_MAX__;
constexpr uint64 uint64_MAX = __UINT64_MAX__;

#endif  // MIKROTIK_IOT_TEST_LIBS_STDINT_HPP