#include "stdafx.h"
#include <conio.h>
#include "StopWatch.h"
#include "mmio.h"
#include "FileIoHelperClass.h"

int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	FileIoHelper f;
	LARGE_INTEGER size2;
	size2.HighPart = 2;
	size2.LowPart = 0;
	PUCHAR buf = { 0 };

	f.FIOCreateFile(L"big.txt", size2);
	
	StopWatch sw;
	sw.Start();

	HANDLE hfile = CreateFileW(L"big.txt", GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hfile)
	{
		print("CreateFile1() failed, gle=0x%08x", GetLastError());

	}

	__int64 dwFileSize = 0;
	DWORD dwFileSizeHigh;
	__int64 qwFileSize = GetFileSize(hfile, &dwFileSizeHigh);
	qwFileSize += (((__int64)dwFileSize) << 32);
	__int64 qwFileOffset = 0;
	LARGE_INTEGER size3;
	size3.HighPart = dwFileSizeHigh;
	size3.LowPart = (((__int64)dwFileSize) << 32);
	f.FIOCreateFile(L"big4.txt", size3);

	HANDLE cfile = CreateFileW(L"big4.txt", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == cfile)
	{
		print("CreateFile2() failed, gle=0x%08x", GetLastError());

	}

	
	HANDLE hmap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, 0);
	HANDLE cmap = CreateFileMapping(cfile, NULL, PAGE_READWRITE, 0, 0, 0);

	if (NULL == cmap)
	{
		print("err ] CreateFileMapping() failed. gle = %u", GetLastError());

	}

	while (qwFileSize > 0)
	{


	
		DWORD dwBytesInBlock = si.dwAllocationGranularity * 16;
		if (qwFileSize < si.dwAllocationGranularity * 16)
		{
			dwBytesInBlock = qwFileSize;
		}



		PCHAR hview = (PCHAR)MapViewOfFile(hmap, FILE_MAP_READ,
			(DWORD)(qwFileOffset >> 32),  
			(DWORD)(qwFileOffset & 0xFFFFFFFF), 
			dwBytesInBlock);

		PCHAR cview = (PCHAR)MapViewOfFile(cmap, FILE_MAP_WRITE,
			(DWORD)(qwFileOffset >> 32), 
			(DWORD)(qwFileOffset & 0xFFFFFFFF),
			dwBytesInBlock);


		for (__int64 i = qwFileOffset; i < qwFileOffset + dwBytesInBlock-1; i++)
		{
			cview[i] = hview[i];
		}

		UnmapViewOfFile(hview);
		UnmapViewOfFile(cview);

		
		qwFileOffset += dwBytesInBlock;
		qwFileSize -= dwBytesInBlock;


	}
	CloseHandle(hmap);
	CloseHandle(cmap);
	CloseHandle(hfile);
	CloseHandle(cfile);

	/*
	f.FIOpenForRead(L"big.txt");
	f.FIOCreateFile(L"big3.txt", size2);
	f.FIOpenForRead(L"big3.txt");
	for (i = 0; i < 8; i++)
	{
	size3.HighPart = i >> 32;
	size3.LowPart = i & 0xffffffff;
	printf("%32x\n", size3);
	f.FIOReadFromFile(size3,1024*1024*1024,buf);
	d.FIOWriteToFile(size3, 1024*1024*1024, buf);
	}
	*/
	sw.Stop();
	print("info] time elapsed = %f", sw.GetDurationSecond());
	StopWatch sw2;
	sw2.Start();
	_ASSERTE(file_copy_using_read_write(L"big.txt", L"big2.txt"));
	sw2.Stop();
	print("info] time elapsed = %f", sw2.GetDurationSecond());

	return 0;
}