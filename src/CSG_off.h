#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/IO/Polyhedron_builder_from_OBJ.h>
#include <CGAL/boost/graph/copy_face_graph.h>
#include <CGAL/IO/print_wavefront.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;
typedef CGAL::Surface_mesh<Kernel::Point_3> Surface_mesh;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;


class CSGER_off {
private:
    std::string fileNameA;
    std::string fileNameB;
    std::string resultFilePath;

    Polyhedron polyA;
    Polyhedron polyB;

    Polyhedron result;      // save the result polyhedron

public:
    // get info by read .obj file
    CSGER_off(const std::string& fileNameA_, const std::string& fileNameB_, const std::string& resultFilePath_) :
        fileNameA(fileNameA_),
        fileNameB(fileNameB_),
        resultFilePath(resultFilePath_)
    {
    }

    void Read2Offs();
    void Union();           // polyA + polyB
    void Difference();      // polyA - polyB
    void WriteResult2OFF();
};


void CSGER_off::Read2Offs()
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
    ifs1 >> polyA;
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
    ifs2 >> polyB;
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

void CSGER_off::Union()
{
    result.clear();

    Nef_polyhedron nef1(polyA);     // TIME CONSUMING
    Nef_polyhedron nef2(polyB);

    // compute the union of the nested cubes
    Nef_polyhedron nef = nef1 + nef2;

    // Conversion of Nef_polyhedron_3 to Polygon Mesh
    Surface_mesh output;
    CGAL::convert_nef_polyhedron_to_polygon_mesh(nef, output);

    // Conversion Polygon mesh to Polyhedron
    CGAL::copy_face_graph(output, result);
}

void CSGER_off::Difference()
{
    result.clear();

    Nef_polyhedron nef1(polyA);
    Nef_polyhedron nef2(polyB);
    // compute the difference of the nested cubes
    Nef_polyhedron nef = nef1 - nef2;

    // Conversion of Nef_polyhedron_3 to Polygon Mesh
    Surface_mesh output;
    CGAL::convert_nef_polyhedron_to_polygon_mesh(nef, output);

    // Conversion Polygon mesh to Polyhedron
    CGAL::copy_face_graph(output, result);
}

void CSGER_off::WriteResult2OFF()
{
#define MYDEBUG 0

    if (result.empty())
        return;

    if (!result.is_closed())
    {
        std::cerr << "csg result \" " << resultFilePath << " \" is not a 2-manifold polyhedron!\n";
        printf("---------------------------------\n");
        exit(EXIT_FAILURE);
    }

#if MYDEBUG
    std::cout << result << std::endl;
#endif

    std::ofstream ofs(resultFilePath);
    CGAL::Polygon_mesh_processing::triangulate_faces(result);       // triangulate Polyhedron faces, make it a pure triangle mesh
    ofs << result;
    ofs.close();

    std::cout << "write result to off file: " << resultFilePath << std::endl;
}

/*

// to compile CSG.exe
// -U to union
// -D to diff
// Example:
// "CSG.exe -U C:\\Bimba_0.off C:\\Union_0.off C:\\Bimba_0_.off"  最后一个参数为输出路径

#include "CSG_off.h"
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

    CSGER_off csg(s1, s2, s3);
    csg.Read2Offs();

    switch (manipulate)
    {
    case 'U':
        csg.Union();
        csg.WriteResult2OFF();
        break;
    case 'D':
        csg.Difference();
        csg.WriteResult2OFF();
        break;
    default:
        break;
    }

    clock_t endTime = clock();
    float endElapsed = ((float) (endTime - beginTime)) / (CLOCKS_PER_SEC); // Time unit: second
    printf("Total Time taken for csg is %.3f  seconds \n", endElapsed);
    printf("---------------------------------\n");


    return 0;
}

*/