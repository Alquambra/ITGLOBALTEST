#include "ITGLOBALTEST.h"
#include <iostream>
#include <thread>
#include <mutex>


BMP::BMP() : FILE_HEADER_SIZE(14), INFO_HEADER_SIZE(40), FRAME_WIDTH(512), FRAME_HEIGHT(512)
{

}


bmp_status_t BMP::read(string filename)
{
	file = fopen(filename.c_str(), "rb");
	if (file == NULL)
	{
		return FILE_NOT_FOUND;
	}
	size_t res = fread(&file_header, 1, FILE_HEADER_SIZE, file);

	if (
		file_header.type != BITMAP_SIGNATURE_1 &&
		file_header.type != BITMAP_SIGNATURE_2 &&
		file_header.type != BITMAP_SIGNATURE_3
		)
	{
		fclose(file);
		return WRONG_FORMAT;
	}

	res = fread(&info_header, 1, INFO_HEADER_SIZE, file);

	if (
		info_header.width != FRAME_WIDTH ||
		info_header.height != FRAME_HEIGHT
		)
	{
		fclose(file);
		return WRONG_FORMAT;
	}


	uint8_t* buf = new uint8_t[FRAME_HEIGHT * FRAME_WIDTH * 3];
	res = fread(buf, 1, FRAME_HEIGHT * FRAME_WIDTH * 3, file);
	fclose(file);
	if (res != FRAME_HEIGHT * FRAME_WIDTH * 3)
	{
		delete[] buf;
		return WRONG_FORMAT;
	}

	frame.reserve(FRAME_HEIGHT * FRAME_WIDTH);

	uint64_t counter = 0;

	for (int i = 0; i < FRAME_HEIGHT * FRAME_WIDTH; ++i)
	{
		pixel_t pixel;
		pixel.r = buf[counter++];
		pixel.g = buf[counter++];
		pixel.b = buf[counter++];
		frame.push_back(pixel);
	}

	delete[] buf;
	return SUCCESS;
}

bmp_status_t BMP::write(string filename)
{
	file = fopen(filename.c_str(), "wb");
	size_t res = fwrite(&file_header, 1, FILE_HEADER_SIZE, file);
	if (res != FILE_HEADER_SIZE)
	{
		fclose(file);
		return WRITE_ERROR;
	}

	res = fwrite(&info_header, 1, INFO_HEADER_SIZE, file);
	if (res != INFO_HEADER_SIZE)
	{
		fclose(file);
		return WRITE_ERROR;
	}
	uint32_t counter = 0;
	uint8_t* buf = new uint8_t[FRAME_HEIGHT * FRAME_WIDTH * 3];
	for (int i = 0; i < FRAME_HEIGHT * FRAME_WIDTH; ++i)
	{
		buf[counter++] = frame[i].r;
		buf[counter++] = frame[i].g;
		buf[counter++] = frame[i].b;
	}
	fwrite(buf, 1, FRAME_HEIGHT * FRAME_WIDTH * 3, file);
	fclose(file);
	delete[] buf;
	frame.clear();
	return SUCCESS;
}

void BMP::invert()
{
	for (int i = 0; i < FRAME_HEIGHT * FRAME_WIDTH; ++i)
	{
		frame[i].r = ~frame[i].r;
		frame[i].g = ~frame[i].g;
		frame[i].b = ~frame[i].b;
	}

}

void BMP::invertWithThreads()
{
	int cores = thread::hardware_concurrency();
	vector<thread> threads(cores);

	int step = FRAME_HEIGHT * FRAME_WIDTH / cores;

	for (int i = 0; i < cores - 1; ++i)
	{
		threads[i] = thread([&, i]()
		{
			worker(ref(frame), step * i, step * (i + 1));
		});
	}

	threads[cores - 1] = thread([&]()
	{
		worker(ref(frame), step * (cores - 1), frame.size());
	});

	for (int i = 0; i < cores; ++i)
		threads[i].join();

}

void BMP::worker(vector<pixel_t>& row, int start, int end)
{
	for (int i = start; i < end; ++i)
	{
		row[i].r = ~row[i].r;
		row[i].g = ~row[i].g;
		row[i].b = ~row[i].b;
	}
}