/*

Here are the method signatures for the Elevator and Building classes.
You should feel free to add to these classes, but do not change the
existing interfaces.

*/

#define STAY 0
#define UP 1
#define DOWN 2
#define TICK 100000  //Pause value


class Elevator {

public:
    friend class Building;
    Elevator(char *debugName, int numFloors, int myID);
    ~Elevator();
    char *getName() { return name; }

    // elevator control interface: called by Elevator thread
    void OpenDoors();                //   signal exiters and enterers to action
    void CloseDoors();               //   after exiters are out and enterers are in
    void VisitFloor(int floor);      //   go to a particular floor

    // elevator rider interface (part 1): called by rider threads.
    bool Enter();                    //   get in
    void Exit();                     //   get out (iff destinationFloor)
    void RequestFloor(int floor);    //   tell the elevator our destinationFloor

    // insert your methods here, if needed

    int getRequest();//Get a floor that makes the request
    int goUp();//The elevator goes up one floor.
    int goDown();//The elevator goes down one floor.
    EventBarrier **upRequest;//Record the request waiting for the uplink elevator
    EventBarrier **downRequest;//Record the request waiting for the downlink elevator
    EventBarrier **outRequest;//Record the request waiting for the leave
    bool *isUp;//Used in conjunction with upRequest[MAX]
    bool *isDown;//Used in conjunction with downRequest[MAX]
    bool *isOut;//Used in conjunction with outRequest[MAX]
    int currentfloor;// floor where currently stopped
    int elevatorState;// Record the current status of the elevator
    int topFloor;//Record the number of floors
    Lock *ElevatorLock;
    Condition *HaveRequest;
    Condition *ElevatorNotFull;
    int occupancy;//Record number of people that in elevator
    int capacity;

private:
    char *name;
    int elevatorID;
};

class Building {
public:
    Building(char *debugname, int numFloors, int numElevators);
    ~Building();
    char *getName() { return name; }


    // elevator rider interface (part 2): called by rider threads
    void CallUp(int fromFloor);      //   signal an elevator we want to go up
    void CallDown(int fromFloor);    //   ... down
    Elevator *AwaitUp(int fromFloor); // wait for elevator arrival & going up
    Elevator *AwaitDown(int fromFloor); // ... down
    int topfloors;
    Elevator *elevator;         // the elevators in the building (array)

private:
    char *name;
};

   // here's a sample portion of a rider thread body showing how we
   // expect things to be called.
   //
   // void rider(int id, int srcFloor, int dstFloor) {
   //    Elevator *e;
   //
   //    if (srcFloor == dstFloor)
   //       return;
   //
   //    DEBUG('t',"Rider %d travelling from %d to %d\n",id,srcFloor,dstFloor);
   //    do {
   //       if (srcFloor < dstFloor) {
   //          DEBUG('t', "Rider %d CallUp(%d)\n", id, srcFloor);
   //          building->CallUp(srcFloor);
   //          DEBUG('t', "Rider %d AwaitUp(%d)\n", id, srcFloor);
   //          e = building->AwaitUp(srcFloor);
   //       } else {
   //          DEBUG('t', "Rider %d CallDown(%d)\n", id, srcFloor);
   //          building->CallDown(srcFloor);
   //          DEBUG('t', "Rider %d AwaitDown(%d)\n", id, srcFloor);
   //          e = building->AwaitDown(srcFloor);
   //       }
   //       DEBUG('t', "Rider %d Enter()\n", id);
   //    } while (!e->Enter()); // elevator might be full!
   //
   //    DEBUG('t', "Rider %d RequestFloor(%d)\n", id, dstFloor);
   //    e->RequestFloor(dstFloor); // doesn't return until arrival
   //    DEBUG('t', "Rider %d Exit()\n", id);
   //    e->Exit();
   //    DEBUG('t', "Rider %d finished\n", id);
   // }

