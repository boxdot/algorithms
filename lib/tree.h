#import <assert.h>

#include <memory>

// Persitent Binary Tree
// TODO: add full address
// cf. bartoszmilewski.com
template<typename T>
class Tree {
    struct Node {
        Node(T val,
                const std::shared_ptr<const Node>& lft,
                const std::shared_ptr<const Node>& rht)
            : val_(val), lft_(lft), rht_(rht)
            {}

        T val_;
        std::shared_ptr<const Node> lft_;
        std::shared_ptr<const Node> rht_;
    };

    explicit Tree(const std::shared_ptr<const Node>& node)
        : root_(node)
        {}

public:
    // empty tree
    Tree() {}
    // leaf tree
    Tree(T val)
        : root_(std::make_shared<const Node>(val, nullptr, nullptr))
        {}
    Tree(T val, const Tree& lft, const Tree& rht)
        : root_(std::make_shared<const Node>(val, lft.root_, rht.root_))
        {}

    bool empty() const { return !root_; }
    bool is_leaf() const {
        return !empty() && left().empty() &&  right().empty();
    }

    T root() const {
        assert(!empty());
        return root_->val_;
    }

    Tree left() const {
        assert(!empty());
        return Tree(root_->lft_);
    }

    Tree right() const {
        assert(!empty());
        return Tree(root_->rht_);
    }

private:
    std::shared_ptr<const Node> root_;
};
