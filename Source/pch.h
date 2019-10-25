/**************************************************************************************\
** File: pch.h
** Project: Pong
** Author: David Leksen
** Date:
**
** Header file for the precompiled header
**
\**************************************************************************************/
// D2_ASSERT_LEVEL:
//		0 Disable all
//		1 enable d2AssertRelease
//		2 enable d2Assert
//		3 enable d2AssertParanoid 
#define D2_ASSERT_LEVEL 2

#include <Box2D/Box2D.h>
#include <SDL.h>

// uncomment to disable assert():
// #define NDEBUG

#include <cassert>
#include <memory>
#include <iostream>	
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>