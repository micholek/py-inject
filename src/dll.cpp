#include "dll.h"

#include <fstream>
#include <iostream>
#include <print>
#include <string>

typedef int (*Py_IsInitialized_t)();
typedef void (*Py_Initialize_t)();
typedef int (*PyRun_SimpleString_t)(const char *);

#define DECLARE(Name) static Name##_t Name = nullptr
DECLARE(Py_IsInitialized);
DECLARE(Py_Initialize);
DECLARE(PyRun_SimpleString);
#undef DECLARE

template <class T> T static get_proc(HMODULE mod, const char *proc_name) {
    uintptr_t proc = (uintptr_t) GetProcAddress(mod, proc_name);
    std::print("{:.<30}", proc_name);
    if (proc == NULL) {
        std::println("not found");
        return nullptr;
    }
    std::println("found at {:#010x}", (uintptr_t) proc);
    return reinterpret_cast<T>(reinterpret_cast<void *>(proc));
}

static void init_cpython_functions();
static void load_script_if_exists(const std::string &script_name);

DWORD WINAPI main_thread([[maybe_unused]] LPVOID param) {
    AllocConsole();
    FILE *console_out_f;
    FILE *console_in_f;
    (void) freopen_s(&console_out_f, "CONOUT$", "w", stdout);
    (void) freopen_s(&console_in_f, "CONIN$", "r", stdin);

    init_cpython_functions();

    if (Py_IsInitialized() == 0) {
        std::println("Py_IsInitialized returned 0, calling Py_Initialize");
        Py_Initialize();
    }

    load_script_if_exists("hook_compile");

    while (1) {
        std::print(" > ");
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") {
            FreeConsole();
            break;
        }
        load_script_if_exists(input);
    }
    return 0;
}

static void init_cpython_functions() {
    HMODULE python27 = GetModuleHandleA("python27");
    if (python27 == nullptr) {
        std::println("python27.dll not found");
        return;
    }
    std::println("python27.dll found at {:#010x}", (uintptr_t) python27);

#define INIT(Name) (Name = get_proc<Name##_t>(python27, #Name))
    INIT(Py_IsInitialized);
    INIT(Py_Initialize);
    INIT(PyRun_SimpleString);
#undef INIT
}

static void load_script_if_exists(const std::string &script_name) {
    std::string file {script_name + ".py"};
    std::ifstream f(file, std::ios::binary);
    if (!f.is_open()) {
        std::println("{} could not be open", file);
        return;
    }
    f.seekg(0, std::ios::end);
    size_t size = size_t(f.tellg());
    f.seekg(0);
    std::string data(size, '\0');
    f.read(&data[0], (long long) size);
    std::print("{} ", file);
    if (PyRun_SimpleString(data.c_str()) != 0) {
        std::println("[FAILED]");
    } else {
        std::println("[OK]");
    }
}
