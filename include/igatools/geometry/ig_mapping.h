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

#ifndef __IG_MAPPING_H_
#define __IG_MAPPING_H_

#include <igatools/base/config.h>
#include <igatools/geometry/mapping.h>
#include <igatools/linear_algebra/distributed_vector.h>
#include <igatools/utils/dynamic_multi_array.h>
#include <igatools/utils/static_multi_array.h>

IGA_NAMESPACE_OPEN

template <class RefSpace>
class IgMapping
    : public Mapping<RefSpace::dim, RefSpace::dim_range - RefSpace::dim>
{
private:
    using base_t = Mapping<RefSpace::dim, RefSpace::dim_range - RefSpace::dim>;

    using base_t::dim;
    using base_t::codim;
    using base_t::space_dim;

    using typename base_t::PointType;
    using typename base_t::ValueType;
    using typename base_t::GradientType;
    using typename base_t::HessianType;
    using typename base_t::GridType;

    using self_t = IgMapping<RefSpace>;

public:
    /**
     * Default constructor.
     */
    IgMapping() = delete;

    /**
     * \brief Constructor. It builds a mapping from a function space and a vector of
     * control points.
     * \param[in] space The function space (i.e. a set of basis function) used to represents the mapping.
     * \param[in] control_points The coefficients of the linear combination of the basis function
     * of the function space used to represents the mapping.
     */
    IgMapping(const std::shared_ptr<RefSpace> space,
              const Vector &control_points);

    /**
     *
     *
     * It builds a Mapping object wrapped in a std::shared_ptr,
     * from a function space and a vector of control points.
     */
    static std::shared_ptr<base_t>
    create(const std::shared_ptr<RefSpace> space, const Vector &control_points);

    /**
     * Copy constructor.
     */
    IgMapping(const self_t &map) = default;

    virtual std::shared_ptr<base_t> clone() const override;

    /**
     * Copy assignment operator.
     */
    self_t &operator=(const self_t &map) = delete;

    void init_element(const ValueFlags flag,
                      const Quadrature<dim> &quad) override;

    void set_element(const CartesianGridElementAccessor<dim> &elem) override;

    /** @name Mapping as a standard function */
    ///@{
    virtual void evaluate(std::vector<ValueType> &values) const override;

    virtual void evaluate_gradients
    (std::vector<GradientType> &gradients) const override;

    virtual void evaluate_hessians
    (std::vector<HessianType> &hessians) const override;

    ///@}

    /** @name Function used to modify the position of the control points */
    ///@{
    /**
     * Sets the control points defining the map.
     * @param[in] control_points - Coordinates of the control points in the Euclidean space.
     */
    void set_control_points(const Vector &control_points);
    ///@}

    std::shared_ptr<RefSpace> get_iga_space()
    {
        return ref_space_;
    }

    /**
     * Prints internal information about the mapping.
     * @note Mostly used for debugging and testing.
     * Try to call the same function on a derived class.
     */
    void print_info(LogStream &out) const override;

protected:
    /** The function space used to represents the mapping.*/
    std::shared_ptr<RefSpace> ref_space_;

    /** Coordinates of the control points in the Euclidean space. */
    Vector control_points_;

    typename RefSpace::ElementIterator element_;

private:
    /**
     * h-refines the control mesh of the mapping space after a grid uniform refinement.
     *
     * @param[in] refinement_directions Directions along which the refinement is performed.
     * @param[in] grid_old Grid before the refinement.
     *
     * @pre Before invoking this function, must be invoked the function grid_->refine().
     * @note This function is connected to the CartesianGrid's signal for the refinement, and
     * it is necessary in order to avoid infinite loops in the refine() function calls.
     * @note The implementation of this function is based on "The NURBS Book" Algorithm A5.4.
     */
    void refine_h_control_mesh(
        const std::array<bool,dim> &refinement_directions,
        const typename base_t::GridType &grid_old);


    template< class T >
    using ComponentTable = StaticMultiArray<T,base_t::space_dim,1>;

    /**
     * Weights associated with the control points (if NURBSpace is used).
     *
     * @note The weights are necessary in order to perform the h-refinement,
     * because the control points are in the euclidean space, while the
     * h-refinement algorithm (based on knot insertion) require them to be in
     * the projective space.
     */
    ComponentTable<DynamicMultiArray<Real,dim>> weights_pre_refinement_;

    /** Knots with repetitions PRE-refinement */
    ComponentTable<CartesianProductArray<Real,dim>> knots_with_repetitions_pre_refinement_;


    /** Control mesh (the coordinates are in the projective space). */
    ComponentTable<DynamicMultiArray<Real,dim>> ctrl_mesh_;

};

IGA_NAMESPACE_CLOSE

#endif