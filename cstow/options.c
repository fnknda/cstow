#include "options.h"

#include <stdio.h>
#include <unistd.h>

#define CHAR_OPTION(c, f)                                                      \
	if (argv[i][j] == c) {                                                     \
		f;                                                                     \
	}

#define STRING_OPTION(s, f)                                                    \
	if (strcmp(&argv[i][2], s) == 0) {                                         \
		f;                                                                     \
	}

Options opt;

void clean_dir(char* dir)
{
	int l = strlen(dir);
	if (dir[l - 1] == '/')
		dir[l - 1] = '\0';
}

void setStow(void)
{
	opt.type = Stow;
}

void setRestow(void)
{
	opt.type = ReStow;
}

void setDelete(void)
{
	opt.type = Delete;
}

void setVerbose(void)
{
	opt.verbose = true;
}

void setMkdir(void)
{
	opt.mkdir = true;
}

void setPackageDir(const char* path)
{
	opt.target_dir = realpath(path, NULL);
	clean_dir(opt.target_dir);
}

void setTargetDir(const char* path)
{
	opt.target_dir = realpath(path, NULL);
	clean_dir(opt.target_dir);
}

void check_errors(void)
{
	bool errored = false;
	if (opt.type == NoStow) {
		fputs("You need to define a Stow action (Stow, ReStow, or Delete)\n",
		      stderr);
		errored = true;
	}
	else if (opt.package_dir == NULL) {
		fputs("Package directory not specified\n", stderr);
		errored = true;
	}
	else if (opt.target_dir == NULL) {
		fputs("Target directory not specified, please specify one with -t\n",
		      stderr);
		errored = true;
	}

	if (errored) {
		puts("You can use --help to find out more about how to run this "
		     "program");
		exit(1);
	}
}

void help(void)
{
	puts("CStow - Stow re implemented in C");
	puts("");
	puts("USAGE:");
	puts("  cstow (-S|-R|-D) [FLAGS] <package_dir>");
	puts("");
	puts("FLAGS:");
	puts("  --target, -t <target_dir> - Target is the base destination for the "
	     "symlinks [Default: ./]");
	puts("  --stow, -S                - Creates symlinks in Target linking to "
	     "respective files in Package");
	puts("  --delete, -D              - Deletes all symlinks in Target "
	     "refering to the files in Package");
	puts("  --restow, -R              - Same as a Delete followed by a Stow");
	puts("  --verbose, -v             - Shows actions being taken");
	puts("  --mkdir, -d               - Don't symlink directories, make them "
	     "instead");
	puts("  --help, -h                - This message (you just did it)");
	exit(0);
}

void parse_options(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				STRING_OPTION("target", setTargetDir(argv[++i]));
				STRING_OPTION("stow", setStow());
				STRING_OPTION("restow", setRestow());
				STRING_OPTION("delete", setDelete());
				STRING_OPTION("verbose", setVerbose());
				STRING_OPTION("mkdir", setMkdir());
				STRING_OPTION("help", help());
			}
			else {
				for (size_t j = 1; j < strlen(argv[i]); ++j) {
					CHAR_OPTION('t', setTargetDir(argv[++i]));
					CHAR_OPTION('S', setStow());
					CHAR_OPTION('R', setRestow());
					CHAR_OPTION('D', setDelete());
					CHAR_OPTION('v', setVerbose());
					CHAR_OPTION('d', setMkdir());
					CHAR_OPTION('h', help());
				}
			}
		}
		else {
			setPackageDir(realpath(argv[i], NULL));
		}
	}

	if (opt.target_dir == NULL)
		setTargetDir(getcwd(NULL, 0));

	check_errors();
}

void free_options(void)
{
	free(opt.target_dir);
	free(opt.package_dir);
}
