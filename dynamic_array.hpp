#ifndef DYNAMIC_ARRAY_HPP_INCLUDED
#define DYNAMIC_ARRAY_HPP_INCLUDED

#include <memory>
#include <stdexcept>
#include <string>
#include <cmath>
#include <initializer_list>
#include <cstddef>
#include <iterator>


template < typename T >
class RandomAccessIterator
{
public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

template < typename T1, typename Allocator >
    friend class DynArr;

template < typename T1 >
    friend class RandomAccessIterator;

private:
    pointer _getPointer() noexcept
    {
        return _p;
    }

    const pointer _getPointer() const noexcept
    {
        return _p;
    }

    RandomAccessIterator(pointer p): _p(p)
    {}

    operator RandomAccessIterator<std::remove_const<value_type>>() const noexcept
    {
        auto p = (std::remove_const<value_type>*)_p;
        return RandomAccessIterator<std::remove_const<value_type>>(p);
    }

public:
    RandomAccessIterator(): _p(nullptr)
    {}
    RandomAccessIterator(const RandomAccessIterator& other) = default;

    ~RandomAccessIterator()
    {}

    RandomAccessIterator& operator=(const RandomAccessIterator& other)
    {
        _p = other._p;
        return *this;
    }

    reference operator*()
    {
        return *_p;
    }
    const_reference operator*() const
    {
        return *_p;
    }

    pointer operator->()
    {
        return _p;
    }
    const_pointer operator->() const
    {
        return _p;
    }

    RandomAccessIterator& operator++()
    {
        _p++;
        return *this;
    }
    RandomAccessIterator& operator++(int)
    {
        RandomAccessIterator<value_type>oldIter(*this);
        _p++;
        return oldIter;
    }

    RandomAccessIterator& operator--()
    {
        _p--;
        return *this;
    }
    RandomAccessIterator& operator--(int)
    {
        RandomAccessIterator<value_type>oldIter(*this);
        _p--;
        return oldIter;
    }

    bool operator==(const RandomAccessIterator& other) const
    {
        return _p == other._p;
    }
    bool operator!=(const RandomAccessIterator& other) const
    {
        return !(*this == other);
    }

    value_type& operator[](size_t n)
    {
        return _p[n];
    }
    const value_type& operator[](size_t n) const
    {
        return _p[n];
    }

    RandomAccessIterator& operator+=(difference_type n)
    {
        _p += n;
        return *this;
    }
    RandomAccessIterator& operator-=(difference_type n)
    {
        return *this += -n;
    }
    RandomAccessIterator& operator+(difference_type n)
    {
        RandomAccessIterator it = *this;
        return it += n;
    }
    RandomAccessIterator& operator-(difference_type n)
    {
        return *this + (-n);
    }

    difference_type operator-(const RandomAccessIterator& other) const
    {
        return _p - other._p;
    }

    operator RandomAccessIterator<const value_type>() const noexcept
    {
        auto p = (const value_type*)_p;
        return RandomAccessIterator<const value_type>(p);
    }

private:
    pointer _p;
};

// https://en.cppreference.com/w/cpp/container/vector
template < typename T, typename Allocator = std::allocator<T> >
class DynArr
{
public:
    // member types
    typedef T value_type;
    typedef Allocator allocator_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename std::allocator_traits<Allocator>::pointer pointer;
    typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
    typedef RandomAccessIterator<value_type> iterator;
    typedef RandomAccessIterator<const value_type> const_iterator;

private:
    template < typename Alloc >
    using traits = std::allocator_traits<Alloc>;

public:
    // member functions
    DynArr() noexcept(noexcept(Allocator())):
            _p(nullptr), _size(0), _cap(0), _alloc(Allocator())
    {}

    explicit DynArr(const Allocator& alloc) noexcept: 
            _p(nullptr), _size(0), _cap(0), 
            _alloc(traits<Allocator>::select_on_container_copy_construction(alloc))
    {}

    DynArr(size_type count, const T& value, const Allocator& alloc = Allocator()):
            DynArr(alloc)
    {
        _cap = calcCapacity(count);
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (size_type i = 0; i < count; i++)
            push_back(value);
        
        _size = count;
    }

    explicit DynArr(size_type count, const Allocator& alloc = Allocator()):
            DynArr(alloc)
    {
        _cap = calcCapacity(count);
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (size_type i = 0; i < count; i++)
            push_back(value_type());

        _size = count;
    }

    DynArr(const DynArr& other): DynArr(other.get_allocator())
    {
        _cap = calcCapacity(other.size());
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (size_type i = 0; i < other.size(); i++)
            push_back(other.at(i));
    }

    DynArr(const DynArr& other, const Allocator& alloc): DynArr(alloc)
    {
        _cap = calcCapacity(other.size());
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (size_type i = 0; i < other.size(); i++)
            push_back(other.at(i));

        other.clear();
    }

    DynArr(DynArr&& other) noexcept: DynArr(other.get_allocator())
    {
        _cap = calcCapacity(other.size());
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (size_type i = 0; i < other.size(); i++)
            push_back(std::move_if_noexcept(other.at(i)));

        other.clear();
    }
    
    DynArr(DynArr&& other, const Allocator& alloc): DynArr(alloc)
    {
        _cap = calcCapacity(other.size());
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (size_type i = 0; i < other.size(); i++)
            push_back(std::move_if_noexcept(other.at(i)));

        other.clear();
    }
    
    DynArr(std::initializer_list<T> init, const Allocator& alloc = Allocator()): 
            DynArr(alloc)
    {
        _cap = calcCapacity(init.size());
        _p = traits<Allocator>::allocate(_alloc, capacity());
        for (auto i = init.begin(); i != init.end(); i++)
            push_back(*i);
    }

    ~DynArr()
    {
        clear();
    }

    // element access
    reference at(size_type pos)
    {
        if (!(pos < size()))
            throw std::out_of_range(std::string("DynArr::at: pos >= this.size() (which is ") + 
                    std::to_string(size()) + ")");
        
        return _p[pos];
    }

    const_reference at(size_type pos) const
    {
        if (!(pos < size()))
            throw std::out_of_range(std::string("DynArr::at: pos >= this.size() (which is ") + 
                    std::to_string(size()) + ")");
        
        return _p[pos];
    }

    reference operator[](size_type pos)
    {
        return _p[pos];
    }
    
    const_reference operator[](size_type pos) const
    {
        return _p[pos];
    }

    allocator_type get_allocator() const noexcept
    {
        _alloc;
    }

    // iterators
    iterator begin() noexcept
    {
        return iterator(_p);
    }
    const_iterator begin() const noexcept
    {
        return const_iterator(_p);
    }
    const_iterator cbegin() const noexcept
    {
        const_iterator(_p);
    }

    iterator end() noexcept
    {
        return iterator(_p + size());
    }
    const_iterator end() const noexcept
    {
        return const_iterator(_p + size());
    }
    const_iterator cend() const noexcept
    {
        return const_iterator(_p + size());
    }

    // capasity
    bool empty() const noexcept
    {
        return size() == 0;
    }
    
    size_type size() const noexcept
    {
        return _size;
    }

    size_type capacity() const noexcept
    {
        return _cap;
    }
    
    void shrink_to_fit()
    {
        auto newCap = size();
        auto newP = traits<Allocator>::allocate(_alloc, newCap);
        for (size_type i = 0; i < size(); i++)
            traits<Allocator>::construct(_alloc, newP + i, std::move_if_noexcept(_p[i]));

        traits<Allocator>::deallocate(_alloc, _p, capacity());
        _p = newP;
        _cap = newCap;
    }

    // modifiers
    void clear() noexcept
    {
        try
        {
            resize(0);
        }
        catch(...)
        {}
        traits<Allocator>::deallocate(_alloc, _p, capacity());
        _cap = 0;
    }

    void push_back(const T& value)
    {
        if (size() < capacity())
        {
            traits<Allocator>::construct(_alloc, _p + size(), value);
            _size++;
        }
        else
        {
            auto newCap = calcCapacity(size() + 1);
            auto newP = traits<Allocator>::allocate(_alloc, newCap);
            try
            {
                traits<Allocator>::construct(_alloc, newP + size(), value);
            }
            catch(...)
            {
                traits<Allocator>::deallocate(_alloc, newP, newCap);
                throw;
            }
            
            for (size_type i = 0; i < size(); i++)
                traits<Allocator>::construct(_alloc, newP + i, std::move_if_noexcept(_p[i]));

            traits<Allocator>::deallocate(_alloc, _p, capacity());
            _p = newP;
            _size++;
            _cap = newCap;
        }
    }
    
    void push_back(T&& value)
    {
        if (size() < capacity())
        {
            traits<Allocator>::construct(_alloc, _p + size(), value);
            _size++;
        }
        else
        {
            auto newCap = calcCapacity(size() + 1);
            auto newP = traits<Allocator>::allocate(_alloc, newCap);
            try
            {
                traits<Allocator>::construct(_alloc, newP + size(), std::move(value));
            }
            catch(...)
            {
                traits<Allocator>::deallocate(_alloc, newP, newCap);
                throw;
            }
            
            for (size_type i = 0; i < size(); i++)
                traits<Allocator>::construct(_alloc, newP + i, std::move_if_noexcept(_p[i]));

            traits<Allocator>::deallocate(_alloc, _p, capacity());
            _p = newP;
            _size++;
            _cap = calcCapacity(size());
        }
    }
    
    template <typename... Args >
        reference emplace_back(Args&&... args)
        {
            push_back(value_type(std::forward<Args>(args)...));
            return at(size()-1);
        }
    
    void pop_back()
    {
        resize(size() - 1);
    }
    
    void resize(size_type count)
    {
        if (!(count <= size()))
            throw std::out_of_range(std::string("DynArr::resize: count > this.size() (which is ") + 
                    std::to_string(size()) + ")");

        for (size_type i = count; i < size(); i++)
            traits<Allocator>::destroy(_alloc, _p + i);
    
        _size = count;
    }

    void resize(size_type count, const T& val)
    {
        if (count < size())
        {
            for (size_type i = count; i < size(); i++)
                traits<Allocator>::destroy(_alloc, _p + i);
        
            _size = count;
        }
        else if (count > size())
            for (; size() < count;)
                push_back(val);
    }

private:
    size_type calcCapacity(size_type sz) const noexcept
    {
        // return std::pow(2, (size_type)std::log2(sz) + 1);
        return sz * 2;
    }

private:
    // fields
    pointer _p;
    size_type _size;
    size_type _cap;
    Allocator _alloc;
};

// non-member functions
template < typename T, typename Alloc >
std::ostream& operator<<(std::ostream& os, const DynArr<T, Alloc>& arr)
{
    if (arr.size() == 0)
    {
        std::cout << "[]";
        return os;
    }
    os << "[";
    for (int i = 0; i < arr.size()-1; i++)
        os << arr.at(i) << ", ";
    os << arr.at(arr.size()-1);
    os << "]";
    return os;
}

template<class T, class Allocator>
    typename DynArr<T, Allocator>::iterator begin(const DynArr<T, Allocator>& arr)
    {
        return arr.begin();
    }

template<class T, class Allocator>
    typename DynArr<T, Allocator>::iterator end(const DynArr<T, Allocator>& arr)
    {
        arr.end();
    }


#endif // DYNAMIC_ARRAY_HPP_INCLUDED
