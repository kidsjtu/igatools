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

#include <igatools/geometry/grid_element_handler.h>
#include <igatools/geometry/unit_element.h>
using std::shared_ptr;
using std::array;

IGA_NAMESPACE_OPEN

template <int dim_>
const array<Size, UnitElement<dim_>::faces_per_element>
GridElementHandler<dim_>::faces  = UnitElement<dim_>::faces;

template <int dim_>
GridElementHandler<dim_>::
GridElementHandler(shared_ptr<const GridType> grid,
                     const ValueFlags flag,
                     const Quadrature<dim> &quad)
    :
    grid_(grid),
    flags_ {flag, flag},
    quad_(quad),
       lengths_(grid->get_element_lengths())
{}


template <int dim_>
void
GridElementHandler<dim_>::
reset(const ValueFlags flag,
      const Quadrature<dim> &quad)
{
	flags_ = std::make_tuple(flag, flag);
	quad_ = quad;
}



template <int dim_>
void
GridElementHandler<dim_>::
init_element_cache(ElementAccessor &elem)
{
    auto &cache = elem.local_cache_;
    if (cache == nullptr)
    {
        using Cache = typename ElementAccessor::LocalCache;
        cache = shared_ptr<Cache>(new Cache);
    }

    auto &elem_cache = cache->template get_value_cache<0>(0);
    elem_cache.resize(std::get<0>(flags_), quad_);


    for (auto &f: faces)
    {
        auto &face_cache = cache->template get_value_cache<1>(f);
        face_cache.resize(std::get<1>(flags_), quad_.collapse_to_face(f));
    }
}




template <int dim_>
void
GridElementHandler<dim_>::
init_element_cache(ElementIterator &elem)
{
    init_element_cache(elem.get_accessor());
}

template <int dim_>
template <int k>
void
GridElementHandler<dim_>::
fill_element_cache_(ElementAccessor &elem, const int j)
{
    Assert(elem.local_cache_ != nullptr,ExcNullPtr());
    auto &cache = elem.local_cache_->template get_value_cache<k>(j);

    const auto &index = elem.get_tensor_index();
    auto &flags = cache.flags_handler_;
    auto all_elems = UnitElement<dim>::all_elems;
    static const int a = dim >0? dim-k : 0;
    auto elements = std::get<a>(all_elems);

    const TensorIndex<a> active(elements[j].active_directions);
    //auto meas = lengths_.tensor_product(index);
    auto meas = lengths_.template sub_tensor_product<a>(index, active);

    if (flags.fill_measures())
    {
        cache.measure_ = meas;
        flags.set_measures_filled(true);
    }
    if (flags.fill_lengths())
    {
        cache.lengths_ = lengths_.cartesian_product(index);
        flags.set_lengths_filled(true);
    }

    cache.set_filled(true);
}



template <int dim_>
void
GridElementHandler<dim_>::
fill_element_cache(ElementAccessor &elem)
{
    fill_element_cache_<0>(elem, 0);
}



template <int dim_>
void
GridElementHandler<dim_>::
fill_element_cache(ElementIterator &elem)
{
    fill_element_cache(elem.get_accessor());
}



template <int dim_>
void
GridElementHandler<dim_>::
fill_face_cache(ElementIterator &elem, const int face)
{
    fill_element_cache_<1>(elem.get_accessor(), face);
}



template <int dim_>
void
GridElementHandler<dim_>::
print_info(LogStream &out) const
{
    out.begin_item("Lengths:");
    lengths_.print_info(out);
    out.end_item();
}

IGA_NAMESPACE_CLOSE

#include <igatools/geometry/grid_element_handler.inst>
