#include <string>
#pragma once
#ifndef UTILITIES_H_
#define UTILITIES_H_

using namespace std;

void progress_bar(float progress, string message = "");
void progress_percent(float progress);
float round(float number, int decimal_places);

#endif
