// to compile CSG.exe
// -U to union
// -D to diff
// Example:
// "CSG.exe -U C:\\Bimba_0.obj C:\\Union_0.obj C:\\Bimba_0_.obj"  最后一个参数为输出路径

#include "CSG.h"
#include <string>
#include <ctime>

int main(int argc, char** argv)
{
	printf("--------------CSG----------------\n");
	clock_t beginTime = clock();

	//for (int i = 0; i < argc; i++)
	//    printf("Argument %d is %s.\n", i, argv[i]);

	if (argc != 5)
	{
		printf("CSG.exe need 5 arguments\n");
		printf("---------------------------------\n");
		return 0;
	}

	char manipulate = argv[1][1];
	manipulate = toupper(manipulate);

	std::string s1(argv[2]);
	std::string s2(argv[3]);
	std::string s3(argv[4]);

	CSGER csg(s1, s2, s3);
	csg.Read2OBJs();

	switch (manipulate)
	{
	case 'U':
	case 'u':
		csg.Union();
		csg.WriteResult2OBJ();
		break;
	case 'D':
	case 'd':
		csg.Difference();
		csg.WriteResult2OBJ();
		break;
	default:
		break;
	}

	clock_t endTime = clock();
	float endElapsed = ((float)(endTime - beginTime)) / (CLOCKS_PER_SEC); // Time unit: second
	printf("Total Time taken for csg is %.3f  seconds \n", endElapsed);
	printf("---------------------------------\n");


	return 0;
}
