#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum StowType
{
	NoStow = 0,
	Stow,
	Delete,
	ReStow,
};

typedef struct {
	bool verbose;
	bool mkdir;
	char* target_dir;
	char* package_dir;
	enum StowType type;
} Options;

extern Options opt;

void parse_options(int argc, char* argv[]);
void free_options();

#endif
