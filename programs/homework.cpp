#include <IMC/Spec/Abort.hpp>
#include <IMC/Spec/PlanSpecification.hpp>
#include <IMC/Spec/PlanManeuver.hpp>
#include <IMC/Spec/Loiter.hpp>
#include <IMC/Spec/SetEntityParameters.hpp>
#include <IMC/Spec/EntityParameter.hpp>
#include <IMC/Base/ByteBuffer.hpp>
#include <IMC/Base/Packet.hpp>
// #include <UDP/DatagramSocket.h>
#define LAUVXPLORE1 (30)
using namespace IMC;

int main() {

    int size = planControl.getSerializationSize();
    char buffer[size];
    Packet::serialize(&planSpecification, (uint8_t *) buffer, size);
    char localhost[] = "127.0.0.1";
    DatagramSocket socket(6002, localhost, true, true);
    socket.sendTo(buffer, size, localhost);

    SetEntityParameters setEntParam;
    setEntParam.name = "LBL";

    EntityParameter entParam;
    entParam.name = "Active";
    entParam.value = "false";

    setEntParam.params.push_back(entParam);

    Circular circular
    circular.lat = 0.7188016469344056;
    circular.lon = -0.15194250254286037;
    circular.z = 0;
    circular.z_units = 0;
    circular.duration = 300;
    circular.speed = 10;
    circular.speed_units = 1;
    circular.type = 1;
    circular.radius = 20;
    circular.length = 1;
    circular.bearing = 0;
    circular.direction = 1;
    circular.custom = "";


    PlanManeuver planMan;
    planMan.maneuver_id = "1";
    planMan.data.set(Circular);
    planMan.start_actions.push_back(setEntParam);


    PlanSpecification planSpec;
    planSpec.setTimeStamp(1.5628304294681385E9);
    planSpec.setSource(16652);
    planSpec.setSourceEntity(44);
    planSpec.setDestination(30);
    planSpec.setDestinationEntity(255);
    planSpec.plan_id = "caravela";
    planSpec.description = "";
    planSpec.vnamespace = "";
    planSpec.variables = {};
    planSpec.start_man_id = "1";
    planSpec.maneuvers.push_back(planMan);
    planSpec.transitions = {};
    planSpec.start_actions = {};
    planSpec.end_actions = {};

    
    planSpec.toJSON(std::cout);

    int size_ps = planSpec.getSerializationSize();
    char buf_ps[size_ps];
    

    Packet::serialize(&planSpec, (uint8_t *) buf_ps, size_ps);    // serialize message

    std::cout << buf_ps;
