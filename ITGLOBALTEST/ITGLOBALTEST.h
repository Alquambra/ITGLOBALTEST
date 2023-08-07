#ifndef ITGLOBALTEST_H
#define ITGLOBALTEST_H

#include <stdint.h>
#include <fstream>
#include <vector>



using namespace std;


typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel_t;


typedef struct
{
	uint16_t type;
	uint32_t size;
	uint32_t reserved;
	uint32_t offset;
} file_header_t;


typedef enum
{
	BITMAP_SIGNATURE_1 = 0x4d42,
	BITMAP_SIGNATURE_2 = 0x4349,
	BITMAP_SIGNATURE_3 = 0x5450
} file_type_t;

typedef struct
{
	uint32_t length;
	uint32_t width;
	uint32_t height;
	uint16_t color_planes;
	uint16_t bit_per_pixel;
	uint32_t compressing;
	uint32_t length_array;
	uint32_t hor_resolution;
	uint32_t ver_resolution;
	uint32_t number_of_colors;
	uint32_t number_of_main_colors;
} info_header_t;


typedef enum
{
	SUCCESS        = 0,
	FILE_NOT_FOUND = 1,
	WRONG_FORMAT   = 2,
	WRITE_ERROR    = 3
} bmp_status_t;





class BMP
{
public:
	BMP();
	bmp_status_t read(string filename);
	bmp_status_t write(string filename);
	void invert();
	void invertWithThreads();
private:
	FILE* file;
	const int FILE_HEADER_SIZE;
	const int INFO_HEADER_SIZE;
	const int FRAME_WIDTH;
	const int FRAME_HEIGHT;
	vector<pixel_t> frame;
	file_header_t file_header;
	info_header_t info_header;
	void worker(vector<pixel_t>& row, int start, int end);

};


#endif