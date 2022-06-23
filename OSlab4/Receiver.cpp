#include <windows.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <string>
#include <locale>


const int EXIT_VALUE = '1';

void startProcesses(std::string fileName, int records, int processes)
{
	STARTUPINFO* si = new STARTUPINFO[processes];
	PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[processes];
	ZeroMemory(si, processes * sizeof(si[0]));
	for (int i = 0; i < processes; i++)
	{
		si[i].cb = sizeof(STARTUPINFO);
		CreateProcess(NULL,
			(LPWSTR)std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("Sender.exe " + fileName + " " + std::to_string(records)).c_str(),
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&(si[i]),
			&(pi[i]));
	}
}

void startWorkingLoop(HANDLE hSemaphore, std::ifstream& in)
{
	HANDLE hEvent = CreateEvent(NULL, true, false, L"message");
	while (true)
	{
		std::cout << "Press 1 to exit\nElse read message\n";
		int answer;
		std::cin >> answer;
		if (answer == EXIT_VALUE)
		{
			in.close();
			return;
		}

		unsigned strLen;
		in.read((char*)&strLen, sizeof(unsigned));
		if (in.eof())
		{
			std::cout << "You received 0 messages. Wait...\n";
			in.clear();
			WaitForSingleObject(hEvent, INFINITE);
		}
		else
		{
			std::string message;
			message.resize(strLen);
			in.read((char*)message.data(), strLen);
			ReleaseSemaphore(hSemaphore, 1, NULL);
			std::cout << message << std::endl;
		}
	}
}

int main()
{
	std::string fileName;
	std::cout << "Input file name :\n";
	std::getline(std::cin, fileName);

	std::ifstream in(fileName, std::ios::binary);

	int records;
	std::cout << "Records maximum number:\n";
	std::cin >> records;

	int processes;
	std::cout << "Processes number:\n";
	std::cin >> processes;

	startProcesses(fileName, records, processes);

	HANDLE hSemaphore = CreateSemaphore(NULL, -processes, 0, L"ready");
	WaitForSingleObject(hSemaphore, INFINITE);

	startWorkingLoop(CreateSemaphore(NULL, records, records, L"isFilled"), in);

	return 0;
}