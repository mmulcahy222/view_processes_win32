I made a Windows Pop Up that displays the Processes that hogs up the most resources using C++ & Direct Win32 API.

I know what you're thinking now.

"BUT MARK!!! THE TASK MANAGER ALREADY DOES THE SAME THING!! What even is the purpose!"

Super easy!

1) The task manager takes 10-30 seconds to show up. The details tab takes a while. However, this popup comes up in 0.002 seconds. Very efficient!
2) How else can one know these inner details unless they practically do this themselves, such as getting these snapshots like PROCESSENTRY32 structure & CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

The WM_TIMER in the Windows Procedure will update the popup every minute, it was activated with SetTimer(hWnd, 1, 1000, NULL) before the message loop.

![](images/popup_process.gif)