#pragma once
#include <chrono>

//#include "..\..\Server\Server\stdafx.h"

using namespace std;
using namespace chrono;

void InitializeNetwork();
void GetPointCloud(int *size, float **points);