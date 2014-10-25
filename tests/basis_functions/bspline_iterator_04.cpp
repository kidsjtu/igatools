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
 *  Test for the BSplineSpace element iterator derivatives values
 *  in a non homogenous range
 *  author: pauletti
 *  date: Aug 21, 2014
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/basis_functions/bspline_element_handler.h>

const std::array<NewValueFlags, 3> der_flag = {NewValueFlags::value,
                                               NewValueFlags::gradient, NewValueFlags::hessian
                                              };

template <int der, int dim, int range=1, int rank=1>
void elem_derivatives(const int n_knots,
                      typename SplineSpace<dim,range,rank>::DegreeTable &deg)
{
    OUTSTART

    using Space = NewBSplineSpace<dim, range, rank>;
    auto grid  = CartesianGrid<dim>::create(n_knots);
    auto space = Space::create(deg, grid);

    auto flag = der_flag[der];
    auto quad = QGauss<dim>(2);
    typename Space::ElementHandler value_handler(space, flag, quad);

    auto elem = space->begin();
    auto end = space->end();

    value_handler.init_element_cache(elem);
    for (; elem != end; ++elem)
    {
        value_handler.fill_element_cache(elem);
        elem->template get_basis_ders<0,der>(0).print_info(out);
    }
    OUTEND
}


int main()
{
    out.depth_console(10);

    const int values = 0;
    const int grad   = 1;
    const int hess   = 2;

    const int n_knots = 3;
    typename SplineSpace<2,2,1>::DegreeTable deg1 = { {{3,2}}, {{2,3}} };
    elem_derivatives<values, 2, 2>(n_knots, deg1);
    elem_derivatives<grad,   2, 2>(n_knots, deg1);
    elem_derivatives<hess,   2, 2>(n_knots, deg1);

    typename SplineSpace<3,3,1>::DegreeTable
    deg2 = { {{3,2,2}}, {{2,3,2}}, {{2,2,3}} };
    elem_derivatives<values, 3, 3>(n_knots, deg2);
    elem_derivatives<grad,   3, 3>(n_knots, deg2);
    elem_derivatives<hess,   3, 3>(n_knots, deg2);

}
