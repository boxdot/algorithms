#pragma once

#include <queue>
#include <future>
#include <map>
#include <string>


class Node;
class Channel;
class DistributedSystem;

using Mailbox = std::queue<std::pair<uint32_t, std::string>>;


class Node {
public:

    using Id = uint32_t;

    template <typename Behavior>
    Node(Behavior behavior) : behavior_(behavior) {}

    Id id() const { return id_; }
    bool is_alive() const { return is_alive_; }
    bool is_stopped() const { return is_stopped_; }

    void send(Id id, const std::string& message);
    std::pair<Node::Id, std::string> receive();

private:

    static Id uniqueId() {
        static Id global_id = 0;
        return global_id++;
    }

    void addOutChannel(Channel* ch);
    void addIncChannel(Channel* ch);

    friend class DistributedSystem;
    friend class Channel;

private:

    Id id_ = uniqueId();
    std::function<void(Node*)> behavior_;
    std::map<Id, Channel*> inc_channels_;
    std::map<Id, Channel*> out_channels_;
    Mailbox mb_;

    bool is_alive_ = false;
    bool is_stopped_ = false;
};


class Channel {

public:

    using Id = uint32_t;

    Channel(Node* p, Node* q);

    Id id() const { return id_; }
    Node::Id from() const { return from_->id(); }
    Node::Id to() const { return to_->id(); }

    void send(const std::string& message);

private:

    static Id uniqueId() {
        static Id global_id = 0;
        return global_id++;
    }

private:

    Id id_ = uniqueId();
    Node* from_;
    Node* to_;
};


struct DistributedSystem {

    Node::Id addNode(Node p);
    Channel::Id addChannel(Node::Id pid, Node::Id qid);
    void addBiChannel(Node::Id pid, Node::Id qid);

    void run();

    // -> (#Nodes, #Channels)
    std::pair<size_t, size_t> size() const {
        return {nodes_.size(), channels_.size()};
    };

private:

    std::vector<std::unique_ptr<Node>> nodes_;
    std::vector<std::unique_ptr<Channel>> channels_;
    std::vector<std::future<void>> futures_;
};
