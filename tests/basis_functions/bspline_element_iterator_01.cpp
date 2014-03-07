//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------
/*
 *  Test for the bspline element iterator
 *  Computes values and derivatives of the basis functions
 *
 *  author: pauletti
 *  date: Aug 28, 2013
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/bspline_element_accessor.h>

template< int dim_domain, int dim_range >
void do_test()
{
    out << "do_test<" << dim_domain << "," << dim_range << ">" << endl ;

    CartesianProductArray< iga::Real, dim_domain > knots_tmp ;
    CartesianProductArray< Index, dim_domain> multiplicity ;

    auto knots = CartesianGrid<dim_domain>::create();

    const int degree = 2;
    const int rank =  1 ;
    typedef BSplineSpace< dim_domain, dim_range, rank > Space_t ;
    Space_t space(knots, degree);

    const int n_points = 3;
    QGauss< dim_domain > quad_scheme(n_points) ;

    auto element = space.begin();
    element->init_values(ValueFlags::value|ValueFlags::gradient|ValueFlags::hessian,
                         quad_scheme);
    element->fill_values() ;

    auto values    = element->get_basis_values();
    auto gradients = element->get_basis_gradients();
    auto hessians  = element->get_basis_hessians();

    out << "Values basis functions:" << endl ;
    values.print_info(out) ;
    out << endl ;

    out << "Gradients basis functions:" << endl ;
    gradients.print_info(out) ;
    out << endl ;

    out << "Hessians basis functions:" << endl;
    hessians.print_info(out) ;
    out << endl << endl;
}


int main(int argc, char *argv[])
{
    out.depth_console(10);

    do_test<1,1>();
    do_test<1,2>();
    do_test<1,3>();

    do_test<2,1>();
    do_test<2,2>();
    do_test<2,3>();

    do_test<3,1>();
    do_test<3,3>();

    return 0;
}