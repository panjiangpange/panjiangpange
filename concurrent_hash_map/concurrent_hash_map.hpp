#pragma once

#include <shared_mutex>
#include <unordered_map>
namespace chm
{
    template <typename K, typename V>
    class hash_node
    {
    public:
        hash_node() : _next(nullptr) {}
        hash_node(K key, V value) : _key(key), _value(value) {}
        ~hash_node() { this->_next = nullptr; }
        const K &key() const { return this->_key; }
        const V &value() const { return this->_value; }
        void set_value(const V &value) { this->_value = value; }

    private:
        K _key;
        V _value;

    public:
        hash_node *_next;
    };
    template <typename K, typename V>
    class hash_bucket
    {
    public:
        hash_bucket()
        {
            _head = new hash_node<K, V>();
        }
        ~hash_bucket()
        {
            std::unique_lock<std::shared_timed_mutex> lock(_mutex);
            hash_node<K, V> *prev = nullptr;
            hash_node<K, V> *curr = _head;
            while (curr)
            {
                prev = curr;
                curr = curr->_next;
                delete prev;
            }
        }
        void insert(const K &key, const V &value)
        {
            // if key already exists ,update
            // if key not exits,insert a new <K,V>
            std::unique_lock<std::shared_timed_mutex> lock(_mutex);
            hash_node<K, V> *prev = _head;
            hash_node<K, V> *curr = _head->_next;
            while (curr && curr->key() != key)
            {
                prev = curr;
                curr = curr->_next;
            }
            if (curr == nullptr)
            {
                prev->_next = new hash_node<K, V>(key, value);
            }
            else
            {
                curr->set_value(value);
            }
        }
        bool find(const K &key, V &value) const
        {
            std::shared_lock<std::shared_timed_mutex> lock(_mutex);
            hash_node<K, V> *curr = _head->_next;
            bool result = false;
            while (curr)
            {
                if (curr->key() == key)
                {
                    value = curr->value();
                    result = true;
                    break;
                }
                curr = curr->_next;
            }
            return result;
        }
        void remove(const K &key)
        /// removes a <key, value> pair from the bucket if key is found.
        {
            std::unique_lock<std::shared_timed_mutex> lock(_mutex);
            hash_node<K, V> *prev = _head;
            hash_node<K, V> *curr = _head->_next;
            while (curr && curr->key() != key)
            {
                prev = curr;
                curr = curr->_next;
            }
            if (curr)
            {
                prev->_next = curr->_next;
                delete curr;
            }
            // if key not found, do nothing
        }

        void clear()
        {
            std::unique_lock<std::shared_timed_mutex> lock(_mutex);
            hash_node<K, V> *prev = _head;
            hash_node<K, V> *curr = _head->_next;
            while (curr)
            {
                prev->_next = curr->_next;
                delete curr;
                curr = prev->_next;
            }
        }

    private:
        hash_node<K, V> *_head;
        mutable std::shared_timed_mutex _mutex;
    };

    template <typename K, typename V, typename F =std::hash<K>>
    class hash_map
    {
    public:
        hash_map(size_t hash_size = hash_size_default) : hash_size_(hash_size)
        {
            hash_table_ = new hash_bucket<K, V> *[hash_size_]();
            for (int i = 0; i < hash_size_; ++i)
                hash_table_[i] = new hash_bucket<K, V>();
        }

        ~hash_map()
        {
            for (int i = 0; i < hash_size_; ++i)
                delete hash_table_[i];
            delete[] hash_table_;
        }

        // copy, move and assignments are not supported
        hash_map(const hash_map &) = delete;
        hash_map(hash_map &&) = delete;
        hash_map &operator=(const hash_map &) = delete;
        hash_map &operator=(hash_map &&) = delete;

        void insert(const K &key, const V &value)
        {
            size_t hash_value = hash_func_(key) % hash_size_;
            hash_table_[hash_value]->insert(key, value);
        }

        bool find(const K &key, V &value) const
        {
            size_t hash_value = hash_func_(key) % hash_size_;
            return hash_table_[hash_value]->find(key, value);
        }

        void remove(const K &key)
        {
            size_t hash_value = hash_func_(key) % hash_size_;
            hash_table_[hash_value]->remove(key);
        }

        void clear()
        {
            for (int i = 0; i < hash_size_; ++i)
                (hash_table_[i])->clear();
        }

    private:
        /*
        hash_table_
        0 v1 -> v2 -> ... -> null
        1 v1 -> v2 -> ... -> null
        2 v1 -> v2 -> ... -> null
        ...
        node is a cell containing key-value pair.
        bucket is one linked list at a particular index of the hash table.
        hash table is an array of hash buckets.
        */
        hash_bucket<K, V> **hash_table_;
        F hash_func_;
        static constexpr size_t hash_size_default = 1031;
        size_t hash_size_;

    }; // class hash_map
}
