//***************************************************************************
// Copyright 2007-2014 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Universidade do Porto. For licensing   *
// terms, conditions, and further information contact lsts@fe.up.pt.        *
//                                                                          *
// European Union Public Licence - EUPL v.1.1 Usage                         *
// Alternatively, this file may be used under the terms of the EUPL,        *
// Version 1.1 only (the "Licence"), appearing in the file LICENCE.md       *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://www.lsts.pt/dune/licence.                                        *
//***************************************************************************
// Author: Pedro Calado                                                     *
//***************************************************************************

#ifndef DUNE_MANEUVER_MEMENTO_TABLE_HPP_INCLUDED_
#define DUNE_MANEUVER_MEMENTO_TABLE_HPP_INCLUDED_

// ISO C++ 98 headers.
#include <string>
#include <map>
#include <sstream>

// DUNE headers.
#include <DUNE/Tasks/ParameterTable.hpp>
#include <DUNE/Tasks/BasicParameterParser.hpp>

namespace DUNE
{
  namespace Maneuvers
  {
    // Export DLL Symbol.
    class DUNE_DLL_SYM MementoTable;

    //! Class to gather memento information for a maneuver
    class MementoTable: public Tasks::ParameterTable
    {
    public:
      //! Constructor
      MementoTable(void)
      { }

      //! Destructor
      ~MementoTable(void)
      { }

      //! Add a new pair name=value to the table
      //! @param[in] name the string identifying the memento
      //! @param[in] var the variable associated to the name
      //! @return the corresponding added parameter
      template <typename T>
      Tasks::Parameter&
      add(const std::string& name, T& var)
      {
        Tasks::BasicParameterParser<T>* parser;
        parser = new Tasks::BasicParameterParser<T>(var);
        return ParameterTable::add(name, &var, parser);
      }

      //! Set default values
      void
      setDefaults(void);

      //! Fill the table values with a Tuple List
      //! Parameters must have been previously added
      //! @param[in] str tuple list in the comma separated format "name=value"
      void
      fill(const std::string& str);

      //! Uncast all parameter values to it's strings
      void
      uncastAll(void);

      //! Write the tuples to a string
      //! @param[out] str string to which the parameters will be written
      void
      writeTuples(std::string& str);
    };
  }
}

#endif
