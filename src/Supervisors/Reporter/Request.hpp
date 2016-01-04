//***************************************************************************
// Copyright 2007-2016 Universidade do Porto - Faculdade de Engenharia      *
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
// http://ec.europa.eu/idabc/eupl.html.                                     *
//***************************************************************************
// Author: José Braga                                                       *
//***************************************************************************

#ifndef SUPERVISORS_REPORTER_TICKET_HPP_INCLUDED_
#define SUPERVISORS_REPORTER_TICKET_HPP_INCLUDED_

// ISO C++ 98 headers.
#include <string>

// DUNE headers.
#include <DUNE/DUNE.hpp>

namespace Supervisors
{
  namespace Reporter
  {
    //! Communication interfaces. The variables must match
    //! IMC::ReportControl communication interface field.
    enum CommInterface
    {
      //! Acoustic.
      IS_ACOUSTIC = 0x01,
      //! Satellite.
      IS_SATELLITE = 0x02,
      //! GSM.
      IS_GSM = 0x04,
      //! Mobile.
      IS_MOBILE = 0x08
    };

    //! Ticket structure.
    struct Ticket
    {
      //! Sequece Id.
      uint16_t id;
      //! IMC source address.
      uint8_t interface;
      //! Desired periodicity.
      uint16_t period;
      //! Destination system.
      std::string destination;
    };
  }
}

#endif
