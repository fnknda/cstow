#include "logging.h"
#include "options.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

void walk_dir(const char *dir_path);

int main(int argc, char *argv[])
{
	parse_options(argc, argv);

	LOGD("Package directory: %s", opt.package_dir);
	LOGD("Target directory: %s", opt.target_dir);

	walk_dir(opt.package_dir);
	free_options();
}

void make_symlink(const char *package_path, const char *target_path)
{
	errno = 0;
	if (!opt.dry && access(target_path, F_OK) == 0) {
		LOGE("File %s already exists", target_path);
		return;
	}
	else if (errno && errno != ENOENT) {
		LOGE("Could not stat target file %s", target_path);
		return;
	}

	LOGD("Creating symlink %s -> %s", target_path, package_path);
	if (!opt.dry) {
		symlink(package_path, target_path);
	}
}

void delete_symlink(const char *target_path)
{
	struct stat target_stat;
	if (lstat(target_path, &target_stat) == -1) {
		if (opt.type != ReStow || errno != ENOENT)
			LOGE("Could not stat target file %s", target_path);
		return;
	}

	if (!opt.force && (target_stat.st_mode & S_IFMT) != S_IFLNK) {
		LOGW("Didn't delete %s, it wasn't a symlink", target_path);
		return;
	}

	LOGD("Deleting symlink/regular file %s", target_path);
	if (!opt.dry) {
		unlink(target_path);
	}
}

void walk_dir(const char *package_dir_path)
{
	const char *relative_path = &package_dir_path[strlen(opt.package_dir)];
	char target_dir_path[strlen(opt.target_dir) + strlen(relative_path) + 1];
	sprintf(target_dir_path, "%s%s", opt.target_dir, relative_path);

	if (opt.type == Delete || opt.type == ReStow) {
		struct stat target_dir_stat;
		if (lstat(target_dir_path, &target_dir_stat) == 0) {
			if (S_ISLNK(target_dir_stat.st_mode)) {
				LOGD("Deleting directory symlink '%s'", target_dir_path);
				delete_symlink(target_dir_path);
				if (opt.type == Delete) {
					return;
				}
			}
		}
	}

	if ((opt.type == Stow || opt.type == ReStow) && access(target_dir_path, F_OK) != 0) {
		if (opt.mkdir) {
			LOGD("Creating directory %s", target_dir_path);
			mkdir(target_dir_path, 0775);
		}
		else {
			LOGD("Creating directory symlink '%s' -> '%s'", target_dir_path, package_dir_path);
			symlink(package_dir_path, target_dir_path);
			return;
		}
	}

	DIR *dir = opendir(package_dir_path);
	if (!dir) {
		perror("opendir");
		exit(1);
	}

	struct dirent *entry;
	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		char package_path[strlen(package_dir_path) + strlen(entry->d_name) + 2];
		sprintf(package_path, "%s/%s", package_dir_path, entry->d_name);

		if (entry->d_type & DT_DIR) {
			walk_dir(package_path);
		}
		else if (entry->d_type & DT_REG) {
			const char *relative_path = &package_path[strlen(opt.package_dir)];
			char target_path[strlen(opt.target_dir) + strlen(relative_path) + 1];
			sprintf(target_path, "%s%s", opt.target_dir, relative_path);

			if (opt.type == Delete || opt.type == ReStow) {
				delete_symlink(target_path);
			}
			if (opt.type == Stow || opt.type == ReStow) {
				make_symlink(package_path, target_path);
			}
		}
	}

	// Attempt to delete empty directories
	if (opt.type == Delete) {
		rmdir(target_dir_path);
	}

	closedir(dir);
}
