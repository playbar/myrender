// --~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~--
// Copyright 2013-2015 Qualcomm Technologies, Inc.
// All rights reserved.
// Confidential and Proprietary – Qualcomm Technologies, Inc.
// --~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~--
#pragma once

#define SYMPHONY_POINT_KERNEL_1D_1(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_2(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_3(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_4(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_5(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_6(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_7(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_8(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_9(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_10(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_11(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_12(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_13(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_14(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_15(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_16(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_17(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_18(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_19(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_20(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_21(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_22(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_23(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_24(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_25(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , arg25_type, arg25_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
      , arg25_type arg25_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_26(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , arg25_type, arg25_name \
    , arg26_type, arg26_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
      , arg25_type arg25_name \
      , arg26_type arg26_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_27(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , arg25_type, arg25_name \
    , arg26_type, arg26_name \
    , arg27_type, arg27_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
      , arg25_type arg25_name \
      , arg26_type arg26_name \
      , arg27_type arg27_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_28(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , arg25_type, arg25_name \
    , arg26_type, arg26_name \
    , arg27_type, arg27_name \
    , arg28_type, arg28_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
      , arg25_type arg25_name \
      , arg26_type arg26_name \
      , arg27_type arg27_name \
      , arg28_type arg28_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_29(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , arg25_type, arg25_name \
    , arg26_type, arg26_name \
    , arg27_type, arg27_name \
    , arg28_type, arg28_name \
    , arg29_type, arg29_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
      , arg25_type arg25_name \
      , arg26_type arg26_name \
      , arg27_type arg27_name \
      , arg28_type arg28_name \
      , arg29_type arg29_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

#define SYMPHONY_POINT_KERNEL_1D_30(fname, index_type, index, first_x, last_x \
    , arg1_type, arg1_name \
    , arg2_type, arg2_name \
    , arg3_type, arg3_name \
    , arg4_type, arg4_name \
    , arg5_type, arg5_name \
    , arg6_type, arg6_name \
    , arg7_type, arg7_name \
    , arg8_type, arg8_name \
    , arg9_type, arg9_name \
    , arg10_type, arg10_name \
    , arg11_type, arg11_name \
    , arg12_type, arg12_name \
    , arg13_type, arg13_name \
    , arg14_type, arg14_name \
    , arg15_type, arg15_name \
    , arg16_type, arg16_name \
    , arg17_type, arg17_name \
    , arg18_type, arg18_name \
    , arg19_type, arg19_name \
    , arg20_type, arg20_name \
    , arg21_type, arg21_name \
    , arg22_type, arg22_name \
    , arg23_type, arg23_name \
    , arg24_type, arg24_name \
    , arg25_type, arg25_name \
    , arg26_type, arg26_name \
    , arg27_type, arg27_name \
    , arg28_type, arg28_name \
    , arg29_type, arg29_name \
    , arg30_type, arg30_name \
    , ...) \
  int symphony_hexagon_##fname (index_type first_x, index_type last_x \
      , arg1_type arg1_name \
      , arg2_type arg2_name \
      , arg3_type arg3_name \
      , arg4_type arg4_name \
      , arg5_type arg5_name \
      , arg6_type arg6_name \
      , arg7_type arg7_name \
      , arg8_type arg8_name \
      , arg9_type arg9_name \
      , arg10_type arg10_name \
      , arg11_type arg11_name \
      , arg12_type arg12_name \
      , arg13_type arg13_name \
      , arg14_type arg14_name \
      , arg15_type arg15_name \
      , arg16_type arg16_name \
      , arg17_type arg17_name \
      , arg18_type arg18_name \
      , arg19_type arg19_name \
      , arg20_type arg20_name \
      , arg21_type arg21_name \
      , arg22_type arg22_name \
      , arg23_type arg23_name \
      , arg24_type arg24_name \
      , arg25_type arg25_name \
      , arg26_type arg26_name \
      , arg27_type arg27_name \
      , arg28_type arg28_name \
      , arg29_type arg29_name \
      , arg30_type arg30_name \
  ){ \
    for(index_type index = first_x; index < last_x; ++index) \
      __VA_ARGS__\
    return 0; \
  }

