#include "pch.h"
#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <string.h>
using namespace std;
int main()
{
	HANDLE event_handle, com_2_handle;
	char buffer[10];
	bool new_message_flag = true;
	DWORD error_code, number_of_bytes_read;
	if (!(event_handle = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPCWSTR)L"EVENT"))) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	if (!(com_2_handle = CreateFile((LPCWSTR)L"COM2", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	while (true) {
		if (WaitForSingleObject(event_handle, INFINITE) != WAIT_OBJECT_0) {
			error_code = GetLastError();
			cout << "Error code: " << (int)error_code << endl;
			return error_code;
		}
		ResetEvent(event_handle);
		if (!ReadFile(com_2_handle, (LPVOID)buffer, (DWORD)sizeof(buffer), &number_of_bytes_read, NULL)) {
			error_code = GetLastError();
			cout << "Error code: " << (int)error_code << endl;
			return error_code;
		}
		if (!strcmp(buffer, "quit\n")) {
			break;
		}
		if (new_message_flag) {
			cout << "Delivered message:" << endl;
			new_message_flag = false;
		}
		for (int i = 0; i < 10; i++) {
			if (buffer[i] == '\n') {
				new_message_flag = true;
				break;
			}
		}
		cout << buffer;
		SetEvent(event_handle);
	}
	if (!CloseHandle(event_handle)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	if (!CloseHandle(com_2_handle)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	return 0;
}