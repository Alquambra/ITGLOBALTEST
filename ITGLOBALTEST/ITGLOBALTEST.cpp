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

	frame.reserve(FRAME_HEIGHT);

	uint32_t counter = 0;

	for (int i = 0; i < FRAME_HEIGHT; ++i)
	{
		vector<pixel_t> row;
		row.reserve(FRAME_WIDTH);
		for (int j = 0; j < FRAME_WIDTH; ++j)
		{
			pixel_t pixel;
			pixel.r = buf[counter++];
			pixel.g = buf[counter++];
			pixel.b = buf[counter++];
			row.push_back(pixel);
		}
		frame.push_back(row);
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
	for (int i = 0; i < FRAME_HEIGHT; ++i)
	{
		for (int j = 0; j < FRAME_WIDTH; ++j)
		{
			buf[counter++] = frame[i][j].r;
			buf[counter++] = frame[i][j].g;
			buf[counter++] = frame[i][j].b;
		}
	}
	fwrite(buf, 1, FRAME_HEIGHT * FRAME_WIDTH * 3, file);
	fclose(file);
	delete[] buf;
	frame.clear();
	return SUCCESS;
}

void BMP::invert()
{
	for (int i = 0; i < FRAME_HEIGHT; ++i)
	{
		for (int j = 0; j < FRAME_WIDTH; ++j)
		{
			frame[i][j].r = ~frame[i][j].r;
			frame[i][j].g = ~frame[i][j].g;
			frame[i][j].b = ~frame[i][j].b;
		}
	}

}

void BMP::invertWithThreads()
{
	int cores = thread::hardware_concurrency();
	vector<thread> threads(cores);

	int row_number = 0;
	for (;row_number < FRAME_HEIGHT - cores; row_number += cores)
	{
		for (int i = 0; i < cores; ++i)
		{
			threads[i] = thread([&, row_number]()
				{
					worker(frame[row_number + i]);
				});
			threads[i].join();
		}
	}
	int i = row_number;
	for (;row_number < FRAME_HEIGHT; ++row_number)
	{
		threads[row_number - i] = thread([&, row_number]()
			{
				worker(frame[row_number]);
			});
		threads[row_number - i].join();
	}
}

void BMP::worker(vector<pixel_t>& row)
{
	for (int i = 0; i < FRAME_WIDTH; ++i)
	{
		row[i].r = ~row[i].r;
		row[i].g = ~row[i].g;
		row[i].b = ~row[i].b;
	}
}