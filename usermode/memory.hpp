#pragma once

#include "common.hpp"

enum operation
{
	finished,
	read,
	write,
	read_drawing,
	write_drawing,
	leave
};

struct memory_params
{
	void* lpBaseAddress;
	void* lpBuffer;
	size_t nSize;
};

memory_params copy_parameters;

HANDLE hProcess;
HANDLE dProcess;

int operation = 0;
int draw_operation = 0;
#define STR_BUFFER_SIZE 64

class memory
{
public:
	static void setup_drawing(uint32_t pid)
	{
		dProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
	}

	static void d_wait_finish()
	{
		while (draw_operation != operation::finished)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(0));
		}
	}

	template <typename t>
	static t read_drawing(uintptr_t base_address)
	{
		d_wait_finish();
		t buffer{};
		copy_parameters.lpBaseAddress = (void*)base_address;
		copy_parameters.lpBuffer = &buffer;
		copy_parameters.nSize = sizeof(buffer);
		draw_operation = operation::read_drawing;
		d_wait_finish();
		return buffer;
	}

	template <typename t>
	static void write_drawing(uintptr_t base_address, t buffer)
	{
		d_wait_finish();
		copy_parameters.lpBaseAddress = (void*)base_address;
		copy_parameters.lpBuffer = &buffer;
		copy_parameters.nSize = sizeof(buffer);
		draw_operation = operation::write_drawing;
		d_wait_finish();
	}

	static std::string read_str(uintptr_t address, int size = STR_BUFFER_SIZE)
	{
		std::unique_ptr<char[]> buffer(new char[size]);
		copy_memory(address, (uintptr_t)buffer.get(), size);
		return std::string(buffer.get());
	}

	static uint32_t get_pid(const char* proc_name)
	{
		PROCESSENTRY32 proc_info;
		proc_info.dwSize = sizeof(proc_info);

		const auto proc_snapshot =
			CreateToolhelp32Snapshot(
				TH32CS_SNAPPROCESS,
				NULL
			);

		if (proc_snapshot == INVALID_HANDLE_VALUE)
			return NULL;

		Process32First(proc_snapshot, &proc_info);
		if (!strcmp(proc_info.szExeFile, proc_name))
		{
			CloseHandle(proc_snapshot);
			return proc_info.th32ProcessID;
		}

		while (Process32Next(proc_snapshot, &proc_info))
		{
			if (!strcmp(proc_info.szExeFile, proc_name))
			{
				CloseHandle(proc_snapshot);
				return proc_info.th32ProcessID;
			}
		}

		CloseHandle(proc_snapshot);
		return {};
	}

	static uint64_t find_base_address(uint32_t pid, const wchar_t* name)
	{
		uint64_t modBaseAddr = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
		if (snapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32W modEntry;
			modEntry.dwSize = sizeof(modEntry);
			if (Module32FirstW(snapshot, &modEntry))
			{
				do
				{
					if (!_wcsicmp(modEntry.szModule, name))
					{
						modBaseAddr = reinterpret_cast<uint64_t>(modEntry.modBaseAddr);
						break;
					}
				} while (Module32NextW(snapshot, &modEntry));
			}
		}
		CloseHandle(snapshot);
		return modBaseAddr;
	}

	static uintptr_t read_chain(uintptr_t address, std::vector<uintptr_t> offsets)
	{
		uintptr_t current = address;
		for (uintptr_t offset : offsets)
		{
			current = read<uintptr_t>(current + offset);
		}
		return current;
	}

	static void setup(uint32_t pid)
	{
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
	}

	static void wait_finish()
	{
		while (operation != operation::finished)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(0));
		}
	}

	static void exit()
	{
		operation = operation::leave;
	}

	template <typename t>
	static t read(uintptr_t base_address)
	{
		wait_finish();
		t buffer{};
		copy_parameters.lpBaseAddress = (void*)base_address;
		copy_parameters.lpBuffer = &buffer;
		copy_parameters.nSize = sizeof(buffer);
		operation = operation::read;
		wait_finish();
		return buffer;
	}

	template <typename t>
	static void write(uintptr_t base_address, t buffer)
	{
		wait_finish();
		copy_parameters.lpBaseAddress = (void*)base_address;
		copy_parameters.lpBuffer = &buffer;
		copy_parameters.nSize = sizeof(buffer);
		operation = operation::write;
		wait_finish();
	}
	
	static void copy_memory(uintptr_t base_address, uintptr_t buffer, size_t size)
	{
		wait_finish();
		copy_parameters.lpBaseAddress = (void*)base_address;
		copy_parameters.lpBuffer = (void*)buffer;
		copy_parameters.nSize = size;
		operation = operation::read;
		wait_finish();
	}
	static void loop()
	{
		while (true)
		{
			if (operation != operation::finished) 
			{
				if (operation == operation::read)
					ReadProcessMemory(hProcess, copy_parameters.lpBaseAddress, copy_parameters.lpBuffer, copy_parameters.nSize, 0);
				else if (operation == operation::write)
					WriteProcessMemory(hProcess, copy_parameters.lpBaseAddress, copy_parameters.lpBuffer, copy_parameters.nSize, 0);
				else if (operation == operation::leave)
					break;
				operation = operation::finished;
			}
			if (draw_operation != operation::finished)
			{
				if (draw_operation == operation::read_drawing)
					ReadProcessMemory(dProcess, copy_parameters.lpBaseAddress, copy_parameters.lpBuffer, copy_parameters.nSize, 0);
				else if (draw_operation == operation::write_drawing)
					WriteProcessMemory(dProcess, copy_parameters.lpBaseAddress, copy_parameters.lpBuffer, copy_parameters.nSize, 0);
				draw_operation = operation::finished;
			}
			std::this_thread::sleep_for(std::chrono::nanoseconds(0));
		}
	}
private:

};