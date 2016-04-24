//
// Distributed system for simulation of distributed algorithms
//
// Consists of Nodes (Processes) and Channels connecting those. Messages are
// sent asynchronously from Node to Node over a Channel. Behavior of a Node is
// specified by a Lambda. Nodes and Channels are referenced by non-negative
// integers.
//

#pragma once

#include <vector>
#include <future>
#include <map>
#include <string>
#include <atomic>


class Node;
class Channel;
class DistributedSystem;


class Node {
public:

    using Id = uint32_t;

private:

    class Mailbox {
    public:

        void push(Id, const std::string& msg);
        std::pair<Id, std::string> pop();
        size_t size() const;
        bool empty() const;
        std::vector<std::pair<Node::Id, std::string>> all() const;

    private:

        std::vector<std::pair<Node::Id, std::string>> storage_;
        mutable std::mutex access_;
    };

public:

    Id id() const { return id_; }
    bool is_alive() const { return is_alive_; }
    bool is_stopped() const { return is_stopped_; }

    void send(Id id, const std::string& message);
    std::pair<Node::Id, std::string> receive();

    void stop() { is_stopped_ = true; }

    void wait_for(std::chrono::nanoseconds nsec) const;
    std::vector<Channel*> out_channels() const;
    std::vector<Channel*> inc_channels() const;

private:

    static Id uniqueId() {
        static Id global_id = 0;
        return global_id++;
    }

    template <typename Behavior>
    Node(Behavior behavior) : behavior_(behavior) {}

    void addOutChannel(Channel* ch);
    void addIncChannel(Channel* ch);

    friend class DistributedSystem;
    friend class Channel;

public:

    Id id_ = uniqueId();
    std::function<void(Node*)> behavior_;
    std::map<Id, Channel*> inc_channels_;
    std::map<Id, Channel*> out_channels_;
    Mailbox mb_;

    std::atomic<bool> is_alive_{false};
    std::atomic<bool> is_stopped_{false};
};


class Channel {
public:

    using Id = uint32_t;

    Id id() const { return id_; }
    Node::Id from() const { return from_->id(); }
    Node::Id to() const { return to_->id(); }
    std::vector<std::pair<Node::Id, std::string>> messages() const;

    void send(const std::string& message);


private:

    static Id uniqueId() {
        static Id global_id = 0;
        return global_id++;
    }

    Channel(Node* p, Node* q);

    friend class DistributedSystem;

private:

    Id id_ = uniqueId();
    Node* from_;
    Node* to_;
};


class DistributedSystem {
public:
    template <typename Behavior>
    Node::Id addNode(Behavior behavior) {
        nodes_.emplace_back(new Node{behavior});
        return nodes_.back()->id();
    }

    Channel::Id addChannel(Node::Id pid, Node::Id qid);
    void addBiChannel(Node::Id pid, Node::Id qid);

    void run();
    void stop();
    void await_all_done();

    // -> (#Nodes, #Channels)
    std::pair<size_t, size_t> size() const {
        return {nodes_.size(), channels_.size()};
    };

private:

    std::vector<std::unique_ptr<Node>> nodes_;
    std::vector<std::unique_ptr<Channel>> channels_;
    std::vector<std::future<void>> futures_;
};
