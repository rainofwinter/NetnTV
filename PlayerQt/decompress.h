#pragma once
#include "unzip.h"

int do_extract_currentfile(unzFile uf, bool popt_extract_without_path, const char * password);
int do_extract(unzFile uf, int opt_extract_without_path, const char * password);