#include "options.h"

Options opt;

void clean_dir(char *dir)
{
	int l = strlen(dir);
	if (dir[l - 1] == '/')
		dir[l - 1] = '\0';
}

void parse_options(int argc, char* argv[])
{
	memset(&opt, 0, sizeof(Options));

	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-S") == 0) {
			opt.type = Stow;
		}
		else if (strcmp(argv[i], "-R") == 0) {
			opt.type = ReStow;
		}
		else if (strcmp(argv[i], "-D") == 0) {
			opt.type = DelStow;
		}
		else if (strcmp(argv[i], "-v") == 0) {
			opt.verbose = true;
		}
		else if (strcmp(argv[i], "-d") == 0) {
			opt.mkdir = true;
		}
		else if (strcmp(argv[i], "-t") == 0) {
			opt.target_dir = realpath(argv[++i], NULL);
			clean_dir(opt.target_dir);
		}
		else {
			opt.package_dir = realpath(argv[i], NULL);
			clean_dir(opt.package_dir);
		}
	}

	if (opt.type == NoStow) {
		fputs("You need to define a Stow action (Stow, ReStow, or DelStow)\n",
		      stderr);
		exit(1);
	}
	else if (opt.package_dir == NULL) {
		fputs("Package directory not specified\n", stderr);
		exit(1);
	}
	else if (opt.target_dir == NULL) {
		fputs("Target directory not specified, please specify one with -t\n",
		      stderr);
		exit(1);
	}
}

void free_options()
{
	free(opt.target_dir);
	free(opt.package_dir);
}
