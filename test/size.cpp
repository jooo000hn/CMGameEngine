﻿#include<stdio.h>
#include<hgl/algorithm/VectorMath.h>

template<typename T>
void test(T &t)
{
	printf("struct sizeof=%lu\n",sizeof(T));
}

int main(int argc,char **argv)
{
	u16char str[128];

	test(str);

	hgl::Vector3f v3f;

	test(v3f);

	return 0;
}
