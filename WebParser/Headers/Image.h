#pragma once
struct Image
{
	char* buffer;
	int size;
	std::string extension = "";
	~Image() {
		free(buffer);
		buffer = nullptr;
		size = 0;
	}
};