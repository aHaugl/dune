//***************************************************************************
// Copyright 2007-2020 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Faculdade de Engenharia da             *
// Universidade do Porto. For licensing terms, conditions, and further      *
// information contact lsts@fe.up.pt.                                       *
//                                                                          *
// Modified European Union Public Licence - EUPL v.1.1 Usage                *
// Alternatively, this file may be used under the terms of the Modified     *
// EUPL, Version 1.1 only (the "Licence"), appearing in the file LICENCE.md *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://github.com/LSTS/dune/blob/master/LICENCE.md and                  *
// http://ec.europa.eu/idabc/eupl.html.                                     *
//***************************************************************************
// Author: Paulo Dias                                                       *
//***************************************************************************

// DUNE headers.
#include <DUNE/DUNE.hpp>

// Pioneer headers.
#include "Messages/Pioneer_App_Protocol.hpp"

namespace Control
{
  //! Insert short task description here.
  //!
  //! Insert explanation on task behaviour here.
  //! @author Paulo Dias
  namespace Pioneer
  {
    using DUNE_NAMESPACES;

    //! %Task arguments.
    struct Arguments
    {
      //! Communications timeout
      uint8_t comm_timeout;
      //! TCP Port
      uint16_t TCP_port;
      //! TCP Address
      Address TCP_addr;
      //! Use UDP Port
      uint16_t UDP_listen_port;
      //! UDP Port
      uint16_t UDP_port;
      //! UDP Address
      Address UDP_addr;
    };

    struct Task: public DUNE::Tasks::Task
    {
      //! Task arguments.
      Arguments m_args;
      //! TCP socket
      Network::TCPSocket* m_TCP_sock;
      //! UDP socket
      Network::UDPSocket* m_UDP_sock;

      uint8_t m_buf[2048];

      //! Moving Home timer
      Time::Counter<float> m_timer;

      //! Mavlink Timeouts
      bool m_error_missing;
      double m_last_pkt_time;

      //! Constructor.
      //! @param[in] name task name.
      //! @param[in] ctx context.
      Task(const std::string& name, Tasks::Context& ctx):
        DUNE::Tasks::Task(name, ctx),
        m_TCP_sock(NULL),
        m_UDP_sock(NULL),
        m_error_missing(false)
      {
          param("Communications Timeout", m_args.comm_timeout)
          .minimumValue("1")
          .maximumValue("60")
          .defaultValue("10")
          .units(Units::Second)
          .description("Pioneer communications timeout");

          param("TCP - Port", m_args.TCP_port)
          .defaultValue("2011")
          .description("Port for connection to Pioneer");

          param("TCP - Address", m_args.TCP_addr)
          .defaultValue("127.0.0.1")
          .description("Address for connection to Pioneer");

          param("UDP - Listen Port", m_args.UDP_listen_port)
          .defaultValue("2010")
          .description("Port for connection from Pioneer");

          param("UDP - Port", m_args.UDP_port)
          .defaultValue("2010")
          .description("Port for connection to Pioneer");

          param("UDP - Address", m_args.UDP_addr)
          .defaultValue("127.0.0.1")
          .description("Address for connection to Pioneer");

          m_last_pkt_time = 0; //! time of last packet from Pioneer
      }

      //! Initialize resources.
      void
      onResourceInitialization(void)
      {
      }

      //! Acquire resources.
      void
      onResourceAcquisition(void)
      {
          openConnection();
      }

      void
      openConnection(void)
      {
        try
        {
          m_TCP_sock = new TCPSocket;
          m_TCP_sock->connect(m_args.TCP_addr, m_args.TCP_port);
          m_TCP_sock->setNoDelay(true);
          
          m_UDP_sock = new UDPSocket;
          m_UDP_sock->bind(m_args.UDP_listen_port, Address::Any, false);
          
          inf(DTR("Pioneer interface initialized"));
        }
        catch (...)
        {
          Memory::clear(m_TCP_sock);
          Memory::clear(m_UDP_sock);
          war(DTR("Connection failed, retrying..."));
          setEntityState(IMC::EntityState::ESTA_NORMAL, Status::CODE_COM_ERROR);
        }
      }

      void
      closeConnection(void)
      {
        try
        {
          Memory::clear(m_TCP_sock);
          Memory::clear(m_UDP_sock);

          inf(DTR("Pioneer interface disconnected"));
        }
        catch (...)
        {
          war(DTR("Diconnection failed"));
          setEntityState(IMC::EntityState::ESTA_NORMAL, Status::CODE_COM_ERROR);
        }
      }

      //! Release resources.
      void
      onResourceRelease(void)
      {
        Memory::clear(m_TCP_sock);
        Memory::clear(m_UDP_sock);
      }

      //! Update internal state with new parameter values.
      void
      onUpdateParameters(void)
      {
      }

      //! Reserve entity identifiers.
      void
      onEntityReservation(void)
      {
      }

      //! Resolve entity names.
      void
      onEntityResolution(void)
      {
      }

      // char* my_s_bytes = reinterpret_cast<char*>(&my_s);
      // // or, if you prefer static_cast:
      // char* my_s_bytes = static_cast<char*>(static_cast<void*>(&my_s));

      int
      pioneerMessageParse(uint8_t buf[], int startIndex, int length)
      {
        int rb = 0;
        try
        {
          // war("New msg %u:%u  %X  %X  %X", buf[startIndex], buf[startIndex + 1],
          //     (buf[startIndex] << 8) + buf[startIndex + 1], buf[startIndex] << 8, buf[startIndex + 1]);
          switch ((buf[startIndex] << 8) + buf[startIndex + 1])
          {
          // case Message::PIONEER_MSG_VERSION_1_TELEMETRY_CODE:
          case 0x0101:
            rb = handlePioneerV1Telemetry(buf, startIndex, length);
            break;
          // case Message::PIONEER_MSG_VERSION_2_TELEMETRY_CODE:
          case 0x0201:
            rb = handlePioneerV2Telemetry(buf, startIndex, length);
            break;
          // case Message::PIONEER_MSG_VERSION_2_COMPASS_CALIBRATION_CODE:
          case 0x0202:
            rb = handlePioneerV2CompassCalibration(buf, startIndex, length);
            break;
          default:
            // war("skip msg");
            break;
          }
        }
        catch(const std::exception& e)
        {
          err("%s", e.what());
        }
        
        return rb;
      }

      int
      handlePioneerV1Telemetry(uint8_t buf[], int startIndex, int length)
      {
        int rb = 0;
        try
        {
          const int sizeMsg = sizeof(struct Messages::P2AppProtocolDataVersion1Telemetry);
          if (length < startIndex + sizeMsg) {
            war("Message PioneerV2Telemetry too short to decode %d < %d", length, startIndex + sizeMsg);
            return 0;
          }

          Messages::P2AppProtocolDataVersion1Telemetry msg;
          std::memcpy(&msg, &buf[startIndex], sizeMsg);
          rb = sizeMsg;
          spew("RECEIVED MSG: V1Telemetry");
          
          // TODO something with msg
          debug("Voltage %u", msg.battery_voltage);
        }
        catch(const std::exception& e)
        {
          err("%s", e.what());
          return 0;
        }
        
        return rb;
      }

      int
      handlePioneerV2Telemetry(uint8_t buf[], int startIndex, int length)
      {
        int rb = 0;
        try
        {
          const int sizeMsg = sizeof(struct Messages::P2AppProtocolDataVersion2Telemetry);
          if (length < startIndex + sizeMsg) {
            war("Message PioneerV2Telemetry too short to decode %d < %d", length, startIndex + sizeMsg);
            return 0;
          }

          Messages::P2AppProtocolDataVersion2Telemetry msg;
          std::memcpy(&msg, &buf[startIndex], sizeMsg);
          rb = sizeMsg;
          spew("RECEIVED MSG: V2Telemetry");

          // TODO something with msg
          debug("Depth %d",msg.depth);
        }
        catch(const std::exception& e)
        {
          err("%s", e.what());
          return 0;
        }
        
        return rb;
      }

      int
      handlePioneerV2CompassCalibration(uint8_t buf[], int startIndex, int length)
      {
        int rb = 0;
        try
        {
          const int sizeMsg = sizeof(struct Messages::P2AppProtocolDataVersion2Compasscalibration);
          if (length < startIndex + sizeMsg) {
            war("Message PioneerV2Telemetry too short to decode %d < %d", length, startIndex + sizeMsg);
            return 0;
          }

          Messages::P2AppProtocolDataVersion2Compasscalibration msg;
          std::memcpy(&msg, &buf[startIndex], sizeMsg);
          rb = sizeMsg;
          spew("RECEIVED MSG: V2CompassCalibration");

          // TODO something with msg
          debug("progress_thruster %u",msg.progress_thruster);
        }
        catch(const std::exception& e)
        {
          err("%s", e.what());
          return 0;
        }
        
        return rb;
      }

      void
      handlePioneerData(bool tcpOrUdp)
      {
        double now = Clock::get();
        int counter = 0;

        while ((tcpOrUdp ? pollTCP(0.01) : pollUDP(0.01)) && counter < 100)
        {
          counter++;

          int n = tcpOrUdp ? receiveDataTCP(m_buf, sizeof(m_buf))
              : receiveDataUDP(m_buf, sizeof(m_buf));
          if (n < 0)
          {
            debug("Receive error from %s", tcpOrUdp ? "TCP": "UDP");
            break;
          }

          // time stamp!
          now = Clock::get();

          // for each packet
          for (int i = 0; i < n; i++)
          {
            int rv = pioneerMessageParse(m_buf, i, n);

            // handle the parsed packet
            if (rv)
            {
               m_last_pkt_time = now;
            } // end: handle the parsed packet
            
            i += rv;
          } // end: for each packet
        } // end: poll for packets


        // check for timeout
        if (now - m_last_pkt_time >= m_args.comm_timeout)
        {
          if (!m_error_missing)
          {
            setEntityState(IMC::EntityState::ESTA_ERROR, Status::CODE_MISSING_DATA);
            m_error_missing = true;
          }
        }
        else
          m_error_missing = false;
      }
      
      bool
      pollTCP(double timeout)
      {
        if (m_TCP_sock != NULL)
          return Poll::poll(*m_TCP_sock, timeout);

        return false;
      }

      bool
      pollUDP(double timeout)
      {
        if (m_UDP_sock != NULL)
          return Poll::poll(*m_UDP_sock, timeout);

        return false;
      }

      int
      sendDataTCP(uint8_t* bfr, int size)
      {
        if (m_TCP_sock)
        {
          trace("Sending something");
          return m_TCP_sock->write((char*)bfr, size);
        }
        return 0;
      }

      int
      sendDataUDP(uint8_t* bfr, int size)
      {
        if (m_UDP_sock)
        {
          trace("Sending something");
          return m_UDP_sock->write(bfr, size, m_args.UDP_addr, m_args.UDP_port);
        }
        return 0;
      }

      int
      receiveDataTCP(uint8_t* buf, size_t blen)
      {
        if (m_TCP_sock)
        {
          try
          {
            if (m_TCP_sock)
              return m_TCP_sock->read(buf, blen);
          }
          catch (std::runtime_error& e)
          {
            err("%s", e.what());
            war(DTR("Connection lost, retrying..."));
            closeConnection();

            openConnection();

            return 0;
          }
        }
        return 0;
      }

      int
      receiveDataUDP(uint8_t* buf, size_t blen)
      {
        if (m_UDP_sock)
        {
          try
          {
            if (m_UDP_sock)
              return m_UDP_sock->read(buf, blen, &m_args.UDP_addr, &m_args.UDP_listen_port);
          }
          catch (std::runtime_error& e)
          {
            err("%s", e.what());
            war(DTR("Connection lost, retrying..."));
            closeConnection();

            openConnection();

            return 0;
          }
        }
        return 0;
      }

      //! Main loop.
      void
      onMain(void)
      {
        while (!stopping())
        {
          // Handle Autopilot data
          if (m_TCP_sock || m_UDP_sock)
          {
            if (m_TCP_sock)
              handlePioneerData(true);
            if (m_UDP_sock)
              handlePioneerData(false);
          }
          else
          {
            Time::Delay::wait(0.5);
            openConnection();
          }

          if (!m_error_missing)
            setEntityState(IMC::EntityState::ESTA_NORMAL, Status::CODE_ACTIVE);

          // Handle IMC messages from bus
          consumeMessages();        }
      }
    };
  }
}

DUNE_TASK
