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

// QualityAssurance: martinelli, 29 Jan 2014

#ifndef CARTESIAN_PRODUCT_INDEXER_H_
#define CARTESIAN_PRODUCT_INDEXER_H_

#include <igatools/base/config.h>
#include <igatools/utils/tensor_size.h>
#include <igatools/utils/tensor_index.h>



IGA_NAMESPACE_OPEN

/**
 * @brief This class represent a set of <tt>rank</tt>-dimensional
 * indices arranged in a cartesian-product way.
 *
 * Basically it is an look-up table from flat-indices to
 * tensor-indices and it can be used in order to
 * avoid the on-the-fly index transformation
 * TensorSizedContainer<rank>::flat_to_tensor().
 *
 * @author M. Martinelli
 * @date 29 Jan 2014
 */
template <int rank>
class CartesianProductIndexer
{
public:
    /** @name Constructors and destructor */
    ///@{
    /** Default constructor. Not allowed to be used. */
    CartesianProductIndexer() = delete;

    /**
     * Constructor. Builds an indexer with <p>n_indices_direction[i]M</p>
     * indices along the i-th direction.
     */
    CartesianProductIndexer(const TensorSize<rank> &n_indices_direction);

    /** Copy constructor. Not allowed to be used. */
    CartesianProductIndexer(const CartesianProductIndexer<rank> &in) = delete;

    /** Move constructor. Not allowed to be used. */
    CartesianProductIndexer(CartesianProductIndexer<rank> &&in) = delete;

    /** Destructor. */
    ~CartesianProductIndexer() = default;
    ///@}

    /** @name Assignment operators */
    ///@{
    /** Copy assignment operator. Not allowed to be used. */
    CartesianProductIndexer<rank> &operator=(const CartesianProductIndexer<rank> &in) = delete;

    /** Move assignment operator. Not allowed to be used. */
    CartesianProductIndexer<rank> &operator=(CartesianProductIndexer<rank> &&in) = delete;
    ///@}


    /** Returns the tensor index corresponding to the fucntion argument @p flat_index.*/
    TensorIndex<rank> get_tensor_index(const Index flat_index) const ;

    /** Returns the total number of indices. */
    Size get_num_indices() const ;

    void print_info_(LogStream &out) const ;

private:

    /**
     * Map between flat-index to tensor-index.
     * tensor_indices_[i] is the tensor index corresponding to the i-th flat-index.
     */
    std::vector<TensorIndex<rank>> tensor_indices_ ;
};



IGA_NAMESPACE_CLOSE


#endif // #ifndef CARTESIAN_PRODUCT_INDEXER_H_

