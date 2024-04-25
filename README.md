## DLL injecting Python code

The repository contains source code for a DLL capable of injecting Python code into a process using CPython API. Injected DLL spawns a console allowing for loading code from a script file.

Injection is based on `PyRun_SimpleString` CPython function which is first resolved from `python27.dll` using WinAPI calls. Because of that, `python27.dll` needs to be loaded into the binary before DLL app tries to inject any Python code. What is more, `python27.dll` has to contain definition for `PyRun_SimpleString` function and export it outside.
