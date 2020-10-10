#ifndef LINKED_HASHMAP_HPP
#define LINKED_HASHMAP_HPP
#include <list>
#include <tuple>
#include <unordered_map>

namespace sharper {
template <typename Key,
    typename T,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
class linked_hashmap {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::list<value_type>::pointer;
    using const_pointer = typename std::list<value_type>::const_pointer;
    using iterator = typename std::list<value_type>::iterator;
    using const_iterator = typename std::list<value_type>::const_iterator;

    linked_hashmap() = default;

    explicit linked_hashmap(size_type bucket_count)
        : map_(bucket_count)
    {
    }

    explicit linked_hashmap(size_type bucket_count,
        const hasher& hash = hasher(),
        const key_equal& equal = key_equal())
        : map_(bucket_count, hash, equal)
    {
    }

    template <typename InputIt>
    linked_hashmap(InputIt first, InputIt last)
    {
        insert(first, last);
    }

    template <typename InputIt>
    linked_hashmap(InputIt first,
        InputIt last,
        size_type bucket_count,
        const hasher& hash = hasher(),
        const key_equal& equal = key_equal())
        : map_(bucket_count, hash, equal)
    {
        insert(first, last);
    }

    linked_hashmap(const linked_hashmap& other) = default;

    linked_hashmap(linked_hashmap&& other) = default;

    linked_hashmap(std::initializer_list<value_type> init)
    {
        insert(init);
    }

    linked_hashmap(std::initializer_list<value_type> init,
        size_type bucked_count,
        const hasher& hash = hasher(),
        const key_equal& equal = key_equal())
        : map_(bucked_count, hash, equal)
    {
        insert(init);
    }

    ~linked_hashmap() = default;

    linked_hashmap& operator=(const linked_hashmap& other) = default;
    linked_hashmap& operator=(linked_hashmap&& other) noexcept = default;

    linked_hashmap& operator=(std::initializer_list<value_type> ilist)
    {
        clear();
        insert(ilist);
        return *this;
    }


    iterator begin() noexcept { return list_.begin(); }

    const_iterator begin() const noexcept { return list_.begin(); }

    const_iterator cbegin() const noexcept { return begin(); }

    iterator end() noexcept { return list_.end(); }

    const_iterator end() const noexcept { return list_.end(); }

    const_iterator cend() const noexcept { return end(); }

    bool empty() const noexcept { return list_.empty(); }

    size_type size() const noexcept { return list_.size(); }

    void clear() noexcept
    {
        list_.clear();
        map_.clear();
    }

    std::pair<iterator, bool> insert(const value_type& value)
    {
        return insert(std::move(value));
    }

    std::pair<iterator, bool> insert(value_type&& value)
    {
        const auto it_find = map_.find(value.first);
        if (it_find == map_.end()) {
            list_.emplace_back(value);
            map_.emplace(value.first, --list_.end());
        } else {
            list_.erase(it_find->second);
            list_.emplace_back(value);
            it_find->second = --list_.end();
        }

        return { --list_.end(), true };
    }

    template <typename P,
        std::enable_if_t<std::is_constructible_v<value_type, P>, int>  = 0>
    std::pair<iterator, bool> insert(P&& value)
    {
        return insert(std::forward(value));
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        while (first != last) {
            insert({ first->first, first->second });
            ++first;
        }
    }

    void insert(std::initializer_list<value_type> ilist)
    {
        insert(ilist.begin(), ilist.end());
    }

    iterator erase(const_iterator pos)
    {
        map_.erase(pos->first);
        return list_.erase();
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        while (first != last) {
            map_.erase(first->first);
            ++first;
        }
        return list_.erase(first, last);
    }

    size_type erase(const key_type& key)
    {
        const auto it_find = map_.find(key);
        if (it_find == map_.end())
            return 0;
        list_.erase(it_find->second);
        return map_.erase(key);
    }

    void swap(linked_hashmap& other) noexcept
    {
        map_.swap(other.map_);
        list_.swap(other.list_);
    }

    mapped_type& at(const key_type& key) { return map_.at(key)->second; }

    const mapped_type& at(const key_type& key) const
    {
        return map_.at(key)->second;
    }

    mapped_type& operator[](const key_type& key)
    {
        return operator[](std::move(key));
    }

    mapped_type& operator[](key_type&& key)
    {
        const auto it_find = map_.find(key);
        if (it_find == map_.end()) {
            list_.emplace_back(std::piecewise_construct,
                std::forward_as_tuple(std::move(key)), std::tuple<>());

            map_.emplace(key, --list_.end());
            return (--list_.end())->second;
        }

        return it_find->second->second;
    }

    size_type count(const key_type& key) const { return map_.count(key); }

    iterator find(const key_type& key)
    {
        const auto it_find = map_.find(key);
        return it_find == map_.end() ? end() : it_find->second;
    }

    const_iterator find(const key_type& key) const { return find(key); }

    std::pair<iterator, iterator> equal_range(const key_type& key)
    {
        return { find(key), end() };
    }

    std::pair<const_iterator, const_iterator> equal_range(
        const key_type& key) const
    {
        return equal_range(key);
    }

private:
    std::unordered_map<key_type, iterator, hasher, key_equal> map_;
    std::list<value_type> list_;
};
} // namespace sharper

#endif
