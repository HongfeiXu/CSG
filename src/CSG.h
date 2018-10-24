#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/boost/graph/copy_face_graph.h>
#include <CGAL/IO/print_wavefront.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

#include "Polyhedron_builder_from_OBJ.h"


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;
typedef CGAL::Surface_mesh<Kernel::Point_3> Surface_mesh;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;


class CSGER {
private:
    std::string fileNameA;
    std::string fileNameB;
    std::string resultFilePath;

    Polyhedron polyA;
    Polyhedron polyB;

    Polyhedron result;      // save the result polyhedron

public:
    // get info by read .obj file
    CSGER(const std::string& fileNameA_, const std::string& fileNameB_, const std::string& resultFilePath_) :
        fileNameA(fileNameA_),
        fileNameB(fileNameB_),
        resultFilePath(resultFilePath_)
    {
    }

    void Read2OBJs();
    void Union();           // polyA + polyB
    void Difference();      // polyA - polyB
    void WriteResult2OBJ();
};


void CSGER::Read2OBJs()
{
#define MYDEBUG 0

    polyA.clear();
    polyB.clear();

    std::ifstream ifs1(fileNameA);
    if (!ifs1.is_open())
    {
        std::cerr << "failed to open " << fileNameA << '\n';
        printf("---------------------------------\n");
        exit(EXIT_FAILURE);
    }
    CGAL::Polyhedron_builder_from_OBJ<HalfedgeDS> obj1(ifs1);
    polyA.delegate(obj1);
    ifs1.close();

    if (!polyA.is_closed())
    {
        std::cerr << "left operand \" " << fileNameA << " \" is not a 2-manifold polyhedron!\n";
        printf("---------------------------------\n");
        exit(EXIT_FAILURE);
    }

#if MYDEBUG
    std::cout << polyA << std::endl;
#endif

    std::ifstream ifs2(fileNameB);
    if (!ifs2.is_open())
    {
        std::cerr << "failed to open " << fileNameB << '\n';
        printf("---------------------------------\n");
        exit(EXIT_FAILURE);
    }
    CGAL::Polyhedron_builder_from_OBJ<HalfedgeDS> obj2(ifs2);
    polyB.delegate(obj2);
    ifs2.close();

    if (!polyB.is_closed())
    {
        std::cerr << "left operand \" " << fileNameB << " \" is not a 2-manifold polyhedron!\n";
        printf("---------------------------------\n");
        exit(EXIT_FAILURE);
    }

#if MYDEBUG
    std::cout << polyB << std::endl;
#endif
}

void CSGER::Union()
{
    result.clear();

    Nef_polyhedron nef1(polyA);     // TIME CONSUMING
    Nef_polyhedron nef2(polyB);

    // compute the union of the nested cubes
    Nef_polyhedron nef = nef1 + nef2;

	// new 

	//if (nef.is_simple())
	//{
	//	nef.convert_to_Polyhedron(result);
	//}
	//else
	//{
	//	std::cerr << "CSGER::Union() result is not a 2-manifold polyhedron!\n";
	//	printf("---------------------------------\n");
	//	exit(EXIT_FAILURE);
	//}

    // Conversion of Nef_polyhedron_3 to Polygon Mesh
    Surface_mesh output;
    CGAL::convert_nef_polyhedron_to_polygon_mesh(nef, output);

    // Conversion Polygon mesh to Polyhedron
    CGAL::copy_face_graph(output, result);
}

void CSGER::Difference()
{
    result.clear();

    Nef_polyhedron nef1(polyA);
    Nef_polyhedron nef2(polyB);
    // compute the difference of the nested cubes
    Nef_polyhedron nef = nef1 - nef2;

	// new 

	//if (nef.is_simple())
	//{
	//	nef.convert_to_Polyhedron(result);
	//}
	//else
	//{
	//	std::cerr << "CSGER::Difference() result is not a 2-manifold polyhedron!\n";
	//	printf("---------------------------------\n");
	//	exit(EXIT_FAILURE);
	//}

    // Conversion of Nef_polyhedron_3 to Polygon Mesh
    Surface_mesh output;
    CGAL::convert_nef_polyhedron_to_polygon_mesh(nef, output);

    // Conversion Polygon mesh to Polyhedron
    CGAL::copy_face_graph(output, result);
}

void CSGER::WriteResult2OBJ()
{
#define MYDEBUG 0

    if (result.empty())
        return;

    if (!result.is_closed())
    {
        std::cerr << "csg result \" "<<resultFilePath << " \" is not a 2-manifold polyhedron!\n";
        printf("---------------------------------\n");
        exit(EXIT_FAILURE);
    }

#if MYDEBUG
    std::cout << result << std::endl;
#endif

    std::ofstream ofs(resultFilePath);

    CGAL::Polygon_mesh_processing::triangulate_faces(result);       // triangulate Polyhedron faces, make it a pure triangle mesh

    CGAL::print_polyhedron_wavefront(ofs, result);        // save as .obj
    ofs.close();

    std::cout << "write result to obj file: " << resultFilePath << std::endl;
}

/*

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


*/