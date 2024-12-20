#pragma once

#include "logging.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum StowType {
	NoStow = 0,
	Stow,
	Delete,
	ReStow,
};

typedef struct {
	char *target_dir;
	char *package_dir;
	enum StowType type;
	enum LogLevel log_level;
	bool mkdir;
	bool force;
	bool dry;
} Options;

extern Options opt;

void parse_options(int argc, char *argv[]);
void free_options(void);
