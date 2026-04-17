#pragma once
#include "util.hpp"
#include <algorithm>

namespace leb {
    // In-memory B+ Tree for Key64 -> list<SetID>.
    // ORDER is the maximum number of keys per node before splitting.
    struct BPTree {
        struct Node {
            bool is_leaf = true;
            std::vector<Key64> keys;
            std::vector<Node*> children;                    // internal nodes only
            std::vector<std::vector<SetID>> values;         // leaf nodes only
            Node* next = nullptr;                           // leaf-level linked list

            explicit Node(bool leaf) : is_leaf(leaf) {}
        };

        struct LeafIter {
            const Node* node = nullptr;
            size_t idx = 0;
            bool leaf = false;
            size_t pos = 0;
        };

        const size_t ORDER;
        Node* root = nullptr;

        explicit BPTree(size_t order = 256) : ORDER(order) {}
        ~BPTree() { destroy(root); }

        BPTree(const BPTree&) = delete;
        BPTree& operator=(const BPTree&) = delete;
        BPTree(BPTree&&) = delete;
        BPTree& operator=(BPTree&&) = delete;

        void insert(Key64 k, SetID id) {
            if (!root) {
                root = new Node(true);
                root->keys.push_back(k);
                root->values.push_back(std::vector<SetID>{id});
                return;
            }

            std::vector<Node*> path_nodes;
            std::vector<size_t> path_child_idx;
            Node* cur = root;

            while (!cur->is_leaf) {
                size_t ci = child_index_for_key(cur, k);
                path_nodes.push_back(cur);
                path_child_idx.push_back(ci);
                cur = cur->children[ci];
            }

            auto it = std::lower_bound(cur->keys.begin(), cur->keys.end(), k);
            size_t pos = static_cast<size_t>(it - cur->keys.begin());
            if (it != cur->keys.end() && *it == k) {
                cur->values[pos].push_back(id);
                return;
            }

            cur->keys.insert(it, k);
            cur->values.insert(cur->values.begin() + static_cast<std::ptrdiff_t>(pos), std::vector<SetID>{id});

            if (cur->keys.size() <= max_keys()) return;

            Node* right = split_leaf(cur);
            Key64 promote = right->keys.front();

            while (true) {
                if (path_nodes.empty()) {
                    Node* new_root = new Node(false);
                    new_root->keys.push_back(promote);
                    new_root->children.push_back(cur);
                    new_root->children.push_back(right);
                    root = new_root;
                    return;
                }

                Node* parent = path_nodes.back();
                size_t child_idx = path_child_idx.back();
                path_nodes.pop_back();
                path_child_idx.pop_back();

                parent->keys.insert(parent->keys.begin() + static_cast<std::ptrdiff_t>(child_idx), promote);
                parent->children.insert(parent->children.begin() + static_cast<std::ptrdiff_t>(child_idx + 1), right);

                if (parent->keys.size() <= max_keys()) return;

                right = split_internal(parent, promote);
                cur = parent;
            }
        }

        LeafIter lower_bound(Key64 k) const {
            if (!root) return LeafIter{};

            const Node* leaf = find_leaf(k);
            if (!leaf) return LeafIter{};

            auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), k);
            size_t idx = static_cast<size_t>(it - leaf->keys.begin());

            while (leaf && idx >= leaf->keys.size()) {
                leaf = leaf->next;
                idx = 0;
            }

            if (!leaf) return LeafIter{};
            return LeafIter{leaf, idx, true, 0};
        }

        bool next(LeafIter& it) const {
            if (!it.leaf) return false;

            ++it.idx;
            while (it.node && it.idx >= it.node->keys.size()) {
                it.node = it.node->next;
                it.idx = 0;
            }

            it.leaf = (it.node != nullptr);
            it.pos = 0;
            return it.leaf;
        }

        Key64 curr_key(const LeafIter& it) const { return it.node->keys[it.idx]; }
        const std::vector<SetID>& curr_list(const LeafIter& it) const {
            return it.node->values[it.idx];
        }

    private:
        size_t max_keys() const {
            return ORDER < 2 ? 2 : ORDER;
        }

        static size_t child_index_for_key(const Node* node, Key64 k) {
            return static_cast<size_t>(std::upper_bound(node->keys.begin(), node->keys.end(), k) - node->keys.begin());
        }

        const Node* find_leaf(Key64 k) const {
            const Node* cur = root;
            while (cur && !cur->is_leaf) {
                size_t ci = child_index_for_key(cur, k);
                cur = cur->children[ci];
            }
            return cur;
        }

        static Node* split_leaf(Node* leaf) {
            size_t mid = leaf->keys.size() / 2;
            Node* right = new Node(true);

            right->keys.assign(leaf->keys.begin() + static_cast<std::ptrdiff_t>(mid), leaf->keys.end());
            right->values.assign(leaf->values.begin() + static_cast<std::ptrdiff_t>(mid), leaf->values.end());

            leaf->keys.resize(mid);
            leaf->values.resize(mid);

            right->next = leaf->next;
            leaf->next = right;
            return right;
        }

        static Node* split_internal(Node* node, Key64& promote_out) {
            size_t mid = node->keys.size() / 2;
            promote_out = node->keys[mid];

            Node* right = new Node(false);
            right->keys.assign(node->keys.begin() + static_cast<std::ptrdiff_t>(mid + 1), node->keys.end());
            right->children.assign(node->children.begin() + static_cast<std::ptrdiff_t>(mid + 1), node->children.end());

            node->keys.resize(mid);
            node->children.resize(mid + 1);
            return right;
        }

        static void destroy(Node* n) {
            if (!n) return;
            if (!n->is_leaf) {
                for (Node* c : n->children) destroy(c);
            }
            delete n;
        }
    };
}