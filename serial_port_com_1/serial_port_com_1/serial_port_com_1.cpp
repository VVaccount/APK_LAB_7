//в папке данного проекта должен лежать файл "serial_port_com_2.exe" (из папки Debug одноименного проекта)
//для выхода надо отправить сообщение "quit"
//необходимо запустить только тот код, который приведен ниже
#include "pch.h"
#define _SCL_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <math.h>
using namespace std;
int main() {
	string buffer;
	HANDLE event_handle, com_1_handle;
	DCB com_1_dcb;
	COMMTIMEOUTS com_1_commtimeouts;
	DWORD error_code, number_of_bytes_written;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(si);
	si.lpTitle = (LPWSTR)L"CLIENT";
	si.wShowWindow = TRUE;
	if (!(event_handle = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)L"EVENT"))) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	if (!(com_1_handle = CreateFile((LPCWSTR)L"COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	if (!CreateProcess((LPCWSTR)L".\\serial_port_com_2.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	com_1_commtimeouts.ReadIntervalTimeout = 0;
	com_1_commtimeouts.ReadTotalTimeoutMultiplier = 0;
	com_1_commtimeouts.ReadTotalTimeoutConstant = 1000;
	com_1_commtimeouts.WriteTotalTimeoutMultiplier = 0;
	com_1_commtimeouts.WriteTotalTimeoutConstant = 1000;
	if (!SetCommTimeouts(com_1_handle, &com_1_commtimeouts)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	com_1_dcb.DCBlength = sizeof(DCB);										// Длина структуры DCB в байтах 
	com_1_dcb.BaudRate = CBR_9600;											// Скорость передачи данных
	com_1_dcb.ByteSize = 8;													// Число информационных бит в передаваемых и принимаемых байтах
	com_1_dcb.Parity = NOPARITY;											// Выбор схемы контроля четности (бит четности отсутствует)
	com_1_dcb.StopBits = ONESTOPBIT;										// Количество стоповых бит (один бит) 
	com_1_dcb.fAbortOnError = TRUE;											// Игнорирование всех операций чтения/записи при возникновении ошибки
	com_1_dcb.fDtrControl = DTR_CONTROL_DISABLE;							// Режим управления обменом для сигнала DTR
	com_1_dcb.fRtsControl = RTS_CONTROL_DISABLE;							// Режим управления потоком для сигнала RTS
	com_1_dcb.fBinary = TRUE;												// Двоичный режим обмена
	com_1_dcb.fParity = FALSE;												// Режим контроля четности
	com_1_dcb.fInX = FALSE;													// Использование XON/XOFF управления потоком при приеме
	com_1_dcb.fOutX = FALSE;												// Использование XON/XOFF управления потоком при передаче
	com_1_dcb.XonChar = 0;													// Символ XON используемый для приема и передачи
	com_1_dcb.XoffChar = (unsigned char)0xFF;								// Символ XOFF используемый для приема и передачи
	com_1_dcb.fErrorChar = FALSE;											// Символ, использующийся для замены символов с ошибочной четностью
	com_1_dcb.fNull = FALSE;												// Замена символов с ошибкой четности на символ в поле ErrorChar
	com_1_dcb.fOutxCtsFlow = FALSE;											// Режим слежения за сигналом CTS
	com_1_dcb.fOutxDsrFlow = FALSE;											// Режим слежения за сигналом DSR
	com_1_dcb.XonLim = 128;													// Минимальное число символов в приемном буфере перед посылкой символа XON
	com_1_dcb.XoffLim = 128;												// Максимальное количество байт в приемном буфере перед посылкой символа XOFF
	if (!SetCommState(com_1_handle, &com_1_dcb)) {
		if (!CloseHandle(com_1_handle)) {
			error_code = GetLastError();
			cout << "Error code: " << (int)error_code << endl;
			return error_code;
		}
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	int buffer_size = 10, number_of_parcels;
	string buf;
	while (true) {
		cout << "Type your message:" << endl;
		cin >> buffer;
		buffer.push_back('\n');
		number_of_parcels = ceil((double)buffer.size() / (buffer_size - 1));
		buffer.resize(number_of_parcels * (buffer_size - 1));
		for (int index = 0; index < number_of_parcels; index++) {
			if (buffer.empty()) {
				break;
			}
			buf = buffer;
			buf.resize(9);
			buf.push_back('\0');
			buffer.erase(buffer.begin(), buffer.begin() + 9);
			if (buffer.size() < 9 && buffer.size() > 0) {
				buffer.push_back('\0');
			}
			if (!WriteFile(com_1_handle, (LPCVOID)buf.c_str(), (DWORD)buffer_size, &number_of_bytes_written, NULL)) {
				error_code = GetLastError();
				cout << "Error code: " << (int)error_code << endl;
				return error_code;
			}
			SetEvent(event_handle);
			if (WaitForSingleObject(event_handle, INFINITE) != WAIT_OBJECT_0) {
				error_code = GetLastError();
				cout << "Error code: " << (int)error_code << endl;
				return error_code;
			}
			if (!strcmp(buf.c_str(), "quit\n")) {
				break;
			}
			ResetEvent(event_handle);
			buf.clear();
		}
		if (!strcmp(buf.c_str(), "quit\n")) {
			break;
		}
		buf.clear();
		buffer.clear();
	}
	if (!CloseHandle(pi.hProcess)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	if (!CloseHandle(event_handle)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	if (!CloseHandle(com_1_handle)) {
		error_code = GetLastError();
		cout << "Error code: " << (int)error_code << endl;
		return error_code;
	}
	return 0;
}
