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
        return {std::numeric_limits<size_t>::max(), ""};
    }
    return mb_.pop();
}

void Node::addOutChannel(Channel* ch) {
    out_channels_[ch->to()] = ch;
}

void Node::addIncChannel(Channel* ch) {
    inc_channels_[ch->from()] = ch;
}

//
// Mailbox impl.
//

void Node::Mailbox::push(Node::Id id, const std::string& msg) {
    std::lock_guard<std::mutex> lock(access_);
    storage_.push_back({id, msg});
}

std::pair<Node::Id, std::string> Node::Mailbox::pop() {
    std::lock_guard<std::mutex> lock(access_);
    auto msg = storage_.front();
    storage_.erase(storage_.begin());
    return msg;
}

std::vector<std::pair<Node::Id, std::string>> Node::Mailbox::all() const {
    std::lock_guard<std::mutex> lock(access_);
    return storage_;
}

size_t Node::Mailbox::size() const {
    std::lock_guard<std::mutex> lock(access_);
    return storage_.size();
}

bool Node::Mailbox::empty() const {
    std::lock_guard<std::mutex> lock(access_);
    return storage_.empty();
}

//
// Channel impl.
//

Channel::Channel(Node* p, Node* q) : from_(p), to_(q) {
    from_->addOutChannel(this);
    to_->addIncChannel(this);
}

void Channel::send(const std::string& message) {
    to_->mb_.push(from_->id(), message);
}


//
// DistributedSystem impl.
//

Channel::Id DistributedSystem::addChannel(Node::Id pid, Node::Id qid) {
    auto offset = nodes_.front()->id();
    auto p = nodes_.at(pid - offset).get();
    auto q = nodes_.at(qid - offset).get();
    channels_.emplace_back(new Channel{p, q});
    return channels_.back()->id();
}

void DistributedSystem::addBiChannel(Node::Id pid, Node::Id qid) {
    addChannel(pid, qid);
    addChannel(qid, pid);
}

void DistributedSystem::run() {
    for (auto& p : nodes_) {
        futures_.push_back(std::async(std::launch::async,
            [](Node* p){
                p->behavior_(p);
                p->is_stopped_ = true;
                p->is_alive_ = false;
            }, p.get()));
    }
}

void DistributedSystem::stop() {
    for (auto& p : nodes_) {
        p->stop();
    }
}

void DistributedSystem::await_all_done() {
    for (auto& fut : futures_) {
        fut.get();
    }
}
