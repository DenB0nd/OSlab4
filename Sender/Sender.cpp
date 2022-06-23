#include <windows.h>
#include <fstream>
#include <iostream>

const int MESSAGE_MAX_SIZE = 20;

std::string messageInput()
{
	std::string str;
	while (true)
	{
		std::cin >> str;
		if (str.size() < MESSAGE_MAX_SIZE)
		{
			return str;
		}
		std::cout << "String length must be less than 20\n";
	}
}

int main(int argc, char* argv[])
{
	HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"ready");
	HANDLE hSemaphoreFilled = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"isFilled");
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"message");
	ReleaseSemaphore(hSemaphore, 1, NULL);

	while (true)
	{
		std::cout << "Press 1 to exit\nElse send message\n";
		int answer;
		std::cin >> answer;
		if (answer == 1)
		{
			return 0;
		}

		if (WaitForSingleObject(hSemaphoreFilled, 0) != WAIT_OBJECT_0)
		{
			std::cout << "There is no space for a message. Wait...\n";
			WaitForSingleObject(hSemaphoreFilled, INFINITE);
		}

		std::cout << "Message:\n";
		std::string message = messageInput();

		unsigned strLen = message.size();
		std::ofstream fout(argv[1], std::ios::binary | std::ios::app);
		fout.write((char*)&strLen, sizeof(unsigned));
		fout.write((char*)message.c_str(), message.size());
		fout.close();
		PulseEvent(hEvent);
	}
}