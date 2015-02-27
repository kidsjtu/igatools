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

#include <igatools/base/evaluation_points.h>
#include <igatools/base/exceptions.h>
#include <igatools/geometry/unit_element.h>
#include <igatools/utils/multi_array_utils.h>


#include <set>

using std::array;
using std::endl;
using std::set;

IGA_NAMESPACE_OPEN


template<int dim_>
EvaluationPoints<dim_>::
EvaluationPoints()
{
    map_point_id_to_coords_id_.resize(1);

    for (auto &box_direction : bounding_box_)
    {
        box_direction[0] = 0.0;
        box_direction[1] = 1.0;
    }
}

template<int dim_>
EvaluationPoints<dim_>::
EvaluationPoints(const BBox<dim_> &bounding_box)
    :
    bounding_box_(bounding_box)
{}



template<int dim_>
EvaluationPoints<dim_>::
EvaluationPoints(const PointArray &points,
                 const WeightArray &weights_1d)
                 :
                 coordinates_(points),
                 weights_1d_(weights_1d)
{
    this->points_have_tensor_product_struct_ = true;
    this->weights_have_tensor_product_struct_ = true;

    const auto n_pts = coordinates_.flat_size();
    for (int i = 0 ; i < n_pts ; ++i)
        map_point_id_to_coords_id_.push_back(coordinates_.flat_to_tensor(i));
}



template<int dim_>
EvaluationPoints<dim_>::
EvaluationPoints(const ValueVector<Point> &pts)
    :
    EvaluationPoints()
{
    const int n_pts = pts.size();
    TensorSize<dim_> size(n_pts);
    WeightArray weights_1d(size);

    Assert(false, ExcMessage("put weight to 1"));
    this->reset_points_coordinates_and_weights(pts,weights_1d);
}



template<int dim_>
EvaluationPoints<dim_>::
EvaluationPoints(
    const PointVector &pts,
    const WeightArray &weights_1d,
    const BBox<dim_> &bounding_box)
    :
    EvaluationPoints(bounding_box)
{
    this->reset_points_coordinates_and_weights(pts,weights_1d);
}



template<int dim_>
const BBox<dim_> &
EvaluationPoints<dim_>::
get_bounding_box() const
{
    return bounding_box_;
}



template<int dim_>
void
EvaluationPoints<dim_>::
reset_bounding_box(const BBox<dim_> &bounding_box)
{
    bounding_box_ = bounding_box;

#ifndef NDEBUG
    for (const auto &box_direction : bounding_box_)
        Assert(box_direction[0] <= box_direction[1],
               ExcMessage("Wrong coordinates for the bounding box."));
#endif
}



template<int dim_>
void
EvaluationPoints<dim_>::
dilate(const Point &dilate)
{
    coordinates_.dilate(dilate);
    weights_1d_.dilate(dilate);


    for (int i = 0 ; i < dim_ ; ++i)
    {
        Assert(dilate[i] > 0., ExcMessage("Dilation factor must be positive."));
        bounding_box_[i][0] *= dilate[i];
        bounding_box_[i][1] *= dilate[i];
    }
}

template<int dim_>
void
EvaluationPoints<dim_>::
translate(const Point &translate)
{
    coordinates_.translate(translate);

    // TODO (pauletti, Feb 27, 2015): code BBox translate
    for (int i = 0 ; i < dim_ ; ++i)
    {
        bounding_box_[i][0] += translate[i];
        bounding_box_[i][1] += translate[i];
    }
}


template<int dim_>
void
EvaluationPoints<dim_>::
dilate_translate(const Point &dilate, const Point &translate)
{
    this->dilate(dilate);
    this->translate(translate);
}



template<int dim_>
void
EvaluationPoints<dim_>::
reset_points_coordinates_and_weights(
    const PointVector &pts,
    const WeightArray &weights_1d)
{
    const int n_pts = pts.size();
    Assert(n_pts > 0 , ExcEmptyObject());

    //-----------------------------------------------------------------
    TensorSize<dim_> n_dirs;
    for (int i = 0 ; i < dim_ ; ++i)
    {
        set<Real> coords_set;
        for (const auto &pt : pts)
            coords_set.emplace(pt[i]);

        //inserting the point coordinates and removing the duplicates
        coordinates_.copy_data_direction(i,vector<Real>(coords_set.begin(),coords_set.end()));
        weights_1d_ = weights_1d;

#ifndef NDEBUG
        // check that the points coordinate are within the bounding box
        const auto box_min = bounding_box_[i][0];
        const auto box_max = bounding_box_[i][1];
        for (const auto &coord : coordinates_.get_data_direction(i))
            Assert(coord >= box_min && coord <= box_max,
                   ExcMessage("Point coordinate outside the bounding box."));
#endif


//        Assert(n_dirs[i] == weights_1d[i].size(),
//               ExcDimensionMismatch(n_dirs[i],weights_1d[i].size()));

    } // end loop i
    //-----------------------------------------------------------------



    //-----------------------------------------------------------------
    //for each point :
    //  - we retrieve the coordinate index;
    //  - we set the associated weight
    map_point_id_to_coords_id_.resize(n_pts);
    for (int j = 0 ; j < n_pts ; ++j)
    {
        auto &pt = pts[j];
        TensorIndex<dim_> coords_tensor_id;
        for (int i = 0 ; i < dim_ ; ++i)
        {
            const auto coords_begin = coordinates_.get_data_direction(i).begin();
            const auto coords_end   = coordinates_.get_data_direction(i).end();

            const auto it = std::find(coords_begin, coords_end, pt[i]);

            coords_tensor_id[i] = std::distance(coords_begin,it);
        }
        map_point_id_to_coords_id_[j]=(coords_tensor_id);
    }
    //-----------------------------------------------------------------



    //-----------------------------------------------------------------
    //here we check if the points have a tensor-product structure,
    //comparing the coordinate indices
    //with the ones expected from points having tensor_product structure.

    //first of all, if the number of points is different
    // to the tensor product of the coordinates size
    // the points have not a tensor product structure
    if (n_pts != n_dirs.flat_size())
    {
        this->points_have_tensor_product_struct_ = false;
    }
    else
    {
        const auto pt_w_size = MultiArrayUtils<dim_>::compute_weight(n_dirs);

        const auto coords_id_begin = map_point_id_to_coords_id_.begin();
        const auto coords_id_end   = map_point_id_to_coords_id_.end();

        this->points_have_tensor_product_struct_ = true;
        for (int pt_flat_id = 0 ; pt_flat_id < n_pts ; ++pt_flat_id)
        {
            const auto pt_tensor_id =
                MultiArrayUtils<dim_>::flat_to_tensor_index(pt_flat_id,pt_w_size);
            if (std::find(coords_id_begin, coords_id_end, pt_tensor_id)
                == coords_id_end)
            {
                // coordinates id not found --> the points have not a
                // tensor-product structure
                this->points_have_tensor_product_struct_ = false;
                break;
            }
        } // end loop pt_flat_id

    }
    //-----------------------------------------------------------------
}


template<int dim_>
bool
EvaluationPoints<dim_>::
have_points_tensor_product_struct() const
{
    return this->points_have_tensor_product_struct_;
}

template<int dim_>
bool
EvaluationPoints<dim_>::
have_weights_tensor_product_struct() const
{
    return this->weights_have_tensor_product_struct_;
}



template<int dim_>
const vector<Real> &
EvaluationPoints<dim_>::
get_coords_direction(const int i) const
{
    return coordinates_.get_data_direction(i);
}



template<int dim_>
TensorIndex<dim_>
EvaluationPoints<dim_>::
get_coords_id_from_point_id(const int point_id) const
{
    Assert(point_id >= 0 && point_id < this->get_num_points(),
           ExcIndexRange(point_id,0,this->get_num_points()));

    return map_point_id_to_coords_id_[point_id];
}



template<int dim_>
auto
EvaluationPoints<dim_>::
get_point(const int pt_id) const -> Point
{
    const auto tensor_id = this->get_coords_id_from_point_id(pt_id);
    return coordinates_.cartesian_product(tensor_id);
}


template<int dim_>
auto
EvaluationPoints<dim_>::
get_points() const -> ValueVector<Point>
{
    const int n_pts = this->get_num_points();
    ValueVector<Point> points(n_pts);
    for (int ipt = 0 ; ipt < n_pts ; ++ipt)
        points[ipt] = this->get_point(ipt);

    return points;
}

template<int dim_>
Real
EvaluationPoints<dim_>::
get_weight(const int pt_id) const
{
    const auto tensor_id = this->get_coords_id_from_point_id(pt_id);
    return weights_1d_.tensor_product(tensor_id);
}


template<int dim_>
ValueVector<Real>
EvaluationPoints<dim_>::
get_weights() const
{
    const int n_pts = this->get_num_points();
    ValueVector<Real> weights(n_pts);
    for (int ipt = 0 ; ipt < n_pts ; ++ipt)
        weights[ipt] = this->get_weight(ipt);


    return weights;
}

template<int dim_>
auto
EvaluationPoints<dim_>::
get_weights_1d() const -> const WeightArray&
{
    return weights_1d_;
}

template<int dim_>
int
EvaluationPoints<dim_>::
get_num_points() const
{
    return map_point_id_to_coords_id_.size();
}



template<int dim_>
TensorSize<dim_>
EvaluationPoints<dim_>::
get_num_coords_direction() const noexcept
{
    return coordinates_.tensor_size();
}



template<int dim_>
void
EvaluationPoints<dim_>::
print_info(LogStream &out) const
{
    out << "Number of points:" << this->get_num_points() << endl;

    out.begin_item("Weights:");
    get_weights().print_info(out);
    out.end_item();
    out << endl;

    out.begin_item("Coordinates:");
    for (int dir = 0 ; dir < dim_ ; ++dir)
    {
        out << "Direction: " << dir << endl;
        this->get_coords_direction(dir).print_info(out);
        out << endl;
    }
    out.end_item();

    out.begin_item("Points:");
    this->get_points().print_info(out);
    out.end_item();
    out << endl;

    out.begin_item("Bounding box:");
    for (int dir = 0 ; dir < dim_ ; ++dir)
    {
        out << "Direction: " << dir << " ---- [ "
            << bounding_box_[dir][0] << " , "
            << bounding_box_[dir][1] << " ]" << endl;
    }
    out.end_item();
    out << endl;
}



template<int dim_>
template<int k>
auto
EvaluationPoints<dim_>::
collapse_to_sub_element(const int sub_elem_id) const -> EvaluationPoints<dim_>
{
    auto &k_elem = UnitElement<dim_>::template get_elem<k>(sub_elem_id);

    if (this->have_weights_tensor_product_struct() &&
            this->have_points_tensor_product_struct())
    {
        PointArray new_coords_1d;
        WeightArray new_weights_1d;
        const int n_dir = k_elem.constant_directions.size();
        for (int j = 0 ; j < n_dir; ++j)
        {
            auto dir = k_elem.constant_directions[j];
            auto val = k_elem.constant_values[j];

            new_coords_1d.copy_data_direction(dir, vector<Real>(1, val));
            new_weights_1d.copy_data_direction(dir, vector<Real>(1, 1.));

        }

        for (auto i : k_elem.active_directions)
        {
            new_coords_1d.copy_data_direction(i, coordinates_.get_data_direction(i));
            new_weights_1d.copy_data_direction(i, weights_1d_.get_data_direction(i));
        }

        return self_t(new_coords_1d, new_weights_1d);
    }
    else
    {
        Assert(false,ExcNotImplemented());

    }
    return self_t();
}




template<int k, int dim>
EvaluationPoints<dim>
extend_sub_elem_quad(const EvaluationPoints<k> &eval_pts,
                     const int sub_elem_id)
{
    auto &k_elem = UnitElement<dim>::template get_elem<k>(sub_elem_id);

    //------------------------------------------------------
    // creating the bounding box and the weights_1d for the extension --- begin
    const BBox<k> old_bounding_box = eval_pts.get_bounding_box();
    BBox<dim> new_bounding_box;

    const auto &old_weights_1d = eval_pts.get_weights_1d();
    typename EvaluationPoints<dim>::WeightArray new_weights_1d;

    const int n_dir = k_elem.constant_directions.size();
    for (int j = 0 ; j < n_dir ; ++j)
    {
        const auto dir = k_elem.constant_directions[j];
        const auto val = k_elem.constant_values[j];

        new_bounding_box[dir][0] = val;
        new_bounding_box[dir][1] = val;

        new_weights_1d[dir].assign(1,1.0);
    }
    int ind = 0;
    for (auto i : k_elem.active_directions)
    {
        new_bounding_box[i] = old_bounding_box[ind];

        new_weights_1d[i] = old_weights_1d[ind];
        ++ind;
    }
    // creating the bounding box and the weights_1d for the extension --- end
    //------------------------------------------------------



    //------------------------------------------------------
    // defining the points for the extension --- begin
    const int n_pts = eval_pts.get_num_points();
    ValueVector<Points<dim>> new_points(n_pts);

    for (int pt_id = 0 ; pt_id < n_pts ; ++pt_id)
    {
        Points<dim> &new_pt = new_points[pt_id];

        for (int j = 0; j < n_dir; ++j)
        {
            const auto dir = k_elem.constant_directions[j];
            const auto val = k_elem.constant_values[j];
            new_pt[dir] = val;
        }

        const Points<k> old_pt = eval_pts.get_point(pt_id);
        int ind = 0;
        for (auto i : k_elem.active_directions)
            new_pt[i] = old_pt[ind++];
    }
    // defining the points for the extension --- end
    //------------------------------------------------------



    return EvaluationPoints<dim>(new_points,new_weights_1d,new_bounding_box);
}

#if 0
template<>
EvaluationPoints<1>
extend_sub_elem_quad(const EvaluationPoints<0> &eval_pts,
                     const int sub_elem_id)
{
    auto &k_elem = UnitElement<1>::template get_elem<0>(sub_elem_id);

    //------------------------------------------------------
    // creating the bounding box for the extension --- begin
    BBox<1> new_bounding_box;
    const auto val = k_elem.constant_values[0];
    new_bounding_box[0][0] = val;
    new_bounding_box[0][1] = val;
    //------------------------------------------------------


    //------------------------------------------------------
    // defining the point for the extension --- begin
    const int n_pts = 1;
    ValueVector<Points<1>> new_points(n_pts);

    special_array<vector<Real>,1> new_weights_1d;
    new_weights_1d[0].assign(1,1.0);

    new_points[0] = val;
    // defining the point for the extension --- end
    //------------------------------------------------------


    return EvaluationPoints<1>(new_points,new_weights_1d,new_bounding_box);
}
#endif

IGA_NAMESPACE_CLOSE

#include <igatools/base/evaluation_points.inst>
