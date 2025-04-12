#pragma once

#include <iostream>
#include <memory>
#include <limits>
#include <iterator>

namespace buff {

    template<class T, class Allocator= std::allocator<T>>
    class CCircularBufferBase;

    template<class T, class Allocator = std::allocator<T>>
    class Iterator;

    template<class T, class Allocator=std::allocator<T>>
    class ConstIterator;

    template<class T, class Allocator>
    class Iterator {
        friend CCircularBufferBase<T, Allocator>;
        friend ConstIterator<T, Allocator>;
    public:
        typedef ptrdiff_t difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef size_t size_type;
        typedef std::random_access_iterator_tag iterator_category;

        difference_type operator-(const Iterator &lhs) const {
            if (array != lhs.array) {
                return array + index - lhs.array - lhs.index;
            }
            if (index == lhs.index) {
                if (is_begin == lhs.is_begin) {
                    return 0;
                }
                if (!is_begin and lhs.is_begin) {
                    return get_size();
                }
                if (is_begin and !lhs.is_begin) {
                    return -get_size();
                }
            }
            if (index == end_index) {
                size_t pred_ind = end_index;
                if (pred_ind == 0) {
                    pred_ind = capacity_;
                }
                --pred_ind;
                return Iterator(capacity_, array, begin_index, end_index, pred_ind, is_empty, true) - lhs + 1;
            }
            if (index >= begin_index and lhs.index >= begin_index) {
                if (index == lhs.index and begin_index == index and begin_index == end_index) {
                    if (!is_begin and lhs.is_begin) {
                        return get_size();
                    }
                    if (is_begin and !lhs.is_begin) {
                        return -get_size();
                    }
                    return 0;
                }
                return index - lhs.index;
            }
            if (index >= begin_index) {
                return -(lhs.index + capacity_ - index);
            }

            return (index + capacity_ - lhs.index);
        }

        bool operator==(const Iterator &lhs) {
            if ((array + index) == (lhs.array + lhs.index)) {
                if (begin_index == end_index and index == end_index) {
                    return is_begin == lhs.is_begin;
                }

                return true;
            }

            return false;
        }

        bool operator!=(const Iterator &lhs) {
            return !(*this == lhs);
        }

        bool operator<(const Iterator &lhs) {
            return ((lhs - *this) > 0);
        }

        bool operator>(const Iterator &lhs) {
            return ((*this - lhs) > 0);
        }

        bool operator>=(const Iterator &lhs) {
            return !(*this < lhs);
        }

        bool operator<=(const Iterator &lhs) {
            return !(*this > lhs);
        }

        reference operator->() {
            return *(array + index);
        }

        virtual reference operator*() {
            return *(array + index);
        }

        Iterator &operator++() {
            if (is_empty or capacity_ == 0) {
                return *this;
            }
            index = (index + 1) % capacity_;
            if (index == end_index) {
                is_begin = !is_begin;
            }
            return *this;
        }

        Iterator operator++(int) {
            if (is_empty or capacity_ == 0) {
                return *this;
            }
            Iterator old_value(capacity_, array, begin_index, end_index, index, is_empty);
            ++(*this);
            return old_value;
        }

        Iterator &operator--() {
            if (is_empty or capacity_ == 0) {
                return *this;
            }
            if (index == begin_index and is_begin) {
                is_begin = !is_begin;
                return *this;
            }

            if (begin_index == end_index) {
                if (index == 0) {
                    index = capacity_ - 1;
                } else {
                    index--;
                }

                if (index == begin_index) {
                    is_begin = true;
                    return *this;
                }

            } else if (begin_index > end_index) {
                if (index == 0) {
                    index = capacity_ - 1;
                } else if (index == begin_index) {
                    index = end_index;
                } else {
                    index--;
                }
            } else {
                index--;
                if (index < begin_index) {
                    index = end_index;
                }
            }
            if (index == end_index) {
                is_begin = !is_begin;
            }
            return *this;
        }

        Iterator operator--(int) {
            if (is_empty) {
                return *this;
            }
            Iterator old_value(capacity_, array, begin_index, end_index, index, is_empty);
            --(*this);
            return old_value;
        }

        Iterator &operator+=(difference_type diff) {
            if (is_empty or capacity_ == 0) {
                return *this;
            }
            if (diff == 0) {
                return *this;
            }
            diff = diff % get_size();
            if (diff == 0) {
                if (index == end_index and begin_index == end_index) {
                    is_begin = false;
                }
                return *this;
            }

            index = (index + diff) % capacity_;
            if (index > end_index and index < begin_index) {
                index = (begin_index - end_index + index - 1) % capacity_;
            }

            if (index == end_index and end_index == begin_index) {
                is_begin = !is_begin;
            }
            return *this;
        }

        Iterator operator+(difference_type lhs) const {
            return Iterator<T>(capacity_, array, begin_index, end_index, index, is_empty, is_begin) += lhs;
        }

        Iterator &operator-=(difference_type lhs) {
            return *this += -lhs;
        }

        virtual reference operator[](difference_type idx) const {
            Iterator old_value(capacity_, array, begin_index, end_index, index, is_empty, is_begin);
            old_value += idx;
            return *old_value;
        }

        Iterator &operator=(const Iterator &other) {
            array = other.array;
            capacity_ = other.capacity_;
            end_index = other.end_index;
            begin_index = other.begin_index;
            is_empty = other.is_empty;
            index = other.index;
            return *this;
        }

    protected:
        Iterator() = default;

        size_t capacity_;
        size_t end_index;
        size_t begin_index;
        size_t index;
        bool is_empty;
        pointer array;
        bool is_begin = true;

        difference_type get_size() const {
            if (end_index > begin_index) {
                return end_index - begin_index;
            } else if (end_index < begin_index) {
                return capacity_ - begin_index + end_index;
            } else if (is_empty) {
                return 0;
            } else {
                return capacity_;
            }
        }

        Iterator(size_t capacity, T *ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg) : capacity_(
                capacity),
                                                                                                        array(ptr),
                                                                                                        index(ind),
                                                                                                        begin_index(
                                                                                                                beg),
                                                                                                        end_index(end),
                                                                                                        is_empty(
                                                                                                                empty) {
            if (!is_empty and begin_index == end_index) {
                is_begin = isBeg;
            }
        };
    };

    template<class T, class Allocator>
    class ConstIterator : public Iterator<T, Allocator> {
        friend CCircularBufferBase<T, Allocator>;
    public:
        typedef ptrdiff_t difference_type;
        typedef T &reference;

        reference operator*() const {
            return *(this->array + this->index);
        }

        reference operator[](difference_type idx) const {
            Iterator<T, Allocator> old_value(this->capacity_, this->array, this->begin_index, this->end_index, this->begin_index,
                               this->is_empty,
                               this->is_begin);
            return old_value[idx];
        }

    protected:
        ConstIterator(size_t capacity, T *ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg)
                : Iterator<T, Allocator>(
                capacity, ptr, beg, end, ind, empty, isBeg) {}
    };

    template<class T, class Allocator>
    class CCircularBufferBase {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef ConstIterator<T> const_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<Iterator<T>> reverse_iterator;
        typedef size_t size_type;

        size_t size() const {
            if (end_index > beg_index) {
                return end_index - beg_index;
            } else if (end_index < beg_index) {
                return capacity_ - beg_index + end_index;
            } else if (empty_) {
                return 0;
            } else {
                return capacity_;
            }
        }

        void reserve(size_type new_cap) {
            if (new_cap > allocator_.max_size()) {
                return;
            }
            if (new_cap <= capacity_) {
                return;
            }
            size_t sizes = size();
            size_t old_cap = capacity_;
            capacity_ = new_cap;
            T *mass2 = std::allocator_traits<Allocator>::allocate(allocator_, capacity_);

            for (uint64_t i = 0; i < sizes; i++) {
                std::allocator_traits<Allocator>::construct(allocator_, mass2 + i, (*this)[i]);
            }

            pointer temp = mass;
            uint64_t old_beg = beg_index;
            mass = mass2;
            beg_index = 0;
            end_index = sizes;
            if (end_index == new_cap) {
                end_index = 0;
            }

            if (old_cap != 0) {
                for (uint64_t i = 0; i < sizes; i++) {
                    std::allocator_traits<Allocator>::destroy(allocator_, temp + old_beg);
                    old_beg = (old_beg + 1) % old_cap;
                }
                std::allocator_traits<Allocator>::deallocate(allocator_, temp, old_cap);
            }
        }

        size_t capacity() const {
            return capacity_;
        }

        bool empty() const {
            return empty_;
        }

        void clear() {
            uint64_t j = beg_index;
            for (uint64_t i = 0; i < size(); i++) {
                std::allocator_traits<Allocator>::destroy(allocator_, mass + j);
                j = (j + 1) % capacity_;
            }
            beg_index = 0;
            end_index = 0;
            empty_ = true;
        }

        T get() {
            T ret;
            if (empty_) {
                return ret;
            }
            ret = this->front();
            std::allocator_traits<Allocator>::destroy(allocator_, mass + beg_index);
            beg_index = (beg_index + 1) % capacity_;
            if (beg_index == end_index) {
                empty_ = true;
            }
            return ret;
        }

        Iterator<value_type> begin() {
            return Iterator<value_type, Allocator>(capacity_, mass, beg_index, end_index, beg_index, empty_, true);
        }

        Iterator<value_type> end() {
            return Iterator<value_type, Allocator>(capacity_, mass, beg_index, end_index, end_index, empty_, false);
        }

        ConstIterator<value_type> cbegin() const {
            return ConstIterator<value_type, Allocator>(capacity_, mass, beg_index, end_index, beg_index, empty_,
                                                        true);
        }

        ConstIterator<value_type> cend() const {
            return ConstIterator<value_type, Allocator>(capacity_, mass, beg_index, end_index, end_index, empty_,
                                                        false);
        }

        reverse_iterator rbegin() {
            Iterator<value_type, Allocator> i(capacity_, mass, beg_index, end_index, end_index, empty_, false);
            return std::reverse_iterator<Iterator<value_type, Allocator>>(i);
        }

        reverse_iterator rend() {
            Iterator<value_type, Allocator> i(capacity_, mass, beg_index, end_index, beg_index, empty_, true);
            return std::reverse_iterator<Iterator<value_type, Allocator>>(i);
        }

        const_reverse_iterator rcbegin() const {
            ConstIterator<value_type, Allocator> i(capacity_, mass, beg_index, end_index, end_index, empty_, false);
            return std::reverse_iterator<ConstIterator<value_type, Allocator>>(i);
        }

        const_reverse_iterator rcend() const {
            ConstIterator<value_type, Allocator> i(capacity_, mass, beg_index, end_index, end_index, empty_, true);
            return std::reverse_iterator<ConstIterator<value_type, Allocator>>(i);
        }

        value_type &operator[](size_t idx) {
            return *(begin() + idx);
        }

        const T &operator[](size_t idx) const {
            return cbegin()[idx];
        }

        const T &front() const {
            return mass[(beg_index)];
        }

        const T &back() const {
            if (end_index == 0) {
                return mass[capacity_ - 1];
            }
            return mass[end_index - 1];
        }

        CCircularBufferBase &operator=(const CCircularBufferBase &other) {
            uint64_t j = beg_index;
            for (uint64_t i = 0; i < size(); i++) {
                allocator_.destroy(mass + j);
                j = (j + 1) % capacity_;
            }
            if (capacity_ > 0) {
                allocator_.deallocate(mass, capacity_);
            }

            capacity_ = other.capacity_;
            end_index = other.end_index;
            beg_index = other.beg_index;
            empty_ = other.empty_;
            allocator_ = other.allocator_;

            if (capacity_ != 0) {
                mass = allocator_.allocate(capacity_);
                for (uint64_t i = 0; i < other.size(); i++) {
                    allocator_.construct(mass + i + beg_index, other[i]);
                }
            }
            return *this;
        }

        bool operator==(const CCircularBufferBase &lhs) const {
            if (empty_ != lhs.empty_ or size() != lhs.size()) {
                return false;
            }
            for (uint64_t i = 0; i < size(); i++) {
                if ((*this)[i] != lhs[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const CCircularBufferBase &lhs) const {
            return !(*this == lhs);
        }

        void swap(CCircularBufferBase &lhs) {
            std::swap<CCircularBufferBase>(*this, lhs);
        }

        CCircularBufferBase() : capacity_(0), beg_index(0), end_index(0), empty_(true) {};

        CCircularBufferBase(const CCircularBufferBase &other) : allocator_(other.allocator_) {
            setCapacity(other.capacity_);
            end_index = other.end_index;
            beg_index = other.beg_index;
            empty_ = other.empty_;
            for (uint64_t i = 0; i < other.size(); i++) {
                std::allocator_traits<Allocator>::construct(allocator_, mass + (beg_index + i) % capacity_, other[i]);
            }
        }

        CCircularBufferBase(size_t n, const T &value) {
            setCapacity(n);
            for (uint64_t i = 0; i < capacity_; i++) {
                std::allocator_traits<Allocator>::construct(allocator_, mass + i, value);
                empty_ = false;
            }
        }

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        CCircularBufferBase(InputIterator first, InputIterator last): capacity_(0), beg_index(0), end_index(0),
                                                                      empty_(true) {
            uint64_t n = std::distance(first, last);
            setCapacity(n);
            empty_ = false;
            for (uint64_t i = 0; i < capacity_; i++) {
                std::allocator_traits<Allocator>::construct(allocator_, mass + i, *first);
                ++first;
            }
            return;
        }

        CCircularBufferBase(const std::initializer_list<value_type> &list) : CCircularBufferBase(list.begin(),
                                                                                                 list.end()) {}

        CCircularBufferBase(size_t capacity) {
            setCapacity(capacity);
        }

        virtual ~CCircularBufferBase() {
            uint64_t j = beg_index;
            for (uint64_t i = 0; i < size(); i++) {
                std::allocator_traits<Allocator>::destroy(allocator_, mass + j);
                j = (j + 1) % capacity_;
            }
            if (capacity_ > 0) {
                std::allocator_traits<Allocator>::deallocate(allocator_, mass, capacity_);
            }
        }

    protected:
        T *mass;
        size_t capacity_;
        size_t end_index;
        size_t beg_index;
        bool empty_;
        Allocator allocator_;

        Iterator<T>
        CreateIterator(size_t capacity, T *ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg) {
            return Iterator<value_type, Allocator>(capacity, ptr, beg, end, ind, empty, isBeg);
        }

        const_iterator
        CreateConstIterator(size_t capacity, T *ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg) {
            return ConstIterator<value_type, Allocator>(capacity, ptr, beg, end, ind, empty, isBeg);
        }

        void setCapacity(size_t capacity) {
            capacity_ = capacity;
            mass = std::allocator_traits<Allocator>::allocate(allocator_, capacity);
            beg_index = 0;
            end_index = 0;
            empty_ = true;
        }
    };

    template<class T, class Allocator = std::allocator<T>>
    class CCircularBuffer : public CCircularBufferBase<T, Allocator> {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef ConstIterator<T> const_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<Iterator<T>> reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        CCircularBuffer() : CCircularBufferBase<T, Allocator>() {};

        void put(const T &value) {
            if (this->capacity_ == 0) {
                return;
            }
            this->allocator_.construct(this->mass + this->end_index, value_type());
            if (!this->empty_ && this->end_index == this->beg_index) {
                std::allocator_traits<Allocator>::construct(this->allocator_, this->mass + this->beg_index, value);
                this->beg_index++;
                this->end_index++;
                return;
            }

            std::allocator_traits<Allocator>::construct(this->allocator_, this->mass + this->end_index, value);
            this->end_index = (this->end_index + 1) % this->capacity_;
            this->empty_ = false;
        }

        void resize(size_type new_size) {
            if (new_size >= this->capacity_) {
                return;
            }
            if (new_size == 0) {
                this->empty_ = true;
            }

            if (new_size <= this->size()) {
                uint64_t old_end = this->end_index;
                uint64_t diff = this->size() - new_size - 1;
                this->end_index = (this->beg_index + new_size) % this->capacity_;
                uint64_t copy_end = this->end_index;

                for (uint64_t i = 0; i < diff; i++) {
                    std::allocator_traits<Allocator>::destroy(this->allocator_, this->mass + copy_end);
                    copy_end = (copy_end + 1) % this->capacity_;
                }
                return;
            }

            for (uint64_t i = this->size(); i < new_size; i++) {
                std::allocator_traits<Allocator>::construct(this->allocator_, this->mass + i, value_type());
                this->end_index++;
            }
        }

        CCircularBuffer(const std::initializer_list<value_type> &list) : CCircularBufferBase<T, Allocator>(list) {};

        CCircularBuffer(size_t capacity) : CCircularBufferBase<T, Allocator>(capacity) {}

        CCircularBuffer(const CCircularBuffer &other) : CCircularBufferBase<T, Allocator>(other) {};

        CCircularBuffer(size_t n, const T &value) : CCircularBufferBase<T, Allocator>(n, value) {}

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        CCircularBuffer(InputIterator first, InputIterator last) : CCircularBufferBase<T, Allocator>(first, last) {}

        ~CCircularBuffer() = default;
    };

    template<class T, class Allocator = std::allocator<T>>
    class CCircularBufferExt : public CCircularBufferBase<T, Allocator> {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef size_t size_type;

        CCircularBufferExt() : CCircularBufferBase<T, Allocator>() {};

        CCircularBufferExt(const CCircularBufferExt &other) : CCircularBufferBase<T, Allocator>(other) {};


        CCircularBufferExt(size_t capacity) : CCircularBufferBase<T, Allocator>(capacity) {};

        CCircularBufferExt(size_t n, const T &value) : CCircularBufferBase<T, Allocator>(n, value) {};

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        CCircularBufferExt(InputIterator first, InputIterator last):CCircularBufferBase<T, Allocator>(first, last) {};

        CCircularBufferExt(const std::initializer_list<value_type> &list) : CCircularBufferBase<T, Allocator>(list) {};

        void resize(size_type new_size) {
            if (new_size > this->capacity_) {
                this->reserve(new_size);
            }

            if (new_size == 0) {
                this->empty_ = true;
            }

            if (new_size <= this->size()) {
                uint64_t old_end = this->end_index;
                uint64_t diff = this->size() - new_size - 1;
                this->end_index = (this->beg_index + new_size) % this->capacity_;
                uint64_t copy_end = this->end_index;

                for (uint64_t i = 0; i < diff; i++) {
                    std::allocator_traits<Allocator>::destroy(this->allocator_, this->mass + copy_end);
                    copy_end = (copy_end + 1) % this->capacity_;
                }
                return;
            }
            for (uint64_t i = this->size(); i < new_size; i++) {
                std::allocator_traits<Allocator>::construct(this->allocator_, this->mass + i, value_type());
                this->end_index++;
            }
        }

        void put(const T &value) {
            if (this->size() == this->capacity_) {
                if (this->capacity_ == 0) {
                    this->reserve(1);
                } else {
                    this->reserve(2 * this->capacity_);
                }
            }
            std::allocator_traits<Allocator>::construct(this->allocator_, this->mass + this->end_index, value);
            this->end_index = (this->end_index + 1) % this->capacity_;
            this->empty_ = false;
        }

        ~CCircularBufferExt() = default;
    };

    template<class T>
    void swap(CCircularBufferBase<T> &lhs, CCircularBufferBase<T> &rhs) {
        lhs.swap(rhs);
    }
}