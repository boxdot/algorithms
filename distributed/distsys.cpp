#include "distsys.h"


//
// Node impl.
//

void Node::send(Id address, const std::string& message) {
    if (out_channels_.count(address)) {
        out_channels_.at(address)->send(message);
    }
}

std::pair<Node::Id, std::string> Node::receive() {
    if (mb_.empty()) {
        return {0, "NONE"};
    }

    auto result = mb_.front();
    mb_.pop();
    return result;
}

void Node::addOutChannel(Channel* ch) {
    out_channels_[ch->to()] = ch;
}

void Node::addIncChannel(Channel* ch) {
    inc_channels_[ch->from()] = ch;
}


//
// Channel impl.
//

Channel::Channel(Node* p, Node* q) : from_(p), to_(q) {
    from_->addOutChannel(this);
    to_->addIncChannel(this);
}

void Channel::send(const std::string& message) {
    to_->mb_.push({from_->id(), message});
}


//
// DistributedSystem impl.
//


Node::Id DistributedSystem::addNode(Node p) {
    nodes_.emplace_back(new Node{p});
    return nodes_.back()->id();
}

Channel::Id DistributedSystem::addChannel(Node::Id pid, Node::Id qid) {
    auto p = nodes_.at(pid).get();
    auto q = nodes_.at(qid).get();
    channels_.emplace_back(new Channel{p, q});
    return channels_.back()->id();
}

void DistributedSystem::addBiChannel(Node::Id pid, Node::Id qid) {
    addChannel(pid, qid);
    addChannel(qid, pid);
}

void DistributedSystem::run() {
    for (const auto& p : nodes_) {
        futures_.push_back(
            std::async(std::launch::async, p->behavior_, p.get()));
    }
}
