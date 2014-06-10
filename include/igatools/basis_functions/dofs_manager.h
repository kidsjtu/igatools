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

#ifndef DOFS_MANAGER_H_
#define DOFS_MANAGER_H_

#include <igatools/base/config.h>
#include <igatools/base/logstream.h>
#include <igatools/utils/concatenated_forward_iterator.h>

//#include <boost/graph/adjacency_list.hpp>

#include <memory>

IGA_NAMESPACE_OPEN



/**
 * @brief The purpose of this class is to provide an unified way to access the dofs information provided
 * by different type of function spaces.
 *
 * For example, let consider this
 * @code{.cpp}
   auto space = BSplineSpace<3,3,1>:create();
   @endcode
 * The space represented by the object <tt>space</tt> has 3 scalar components and its dofs are
 * organized as 3 object of type DynamicMultiArray<Index,3>.
 *
 * @todo: complete the documentation
 * @author M. Martinelli
 * @date 10 Jun 2014
 */
class DofsManager
{
public:
    /** Type alias for the dofs container used in each scalar component of a single-patch space. */
    using DofsComponentContainer = std::vector<Index>;

    /** Type alias for the View on the dofs in each scalar component of a single-patch space. */
    using DofsComponentView = ContainerView<DofsComponentContainer>;

    /** Type alias for the ConstView on the dofs in each scalar component of a single-patch space. */
    using DofsComponentConstView = ConstContainerView<DofsComponentContainer>;

    /** Type alias for a concatenated iterator defined on several compoenent views. */
    using DofsIterator = ConcatenatedForwardIterator<DofsComponentView>;

    /** Type alias for a concatenated const-iterator defined on several compoenent views. */
    using DofsConstIterator = ConcatenatedForwardConstIterator<DofsComponentConstView>;

    /** Type alias for the View on the dofs held by each space in the DofsManager object. */
    using SpaceDofsView = View<DofsIterator,DofsConstIterator>;

    /** Type alias for the View on the dofs held by the DofsManager object. */
    using DofsView = View<DofsIterator,DofsConstIterator>;


    /** Default constructor. */
    DofsManager();

    /**
     * Prints internal information about the DofsManager.
     * @note Mostly used for debugging and testing.
     */
    void print_info(LogStream &out) const;


    /** @name Functions for changing the internal state of the DofsManager */
    ///@{
    /**
     * Sets the DofsManager in a state that can receive the views of the dofs by some spaces.
     */
    void dofs_arrangement_open();

    /**
     *
     */
    void dofs_arrangement_close();

    /**
     * Adds the view to the dofs of a space to the vector of views held by the DofsManager.
     * @param[in] space_id The identifier of the space.
     * @param[in] num_dofs_space Number of dofs that are represented by the view that is added by this function.
     * @param[in] dofs_space_view View of the dofs that must be added to the DofsManager.
     * @pre In order to call this function, the DofsManager must be be in the state that permits to receive
     * the dofs view. In other words, the user should call dofs_arrangement_open().
     */
    void add_dofs_space_view(const int space_id,
                             const Index num_dofs_space,
                             const SpaceDofsView &dofs_space_view);
    ///@}

    /** @name Functions for querying dofs information */
    ///@{
    const DofsView &get_dofs_view() const;

    /**
     * Returns the global dof corresponding to the @p local_dof
     * in the space with id equal to @p space_id.
     */
    Index get_global_dof(const int space_id, const Index local_dof) const;

    /**
     * Returns the global dofs corresponding to the @p local_dofs
     * in the space with id equal to @p space_id.
     */
    std::vector<Index> get_global_dofs(const int space_id, const std::vector<Index> &local_dof) const;
    ///@}

    /** Returns the number of linear constraints. */
    int get_num_linear_constraints() const;


    /** Returns the number of equality constraints. */
    int get_num_equality_constraints() const;




private:
    bool is_dofs_arrangement_open_ = false;

    std::vector<DofsComponentView> dofs_components_view_;

    struct SpaceInfo
    {
        SpaceInfo() = delete;
        SpaceInfo(const Index n_dofs, const SpaceDofsView &dofs_view);

        Index n_dofs_;
        Index offset_;
        SpaceDofsView dofs_view_;
    };

    std::map<int,SpaceInfo> spaces_info_;



    std::unique_ptr<DofsView> dofs_view_;



    class LinearConstraint
    {
    public:

    private:
        /** Vector of pairs dof_id/value defining the linear constraint.*/
        std::vector<std::pair<Index,Real> > dofs_id_and_value_;
    };

    std::vector<LinearConstraint> linear_constraints_;


    class EqualityConstraint
    {
    public:
    private:
        Index dof_id_master_;
        Index dof_id_slave_;
    };


    std::vector<EqualityConstraint> equality_constraints_;


};


IGA_NAMESPACE_CLOSE


#endif // #ifndef DOFS_MANAGER_H_
