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



#ifndef NURBS_ELEMENT_ACCESSOR_H_
#define NURBS_ELEMENT_ACCESSOR_H_

#include <igatools/base/config.h>
#include <igatools/basis_functions/bspline_element_accessor.h>

IGA_NAMESPACE_OPEN


template < int, int , int > class NURBSSpace ;



/**
 * See module on @ref accessors_iterators for a general overview.
 * @ingroup accessors_iterators
 */
template <int dim, int range, int rank >
class NURBSElementAccessor :
//    public BSplineElementAccessor< dim, range, rank >
public SpaceElementAccessor<
    NURBSElementAccessor<dim,range,rank>,NURBSSpace<dim,range,rank>,dim,0,range,rank>
{
public:


    using parent_t = SpaceElementAccessor<
                     NURBSElementAccessor<dim,range,rank>,NURBSSpace<dim,range,rank>,dim,0,range,rank>;

    using ContainerType = const NURBSSpace< dim, range, rank>;
    using Space_t = NURBSSpace< dim, range, rank >;

    typedef NURBSElementAccessor<dim,range,rank> Self_t ;

    using Parent_t = BSplineElementAccessor<dim,range,rank>;

//    using BSplineElementAccessor< dim, range, rank >::n_faces;



    /** Number of faces of the element. */
    using parent_t::n_faces;



    /** @name Constructors */
    ///@{
    /**
     * Default constructor. Not allowed to be used.
     */
    NURBSElementAccessor() = delete ;

    /**
     * \brief Constructor.
     * \todo Missing documentation.
     */
    NURBSElementAccessor(const std::shared_ptr<ContainerType> space,
                         const int elem_index);

    /**
     * Copy constructor.
     */
    NURBSElementAccessor(const NURBSElementAccessor< dim, range, rank > &element) = default;

    /**
     * Move constructor.
     */
    NURBSElementAccessor(NURBSElementAccessor< dim, range, rank > &&element) = default;

    /** Destructor.*/
    ~NURBSElementAccessor() = default;
    ///@}

    /** @name Assignment operators */
    ///@{
    /**
     * Copy assignment operator.
     */
    NURBSElementAccessor< dim, range, rank > &
    operator=(const NURBSElementAccessor< dim, range, rank > &element) = default;



    /**
     * Move assignment operator.
     */
    NURBSElementAccessor< dim, range, rank > &
    operator=(NURBSElementAccessor< dim, range, rank > &&element) = default;
    ///@}





    /**@name Getting values at points */
    ///@{

    /**
     * Prepares the internal cache for the efficient
     * computation of the values requested in
     * the fill_flag on the given quadrature points.
     * This implies a uniform quadrature scheme
     * (i.e. the same for all elements).
     * @note This function should be called before fill_values()
     */
    void init_values(const ValueFlags fill_flag,
                     const Quadrature<dim> &quad);

    /**
     * For a given face quadrature.
     */
    void init_face_values(const Index face_id,
                          const ValueFlags fill_flag,
                          const Quadrature<dim-1> &quad);

    /**
     * Precomputes the values needed to get the quantities specified by the ValueFlags used as input argument of the reset() function.
     * The computed quantities are evaluated at the quadrature point specified by the Quadrature used as input argument of the reset() function.
     * \note This function must always be invoked if you want to get values related to basis functions.
     */
    void fill_values();

    void fill_face_values(const Index face_id);


    /**
     * Get the NURBS weights associated to the element.
     */
    std::vector<Real> get_weights() const ;



    /**
     * Typedef for specifying the derivatives of the basis function in the reference domain.
     * \tparam deriv_order - order of the derivative.
     */
    template <int deriv_order>
    using DerivativeRef_t = Derivatives<dim, range, rank, deriv_order> ;

    /**
     * TODO: document me .
     */
    using Value = Values<dim, range, rank>;



protected:
    bool operator==(const NURBSElementAccessor<dim,range,rank> &a) const;

    bool operator!=(const NURBSElementAccessor<dim,range,rank> &a) const;

    void operator++();

private:

    /**
     * Computes the 0-th order derivative of the non-zero NURBS basis functions over the element
     * at the evaluation points, from the BSpline values contained in <tt>bspline_cache</tt>.
     * \warning If the output result @p D0_phi_hat is not correctly pre-allocated,
     * an exception will be raised.
     */
    void
    evaluate_nurbs_values(
        const typename Parent_t::ValuesCache &bspline_cache,
        ValueTable<Value> &D0_phi_hat) const ;

    /**
     * Computes the 1-st order derivative of the non-zero NURBS basis functions over the element
     * at the evaluation points, from the BSpline values contained in <tt>bspline_cache</tt>.
     * \warning If the output result @p D1_phi_hat is not correctly pre-allocated,
     * an exception will be raised.
     */
    void
    evaluate_nurbs_gradients(
        const typename Parent_t::ValuesCache &bspline_cache,
        ValueTable< Derivatives< dim, range, rank, 1 > > &D1_phi_hat) const ;

    /**
     * Computes the 2-st order derivative of the non-zero NURBS basis functions over the element,
     * at the evaluation points, from the BSpline values contained in <tt>bspline_cache</tt>.
     * \warning If the output result @p D1_phi_hat is not correctly pre-allocated,
     * an exception will be raised.
     */
    void
    evaluate_nurbs_hessians(
        const typename Parent_t::ValuesCache &bspline_cache,
        ValueTable< Derivatives< dim, range, rank, 2 > > &D2_phi_hat) const ;





    /**
     * Element accessor used to compute the BSpline basis functions (and derivatives)
     * needed to evaluate ne NURBS basis functions (and derivatives).
     */
    BSplineElementAccessor<dim,range,rank> bspline_element_accessor_;


    template <typename Accessor> friend class GridForwardIterator ;
} ;





IGA_NAMESPACE_CLOSE


#endif /* NURBS_ELEMENT_ACCESSOR_H_ */


