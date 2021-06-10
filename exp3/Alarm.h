#include "list.h"

class Alarm
{
public:
    Alarm();
    ~Alarm();
    void Pause(int howLong);
    void Wakeup();
    void CheckList();

private:
    List *list; // waiting list
    int num; // num of threads in waiting list
};
