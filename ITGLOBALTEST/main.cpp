#include "ITGLOBALTEST.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <functional>


template <typename T>
void duration(T f, string function_name)
{
	auto start = chrono::high_resolution_clock::now();
	f();
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << "Duration of function " << function_name << " is " << duration.count() << " us" << endl;
}

void error_description(bmp_status_t status)
{
	switch (status)
	{
		case FILE_NOT_FOUND: 
			cout << "Input file not found" << endl;
			break;
		case WRONG_FORMAT: 
			cout << "wrong format" << endl;
			break;
		case WRITE_ERROR:
			cout << "write error" << endl;
			break;
	}
}

int main()
{
	cout << "Input filename: ";
	string ifilename;
	cin >> ifilename;
	string ofilename;
	cout << "Output filename: ";
	cin >> ofilename;
	BMP bmp;

	bmp_status_t status = bmp.read(ifilename);
	if (status != SUCCESS)
	{
		error_description(status);
		return 0;
	}
	duration([&]() {
		bmp.invertWithThreads();
		}, "invertWithThreads()");

	status = bmp.write(ofilename);
	if (status != SUCCESS)
	{
		error_description(status);
		return 0;
	}


	status = bmp.read(ifilename);
	if (status != SUCCESS)
	{
		error_description(status);
		return 0;
	}
	duration([&]() {
		bmp.invert();
		}, "invert()");

	status = bmp.write(ofilename);
	if (status != SUCCESS)
	{
		error_description(status);
		return 0;
	}
	return 0;
}