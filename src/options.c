#include "options.h"

#define C_OPTION(c, f, ...)                                                    \
	if (argv[i][j] == c) {                                                     \
		f(__VA_ARGS__);                                                        \
	}

#define S_OPTION(s, f, ...)                                                    \
	if (strcmp(&argv[i][2], s) == 0) {                                         \
		f(__VA_ARGS__);                                                        \
	}

Options opt;

void clean_dir(char* dir)
{
	int l = strlen(dir);
	if (dir[l - 1] == '/')
		dir[l - 1] = '\0';
}

void setStow()
{
	opt.type = Stow;
}

void setRestow()
{
	opt.type = ReStow;
}

void setDelete()
{
	opt.type = Delete;
}

void setVerbose()
{
	opt.verbose = true;
}

void setMkdir()
{
	opt.mkdir = true;
}

void setTarget(const char* path)
{
	opt.target_dir = realpath(path, NULL);
	clean_dir(opt.target_dir);
}

void check_errors()
{
	if (opt.type == NoStow) {
		fputs("You need to define a Stow action (Stow, ReStow, or Delete)\n",
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

// Unoptimized, but pretty (so I'll let it go)
void parse_options(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				S_OPTION("target", setTarget, argv[++i])
				S_OPTION("stow", setStow)
				S_OPTION("restow", setRestow)
				S_OPTION("delete", setDelete)
				S_OPTION("verbose", setVerbose)
				S_OPTION("mkdir", setMkdir)
			}
			else {
				for (int j = 1; j < strlen(argv[i]); ++j) {
					C_OPTION('t', setTarget, argv[++i])
					C_OPTION('S', setStow)
					C_OPTION('R', setRestow)
					C_OPTION('D', setDelete)
					C_OPTION('v', setVerbose)
					C_OPTION('d', setMkdir)
				}
			}
		}
		else {
			opt.package_dir = realpath(argv[i], NULL);
			clean_dir(opt.package_dir);
		}
	}
	check_errors();
}

void free_options()
{
	free(opt.target_dir);
	free(opt.package_dir);
}
