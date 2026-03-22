#pragma once
#include "util.hpp"
#include <deque>
#include <memory>
#include <optional>

namespace leb {
    // Minimal in-memory B+ tree for uint64_t keys with inverted lists at leaves where:
    // + Internal nodes store separators and child pointers
    // + Leaves store (key, vector<SetID>) pairs and a pointer to next leaf
    struct BPTree {
        struct Node {
            bool leaf;
            std::vector<Key64> keys;
            std::vector<std::unique_ptr<Node>> children;    // internal node
            std::vector<std::vector<SetID>> payload;        // leaf node
            Node* next_leaf = nullptr;
            
            explicit Node(bool is_leaf) : leaf(is_leaf) {}
        };

        struct LeafIter {
            Node* leaf = nullptr;
            size_t pos = 0;
        };

        // Order by max keys per node
        const size_t ORDER;
        std::unique_ptr<Node> root;

        explicit BPTree(size_t order = 128) : ORDER(order) {
            root = std::make_unique<Node>(true);
        }

        // Key lookup
        Node* find_leaf(Key64 k) const {
            Node* curr = root.get();
            while (!curr->leaf) {
                size_t i = std::lower_bound(curr->keys.begin(), curr->keys.end(), k) - curr->keys.begin();
                curr = curr->children[i].get();
            }
        }

        // Key-ID Insertion
        void insert(Key64 k, SetID id) {
            auto split_info = insert_inner(root.get(), k, id);
            // Create new entry if key not exist
            if (split_info) {
                auto new_root = std::make_unique<Node>(false);
                new_root->keys.push_back(split_info->sep);
                new_root->children.push_back(std::move(split_info->left));
                new_root->children.push_back(std::move(split_info->right));
                root = std::move(new_root);
            }
        }

        // Get lowerbound
        LeafIter lower_bound(Key64 k) const {
            Node* lf = find_leaf(k);
            // Lowerbound is the first leaf position not less than key
            size_t i = std::lower_bound(lf->keys.begin(), lf->keys.end(), k) - lf->keys.begin();
            return { lf, i };
        }

        // Advance to next key in linked leaves
        bool next(LeafIter& it) const {
            if (!it.leaf) return false;
            if (it.pos + 1 < it.leaf->keys.size()) {
                ++it.pos;
                return true;
            }
            it.leaf = it.leaf->next_leaf;
            it.pos = 0;
            return it.leaf != nullptr;
        }

        // Get current key & payload
        Key64 curr_key(const LeafIter& it) const { 
            return it.leaf->keys[it.pos]; 
        }
        const std::vector<SetID>& curr_list(const LeafIter& it) const {
            return it.leaf->payload[it.pos];
        }

    private:
        struct Split {
            Key64 sep;
            std::unique_ptr<Node> left, right;
        };

        std::optional<Split> insert_inner(Node* node, Key64 k, SetID id) {
            if (node->leaf) {
                auto it = std::lower_bound(node->keys.begin(), node->keys.end(), k);
                size_t pos = it - node->keys.begin();

                if (it != node->keys.end() && *it == k) {
                    node->payload[pos].push_back(id);
                } else {
                    node->keys.insert(it, k);
                    node->payload.insert(node->payload.begin() + pos, std::vector<SetID>{id});
                }
                
                if (node->keys.size() > ORDER) return split_leaf(node);
                return std::nullopt;
            } else {
                size_t i = std::lower_bound(node->keys.begin(), node->keys.end(), k) - node->keys.begin();
                auto split = insert_inner(node->children[i].get(), k, id);
                if (!split) return std::nullopt;

                // Else, integrate split into internal node
                node->keys.insert(node->keys.begin() + i, split->sep);
                node->children.insert(node->children.begin() + i + 1, std::move(split->right));
                if (node->keys.size() <= ORDER) return std::nullopt;
                return split_internal(node);
            }
        }
        
        std::optional<Split> split_leaf(Node* node) {
            auto right = std::make_unique<Node>(true);
            size_t mid = node->keys.size() / 2;

            right->keys.assign(node->keys.begin() + mid, node->keys.end());
            right->payload.assign(node->payload.begin() + mid, node->payload.end());
            node->keys.resize(mid);
            node->payload.resize(mid);

            // Link leaves
            right->next_leaf = node->next_leaf;
            node->next_leaf = right.get();

            auto left = std::unique_ptr<Node>(node);
            Key64 sep = right->keys.front();
            return Split{ sep, std::move(left), std::move(right) };
        }
    
        std::optional<Split> split_internal(Node* node) {
            auto right = std::make_unique<Node>(false);
            size_t mid = node->keys.size() / 2;

            Key64 sep = node->keys[mid];

            right->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
            right->children.assign(
            std::make_move_iterator(node->children.begin() + mid + 1),
            std::make_move_iterator(node->children.end()));

            node->keys.resize(mid);
            node->children.resize(mid + 1);

            auto left = std::unique_ptr<Node>(node);
            return Split{ sep, std::move(left), std::move(right) };
        }
    };
}