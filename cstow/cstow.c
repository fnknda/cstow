#include "logging.h"
#include "options.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

void walk_dir(const char* dir_path);

int main(int argc, char* argv[])
{
	parse_options(argc, argv);
	walk_dir(opt.package_dir);
	free_options();
}

void make_symlink(const char* file_path)
{
	const char* relative_path = &file_path[strlen(opt.package_dir)];
	char target_path[strlen(opt.target_dir) + strlen(relative_path) + 1];
	strcpy(target_path, opt.target_dir);
	strcat(target_path, relative_path);

	struct stat target_stat;
	if (lstat(target_path, &target_stat) == 0) {
		LOGE("File %s already exists", target_path);
		return;
	}
	else if (errno != ENOENT) {
		LOGE("Could not stat target file %s (%s)", target_path, strerror(errno));
		return;
	}

	LOGD("Creating symlink %s -> %s", target_path, file_path);
	symlink(file_path, target_path);
}

void delete_symlink(const char* file_path)
{
	const char* relative_path = &file_path[strlen(opt.package_dir)];
	char target_path[strlen(opt.target_dir) + strlen(relative_path) + 1];
	strcpy(target_path, opt.target_dir);
	strcat(target_path, relative_path);

	struct stat target_stat;
	if (lstat(target_path, &target_stat) == -1) {
		if (opt.type != ReStow || errno != ENOENT)
			LOGE("Could not stat target file %s", target_path);
		return;
	}

	if ((target_stat.st_mode & S_IFMT) != S_IFLNK) {
		LOGW("Didn't delete %s, it wasn't a link", target_path);
		return;
	}

	LOGD("Deleting symlink %s", target_path);
	unlink(target_path);
}

void walk_dir(const char* dir_path)
{
	const char* relative_path = &dir_path[strlen(opt.package_dir)];
	char target_path[strlen(opt.target_dir) + strlen(relative_path) + 1];
	strcpy(target_path, opt.target_dir);
	strcat(target_path, relative_path);

	if (access(target_path, F_OK) != 0) {
		if (opt.mkdir) {
			LOGD("Creating directory %s", target_path);
			mkdir(target_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
		else {
			LOGD("Creating directory symlink %s -> %s", target_path, dir_path);
			symlink(dir_path, target_path);
			return;
		}
	}

	DIR* dir = opendir(dir_path);
	if (!dir) {
		perror("opendir");
		exit(1);
	}

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		char entry_path[strlen(dir_path) + strlen(entry->d_name) + 2];
		strcpy(entry_path, dir_path);
		strcat(entry_path, "/");
		strcat(entry_path, entry->d_name);

		if (entry->d_type & DT_DIR) {
			walk_dir(entry_path);
		}
		else if (entry->d_type & DT_REG) {
			if (opt.type == Delete || opt.type == ReStow) {
				delete_symlink(entry_path);
			}
			if (opt.type == Stow || opt.type == ReStow) {
				make_symlink(entry_path);
			}
		}
	}

	closedir(dir);
}
