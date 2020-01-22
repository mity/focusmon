
#include <windows.h>
#include <psapi.h>

#include <stdio.h>


#define SIZEOF_ARRAY(x)     (sizeof(x) / sizeof((x)[0]))


static void CALLBACK
event_proc(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG obj_id,
           LONG child_id, DWORD thread_id, DWORD event_time)
{
    SYSTEMTIME timestamp;
    DWORD pid = 0;
    WCHAR win_title[256] = L"";
    WCHAR exe_path[MAX_PATH] = L"(n/a)";

    GetLocalTime(&timestamp);

    if(hwnd != NULL) {
        HANDLE proc;

        GetWindowTextW(hwnd, win_title, SIZEOF_ARRAY(win_title));

        /* Get info the process. */
        GetWindowThreadProcessId(hwnd, &pid);
        proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if(proc != NULL) {
            GetModuleFileNameExW(proc, NULL, exe_path, SIZEOF_ARRAY(exe_path));
            CloseHandle(proc);
        }
    }

    printf("Time:    %02d:%02d:%02d\n"
           "Process: %u  %S\n"
           "Window:  0x%lx  %S\n"
           "\n",
            (int) timestamp.wHour, (int) timestamp.wMinute, (int) timestamp.wSecond,
            pid, exe_path,
            hwnd, win_title
    );
}

static BOOL WINAPI
ctrl_c_proc(DWORD ctrl_type)
{
    printf("Quitting...\n");
    PostQuitMessage(0);
    return TRUE;
}

int
main(int argc, char** argv)
{
    HWINEVENTHOOK hook;
    MSG msg;

    hook = SetWinEventHook(EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS, NULL,
            event_proc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if(hook == NULL) {
        fprintf(stderr, "Failed to install the event hook [err %ld].\n", GetLastError());
        return EXIT_FAILURE;
    }

    printf("Monitoring keyboard focus changes has started.\n");
    printf("(Press CTRL+C to quit.)\n");
    printf("\n");

    SetConsoleCtrlHandler(ctrl_c_proc, TRUE);

    while(GetMessage(&msg, NULL, 0, 0)) {
        /* noop */
    }

    return 0;
}
