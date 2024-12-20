#include "options.h"
#include "logging.h"

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define CHAR_OPTION(c, f)                                                    \
	if (argv[i][j] == c) {                                                    \
		f;                                                                     \
		continue;                                                              \
	}

#define STRING_OPTION(s, f)                                                  \
	if (strcmp(&argv[i][2], s) == 0) {                                        \
		f;                                                                     \
		continue;                                                              \
	}

Options opt;

void strip_dir(char* dir)
{
	int l = strlen(dir);
	if (dir[l - 1] == '/')
		dir[l - 1] = '\0';
}

void setType(enum StowType type)
{
	opt.type = type;
}

void setVerbose(void)
{
	opt.log_level = LogDbg;
}

void setMkdir(void)
{
	opt.mkdir = true;
}

void setPackageDir(const char* path)
{
	opt.package_dir = realpath(path, NULL);
	if (!opt.package_dir) {
		LOGE("The package directory must exist!");
		exit(1);
	}
	strip_dir(opt.package_dir);
}

bool got_target_real_path = false;
void setTargetDir(const char* path)
{
	opt.target_dir = realpath(path, NULL);
	if (!opt.target_dir) {
		opt.target_dir = path;
		return;
	}
	strip_dir(opt.target_dir);
	got_target_real_path = true;
}

void check_errors(void)
{
	bool errored = false;
	if (opt.type == NoStow) {
		LOGE("You need to define a Stow action (Stow, ReStow, or Delete)");
		errored = true;
	}
	else if (opt.package_dir == NULL) {
		LOGE("Package directory not specified");
		errored = true;
	}
	else if (opt.target_dir == NULL) {
		LOGE("Target directory not specified, please specify one with -t");
	}
	else if (!got_target_real_path) {
		if (mkdir(opt.target_dir, 0755) == -1)
			errored = true;
		else
			setTargetDir(opt.target_dir);
	}

	if (errored) {
		LOGE("You can use --help to find out more about how to run this program");
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
	puts("  --target, -t <target_dir> - Target is the base destination for the symlinks [Default: ./]");
	puts("  --stow, -S                - Creates symlinks in Target linking to respective files in Package");
	puts("  --delete, -D              - Deletes all symlinks in Target refering to the files in Package");
	puts("  --restow, -R              - Same as a Delete followed by a Stow");
	puts("  --verbose, -v             - Shows actions being taken");
	puts("  --mkdir, -d               - Don't symlink directories, make them instead");
	puts("  --help, -h                - This message (you just did it)");
	exit(0);
}

void parse_options(int argc, char* argv[])
{
	opt.log_level = LogWrn;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				STRING_OPTION("target", setTargetDir(argv[++i]));
				STRING_OPTION("stow", setType(Stow));
				STRING_OPTION("restow", setType(ReStow));
				STRING_OPTION("delete", setType(Delete));
				STRING_OPTION("verbose", setVerbose());
				STRING_OPTION("mkdir", setMkdir());
				STRING_OPTION("help", help());
			}
			else {
				int next_i = i;
				for (size_t j = 1; j < strlen(argv[i]); ++j) {
					CHAR_OPTION('t', {
						setTargetDir(argv[i + 1]);
						next_i++;
					});
					CHAR_OPTION('S', setType(Stow));
					CHAR_OPTION('R', setType(ReStow));
					CHAR_OPTION('D', setType(Delete));
					CHAR_OPTION('v', setVerbose());
					CHAR_OPTION('d', setMkdir());
					CHAR_OPTION('h', help());
				}
				i = next_i;
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
