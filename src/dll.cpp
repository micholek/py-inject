#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <windows.h>

typedef int (*Py_IsInitialized_t)();
typedef void (*Py_Initialize_t)();
typedef int (*PyRun_SimpleString_t)(const char *);

#define DECLARE(Name) static Name##_t Name = nullptr
DECLARE(Py_IsInitialized);
DECLARE(Py_Initialize);
DECLARE(PyRun_SimpleString);
#undef DECLARE

template <class T> T static get_proc(HMODULE mod, const char *proc_name) {
    auto proc = GetProcAddress(mod, proc_name);
    std::cout << std::left << std::setw(30) << std::setfill('.') << proc_name;
    if (proc == NULL) {
        std::cout << "not found\n";
        return nullptr;
    }
    std::cout << "found at " << std::setw(8) << std::setfill('0') << std::hex
              << proc << "\n";
    return reinterpret_cast<T>(reinterpret_cast<void *>(proc));
}

static void init_cpython_functions();
static void load_script(const std::string &script_name);

DWORD WINAPI main_thread([[maybe_unused]] LPVOID param) {
    AllocConsole();
    FILE *console_out_f;
    FILE *console_in_f;
    (void) freopen_s(&console_out_f, "CONOUT$", "w", stdout);
    (void) freopen_s(&console_in_f, "CONIN$", "r", stdin);

    init_cpython_functions();

    std::cout << "Calling Py_IsInitialized\n";
    if (Py_IsInitialized() == 0) {
        std::cout << "Py_IsInitialized returned 0, calling Py_Initialize\n";
        Py_Initialize();
    }

    load_script("hook_compile");

    while (1) {
        std::cout << " > ";
        std::string script_name;
        std::cin >> script_name;
        load_script(script_name);
    }
    return 0;
}

static void init_cpython_functions() {
    auto python27_module = GetModuleHandleA("python27");
    if (python27_module == nullptr) {
        std::cout << "python27.dll not found\n";
        return;
    }
    std::cout << "python27.dll found at " << std::hex
              << (uintptr_t) python27_module << "\n";

#define INIT(Name) (Name = get_proc<Name##_t>(python27_module, #Name))
    INIT(Py_IsInitialized);
    INIT(Py_Initialize);
    INIT(PyRun_SimpleString);
#undef INIT
}

static void load_script(const std::string &script_name) {
    std::string file {script_name + ".py"};
    std::ifstream f(file, std::ios::binary);
    if (!f.is_open()) {
        std::cout << file << " could not be open\n";
        return;
    }
    f.seekg(0, std::ios::end);
    size_t size = size_t(f.tellg());
    f.seekg(0);
    std::string data(size, '\0');
    f.read(&data[0], (long long) size);
    std::cout << file << " ";
    if (PyRun_SimpleString(data.c_str()) != 0) {
        std::cout << "[FAILED]\n";
    } else {
        std::cout << "[OK]\n";
    }
    std::cout << std::flush;
}
