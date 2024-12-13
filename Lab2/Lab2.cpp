#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
constexpr auto M_PI = 3.141592653589793238462643383279502884L;

using namespace std;

#pragma comment(lib, "winmm.lib")

#undef min
#undef max

typedef struct
{
	uint8_t r, g, b, a;
} rgb32;

#if !defined(_WIN32) && !defined(_WIN64)
#pragma pack(2)
typedef struct
{
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BITMAPFILEHEADER;
#pragma pack()

#pragma pack(2)
typedef struct
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int16_t biXPelsPerMeter;
	int16_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack()
#endif

#pragma pack(2)
typedef struct
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
} BMPINFO;
#pragma pack()

class bitmap
{
private:
	BMPINFO bmpInfo;
	uint8_t* pixels;

public:
	bitmap(const char* path);
	~bitmap();

	void save(const char* path, uint16_t bit_count = 24);

	rgb32* getPixel(uint32_t x, uint32_t y) const;
	void setPixel(rgb32* pixel, uint32_t x, uint32_t y);

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	uint16_t bitCount() const;
};

bitmap::bitmap(const char* path)
	: bmpInfo()
	, pixels(nullptr)
{
	ifstream file(path, ios::in | ios::binary);

	if (file)
	{
		file.read(reinterpret_cast<char*>(&bmpInfo.bfh), sizeof(bmpInfo.bfh));

		if (bmpInfo.bfh.bfType != 0x4d42)
		{
			throw runtime_error("Invalid format. Only bitmaps are supported.");
		}

		file.read(reinterpret_cast<char*>(&bmpInfo.bih), sizeof(bmpInfo.bih));

		if (bmpInfo.bih.biCompression != 0)
		{
			cerr << bmpInfo.bih.biCompression << "\n";
			throw runtime_error("Invalid bitmap. Only uncompressed bitmaps are supported.");
		}

		if (bmpInfo.bih.biBitCount != 24 && bmpInfo.bih.biBitCount != 32)
		{
			throw runtime_error("Invalid bitmap. Only 24bit and 32bit bitmaps are supported.");
		}

		file.seekg(bmpInfo.bfh.bfOffBits, ios::beg);

		pixels = new uint8_t[bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits];
		file.read(reinterpret_cast<char*>(&pixels[0]), bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits);

		uint8_t* temp = new uint8_t[bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * sizeof(rgb32)];

		uint8_t* in = pixels;
		rgb32* out = reinterpret_cast<rgb32*>(temp);
		int padding = bmpInfo.bih.biBitCount == 24
			? ((bmpInfo.bih.biSizeImage - bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * 3) / bmpInfo.bih.biHeight)
			: 0;

		for (int i = 0; i < bmpInfo.bih.biHeight; ++i, in += padding)
		{
			for (int j = 0; j < bmpInfo.bih.biWidth; ++j)
			{

				out->b = *(in++);
				out->g = *(in++);
				out->r = *(in++);
				out->a = bmpInfo.bih.biBitCount == 32 ? *(in++) : 0xFF;
				++out;
			}
		}

		delete[] pixels;
		pixels = temp;
	}
}

bitmap::~bitmap()
{
	delete[] pixels;
}

void bitmap::save(const char* path, uint16_t bit_count)
{
	ofstream file(path, ios::out | ios::binary);

	if (file)
	{
		bmpInfo.bih.biBitCount = bit_count;
		uint32_t size = ((bmpInfo.bih.biWidth * bmpInfo.bih.biBitCount + 31) / 32) * 4 * bmpInfo.bih.biHeight;
		bmpInfo.bfh.bfSize = bmpInfo.bfh.bfOffBits + size;

		file.write(reinterpret_cast<char*>(&bmpInfo.bfh), sizeof(bmpInfo.bfh));
		file.write(reinterpret_cast<char*>(&bmpInfo.bih), sizeof(bmpInfo.bih));
		file.seekp(bmpInfo.bfh.bfOffBits, ios::beg);

		uint8_t* out = NULL;
		rgb32* in = reinterpret_cast<rgb32*>(pixels);
		uint8_t* temp = out = new uint8_t[bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * sizeof(rgb32)];
		int padding = bmpInfo.bih.biBitCount == 24
			? ((bmpInfo.bih.biSizeImage - bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * 3) / bmpInfo.bih.biHeight)
			: 0;

		for (int i = 0; i < bmpInfo.bih.biHeight; ++i, out += padding)
		{
			for (int j = 0; j < bmpInfo.bih.biWidth; ++j)
			{
				*(out++) = in->b;
				*(out++) = in->g;
				*(out++) = in->r;

				if (bmpInfo.bih.biBitCount == 32)
				{
					*(out++) = in->a;
				}
				++in;
			}
		}

		file.write(reinterpret_cast<char*>(&temp[0]), size);
		delete[] temp;
	}
}

rgb32* bitmap::getPixel(uint32_t x, uint32_t y) const
{
	rgb32* temp = reinterpret_cast<rgb32*>(pixels);
	return &temp[(bmpInfo.bih.biHeight - 1 - y) * bmpInfo.bih.biWidth + x];
}

void bitmap::setPixel(rgb32* pixel, uint32_t x, uint32_t y)
{
	rgb32* temp = reinterpret_cast<rgb32*>(pixels);
	memcpy(&temp[(bmpInfo.bih.biHeight - 1 - y) * bmpInfo.bih.biWidth + x], pixel, sizeof(rgb32));
};

uint32_t bitmap::getWidth() const
{
	return bmpInfo.bih.biWidth;
}

uint32_t bitmap::getHeight() const
{
	return bmpInfo.bih.biHeight;
}

uint16_t bitmap::bitCount() const
{
	return bmpInfo.bih.biBitCount;
}

struct Params
{
	bitmap* init_bmp;
	bitmap* blur_bmp;
	int number;
	uint32_t startHeight;
	uint32_t endHeight;
	uint32_t startWidth;
	uint32_t endWidth;
	int partWidth;
	int partHeight;
	int countThreads;
	int hRemaining;
	int wRemaining;
};

void blur(bitmap* init_bmp, bitmap* blur_bmp, int radius, Params* params)
{
	float rs = ceil(radius * 2.57);
	for (int i = params->startHeight; i < params->endHeight; ++i)
	{
		for (int j = params->startWidth; j < params->endWidth; ++j)
		{
			double r = 0, g = 0, b = 0;
			double count = 0;

			for (int iy = i - rs; iy < i + rs + 1; ++iy)
			{
				for (int ix = j - rs; ix < j + rs + 1; ++ix)
				{
					auto x = min(static_cast<int>(params->endWidth) - 1, max(0, ix));
					auto y = min(static_cast<int>(params->endHeight) - 1, max(0, iy));

					auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
					auto wght = exp(-dsq / (2.0 * radius * radius)) / (M_PI * 2.0 * radius * radius);

					rgb32* pixel = init_bmp->getPixel(x, y);

					r += pixel->r * wght;
					g += pixel->g * wght;
					b += pixel->b * wght;
					count += wght;
				}
			}

			rgb32* pixel = blur_bmp->getPixel(j, i);
			pixel->r = round(r / count);
			pixel->g = round(g / count);
			pixel->b = round(b / count);
		}
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	struct Params* params = (struct Params*)lpParam;
	for (int i = 0; i < params->countThreads; i++)
	{
		params->startHeight = params->partHeight * i;
		params->endHeight = (params->partHeight * (i + 1)) + (i == params->countThreads - 1 ? params->hRemaining : 0);
		params->startWidth = params->number * params->partWidth;
		params->endWidth = ((params->number + 1) * params->partWidth) + (params->number == params->countThreads - 1
			? params->wRemaining : 0);

		blur(params->init_bmp, params->blur_bmp, 5, params);
	}
	ExitThread(0);
}

void threads_runner(bitmap* init_bmp, bitmap* blur_bmp, int radius, int threadsCount, int coreCount)
{
	int partWidth = init_bmp->getWidth() / threadsCount;
	int partHeight = init_bmp->getHeight() / threadsCount;

	int w = init_bmp->getWidth() - partWidth * threadsCount;
	int widthRemaining = max(w, 0);
	int h = init_bmp->getHeight() - partHeight * threadsCount;
	int heightRemaining = max(h, 0);

	Params* arrayParams = new Params[threadsCount];
	for (int i = 0; i < threadsCount; i++)
	{
		Params params;
		params.init_bmp = init_bmp;
		params.blur_bmp = blur_bmp;
		params.partWidth = partWidth;
		params.partHeight = partHeight;
		params.countThreads = threadsCount;
		params.number = i;
		params.hRemaining = heightRemaining;
		params.wRemaining = widthRemaining;
		arrayParams[i] = params;
	}

	HANDLE* handles = new HANDLE[threadsCount];
	for (int i = 0; i < threadsCount; i++)
	{
		handles[i] = CreateThread(NULL, i, &ThreadProc, &arrayParams[i], CREATE_SUSPENDED, NULL);
		SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
	}

	for (int i = 0; i < threadsCount; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);
}

int main(int argc, const char** argv)
{
	
	DWORD start = timeGetTime();

	if (strcmp(argv[1], "/") == 0)
	{
		cout << "Example: Lab2.exe input.bmp output.bmp 3 3" << endl;
		cout << "1 argument - input bmp file" << endl;
		cout << "2 argument - output bmp file" << endl;
		cout << "3 argument - threads count" << endl;
		cout << "4 argument - core count" << endl;

		exit(0);
	}

	int threads_count = atoi(argv[3]);

	bitmap init_bmp{ argv[1] };
	bitmap blur_bmp{ argv[1] };

	threads_runner(&init_bmp, &blur_bmp, 5, threads_count, atoi(argv[4]));

	blur_bmp.save(argv[2]);

	cout << timeGetTime() - start << endl;

	return 0;
}
