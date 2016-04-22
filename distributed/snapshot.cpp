//
// Chandy-Lamport algorithm
//
// I simulate the algorithm with three processes:
// p, q, r; and channels p -> q -> r <-> q
//
// p (grey) initializes the snapshot, sends a message to q and becomes black. q
// (grey) sends a message to r with a small delay, s.t. the snapshot message of
// p arrives before it at r. Then q becomes dark grey. After receiving first
// snapshot message is becomes (black). r does nothing and is always white.
//
// Expected states:
// p - grey, no messages
// q - dark grey, no message
// r - white, { m2 }
//

#include "distsys.h"
#include "astream.h"

#include <map>
#include <assert.h>


using State = std::string;
using Msg = std::pair<Node::Id, std::string>;
using Msgs = std::vector<Msg>;


void print_snapshot(const Node* p, const State& state, const Msgs& msgs) {
    aout out;
    out << p->id() << ": Snapshot finished.\n"
        << "    State(" << state << ")\n";
    if (msgs.empty()) {
        out << "    Msgs()\n";
    }

    for (auto msg : msgs) {
        out << "    Msg(from=" << msg.first << ", msg=" << msg.second << ")\n";
    }
}


void simulation() {
    using namespace std::chrono_literals;

    DistributedSystem sys;

    State p_snapshot_state, q_snapshot_state, r_snapshot_state;
    Msgs p_snapshot_msgs, q_snapshot_msgs, r_snapshot_msgs;

    static constexpr auto TICK = 5ms;

    auto p = sys.addNode([&](Node* self) {
        State state = "grey";
        std::map<Node::Id, bool> ctrl_snapshot_received;
        size_t num_inc_channels = self->inc_channels().size();

        while (!self->is_stopped()) {
            if (state == "grey") {

                aout() << self->id() << ": Snapshot started!" << std::endl;
                p_snapshot_state = state;
                for (auto& ch : self->out_channels()) {
                    self->send(ch->to(), "ctrl_snapshot");
                }
                aout() << self->id() << ": Sending m1 to 1" << std::endl;
                self->send(self->id() + 1, "m1");
                state = "black";

            } else if (state == "black") {

                Node::Id from_id;
                std::string msg;
                std::tie(from_id, msg) = self->receive();

                if (msg == "ctrl_snapshot") {
                    aout() << self->id() << ": Got ctrl_snapshot from "
                        << from_id << std::endl;
                    assert(!ctrl_snapshot_received.count(from_id));
                    ctrl_snapshot_received[from_id] = true;
                } else if (!msg.empty() && !ctrl_snapshot_received[from_id]) {
                    p_snapshot_msgs.emplace_back(from_id, msg);
                }

                // terminate criteria
                if (ctrl_snapshot_received.size() == num_inc_channels) {
                    return;  // terminate
                }
            } else {
                assert(false);  // impossible state
            }

            aout() << self->id() << ": Waiting." << std::endl;
            self->wait_for(TICK);
        }
    });

    auto q = sys.addNode([&](Node* self) {
        State state = "grey";
        std::map<Node::Id, bool> ctrl_snapshot_received;
        size_t num_inc_channels = self->inc_channels().size();

        while (!self->is_stopped()) {

            if (state == "grey") {

                // delay
                self->wait_for(TICK);

                aout() << self->id() << ": Sending message m2 to 2\n";
                self->send(self->id() + 1, "m2");
                state = "dark grey";

            } else if (state == "dark grey" || state == "black") {

                Node::Id from_id;
                std::string msg;
                std::tie(from_id, msg) = self->receive();

                if (msg == "ctrl_snapshot") {
                    aout() << self->id() << ": Got ctrl_snapshot from "
                        << from_id << std::endl;
                    assert(!ctrl_snapshot_received.count(from_id));
                    ctrl_snapshot_received[from_id] = true;

                    if (state == "dark grey") {
                        q_snapshot_state = state;
                        state = "black";
                        for (auto& ch : self->out_channels()) {
                            self->send(ch->to(), "ctrl_snapshot");
                        }
                    }
                } else if (!msg.empty() && !ctrl_snapshot_received[from_id]) {
                    if (state == "black") {
                        q_snapshot_msgs.emplace_back(from_id, msg);
                    }
                }

                // termination criteria
                if (ctrl_snapshot_received.size() == num_inc_channels) {
                    return; // terminate
                }

            } else {
                assert(false);  // impossible state
            }

            aout() << self->id() << ": Waiting." << std::endl;
            self->wait_for(TICK);
        }
    });

    auto r = sys.addNode([&](Node* self) {
        State state = "white";

        std::map<Node::Id, bool> ctrl_snapshot_received;
        bool snapshot_taken = false;
        State snapshot_state;
        Msgs snapshot_msgs;
        size_t num_inc_channels = self->inc_channels().size();

        while (!self->is_stopped()) {

            Node::Id from_id;
            std::string msg;
            std::tie(from_id, msg) = self->receive();

            if (msg == "ctrl_snapshot") {
                aout() << self->id() << ": Got ctrl_snapshot from "
                    << from_id << std::endl;

                assert(!ctrl_snapshot_received.count(from_id));
                ctrl_snapshot_received[from_id] = true;

                if (!snapshot_taken) {
                    r_snapshot_state = state;
                    snapshot_taken = true;
                    for (auto& ch : self->out_channels()) {
                        self->send(ch->to(), "ctrl_snapshot");
                    }
                }
            } else if (!msg.empty() && !ctrl_snapshot_received[from_id]) {
                aout() << self->id() << ": Got " <<  msg<< " from "
                    << from_id << std::endl;
                if (snapshot_taken) {
                    r_snapshot_msgs.emplace_back(from_id, msg);
                }
            }

            // termination criteria
            if (ctrl_snapshot_received.size() == num_inc_channels) {
                return;  // terminate
            }

            aout() << self->id() << ": Waiting." << std::endl;
            self->wait_for(TICK);
        }
    });

    sys.addChannel(p, q);
    sys.addChannel(q, r);
    sys.addBiChannel(p, r);

    sys.run();
    sys.await_all_done();

    assert(p_snapshot_state == "grey");
    assert(p_snapshot_msgs.size() == 0);
    assert(q_snapshot_state == "dark grey");
    assert(q_snapshot_msgs.size() == 0);
    assert(r_snapshot_state == "white");
    assert(r_snapshot_msgs.size() == 1);
    assert(r_snapshot_msgs[0].first == q);
    assert(r_snapshot_msgs[0].second == "m2");
}


int main(int argc, char const *argv[])
{
    for (int i = 0; i != 100; ++i) {
        simulation();
    }
    return 0;
}
