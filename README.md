I made a Windows Pop Up that displays the Processes that hogs up the most resources using C++ & Direct Win32 API.

I know what you're thinking now.

"BUT MARK!!! THE TASK MANAGER ALREADY DOES THE SAME THING!! What even is the purpose!"

Super easy!

1. The task manager takes 10-30 seconds to show up. The details tab takes a while. However, this popup comes up in 0.002 seconds. Very efficient!
2. How else can one know these inner details unless they practically do this themselves, such as getting these snapshots like PROCESSENTRY32 structure & CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

The WM_TIMER in the Windows Procedure will update the popup every minute, it was activated with SetTimer(hWnd, 1, 1000, NULL) before the message loop.

![](images/popup_process.gif)

UPDATE:

I added two more columns into this program months after I first created it, which are the "# of Handles" AND the "CPU Percentage", which makes it a total of five columns!

| Columns        |
| -------------- |
| Process Name   |
| Memory         |
| Process ID     |
| Handles        |
| CPU Percentage |

The first four were easily retrievable with a few lines of advanced C++ Win32 API low-level calls. Just a few lines.

I decided to have the number of Handles in the 4th column, because this is a feature that I only saw in SysInternals/Process Explorer, and NOT in the default Task Manager (except for Total # of Handles, not by process)

Luckily for me the number of handles were retrievable by 4 lines of C++ code, like so:

PDWORD handle_count_struct_instance = new DWORD;
GetProcessHandleCount(hProcess, handle_count_struct_instance);
int handle_count = \*handle_count_struct_instance;
delete handle_count_struct_instance;

Weirdly, I --HAD-- to make this into a HEAP object declared with "New" to get the code to even work, which I try to do a little as possible because of the potential of memory leaks.

But now we're reaching the toughest of them all and that's the CPU Percentage!!!!!!!!!!!!

OH MY GOODNESS, what a freaking challenge. There are no Win32 calls to just simply get it, like there were with the three other columns. You had to do it yourself. I came this far, and I need this for diagnosing, so why was I going to stop. I get to learn something the way.

Sadly, this required lots of studying & homework & taking together the best of many ideas to do so. Since this is using WM_TIMER, the last value of the Kernel/User/Exit/Creation time of the process had to have been recorded, which I did in a map (or a "Dict" of using Python). This was many hours of work with many tries & many solutions.

To see my solution at calculating the CPU percentage from a process, please view the code below!!! The great thing about it is that this solution mirrors the Task Manager CPU's percentage!!

![](images/view_processes_code.png)
