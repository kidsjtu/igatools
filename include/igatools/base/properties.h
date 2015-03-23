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

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <igatools/base/config.h>
#include <string>

IGA_NAMESPACE_OPEN

struct Properties
{
    static const std::string none;
};


//TODO (pauletti, Mar 21, 2015): these may not be to be global but hierarchical only
struct ElementProperties : public Properties
{
    using Properties::none;

    static const std::string active;
};


struct DofProperties
{
	static const std::string active;
};

IGA_NAMESPACE_CLOSE

#endif // #ifndef PROPERTIES_H_


