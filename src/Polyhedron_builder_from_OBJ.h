// Copyright (c) 2015 GeometryFactory
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3 of the License,
// or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Author(s)     : Hongfeixu

#ifndef CGAL_IO_POLYHEDRON_STL_OBJ_H
#define CGAL_IO_POLYHEDRON_STL_OBJ_H

#include <CGAL/license/Polyhedron.h>


#include <CGAL/Modifier_base.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/IO/OBJ_reader.h>

#include <iostream>
#include <vector>

namespace CGAL {

	std::istream &operator >> (std::istream  &input, cpp11::array<double, 3> &p)
	{
		input >> p[0] >> p[1] >> p[2];
		return input;
	}

	template <class HDS>
	class Polyhedron_builder_from_OBJ : public CGAL::Modifier_base<HDS> {
		typedef typename HDS::Vertex::Point Point_3;
		typedef std::vector<cpp11::array<double, 3> > Points_3;
		typedef cpp11::array<int, 3> Facet;
		typedef std::vector<Facet> Surface;

		std::istream& is;
		Points_3 meshPoints;
		Surface mesh;

		std::vector<std::vector<std::size_t>> faces;

	public:

		Polyhedron_builder_from_OBJ(std::istream& is_)
			: is(is_)
		{
		}

		void operator()(HDS& hds)
		{
			if (!read_OBJ(is, meshPoints, faces)) return;

			// faces -> mesh
			int cnt = faces.size();
			for (int i = 0; i < cnt; ++i)
			{
				Facet temp;
				temp[0] = faces[i][0];
				temp[1] = faces[i][1];
				temp[2] = faces[i][2];
				mesh.push_back(temp);
			}


			CGAL::Polyhedron_incremental_builder_3<HDS> B(hds);
			B.begin_surface(meshPoints.size(), mesh.size());
			typedef typename Points_3::size_type size_type;

			for (size_type i = 0; i < meshPoints.size(); i++) {
				B.add_vertex(
					Point_3(meshPoints[i][0], meshPoints[i][1], meshPoints[i][2])
				);
			}

			for (size_type i = 0; i < mesh.size(); i++) {
				B.begin_facet();
				B.add_vertex_to_facet(mesh[i][0]);
				B.add_vertex_to_facet(mesh[i][1]);
				B.add_vertex_to_facet(mesh[i][2]);
				B.end_facet();
			}
			if (B.error())
			{
				std::cerr << "An error occured while creating a Polyhedron" << std::endl;
				B.rollback();
			}

			B.end_surface();
		}
	};

} //end of CGAL namespace

#endif // CGAL_IO_POLYHEDRON_OBJ_BUILDER_H
