
#include "GrayImage.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

GrayImage::GrayImage()
{
	_data = nullptr;
	_refCount = nullptr;
}

GrayImage::GrayImage(int img_width, int img_height, std::vector<uint8_t> const& data)
{
	_stride = img_width;
	_width = img_width;
	_height = img_height;
	_data = new uint8_t[_width * _height];
	if (!data.empty())
	{
		for (int y = 0; y < _height; y++)
		{
			for (int x = 0; x < _width; x++)
			{
				setPixel(y, x, data[y*_stride+x]);
			}
		}
	}
	_refCount = new size_t;
	*_refCount = 1;
}

GrayImage::GrayImage(GrayImage const& base, int x, int y, int ROI_width, int ROI_height)
{
	_stride = base._stride;
	_width = ROI_width;
	_height = ROI_height;
	_data = base._data+(y*_stride+x);
	_refCount = base._refCount;
	(*_refCount)++;
}

GrayImage::GrayImage(GrayImage const& source)
{
	_width = source._width;
	_height = source._height;
	_stride = source._stride;
	bool f = _refCount == nullptr;		
	if (_refCount != nullptr&&--(*_refCount) == 0)
		{
			delete[] _data;
		}
	_refCount = source._refCount;
	_data = source._data;
	(*_refCount)++;
}

GrayImage const& GrayImage::operator=(GrayImage const& other)
{
	_width = other._width;
	_height = other._height;
	_stride = other._stride;
	if (_refCount != nullptr && --(*_refCount) == 0)
	{
		delete[] _data;
	}
	_refCount = other._refCount;
	_data = other._data;
	(*_refCount)++;
	return *this;
}

GrayImage const& GrayImage::operator=(uint8_t const& value)
{
	uint8_t* dst_ptr, * src1_ptr, * src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		dst_ptr = _data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			*(dst_ptr++) = value;
		}
	}
	return *this;
}

GrayImage::GrayImage(GrayImage&& source)noexcept
{
	_width = source._width;
	_height = source._height;
	_stride = source._stride;
	_refCount = source._refCount;
	_data = source._data;
	if(*source._refCount==1)
		source._data = nullptr;
}

GrayImage::~GrayImage()
{
	if (--(*_refCount) == 0)
	{
		delete[]_data;
		delete _refCount;
	}	
}

GrayImage::operator std::vector<uint8_t>() const
{
	std::vector<uint8_t> imageData(_width * _height);
	uint8_t* src;
	for (int y = 0; y < _height; y++)
	{
		src = _data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			imageData[y * _width + x] = *(src++);
		}
	}

	return imageData;
}


int GrayImage::getWidth() const
{
	return _width;
}

int GrayImage::getHeight() const
{
	return _height;
}

size_t GrayImage::getRefCount() const
{
	return *_refCount;
}

int GrayImage::getStride() const
{
	return _stride;
}

uint8_t const* GrayImage:: data() const
{
	return _data;
}

uint8_t GrayImage::getPixel(int row, int col) const
{
	return _data[_stride*row+col];
}

void GrayImage::setPixel(int row, int col, uint8_t value)
{
	_data[_stride * row + col] = value;
}

//before change takes 13 ms
//after change takes 3 ms
GrayImage GrayImage::operator+(const GrayImage& other)const
{
	//const auto start = std::chrono::system_clock::now();
	if (_width != other._width || _height != other._height)
		throw "The width and height not much";
	GrayImage newImage(_width, _height);
	uint8_t* dst_ptr, * src1_ptr, * src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		dst_ptr = newImage._data + y * _stride;
		src1_ptr = _data + y * _stride;
		src2_ptr = other._data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			//newImage.setPixel(y, x, getPixel(y,x)+rhs.getPixel(y,x));
			*(dst_ptr++) = *(src1_ptr++) + *(src2_ptr++);
		}
	}
	//const auto end = std::chrono::system_clock::now();
	//const auto time = end-start;
	//const std::time_t t_c = std::chrono::system_clock::to_time_t(time);
	//std::cout << "The system clock is currently at " << std::ctime(&t_c);
	return newImage;
}

GrayImage GrayImage::operator-(const GrayImage& other)const
{
	if (_width != other._width || _height != other._height)
		throw "The width and height not much";
	GrayImage newImage(_width, _height);
	uint8_t* dst_ptr, * src1_ptr, * src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		dst_ptr = newImage._data + y * _stride;
		src1_ptr = _data + y * _stride;
		src2_ptr = other._data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			*(dst_ptr++) = *(src1_ptr++) - *(src2_ptr++);
		}
	}
	return newImage;
}

GrayImage GrayImage::mul(const GrayImage& other)const
{
	if (_width != other._width || _height != other._height)
		throw "The width and height not much";
	GrayImage newImage(_width, _height);
	uint8_t* dst_ptr, * src1_ptr, * src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		dst_ptr = newImage._data + y * _stride;
		src1_ptr = _data + y * _stride;
		src2_ptr = other._data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			*(dst_ptr++) = *(src1_ptr++) * *(src2_ptr++);
		}
	}
	return newImage;
}

GrayImage GrayImage::absdiff(const GrayImage& other) const
{
	if (_width != other._width || _height != other._height)
		throw "The width and height not much";
	GrayImage newImage(_width, _height);
	uint8_t* dst_ptr, * src1_ptr, * src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		dst_ptr = newImage._data + y * _stride;
		src1_ptr = _data + y * _stride;
		src2_ptr = other._data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			int diff = *(src1_ptr++) - *(src2_ptr++);
			*(dst_ptr++) = (diff > 0) ? diff : -diff;
		}
	}
	return newImage;
}

GrayImage GrayImage::operator*(GrayImage const& other) const {
	if (_width != other.getHeight())
		throw("the _height or the _width is not the same");
	GrayImage newImage(_width, _height);
	for (size_t y = 0; y < _width; y++)
	{
		uint8_t sum = 0;
		for (size_t x = 0; x < other.getHeight(); x++)
		{
			uint8_t num = newImage.getPixel(y, x) * other.getPixel(x, y);
			sum += num;
			newImage.setPixel(y, x, sum);
		}
	}
	return newImage;
}



GrayImage GrayImage::operator*(uint8_t const& value)const
{
	GrayImage newImage(_width, _height);
	uint8_t* dst_ptr, * src_ptr;
	for (int y = 0; y < _height; y++)
	{
		dst_ptr = newImage._data + y * _stride;
		src_ptr = _data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			*(dst_ptr++) = *(src_ptr++) * value;
		}
	}
	return newImage;
}

bool GrayImage::operator ==(GrayImage const& other) const
{
	if (_width != other._width || _height != other._height)
		return false;
	uint8_t *src1_ptr, *src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		src1_ptr = _data + y * _stride;
		src2_ptr = other._data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			if(*(src1_ptr++) != *(src2_ptr++))
				return false;
		}
	}
	return true;
}

bool GrayImage::operator==(uint8_t const& value)const
{
	uint8_t* src1_ptr, * src2_ptr;
	for (int y = 0; y < _height; y++)
	{
		src1_ptr = _data + y * _stride;
		for (int x = 0; x < _width; x++)
		{
			if (*(src1_ptr++) != value)
				return false;
		}
	}
	return true;
}
//void GrayImage::save(string const& path) const
//{
//	ofstream ofs(path,ios::binary);
//	ofs.write((char*)&width,sizeof width);
//	ofs.write((char*)&height, sizeof height);
//	ofs.write((char*)image, width * height);
//	ofs.flush();
//	ofs.close();
//}
//void GrayImage::load(string const& path)
//{
//	ifstream ifs(path, ios::binary);
//	ifs.read((char*)&width, sizeof width);
//	ifs.read((char*)&height, sizeof height);
//	ifs.read((char*)image, (sizeof(uint8_t) * width * height));
//}

template <typename T>
void WriteBytes(std::ofstream& ofs, T t) {
	ofs.write((char*)(&t), sizeof(T));
}

void BMP_WriteHeader(int width, int height, std::ofstream& ofs) {
	int numPixels = width * height;
	const int NUM_SAVED_CHANNELS = 3;
	//BMP Header
	WriteBytes(ofs, uint16_t(0x4D42)); //signature
	WriteBytes(ofs, uint32_t(0x36 + numPixels)); //size of the BMP file (all bytes)
	WriteBytes(ofs, uint16_t(0)); // unused
	WriteBytes(ofs, uint16_t(0)); // unused
	WriteBytes(ofs, uint32_t(0x36)); //offset where the pixel aray begins (size of both headers)
	//DIB Headers
	WriteBytes(ofs, uint32_t(0x28)); //Number of bytes in DIB header (without this field)
	WriteBytes(ofs, uint32_t(width));//width
	WriteBytes(ofs, uint32_t(height));//height
	WriteBytes(ofs, uint32_t(1));//number of planes used
	WriteBytes(ofs, uint16_t(NUM_SAVED_CHANNELS * 8));//bit depth
	WriteBytes(ofs, uint32_t(0));//no compression
	WriteBytes(ofs, uint32_t(numPixels));//size of row bitmap data (including padding)
	WriteBytes(ofs, uint32_t(0xB13));//print resolution pixels/ meter X
	WriteBytes(ofs, uint32_t(0xB13));//print resolution pixels/ meter Y
	WriteBytes(ofs, uint32_t(0));//0 colors in the color palette
	WriteBytes(ofs, uint32_t(0));//0 means all colors are important
}

void GrayImage::saveToBMP(std::string const& filepath)const
{
	std::ofstream ofs(filepath, std::ios::binary);
	BMP_WriteHeader(_width, _height, ofs);
	for (int row = getHeight() - 1; row >= 0; row--)
	{
		for (int col = 0; col < getWidth(); col++)
		{
			for (int i = 0; i < 3; i++)
			{
				ofs.write(reinterpret_cast<char*>(&_data[row * _width + col]),sizeof uint8_t);
			}
		}
	}
}


//void GrayImage::copyTo(GrayImage& other, int y, int x)
//{
//	if (!other.isOwner)
//		throw "Not original image";
//	if (x + width > other.width || y + height > other.height)
//		throw("unsuitable sizes of image!");
//	for (int i = y; i < height + y; i++) {
//		for (int j = x; j < width + x; j++) {
//			other.setPixel(i, j, getPixel(i - y, j - x));
//		}
//	}
//}





