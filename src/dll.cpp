#include <array>
#include <conio.h>
#include <cstdio>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <windows.h>

typedef void PyObject_t;
typedef void PyThreadState_t;
typedef enum { PyGILState_LOCKED, PyGILState_UNLOCKED } PyGILState_STATE;
typedef PyGILState_STATE (*PyGILState_Ensure_t)();
typedef void (*PyGILState_Release_t)(PyGILState_STATE);
typedef PyThreadState_t *(*PyEval_SaveThread_t)();
typedef void (*PyEval_RestoreThread_t)(PyThreadState_t *);
typedef const char (*Py_GetVersion_t)();
typedef int (*Py_IsInitialized_t)();
typedef void (*Py_Initialize_t)();
typedef int (*PyRun_SimpleFile_t)(FILE *, const char *);
typedef int (*PyRun_SimpleString_t)(const char *);
typedef const char *(*Py_GetCompiler_t)();
typedef PyObject_t *(*PyImport_ImportModule_t)(const char *);
typedef PyObject_t *(*PyModule_GetDict_t)(PyObject_t *);
typedef int (*PyDict_Next_t)(PyObject_t *, int *, PyObject_t **, PyObject_t **);
typedef const char *(*PyUnicode_AsUTF8_t)(PyObject_t *);
typedef PyObject_t *(*PyFunction_GetCode_t)(PyObject_t *);
typedef PyObject_t *(*PyObject_GetAttrString_t)(PyObject_t *, const char *);
typedef PyObject_t *(*PyObject_CallObject_t)(PyObject_t *, PyObject_t *);
typedef PyObject_t *(*PyTuple_GetItem_t)(PyObject_t *, int);
typedef double (*PyFloat_AsDouble_t)(PyObject_t *);
typedef void (*Py_DecRef_t)(PyObject_t *);

#define DECLARE(Name) static Name##_t Name = nullptr
DECLARE(PyGILState_Ensure);
DECLARE(PyGILState_Release);
DECLARE(PyEval_SaveThread);
DECLARE(PyEval_RestoreThread);
DECLARE(Py_GetVersion);
DECLARE(Py_IsInitialized);
DECLARE(Py_Initialize);
DECLARE(PyRun_SimpleFile);
DECLARE(Py_GetCompiler);
DECLARE(PyRun_SimpleString);
DECLARE(PyImport_ImportModule);
DECLARE(PyModule_GetDict);
DECLARE(PyDict_Next);
DECLARE(PyUnicode_AsUTF8);
DECLARE(PyFunction_GetCode);
DECLARE(PyObject_GetAttrString);
DECLARE(PyObject_CallObject);
DECLARE(PyTuple_GetItem);
DECLARE(PyFloat_AsDouble);
DECLARE(Py_DecRef);
#undef DECLARE

static PyObject_t *GetMainCharacterPosition = nullptr;
static PyObject_t *GetMousePosition = nullptr;
static PyObject_t *AppendChat = nullptr;
static PyObject_t *SetPixelPosition = nullptr;

// template <class TFunc, class... TArgs>
// auto info(TFunc func, TArgs &&...args,
//                   std::source_location s = std::source_location::current()) {
//     std::cout << "Calling func " << s.function_name() << "\n";
//     auto loc = std::source_location::current();
//     std::cout << loc.function_name() << std::endl;
//     int i = 0;
//     (
//         [&] {
//             // Do things in your "loop" lambda

//             ++i;
//             std::cout << "input " << i << " = " << args << std::endl;
//         }(),
//         ...);
//     return func(args...);
// }

template <class T> T static get_proc(HMODULE mod, const char *proc_name) {
    auto proc = GetProcAddress(mod, proc_name);
    std::cout << std::left << std::setw(30) << std::setfill('.') << proc_name;
    if (proc == NULL) {
        std::cout << "not found\n";
        return nullptr;
    }
    std::cout << "found at " << std::setw(8) << std::setfill('0') << std::hex
              << proc << "\n";
    return (T) proc;
}

static void init_cpython_functions();
static void init_python_functions();
static void load_script(const std::string &script_name);
static std::array<int, 3> get_player_pos();
static std::array<int, 2> get_mouse_pos();

DWORD WINAPI main_thread(LPVOID param) {
    (void) param;
    AllocConsole();
    FILE *console_out_f;
    FILE *console_in_f;
    (void) freopen_s(&console_out_f, "CONOUT$", "w", stdout);
    (void) freopen_s(&console_in_f, "CONIN$", "r", stdin);

    init_cpython_functions();
    // if (py_get_compiler) {
    //     std::cout << "Compiler: " << py_get_compiler() << "\n";
    // }

    std::cout << "Calling Py_IsInitialized\n";
    if (Py_IsInitialized() == 0) {
        std::cout << "Py_IsInitialized returned 0, calling Py_Initialize\n";
        getch();
        Py_Initialize();
    }

    std::cout << "Python version used: " << Py_GetVersion() << "\n";

    // init_python_functions();

    // getch();

    // auto mod = pyimport_import_module("chr");
    // if (mod == nullptr) {
    //     std::cout << "PyImport_ImportModule failed\n";
    //     while (1) {
    //     }
    // }
    // std::cout << "Module `chr` at " << std::hex << (uintptr_t) mod << "\n";

    // auto dict = pymodule_get_dict(mod);
    // if (dict == nullptr) {
    //     std::cout << "PyModule_GetDict failed\n";
    //     while (1) {
    //     }
    // }
    // std::cout << "Dict at " << std::hex << (uintptr_t) dict << "\n";
    // PyObject_t *key, *val;
    // int i = 0;
    // while (pydict_next(dict, &i, &key, &val) != 0) {
    //     // auto k = pyunicode_as_utf8(key);
    //     // std::cout << "key: " << k << "\n";
    //     auto code = pyfunction_get_code(key);
    //     std::cout << "Code: " << (const char *) code << "\n";
    // }

    // std::unordered_map<std::string, const char *> py_code;
    // py_code["get_modules"] = "import sys\n"
    //                          "with open('modules.txt', 'w') as f:\n"
    //                          "    for k in sys.modules.keys():\n"
    //                          "        f.write(k + '\\n')\n";
    // py_code["get_modules_dir"] =
    //     "import sys\n"
    //     "with open('modules_dir.txt', 'w') as f:\n"
    //     "    for module in sys.modules.keys():\n"
    //     "        f.write('--- ' + module + ' ---\\n')\n"
    //     "        for module_f in dir(module):\n"
    //     "            f.write(module_f + '\\n')\n"
    //     "        f.write('\\n')\n";
    // py_code["test_chat"] = "import chat\n"
    //                        "chat.AppendChat(3, 'abcd')\n";
    // py_code["tele_hack_test"] =
    //     "import chr, player\n"
    //     "chr.MoveToDestPosition(player.GetMainCharacterIndex(), 175, 99)\n";
    // py_code["tele_hack2_test"] = "import chr, player\n"
    //                              "chr.SetPixelPosition(88*100, 195*100)\n";
    // py_code["test_chat2"] = "import chat, net\n"
    //                         "net.SendChatPacket('a',
    //                         chat.CHAT_TYPE_TALKING)\n";
    // for (const auto &item : py_code) {
    //     std::cout << "Run " << item.first << " script ";
    //     if (pyrun_simple_string(item.second) != 0) {
    //         std::cout << "[FAILED]\n";
    //     } else {
    //         std::cout << "[OK]\n";
    //     }
    // }
    auto stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(stdout_handle, &info);

    while (1) {
        // auto [player_x, player_y, player_z] = get_player_pos();
        // auto [mouse_x, mouse_y] = get_mouse_pos();
        // std::cout << std::format(
        //     "(x, y, z) = ({:6}, {:6}, {:6})\nMouse (x, y) = ({:6}, {:6})\n",
        //     player_x, player_y, player_z, mouse_x, mouse_y);
        std::cout << " > ";
        std::string script_name;
        std::cin >> script_name;
        load_script(script_name);
        // if (x % 100 == 0) {
        //     AppendChat()
        // }
        // SetConsoleCursorPosition(stdout_handle, info.dwCursorPosition);
        // std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
    INIT(PyGILState_Ensure);
    INIT(PyGILState_Release);
    INIT(PyEval_SaveThread);
    INIT(PyEval_RestoreThread);
    INIT(Py_GetVersion);
    INIT(Py_IsInitialized);
    INIT(Py_Initialize);
    INIT(PyRun_SimpleFile);
    INIT(Py_GetCompiler);
    INIT(PyRun_SimpleString);
    INIT(PyImport_ImportModule);
    INIT(PyModule_GetDict);
    INIT(PyDict_Next);
    INIT(PyUnicode_AsUTF8);
    INIT(PyFunction_GetCode);
    INIT(PyObject_GetAttrString);
    INIT(PyObject_CallObject);
    INIT(PyTuple_GetItem);
    INIT(PyFloat_AsDouble);
    INIT(Py_DecRef);
#undef INIT
}

static void init_python_functions() {
    // PyRun_SimpleString("__builtins__.__import__ = old_import");
    // auto main_mod = PyImport_ImportModule("__main__");
    // if (main_mod == nullptr) {
    //     std::cout << "PyImport_ImportModule failed (__main__)\n";
    //     while (1) {
    //     }
    // }

    auto player_mod = PyImport_ImportModule("player");
    if (player_mod == nullptr) {
        std::cout << "PyImport_ImportModule failed (player_mod)\n";
        while (1) {
        }
    }
    GetMainCharacterPosition =
        PyObject_GetAttrString(player_mod, "GetMainCharacterPosition");
    if (GetMainCharacterPosition == nullptr) {
        std::cout << "PyObject_GetAttrString failed\n";
        while (1) {
        }
    }
    Py_DecRef(player_mod);

    auto wnd_mgr_mod = PyImport_ImportModule("wndMgr");
    if (wnd_mgr_mod == nullptr) {
        std::cout << "PyImport_ImportModule failed (wnd_mgr_mod)\n";
        while (1) {
        }
    }
    GetMousePosition = PyObject_GetAttrString(wnd_mgr_mod, "GetMousePosition");
    if (GetMousePosition == nullptr) {
        std::cout << "PyObject_GetAttrString failed\n";
        while (1) {
        }
    }
    Py_DecRef(wnd_mgr_mod);

    auto chat = PyImport_ImportModule("chat");
    if (chat == nullptr) {
        std::cout << "PyImport_ImportModule failed (chat)\n";
        // while (1) {
        // }
    }
    AppendChat = PyObject_GetAttrString(chat, "AppendChat");
    if (AppendChat == nullptr) {
        std::cout << "PyObject_GetAttrString failed\n";
        while (1) {
        }
    }
    Py_DecRef(chat);

    auto chr = PyImport_ImportModule("chr");
    if (chr == nullptr) {
        std::cout << "PyImport_ImportModule failed\n";
        while (1) {
        }
    }
    SetPixelPosition = PyObject_GetAttrString(chr, "SetPixelPosition");
    if (SetPixelPosition == nullptr) {
        std::cout << "PyObject_GetAttrString failed\n";
        while (1) {
        }
    }
    Py_DecRef(chr);
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
    f.read(&data[0], size);
    std::cout << script_name << " ";
    // auto thread_state = PyEval_SaveThread();
    if (PyRun_SimpleString(data.c_str()) != 0) {
        std::cout << "[FAILED]\n";
    } else {
        std::cout << "[OK]\n";
    }
    // PyEval_RestoreThread(thread_state);
}

static std::array<int, 3> get_player_pos() {
    auto res = PyObject_CallObject(GetMainCharacterPosition, nullptr);
    auto x_obj = PyTuple_GetItem(res, 0);
    auto y_obj = PyTuple_GetItem(res, 1);
    auto z_obj = PyTuple_GetItem(res, 2);
    auto x = (int) PyFloat_AsDouble(x_obj);
    auto y = (int) PyFloat_AsDouble(y_obj);
    auto z = (int) PyFloat_AsDouble(z_obj);
    Py_DecRef(x_obj);
    Py_DecRef(y_obj);
    Py_DecRef(z_obj);
    Py_DecRef(res);
    return {x, y, z};
}

static std::array<int, 2> get_mouse_pos() {
    auto res = PyObject_CallObject(GetMousePosition, nullptr);
    auto x_obj = PyTuple_GetItem(res, 0);
    auto y_obj = PyTuple_GetItem(res, 1);
    auto x = (int) PyFloat_AsDouble(x_obj);
    auto y = (int) PyFloat_AsDouble(y_obj);
    Py_DecRef(x_obj);
    Py_DecRef(y_obj);
    Py_DecRef(res);
    return {x, y};
}

static void teleport() {}
// void send_msg(std::string msg) {
//     auto res = pyobject_call_object(AppendChat, nullptr);
//     auto x = PyFloat_AsDouble(pytuple_get_item(res, 0));
//     auto y = PyFloat_AsDouble(pytuple_get_item(res, 1));
//     return {x, y};
// }
