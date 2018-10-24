#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

#include "Polyhedron_builder_from_OBJ.h"

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;


/*
check a model is 2-manifold by CGAL::Polyhedron_3::is_closed()

*/

class Checker {
private:
    std::string fileNameA;
    Polyhedron polyA;

public:
    // get info by read .obj file
    Checker(const std::string& fileNameA_) :
        fileNameA(fileNameA_)
    {
    }

    void ReadModelAndCheck();
};

void Checker::ReadModelAndCheck()
{
#define MYDEBUG 0

    polyA.clear();

    std::ifstream ifs1(fileNameA);
    if (!ifs1.is_open())
    {
        std::cerr << "failed to open " << fileNameA << '\n';
        exit(EXIT_FAILURE);
    }

    std::string::size_type n = fileNameA.find_last_of('.');
    std::string fileType = fileNameA.substr(n + 1);
    
    if (fileType == std::string("obj"))
    {
        CGAL::Polyhedron_builder_from_OBJ<HalfedgeDS> obj1(ifs1);
        polyA.delegate(obj1);
    }
    else if (fileType == std::string("off"))
    {
        ifs1 >> polyA;
    }
    else
    {
        std::cerr << "only support file type .obj and .off" << std::endl;
        std::cerr << "do not support file type " << fileType << std::endl;
        exit(EXIT_FAILURE);
    }
    ifs1.close();

    if (!polyA.is_closed())
    {
        std::cout << "<--Powered by CGAL::Polyhedron_3::is_closed()-->" << std::endl;
        std::cout << fileNameA << " is not a 2-manifold polyhedron!\n";
    }
    else
    {
        std::cout << "<--Powered by CGAL::Polyhedron_3::is_closed()-->" << std::endl;
        std::cout << fileNameA << " is a 2-manifold polyhedron!\n";
    }

#if MYDEBUG
    std::cout << polyA << std::endl;
#endif
}


/*

// to run Check2Manifold.exe
// just write
// Check2Manifold.exe C:\\Bimba_0.obj
// Check2Manifold.exe C:\\Bimba_0.off

#include "Check2Manifold.h"
#include <string>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Check2Manifold.exe need 2 arguments");
        return 0;
    }

    std::string filename(argv[1]);

    Checker checker(filename);
    checker.ReadModelAndCheck();

    return 0;
}


*/