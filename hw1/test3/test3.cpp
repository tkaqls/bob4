// test3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "test3.h"
#include <strsafe.h>  
#include <memory>
#include <string.h>
#include <locale.h>
/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/

bool is_file_existsW(_In_ const wchar_t* file_path)
{
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH))) return false;

	WIN32_FILE_ATTRIBUTE_DATA info = { 0 };

	if (GetFileAttributesExW(file_path, GetFileExInfoStandard, &info) == 0)
		return false;
	else
		return true;
}


void print(_In_ const char* fmt, _In_ ...)
{
	char log_buffer[2048];
	va_list args;

	va_start(args, fmt);
	HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
	if (S_OK != hRes)
	{
		fprintf(
			stderr,
			"%s, StringCbVPrintfA() failed. res = 0x%08x",
			__FUNCTION__,
			hRes
			);
		return;
	}

	OutputDebugStringA(log_buffer);
	fprintf(stdout, "%s \n", log_buffer);
}


/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool create_bob_txt()
{
	

	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR)* buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	
	wchar_t file_name[260];
	wchar_t copy_name[260];

	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\bob.txt",
		buf)))
	{
		print("err, can not create file name");
		free(buf);
		return false;
	}

	if (!SUCCEEDED(StringCbPrintfW(
		copy_name,
		sizeof(copy_name),
		L"%ws\\bob2.txt",
		buf)))
	{
		print("err, can not copy file name");
		free(buf);
		return false;
	}


	free(buf); buf = NULL;

	if (true == is_file_existsW(file_name))
	{
		::DeleteFileW(file_name);
	}


	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}


	DWORD bytes_written = 0;
	wchar_t string_buf[1024];
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"ddfsjjkfhkse 가나다라마바사아자차카타파하")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	char strUtf8[1024];
	int nLen = WideCharToMultiByte(CP_UTF8, 0, string_buf, lstrlenW(string_buf), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, string_buf, lstrlenW(string_buf), strUtf8, nLen, NULL, NULL);

	if (!WriteFile(file_handle, strUtf8, nLen, &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}


	CloseHandle(file_handle);

	CopyFileW(file_name, copy_name, NULL);

	HANDLE copy_handle = CreateFileW(
		copy_name,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}
	wchar_t data[1024] = { 0 };
	char strUtf[1024] = { 0 };
	char asc[1024];
	ReadFile(copy_handle, strUtf, sizeof(strUtf), &bytes_written, NULL);
	CloseHandle(copy_handle);

	int nLen2 = MultiByteToWideChar(CP_UTF8, 0, strUtf, strlen(strUtf), NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, strUtf, strlen(strUtf), data, nLen2);
	int len = WideCharToMultiByte(CP_ACP, 0, data, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, data, -1, asc, len, NULL, NULL);
	printf("------file------\n%s\n", asc);

	HANDLE copy_handle2 = CreateFileW(
		(LPCWSTR)copy_name,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (INVALID_HANDLE_VALUE == copy_handle2)
	{
		print("err, CreateFile(%ws) failed, gle = %u", file_name, GetLastError());
		return false;
	}

	HANDLE file_map = CreateFileMapping(
		copy_handle2,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
		);
	if (NULL == file_map)
	{
		print("err, CreateFileMapping(%ws) failed, gle = %u", file_name, GetLastError());
		CloseHandle(copy_handle2);
		return false;
	}

	PCHAR file_view = (PCHAR)MapViewOfFile(
		file_map,
		FILE_MAP_READ,
		0,
		0,
		0
		);
	if (file_view == NULL)
	{
		print("err, MapViewOfFile(%ws) failed, gle = %u", file_name, GetLastError());

		CloseHandle(file_map);
		CloseHandle(file_handle);
		return false;
	}

	wchar_t data2[1024] = { 0 };
	char asc2[1024] = { 0 };

	int nLen3 = MultiByteToWideChar(CP_UTF8, 0, file_view, strlen(file_view), NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, file_view, strlen(file_view), data2, nLen3);
	int len2 = WideCharToMultiByte(CP_ACP, 0, data2, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, data2, -1, asc, len2, NULL, NULL);
	printf("------memory------ \n%s\n", asc);

	UnmapViewOfFile(file_view);
	CloseHandle(file_map);
	CloseHandle(copy_handle2);

	DeleteFile(file_name);
	DeleteFile(copy_name);
	return true;

}

int main(void)
{
	create_bob_txt();
	return 0;
}
