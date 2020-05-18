#include <iostream> 
#include <vector>
#include <stdio.h> 
#include <string> 
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <TlHelp32.h>
#include "psapi.h"
using namespace std;
DWORD pid;
HWND hWnd = FindWindowA(0, "RuneScape");
//HWND hWnd = FindWindowA(0, "VALORANT-Win64-Shipping.exe");
HANDLE pHandle;
HMODULE hMods[1024];
string NameMods[1024];


DWORD_PTR GetAdrByModule(string ModuleName) {
    //We get the base address of the dll library by name.
    for (int i = 0; i <= 1024; i++) {
        if (NameMods[i] == ModuleName) {
            return (DWORD_PTR)hMods[i];
        }
    }
    return 0;
}

void InitializationModules()
{//Get the base addresses of the dll libraries in the application
    HANDLE hProcess = 0;
    DWORD cbNeeded = 0;
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, pid);
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH] = {};
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName)))
            {
                string strtmp(&szModName[0], &szModName[260]);
                string a;
                int o;
                for (o = 260; o >= 0; o--) {
                    if (strtmp[o] == '\\') {
                        break;
                    }
                }
                o++;
                for (; o <= 260; o++) {
                    if (strtmp[o] == '\0')
                        break;
                    a += strtmp[o];
                }
                NameMods[i] = a;
            }
        }
    }
    CloseHandle(hProcess);
}
template<typename T>// T is data type
T Read(DWORD_PTR Address) {
    T a = NULL;
    ReadProcessMemory(pHandle, (LPVOID*)Address, &a, sizeof(T), 0);
    return a;
}
DWORD_PTR debug[50] = {};
DWORD64 ReadPointer(DWORD_PTR baseAddres, DWORD offsets[], int lenght) {
    for (int i = 0; i <= lenght - 2; i++) {
        DWORD_PTR f = baseAddres + offsets[i];
        baseAddres = Read<DWORD_PTR>(f);
        cout <<hex<< baseAddres << "  >>  ";
    }
    cout <<dec<< "\n";
    return baseAddres + offsets[lenght - 1];
} 
string ReadString(DWORD_PTR Addres) {
    string g = "";
    char a;
    for (int i = 0; (a = Read<char>(Addres + i)) != '\0'; i++) {
        g += a;
    }
    return g;
}
int temp;
/*void GetModuleListByPID(DWORD pid)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (snapshot != INVALID_HANDLE_VALUE) //Create snapshot of processes
    {
        MODULEENTRY32 module;// write here about find processes
        module.dwSize = sizeof(MODULEENTRY32);
        Module32First(snapshot, &module);

        do {
            cout << module.hModule << '\n';
            //printf("Module...\n");
            //printf("Base Address: %x, Size: %x, Name: %x, MID: %x\n", module.modBaseAddr, module.modBaseSize, module.hModule, module.th32ModuleID);
        } while (Module32Next(snapshot, &module));// find it
    }

    CloseHandle(snapshot);
}*/
int main()
{
    GetWindowThreadProcessId(hWnd, &pid);//Get pid by window name
    cout << "PID: ";
    cin >> temp;
    if (temp != 0) pid = temp;
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    InitializationModules();
    DWORD_PTR adr[6] = {};
    DWORD64 adrsPl[50] = {};
    DWORD64 oldAdr = 0;
    if (GetAdrByModule("rs2client.exe") == 0) return 0;
    while (1) {
        cout <<"rs2client.exe's adr: " << GetAdrByModule("rs2client.exe") << "\n";
        adr[0] = ReadPointer(GetAdrByModule("rs2client.exe"),
            new DWORD[7]{ 0x007FB568, 0x70, 0x2D8, 0x808, 0x1D0, 0xE0, 0x160}, 7);
        if (adr[0] == 0) continue;
        if (oldAdr != adr[0]) {
            bool a = true;
            int i = 0;
            for (; adrsPl[i] != 0; i++) {
                if (adrsPl[i] == adr[0]) {
                    a = false;
                    break;
                }
            }
            if(a) adrsPl[i] = adr[0];
        }
        cout << hex << adr[0] << "\t";
        cout << dec << ReadString(adr[0]) << "\t" << Read<float>(adr[0] + 0x1A8)/10 << "\t"
            << Read<float>(adr[0] + 0x1B0)/10 << "\n\n\n";
        for (int i = 0; adrsPl[i] != 0 && i <= 50;i++) {
            cout << dec << ReadString(adrsPl[i]) << "\t" << Read<float>(adrsPl[i] + 0x1A8) << "\t"
                << Read<float>(adrsPl[i] + 0x1B0) << "\n";
        }
        Sleep(200);
        system("cls");
        oldAdr = adr[0];
    }
    return 0;
}
/*SetTextColor(hDC, RGB(255, 0, 0));
TextOutA(hDC, 20, 20, "Красный текст в блокноте", lstrlenA("Красный текст в блокноте"));*/
