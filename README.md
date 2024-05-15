# YUMRepoManager

YUMRepoManager offers a convenient way to manage YUM settings and repository configuration files.

With this tool, you can enable or disable YUM plugins and edit their configuration files. The **import and export of repositories** are easily done **by dragging and dropping .repo files** to and from the file browser. Additionally, you can edit the **createrepo** arguments.

YUMRepoManager allows you to create a local repository in three different scenarios:

### RPM Folder
Create a repository from a directory containing .rpm files.

### Cache
Create a repository from the YUM cache to a user-specified directory.

### Global Cache
Create a repository from the YUM cache to a user-specified directory. The YUM cache will be linked to this directory, unifying the cache in a single folder. Packages installed after this point will be placed in the new path **(which can be a network folder)** instead of the default local YUM cache.

If the unified folder for the **Global Cache** option is configured as a network folder, it can serve as a **remote repository for other systems on the local network**.

## Binaries

The RPM binaries are available for **x86_64** and **i686** architectures. These RPM files are specifically built for **Fedora Linux**, the primary target distribution for YUMRepoManager.

[SourceForge](http://sourceforge.net/projects/yumrepomanager/)
