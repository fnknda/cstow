# CStow

> /sɛstō/

C implementation of the **stow** tool for managing symbolic links.

```
CStow - Stow re implemented in C

USAGE:
  cstow (-S|-R|-D) [FLAGS] <package_dir>

FLAGS:
  --target, -t <target_dir> - Target is the base destination for the symlinks [Default: ./]
  --stow, -S                - Creates symlinks in Target linking to respective files in Package
  --delete, -D              - Deletes all symlinks in Target refering to the files in Package
  --restow, -R              - Same as a Delete followed by a Stow
  --verbose, -v             - Shows actions being taken
  --mkdir, -d               - Don't symlink directories, make them instead
  --force, -f               - Delete and ReStow will delete files even if they're not symlinks
  --dry                     - Print, and don't execute any actions against the filesystem (implies verbose)
  --help, -h                - This message (you just did it)
```

If the indentation appears a bit off, it's because I set my NeoVim's `tabstop` to 3 (you can check the `.clang-format`).

My project, my rules. :)
