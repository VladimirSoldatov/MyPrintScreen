#include <windows.h>
BOOL WINAPI SaveBitmap(WCHAR* wPath)
{

	//Структура BITMAPFILEHEADER содержит сведения о типе, размере и макете файла, содержащего DIB.
	BITMAPFILEHEADER bfHeader;
	//Структура BITMAPINFOHEADER содержит информацию о размерах и цветовом формате аппаратно - независимого растрового формата(DIB).
	BITMAPINFOHEADER biHeader;
	//Определяет структуру BITMAPINFOHEADER, которая содержит информацию о размерах и формате цвета.
	BITMAPINFO bInfo;
	//Дескриптор представляющего интерес графического объекта
	HGDIOBJ hTempBitmap;
	//Идентификатор изображения в памяти
	HBITMAP hBitmap;
	//Точечный рисунок
	BITMAP bAllDesktops;
	// контекст окна
	HDC hDC, hMemDC;
	// Ширина и длина
	LONG lWidth, lHeight;
	//Набор байт
	BYTE* bBits = NULL;
	// Дескриптор для работы с памятью
	HANDLE hHeap = GetProcessHeap();
	//Инициализация рабочих перменных
	DWORD cbBits, dwWritten = 0;
	//Дескриптор файла
	HANDLE hFile;
	//Извлекаем указанную системную метрику или параметр конфигурации системы. В данном случае высот и ширна экрана
	INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	//Базвая инициализация нулями перемнных
	ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
	ZeroMemory(&bInfo, sizeof(BITMAPINFO));
	ZeroMemory(&bAllDesktops, sizeof(BITMAP));
	//Базовая инициализация декскриптора экрана
	hDC = GetDC(NULL);
	//Запись временного объекта скриншота во времнный объект
	hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
	//Запись скриншота в буфер &bAllDesktops
	GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);
	//Получение размеров скриншота
	lWidth = bAllDesktops.bmWidth;
	lHeight = bAllDesktops.bmHeight;
	// Удаление временного объекта
	DeleteObject(hTempBitmap);
	//Запись типа типа файла как точечный рисунок
	bfHeader.bfType = (WORD)('B' | ('M' << 8));
	//Смещение битов изображения от начала файла в байтах. 
	bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// Размер файла в байтах
	biHeader.biSize = sizeof(BITMAPINFOHEADER);
	//Палитра
	biHeader.biBitCount = 24;
	//Тип сжатия цветов
	biHeader.biCompression = BI_RGB;
	// Количество плоскостей в битовом изображении
	biHeader.biPlanes = 1;
	//Ширина битового изображения в пикселах
	biHeader.biWidth = lWidth;
	//Высота битового изображения в пикселах
	biHeader.biHeight = lHeight;
	//Запись структуры BITMAPINFOHEADER, которая содержит информацию о размерах и формате цвета.
	bInfo.bmiHeader = biHeader;
	// количество бит на один пиксел
	cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;
	// Функция CreateCompatibleDC создает контекст устройства памяти (DC), совместимый с указанным устройством
	hMemDC = CreateCompatibleDC(hDC);
	//Функция CreateDIBSection создает DIB, в который приложения могут выполнять запись напрямую. Функция предоставляет указатель на расположение битовых значений растрового изображения. 
	hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
	//Функция SelectObject выбирает объект в указанном контексте устройства (DC). Новый объект заменяет предыдущий объект того же типа
	SelectObject(hMemDC, hBitmap);
	//Функция BitBlt выполняет битовую передачу данных о цвете, соответствующих прямоугольнику пикселей, из указанного контекста исходного устройства в контекст устройства назначения
	BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);
	// Создание файлового дескриптора на запись
	hFile = CreateFileW(wPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hDC);
		DeleteObject(hBitmap);
		return FALSE;
	}
	//Запись скриншота с технической информацией в файл
	WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);
	//Сброс всей информации
	FlushFileBuffers(hFile);
	//Закрытие файла на запись
	CloseHandle(hFile);
	//Удаления снимка из памяти
	DeleteDC(hMemDC);
	//Высвобождение дексриктора экрана из доступов других процессов
	ReleaseDC(NULL, hDC);
	//Удалени растового изображения
	DeleteObject(hBitmap);
	return TRUE;
}
int main() 
{
	HWND hWnd = GetForegroundWindow();
	ShowWindow(hWnd, 0);
	//получение пути для сохоанения скриншота
	LPWSTR path = (LPWSTR)L"C:\\1.jpg";
	//Вызов фунции
	SaveBitmap(path);
	ShowWindow(hWnd, 4);
	return 0;
}