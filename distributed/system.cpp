#include "astream.h"
#include "distsys.h"

#include <chrono>


int main(int argc, char const *argv[])
{
    using namespace std::chrono_literals;

    DistributedSystem sys;

    Node::Id p = sys.addNode(
        [](Node* self) {
            aout() << "Hello from " << self->id() << std::endl;
            self->send(1, "m1");
            self->send(2, "m2");
            while (!self->is_stopped()) {
                aout() << self->receive().second << std::endl;
                std::this_thread::sleep_for(1s);
            }
        });

    Node::Id q = sys.addNode(
        [](Node* self) {
            aout() << "Hello from " << self->id() << std::endl;
            while (!self->is_stopped()) {
                aout() << self->receive().second << std::endl;
                std::this_thread::sleep_for(1s);
            }
        });

    Node::Id r = sys.addNode(
        [](Node* self) {
            aout() << "Hello from " << self->id() << std::endl;
            self->send(0, "m3");
            while (!self->is_stopped()) {
                aout() << self->receive().second << std::endl;
                std::this_thread::sleep_for(1s);
            }
        });

    sys.addChannel(p, q);
    sys.addBiChannel(p, r);
    sys.addChannel(q, r);

    sys.run();

    std::this_thread::sleep_for(3s);

    sys.stop();
    sys.await_all_done();

    return 0;
}
