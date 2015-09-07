//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
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

#include <igatools/base/value_types.h>

using std::string;

IGA_NAMESPACE_OPEN


namespace grid_element
{
//---------------------------------------------------------------------
const CacheFlags _Point::flag;
const string _Point::name = "Element Quadrature Points";

const CacheFlags _Weight::flag;
const string _Weight::name = "Element Quadrature Weights";

activate::FlagsToCache  activate::grid =
{
  {Flags::point, CacheFlags::point},
  {Flags::weight, CacheFlags::weight}
};
//---------------------------------------------------------------------
};


//---------------------------------------------------------------------
namespace domain_element
{
const CacheFlags _Point::flag;
const string _Point::name = "Element Quadrature Points";

const CacheFlags _Measure::flag;
const string _Measure::name = "Element measure";

const CacheFlags _Gradient::flag;
const string _Gradient::name = "domain gradients";

activate::FlagsToCache  activate::domain =
{
  {Flags::point, CacheFlags::point},
  {Flags::w_measure, CacheFlags::gradient|CacheFlags::measure},
  {Flags::measure, CacheFlags::gradient|CacheFlags::measure},
  {Flags::ext_normal, CacheFlags::gradient}
};

domain_element::activate::FlagsToGrid activate::grid =
{
  {Flags::point, grid_element::Flags::point},
  {Flags::w_measure, grid_element::Flags::weight},
  {Flags::measure, grid_element::Flags::none},
  {Flags::ext_normal, grid_element::Flags::none}
};

};
//---------------------------------------------------------------------



//---------------------------------------------------------------------
namespace grid_function_element
{
const CacheFlags _Point::flag;
const string _Point::name = "Element Quadrature Points";

const CacheFlags _Measure::flag;
const string _Measure::name = "Element measure";

const CacheFlags _Gradient::flag;
const string _Gradient::name = "grid_function gradients";

activate::FlagsToCache  activate::grid_function =
{
  {Flags::point, CacheFlags::point},
  {Flags::w_measure, CacheFlags::gradient|CacheFlags::measure},
  {Flags::measure, CacheFlags::gradient|CacheFlags::measure},
  {Flags::ext_normal, CacheFlags::gradient}
};

grid_function_element::activate::FlagsToGrid activate::grid =
{
  {Flags::point, grid_element::Flags::point},
  {Flags::w_measure, grid_element::Flags::weight},
  {Flags::measure, grid_element::Flags::none},
  {Flags::ext_normal, grid_element::Flags::none}
};

};
//---------------------------------------------------------------------



//---------------------------------------------------------------------
namespace function_element
{
const CacheFlags _Value::flag;
const string _Value::name = "Funtion Values";

const CacheFlags _Gradient::flag;
const string _Gradient::name = "Fuction gradients";

const CacheFlags _D2::flag;
const string _D2::name = "Function D2";

activate::FlagsToCache  activate::function =
{
  {Flags::value, CacheFlags::value},
  {Flags::gradient, CacheFlags::gradient},
  {Flags::D2, CacheFlags::D2}
};

function_element::activate::FlagsToDomain activate::domain =
{
  {Flags::value, domain_element::Flags::none},
  {Flags::gradient, domain_element::Flags::none},
  {Flags::D2, domain_element::Flags::none}
};

};
//---------------------------------------------------------------------



//---------------------------------------------------------------------
namespace space_element
{
const space_element::Flags space_element::_Value::flag;
const string space_element::_Value::name = "Basis function values";

const space_element::Flags space_element::_Gradient::flag;
const string space_element::_Gradient::name = "Basis function gradients";

const space_element::Flags space_element::_Hessian::flag;
const string space_element::_Hessian::name = "Basis function hessians";

const space_element::Flags space_element::_Divergence::flag;
const string space_element::_Divergence::name = "Basis function divergences";
};
//---------------------------------------------------------------------

IGA_NAMESPACE_CLOSE
