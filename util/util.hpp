#pragma once

#include <stdio.h>

#define STR(x) #x
#define WHERE printf("Line %i, File %s\n", __LINE__, __FILE__);
