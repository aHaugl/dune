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
// Author: Eduardo Marques (original maneuver implementation)               *
//***************************************************************************

#ifndef DUNE_MANEUVER_IDLE_HPP_INCLUDED_
#define DUNE_MANEUVER_IDLE_HPP_INCLUDED_

#include <DUNE/DUNE.hpp>

using DUNE_NAMESPACES;

namespace Maneuver
{
  namespace Multiplexer
  {
    // Export DLL Symbol.
    class DUNE_DLL_SYM Idle;

    //! Idle maneuver
    class Idle
    {
    public:
      //! Default constructor.
      //! @param[in] task pointer to Maneuver task
      Idle(Maneuvers::Maneuver* task):
        m_task(task)
      { }

      //! Start maneuver function
      //! @param[in] maneuver idle maneuver message
      void
      start(const IMC::IdleManeuver* maneuver)
      {
        m_task->setControl(0); // maneuver does not enable any control

        if (maneuver->duration)
        {
          m_end_time = Clock::get() + maneuver->duration;
          m_task->signalProgress(maneuver->duration);
        }
        else
        {
          m_task->signalProgress();
          m_end_time = -1;
        }
      }

      void
      onStateReport(void)
      {
        if (m_end_time < 0)
        {
          m_task->signalProgress();
          return;
        }

        double time_left = m_end_time - Clock::get();

        if (time_left < 0)
          m_task->signalCompletion();
        else
          m_task->signalProgress((uint16_t)time_left);
      }

      ~Idle(void)
      { }

    private:
      //! Pointer to task
      Maneuvers::Maneuver* m_task;
      //! End time of the loiter
      double m_end_time;
    };
  }
}

#endif
