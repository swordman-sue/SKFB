#ifndef STRUCT_H
#define STRUCT_H

#include "cocos2d.h"

typedef cocos2d::Size COCOSize;
typedef cocos2d::Point COCOPoint;
typedef cocos2d::Color3B RGBColor;

class ImageData
{
public:
	std::string image_path, clip_image_name, frame_name;
	float scale_size, x, y, width, height, offset_x, offset_y;
	ImageData() :x(0.0), y(0.0), scale_size(1.0), width(0.0), height(0.0), offset_x(0), offset_y(0), image_path(""), clip_image_name(""), frame_name(""){}
	ImageData(const std::string& image_path_str, float scale_factor, float dx, float dy, float w, float h, float off_x, float off_y)
		:image_path(image_path_str), scale_size(scale_factor), x(dx), y(dy), width(w), height(h), offset_x(off_x), offset_y(off_y){}
	ImageData(const std::string& image_path_str, const std::string& clip_name_str, float scale_factor, float dx, float dy, float w, float h, float off_x, float off_y)
		:image_path(image_path_str), clip_image_name(clip_name_str), scale_size(scale_factor), x(dx), y(dy), width(w), height(h), offset_x(off_x), offset_y(off_y){}
};


#endif
