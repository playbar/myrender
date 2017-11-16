// --~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~--
// Copyright 2013-2015 Qualcomm Technologies, Inc.
// All rights reserved.
// Confidential and Proprietary – Qualcomm Technologies, Inc.
// --~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~--
#pragma once

#if __hexagon__
#include "macrodefinitions_hexagon_1d.h"
#include "macrodefinitions_hexagon_2d.h"
#else
#include "converttype.h"
#include "macrodefinitions_1d.hh"
#include "macrodefinitions_2d.hh"
#include "vectype.h"
#endif

