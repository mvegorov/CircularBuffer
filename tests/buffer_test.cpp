#include <lib/CCircularBuffer.h>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

using namespace buff;

template<class T>
class InpIter {
public:
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef size_t size_type;
    typedef std::input_iterator_tag iterator_category;
    typedef InpIter self;

    InpIter(pointer p) : array(p) {}

    bool operator==(const self& lhs) const {
        return array == lhs.array;
    }

    bool operator!=(const self& lhs) const {
        return !(*this == lhs);
    }

    reference operator->() {
        return *array;
    }

    reference operator*() {
        return *array;
    }

    self& operator++() {
        ++array;
        return *this;
    }

    self operator++(int) {
        InpIter oldValue(array);
        ++(*this);
        return oldValue;
    }

protected:
    pointer array;
};

TEST(CCircularBufferTestSuite, EmptyTest) {
    CCircularBuffer<int> bufer;

    ASSERT_TRUE(bufer.empty());
    ASSERT_TRUE(bufer.begin() == bufer.end());
    bufer.put(32); // проверка на то, что программа не упадет
    bufer.get(); // проверка на то же
    ASSERT_TRUE(bufer.empty());

    bufer.reserve(10);
    bufer.put(32);
    ASSERT_FALSE(bufer.empty());
    bufer.clear();
    ASSERT_TRUE(bufer.empty());
}

TEST(CCircularBufferTestSuite, ConstructorTest) {
    CCircularBuffer<int> bufer{1, 1, 1, 1, 1, 1};
    std::vector<int> bufer_copy{1, 1, 1, 1, 1, 1, 1, 1};

    int buf_c[6] = {1, 1, 1, 1, 1, 1};

    CCircularBuffer<int> anotherBufer(bufer);
    CCircularBuffer<int> anotherBufer2(6, 1);
    CCircularBuffer<int> anotherBufer3(bufer.begin(), bufer.end());
    CCircularBuffer<int> anotherBufer4(anotherBufer2.begin(), anotherBufer2.end());
    CCircularBuffer<int> anotherBufer5(bufer_copy.begin() + 1, bufer_copy.end() - 1);
    CCircularBuffer<int> anotherBufer6(InpIter<int>(buf_c), InpIter<int>(buf_c + 6));

    InpIter<int> a(buf_c);
    InpIter<int> b(buf_c + 6);

    ASSERT_TRUE(std::distance(a, b) == std::distance(a, b));
    ASSERT_FALSE(a == b);


    ASSERT_TRUE((anotherBufer2.end() - anotherBufer2.begin()) == 6);
    ASSERT_TRUE(bufer == anotherBufer);
    ASSERT_TRUE(bufer == anotherBufer2);
    ASSERT_TRUE(bufer == anotherBufer3);
    ASSERT_TRUE(bufer == anotherBufer4);
    ASSERT_TRUE(bufer == anotherBufer5);
    ASSERT_TRUE(bufer == anotherBufer6);
}

TEST(CCircularBufferTestSuite, EqualityTest) {
    CCircularBuffer<int> bufer;
    CCircularBuffer<int> bufer2;
    ASSERT_TRUE(bufer == bufer);
    ASSERT_TRUE(bufer == bufer2 && bufer2 == bufer);

    CCircularBuffer<int> bufer3(5);
    bufer3.put(100);
    ASSERT_FALSE(bufer == bufer3);
    ASSERT_TRUE(bufer != bufer3);
}

TEST(CCircularBufferTestSuite, IteratorTest) {
    CCircularBuffer<int> bufer2_{1, 2, 3, 4, 5, 6, 7, 8};
    const CCircularBuffer<int> answer{1, 2, 100, 4, 5, 6, 7, 8};

    ASSERT_TRUE(std::distance(bufer2_.begin(), bufer2_.end()) == bufer2_.size());
    ASSERT_TRUE(bufer2_.begin() < bufer2_.end());

    Iterator<int> first_elem = bufer2_.begin();
    uint64_t i = 0;
    while (first_elem < bufer2_.end()) {
        ASSERT_TRUE(*first_elem == answer[i]);
        ++i;
        ++first_elem;
        if (i == 2) {
            *first_elem = 100;
        }
    }

    i = 0;
    for (int j: bufer2_) {
        ASSERT_TRUE(j == bufer2_[i]);
        ++i;
    }

    bufer2_.put(100);
    ASSERT_TRUE(bufer2_.begin() < bufer2_.end());
    ASSERT_TRUE(bufer2_[8] == bufer2_[0]);
    ASSERT_TRUE(bufer2_[0] == 2);
    ASSERT_TRUE(*(--bufer2_.end()) == 100);
    ASSERT_TRUE(bufer2_.back() == 100);

}

TEST(CCircularBufferTestSuite, ConstIteratorTest) {
    const CCircularBuffer<int> bufer{1, 2, 3, 4, 5, 6, 7, 8};

    std::vector<int> diff_bufer{1, 2, 3, 4, 5, 6, 7, 8};

    ConstIterator<int> first_elem = bufer.cbegin();

    uint64_t i = 0;
    while (first_elem < bufer.cend()) {
        ASSERT_TRUE(*first_elem == diff_bufer[i]);
        ++i;
        ++first_elem;
    }

    for (i = 0; i < bufer.size(); ++i) {
        ASSERT_TRUE(bufer[i] == diff_bufer[i]);
    }

    ASSERT_TRUE(bufer.cbegin() < bufer.cend());
}

TEST(CCircularBufferTestSuite, ReverseIteratorTest) {
    CCircularBuffer<int> bufer{1, 2, 3, 4, 5, 6, 7, 8};
    const CCircularBuffer<int> buferConst{8, 1, 3, 7, 5, 9, 6, 9};

    auto first_elem = bufer.rbegin();
    auto first_elem_const = buferConst.rcbegin();

    uint64_t i = bufer.size() - 1;
    while (first_elem < bufer.rend()) {
        ASSERT_TRUE(*first_elem == bufer[i] && buferConst[i] == *first_elem_const);
        ++first_elem;
        ++first_elem_const;
        --i;
    }
}

TEST(CCircularBufferTestSuite, PutGetTest) {
    CCircularBuffer<int> bufer2_{1, 2, 3, 4, 5, 6, 7, 8};

    bufer2_.put(9);
    ASSERT_TRUE(bufer2_.back() == 9);
    ASSERT_TRUE(bufer2_.front() == 2);

    ASSERT_TRUE(bufer2_.get() == 2);
    ASSERT_TRUE(bufer2_.front() == 3);

    bufer2_.put(10);
    ASSERT_TRUE(bufer2_.back() == 10);
    ASSERT_TRUE(bufer2_.front() == 3);
}

TEST(CCircularBufferTestSuite, ReservResizeTest) {
    CCircularBuffer<int> bufer2_{1, 2, 3, 4, 5, 6, 7, 8};

    bufer2_.reserve(10);
    bufer2_.put(9);
    bufer2_.put(10);

    ASSERT_TRUE(bufer2_.back() == 10);
    ASSERT_TRUE(bufer2_.front() == 1);

    bufer2_.resize(8);
    ASSERT_TRUE(bufer2_.back() == 8);
    ASSERT_TRUE(bufer2_.front() == 1);

    bufer2_.reserve(999999); // проверка на то, что программа не упадет
}

TEST(CCircularBufferTestSuite, SwapTest) {
    const CCircularBuffer<int> bufer{1, 2, 3, 4, 5};
    const CCircularBuffer<int> anotherBufer{333, 666, 999};

    CCircularBuffer<int> bufer2(bufer);
    CCircularBuffer<int> anotherBufer2(anotherBufer);

    bufer2.swap(anotherBufer2);
    ASSERT_TRUE(bufer2 == anotherBufer && anotherBufer2 == bufer);

    swap(bufer2, anotherBufer2);
    ASSERT_TRUE(bufer2 == bufer && anotherBufer2 == anotherBufer);
}

TEST(CCircularBufferExtTestSuite, EmptyTest) {
    CCircularBufferExt<std::string> bufer;

    bufer.get(); // проверка на то, что программа не упадет
    bufer.front(); // проверка на то же

    ASSERT_TRUE(bufer.empty());
    ASSERT_TRUE(bufer.begin() == bufer.end());

    bufer.put("a");
    ASSERT_FALSE(bufer.empty());
    bufer.clear();
    ASSERT_TRUE(bufer.empty());
}

TEST(CCircularBufferExtTestSuite, ConstructorTest) {
    CCircularBufferExt<int> bufer{1, 1, 1, 1, 1, 1};
    CCircularBufferExt<int> anotherBufer(bufer);
    CCircularBufferExt<int> anotherBufer2(6, 1);
    CCircularBufferExt<int> anotherBufer3(bufer.begin(), bufer.end());
    CCircularBufferExt<int> anotherBufer4(anotherBufer2.begin(), anotherBufer2.end());

    ASSERT_TRUE((anotherBufer2.end() - anotherBufer2.begin()) == 6);
    ASSERT_TRUE(bufer == anotherBufer);
    ASSERT_TRUE(bufer == anotherBufer2);
    ASSERT_TRUE(bufer == anotherBufer3);
    ASSERT_TRUE(bufer == anotherBufer4);
}

TEST(CCircularBufferExtTestSuite, constIteratorTest) {
    const CCircularBufferExt<int> bufer{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> diff_bufer{1, 2, 3, 4, 5, 6, 7, 8};

    auto first_elem = bufer.cbegin();

    uint64_t i = 0;
    while (first_elem < bufer.cend()) {
        ASSERT_TRUE(*first_elem == diff_bufer[i]);
        ++i;
        ++first_elem;
    }

    for (i = 0; i < bufer.size(); ++i) {
        ASSERT_TRUE(bufer[i] == diff_bufer[i]);
    }

    ASSERT_TRUE(bufer.cbegin() < bufer.cend());
}

TEST(CCircularBufferExtTestSuite, SwapTest) {
    const CCircularBufferExt<int> bufer{1, 2, 3, 4, 5};
    const CCircularBufferExt<int> anotherBufer{333, 666, 999};

    CCircularBufferExt<int> bufer2(bufer);
    CCircularBufferExt<int> anotherBufer2(anotherBufer);

    bufer2.swap(anotherBufer2);
    ASSERT_TRUE(bufer2 == anotherBufer && anotherBufer2 == bufer);

    swap(bufer2, anotherBufer2);
    ASSERT_TRUE(bufer2 == bufer && anotherBufer2 == anotherBufer);
}

TEST(CCircularBufferExtTestSuite, PutTest) {
    CCircularBufferExt<int> array{1, 2, 3};

    array.put(4);
    CCircularBufferExt<int> answer1{1, 2, 3, 4};
    ASSERT_TRUE(array == answer1);
    ASSERT_TRUE(array.capacity() == 6);

    array.put(5);
    CCircularBufferExt<int> answer2{1, 2, 3, 4, 5};
    ASSERT_TRUE(array == answer2);
    ASSERT_TRUE(array.capacity() == 6);

    array.put(6);
    array.put(7);
    CCircularBufferExt<int> answer3{1, 2, 3, 4, 5, 6, 7};
    ASSERT_TRUE(array == answer3);
    ASSERT_TRUE(array.capacity() == 12);
}

TEST(CCircularBufferExtTestSuite, ResizeTest) {
    CCircularBufferExt<int> array{1, 2, 3, 4, 5, 6, 7, 8, 0, 10};

    ASSERT_TRUE(array.size() == 10);
    array.resize(8);
    ASSERT_TRUE(array.size() == 8);
    ASSERT_TRUE(array.back() == 8);

    array.resize(20);
    ASSERT_TRUE(array.size() == 20);
}

TEST(CCircularBufferExtTestSuite, VectorTest) {
    CCircularBufferExt<std::vector<int>> array;

    array.put({1,2,3});
    array.put({4,5,6});
    array.put({});

    std::vector<int> a{1, 2};

    array[2].push_back(1);
    array[2].push_back(2);

    ASSERT_TRUE(array[2] == a);
    ASSERT_TRUE(array.back() == a);

    a = {1,2,3};

    ASSERT_TRUE(array.front() == a);
}
