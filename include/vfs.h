#ifndef VFS_H
#define VFS_H

#include "sandbox.h"
#include <memory>
#include <vector>

class Filesystem;
class DirentBuilder;

class Filesystem {
public:
  Filesystem();

  virtual int open(const char* name, int flags) = 0;
  virtual ssize_t read(int fd, void* buf, size_t count) = 0;
  virtual int close(int fd) = 0;
  virtual int fstat(int fd, struct stat* buf) = 0;
  virtual int getdents(int fd, struct linux_dirent* dirs, unsigned int count) = 0;
  virtual int openat(int fd, const char* filename, int flags, mode_t mode) = 0;
};

class NativeFilesystem : public Filesystem {
public:
  NativeFilesystem(const std::string& root);
  virtual int open(const char* name, int flags);
  virtual ssize_t read(int fd, void* buf, size_t count);
  virtual int close(int fd);
  virtual int fstat(int fd, struct stat* buf);
  virtual int getdents(int fd, struct linux_dirent* dirs, unsigned int count);
  virtual int openat(int fd, const char* filename, int flags, mode_t mode);

private:
  std::string m_root;
  std::map<int, std::string> m_openFiles;
};

class VFS {
public:
  VFS(Sandbox* sandbox);

  Sandbox::SyscallCall handleSyscall(const Sandbox::SyscallCall& call);
  std::string getFilename(Sandbox::Address addr) const;

private:
  Sandbox* m_sbox;
  std::unique_ptr<Filesystem> m_fs;
  std::vector<std::string> m_whitelist;

  bool isWhitelisted(const std::string& str);
  int fromVirtualFD(int fd);
  int toVirtualFD(int fd);

  void do_open(Sandbox::SyscallCall& call);
  void do_close(Sandbox::SyscallCall& call);
  void do_read(Sandbox::SyscallCall& call);
  void do_fstat(Sandbox::SyscallCall& call);
  void do_getdents(Sandbox::SyscallCall& call);
  void do_openat(Sandbox::SyscallCall& call);
};

#endif // VFS_H