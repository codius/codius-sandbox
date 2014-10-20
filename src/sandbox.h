#ifndef CODIUS_SANDBOX_H
#define CODIUS_SANDBOX_H

#include <vector>
#include <unistd.h>

#define IPC_PARENT_IDX 0
#define IPC_CHILD_IDX 1

class SandboxPrivate;

class Sandbox {
  public:
    Sandbox();
    ~Sandbox();
    void spawn(char** argv);

    using Word = unsigned long;
    using Address = Word;

    typedef struct _SyscallCall {
      Word id;
      Word args[6];
    } SyscallCall;

    virtual SyscallCall handleSyscall(const SyscallCall &call) = 0;
    virtual void handleIPC(const std::vector<char> &request) = 0;
    virtual void handleSignal(int signal);
    virtual void handleExit(int status);
    Word peekData (Address addr);
    int copyData (Address addr, size_t length, void* buf);
    int copyString (Address addr, int maxLength, char* buf);
    int pokeData (Address addr, Word word);
    int writeScratch(size_t length, const char* buf);
    int writeData (Address addr, size_t length, const char* buf);
    Address getScratchAddress () const;

    pid_t getChildPID() const;
    void releaseChild(int signal);
    void kill();

  private:
    SandboxPrivate* m_p;
    void traceChild(int ipc_fds[2]);
    void execChild(char** argv, int ipc_fds[2]) __attribute__ ((noreturn));
};

#endif // CODIUS_SANDBOX_H
