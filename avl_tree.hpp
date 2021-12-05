#ifndef AVL_TREE_HPP_INCLUEDED
#define AVL_TREE_HPP_INCLUEDED

#include <functional>
#include <memory>
#include <cstddef>
#include <iterator>
#include <utility>
#include <stdexcept>

#include <iostream>

#include "stack.hpp"

//! left < root < right

// node structure
template < typename Key >
struct TreeNode
{
    typedef Key value_type;
    typedef TreeNode<value_type> node_type;
    typedef node_type* node_pointer;

    TreeNode(const value_type& value, node_pointer left, node_pointer right, size_t height):
        value(value), left(left), right(right), height(height)
    {}
    TreeNode(value_type&& value, node_pointer left, node_pointer right, size_t height):
        value(std::move(value)), left(left), right(right), height(height)
    {}
    TreeNode(const TreeNode& other) = default;
    TreeNode(TreeNode&& other) = default;

    operator TreeNode<const value_type>() const noexcept
    {
        return TreeNode<const value_type>((const value_type*)value, left, right, height);
    }
    operator TreeNode<std::remove_const<value_type>>() const noexcept
    {
        return TreeNode<std::remove_const<value_type>>((std::remove_const<value_type>*)value, left, right, height);
    }

    value_type value;
    node_pointer left;
    node_pointer right;
    size_t height;
};

// forward iterators

template < typename Key >
class TreeForwardIterator;
template < typename Key >
class RootLeftRightIterator;
template < typename Key >
class LeftRootRightIterator;
template < typename Key >
class LeftRightRootIterator;
template < typename Key >
class WidthIterator;

// https://en.cppreference.com/w/cpp/named_req/ForwardIterator
template < typename Key >
class TreeForwardIterator
{
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Key value_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

protected:
    typedef typename TreeNode<Key>::node_type node_type;
    typedef typename TreeNode<Key>::node_pointer node_pointer;

template < typename Key1, typename Compare, typename Allocator >
    friend class AVLTree;

template < typename Key1 >
    friend class TreeForwardIterator;

protected:
    node_pointer _getNodePointer() noexcept
    {
        return _node;
    }

    const node_pointer _getNodePointer() const noexcept
    {
        return _node;
    }

    TreeForwardIterator(node_pointer node): _node(node)
    {}

    operator TreeForwardIterator<std::remove_const<value_type>>() const noexcept
    {
        auto nodeP = (TreeNode<std::remove_const<value_type>>*)_node;
        return TreeForwardIterator<std::remove_const<value_type>>(nodeP);
    }

public:
    TreeForwardIterator(): _node(nullptr)
    {}
    TreeForwardIterator(const TreeForwardIterator& other) = default;

    ~TreeForwardIterator()
    {}

    reference operator*()
    {
        return _node->value;
    }
    const_reference operator*() const
    {
        return _node->value;
    }

    pointer operator->()
    {
        return &(_node->value);
    }
    const_pointer operator->() const
    {
        return &(_node->value);
    }

    bool operator==(const TreeForwardIterator& other) const
    {
        return _node == other._node;
    }
    bool operator!=(const TreeForwardIterator& other) const
    {
        return !(*this == other);
    }

    operator TreeForwardIterator<const value_type>() const noexcept
    {
        auto nodeP = (TreeNode<const value_type>*)_node;
        return TreeForwardIterator<const value_type>(nodeP);
    }

protected:
    node_pointer _node;
};

template < typename Key >
class RootLeftRightIterator: public TreeForwardIterator<Key>
{
public:
    typedef typename TreeForwardIterator<Key>::iterator_category iterator_category;
    typedef typename TreeForwardIterator<Key>::value_type value_type;
    typedef typename TreeForwardIterator<Key>::difference_type difference_type;
    typedef typename TreeForwardIterator<Key>::pointer pointer;
    typedef typename TreeForwardIterator<Key>::const_pointer const_pointer;
    typedef typename TreeForwardIterator<Key>::reference reference;
    typedef typename TreeForwardIterator<Key>::const_reference const_reference;

private:
    typedef typename TreeForwardIterator<Key>::node_type node_type;
    typedef typename TreeForwardIterator<Key>::node_pointer node_pointer;

template < typename Key1, typename Compare, typename Allocator >
    friend class AVLTree;

template < typename Key1 >
    friend class RootLeftRightIterator;

private:
    RootLeftRightIterator(node_pointer node): TreeForwardIterator<Key>(node)
    {
        _path.push(node);
    }

    operator RootLeftRightIterator<std::remove_const<value_type>>() const noexcept
    {
        auto nodeP = (TreeNode<std::remove_const<value_type>>*)TreeForwardIterator<Key>::_node;
        return RootLeftRightIterator<std::remove_const<value_type>>(nodeP);
    }

public:
    RootLeftRightIterator()
    {}
    RootLeftRightIterator(const TreeForwardIterator<Key>& x): TreeForwardIterator<Key>(x)
    {
        _path.push(this->_node);
    }
    RootLeftRightIterator(const RootLeftRightIterator& other) = default;

    ~RootLeftRightIterator()
    {}

    RootLeftRightIterator& operator++()
    {
        if (this->_node != nullptr)
        {
            _path.pop();
            if (this->_node->right != nullptr)
                _path.push(this->_node->right);
            if (this->_node->left != nullptr)
                _path.push(this->_node->left);
        }
        if (!_path.empty())
            this->_node = _path.top();
        else
            this->_node = nullptr;

        return *this;
    }
    RootLeftRightIterator& operator++(int)
    {
        RootLeftRightIterator<value_type>oldIter(*this);
        if (this->_node != nullptr)
        {
            _path.pop();
            if (this->_node->right != nullptr)
                _path.push(this->_node->right);
            if (this->_node->left != nullptr)
                _path.push(this->_node->left);
        }
        if (!_path.empty())
            this->_node = _path.top();
        else
            this->_node = nullptr;

        return oldIter;
    }

    operator RootLeftRightIterator<const value_type>() const noexcept
    {
        auto nodeP = (TreeNode<const value_type>*)this->_node;
        return RootLeftRightIterator<const value_type>(nodeP);
    }

private:
    Stack<node_pointer> _path;
};

template < typename Key >
class LeftRootRightIterator: public TreeForwardIterator<Key>
{
public:
    typedef typename TreeForwardIterator<Key>::iterator_category iterator_category;
    typedef typename TreeForwardIterator<Key>::value_type value_type;
    typedef typename TreeForwardIterator<Key>::difference_type difference_type;
    typedef typename TreeForwardIterator<Key>::pointer pointer;
    typedef typename TreeForwardIterator<Key>::const_pointer const_pointer;
    typedef typename TreeForwardIterator<Key>::reference reference;
    typedef typename TreeForwardIterator<Key>::const_reference const_reference;

private:
    typedef typename TreeForwardIterator<Key>::node_type node_type;
    typedef typename TreeForwardIterator<Key>::node_pointer node_pointer;

template < typename Key1, typename Compare, typename Allocator >
    friend class AVLTree;

template < typename Key1 >
    friend class LeftRootRightIterator;

private:
    LeftRootRightIterator(node_pointer node): TreeForwardIterator<Key>(node)
    {
        // moving left if it possible
        for (auto n = node; n != nullptr; n = n->left)
            _path.push(n);
        
        if (this->_node != nullptr)
        {
            this->_node = _path.top();
            _path.pop();
            _curr = this->_node->right;
        }
        else
        {
            this->_node = nullptr;
            _curr = nullptr;
        }
    }

    operator LeftRootRightIterator<std::remove_const<value_type>>() const noexcept
    {
        auto nodeP = (TreeNode<std::remove_const<value_type>>*)TreeForwardIterator<Key>::_node;
        return LeftRootRightIterator<std::remove_const<value_type>>(nodeP);
    }

public:
    LeftRootRightIterator()
    {}
    LeftRootRightIterator(const TreeForwardIterator<Key>& x): TreeForwardIterator<Key>(x)
    {
        // moving left if it possible
        for (auto n = this->_node; n != nullptr; n = n->left)
            _path.push(n);
        
        if (this->_node != nullptr)
        {
            this->_node = _path.top();
            _path.pop();
            _curr = this->_node->right;
        }
        else
        {
            this->_node = nullptr;
            _curr = nullptr;
        }
    }
    LeftRootRightIterator(const LeftRootRightIterator& other) = default;

    ~LeftRootRightIterator()
    {}

    LeftRootRightIterator& operator++()
    {
        while (!_path.empty() || _curr != nullptr)
        {
            if (_curr != nullptr)
            {
                _path.push(_curr);
                _curr = _curr->left;
            }
            else
            {
                _curr = _path.top();
                _path.pop();
                this->_node = _curr;
                _curr = _curr->right;
                return *this;
            }
        }
        this->_node = nullptr;
        return *this;
    }
    LeftRootRightIterator& operator++(int)
    {
        LeftRootRightIterator<value_type>oldIter(*this);
        while (!_path.empty() || _curr != nullptr)
        {
            if (_curr != nullptr)
            {
                _path.push(_curr);
                _curr = _curr->left;
            }
            else
            {
                _curr = _path.top();
                _path.pop();
                this->_node = _curr;
                _curr = _curr->right;
                return oldIter;
            }
        }
        this->_node = nullptr;
    }

    operator LeftRootRightIterator<const value_type>() const noexcept
    {
        auto nodeP = (TreeNode<const value_type>*)this->_node;
        return LeftRootRightIterator<const value_type>(nodeP);
    }

private:
    Stack<node_pointer> _path;
    node_pointer _curr;
};

template < typename Key >
class LeftRightRootIterator: public TreeForwardIterator<Key>
{
public:
    typedef typename TreeForwardIterator<Key>::iterator_category iterator_category;
    typedef typename TreeForwardIterator<Key>::value_type value_type;
    typedef typename TreeForwardIterator<Key>::difference_type difference_type;
    typedef typename TreeForwardIterator<Key>::pointer pointer;
    typedef typename TreeForwardIterator<Key>::const_pointer const_pointer;
    typedef typename TreeForwardIterator<Key>::reference reference;
    typedef typename TreeForwardIterator<Key>::const_reference const_reference;

private:
    typedef typename TreeForwardIterator<Key>::node_type node_type;
    typedef typename TreeForwardIterator<Key>::node_pointer node_pointer;

template < typename Key1, typename Compare, typename Allocator >
    friend class AVLTree;

template < typename Key1 >
    friend class LeftRightRootIterator;

private:
    LeftRightRootIterator(node_pointer node): TreeForwardIterator<Key>(node)
    {
        if (node == nullptr)
            return;
    
        _path.push(node);
        for(; !_path.empty();)
        {
            _curr = _path.top();
            _path.pop();
            _out.push(_curr);
            if (_curr->left != nullptr)
                _path.push(_curr->left);
            if (_curr->right != nullptr)
                _path.push(_curr->right);
        }
        this->_node = _out.top();
        _out.pop();
    }

    operator LeftRightRootIterator<std::remove_const<value_type>>() const noexcept
    {
        auto nodeP = (TreeNode<std::remove_const<value_type>>*)TreeForwardIterator<Key>::_node;
        return LeftRightRootIterator<std::remove_const<value_type>>(nodeP);
    }

public:
    LeftRightRootIterator()
    {}
    LeftRightRootIterator(const TreeForwardIterator<Key>& x): TreeForwardIterator<Key>(x)
    {
        if (this->_node == nullptr)
            return;
    
        _path.push(this->_node);
        for(; !_path.empty();)
        {
            _curr = _path.top();
            _path.pop();
            _out.push(_curr);
            if (_curr->left != nullptr)
                _path.push(_curr->left);
            if (_curr->right != nullptr)
                _path.push(_curr->right);
        }
        this->_node = _out.top();
        _out.pop();
    }
    LeftRightRootIterator(const LeftRightRootIterator& other) = default;

    ~LeftRightRootIterator()
    {}

    LeftRightRootIterator& operator++()
    {
        if(!_out.empty())
        {
            this->_node = _out.top();
            _out.pop();
        }
        else
            this->_node = nullptr;

        return *this;
    }
    LeftRightRootIterator& operator++(int)
    {
        LeftRightRootIterator<value_type>oldIter(*this);
        if(!_out.empty())
        {
            this->_node = _out.top();
            _out.pop();
        }
        else
            this->_node = nullptr;

        return oldIter;
    }

    operator LeftRightRootIterator<const value_type>() const noexcept
    {
        auto nodeP = (TreeNode<const value_type>*)this->_node;
        return LeftRightRootIterator<const value_type>(nodeP);
    }

private:
    Stack<node_pointer> _path;
    Stack<node_pointer> _out; // storing LRN traversal
    node_pointer _curr;
};

template < typename Key >
class WidthIterator: public TreeForwardIterator<Key>
{
public:
    typedef typename TreeForwardIterator<Key>::iterator_category iterator_category;
    typedef typename TreeForwardIterator<Key>::value_type value_type;
    typedef typename TreeForwardIterator<Key>::difference_type difference_type;
    typedef typename TreeForwardIterator<Key>::pointer pointer;
    typedef typename TreeForwardIterator<Key>::const_pointer const_pointer;
    typedef typename TreeForwardIterator<Key>::reference reference;
    typedef typename TreeForwardIterator<Key>::const_reference const_reference;

private:
    typedef typename TreeForwardIterator<Key>::node_type node_type;
    typedef typename TreeForwardIterator<Key>::node_pointer node_pointer;

template < typename Key1, typename Compare, typename Allocator >
    friend class AVLTree;

template < typename Key1 >
    friend class WidthIterator;

private:
    WidthIterator(node_pointer node): TreeForwardIterator<Key>(node)
    {
        if (this->_node == nullptr)
            return;

        if (this->_node->left != nullptr)
            _first.push(this->_node->left);
        if (this->_node->right != nullptr)
            _first.push(this->_node->right);
    }

    operator WidthIterator<std::remove_const<value_type>>() const noexcept
    {
        auto nodeP = (TreeNode<std::remove_const<value_type>>*)TreeForwardIterator<Key>::_node;
        return WidthIterator<std::remove_const<value_type>>(nodeP);
    }

public:
    WidthIterator()
    {}
    WidthIterator(const TreeForwardIterator<Key>& x): TreeForwardIterator<Key>(x)
    {
        if (this->_node == nullptr)
            return;

        if (this->_node->left != nullptr)
            _first.push(this->_node->left);
        if (this->_node->right != nullptr)
            _first.push(this->_node->right);
    }
    WidthIterator(const WidthIterator& other) = default;

    ~WidthIterator()
    {}

    WidthIterator& operator++()
    {
        if (_first.empty() && _second.empty())
            this->_node = nullptr;
        else
        {
            if (_isNowFirst)
            {
                this->_node = _first.top();
                _first.pop();
                if (this->_node->left != nullptr)
                    _second.push(this->_node->left);
                if (this->_node->right != nullptr)
                    _second.push(this->_node->right);
            
                if (_first.empty())
                    _isNowFirst = false;
            }
            else if (!_isNowFirst)
            {
                this->_node = _second.top();
                _second.pop();
                if (this->_node->left != nullptr)
                    _first.push(this->_node->left);
                if (this->_node->right != nullptr)
                    _first.push(this->_node->right);

                if (_second.empty())
                    _isNowFirst = true;
            }
        }
        return *this;
    }
    WidthIterator& operator++(int)
    {
        WidthIterator<value_type>oldIter(*this);
        if (_first.empty() && _second.empty())
            this->_node = nullptr;
        else
        {
            if (_isNowFirst)
            {
                this->_node = _first.top();
                _first.pop();
                if (this->_node->left != nullptr)
                    _second.push(this->_node->left);
                if (this->_node->right != nullptr)
                    _second.push(this->_node->right);
            
                if (_first.empty())
                    _isNowFirst = false;
            }
            else if (!_isNowFirst)
            {
                this->_node = _second.top();
                _second.pop();
                if (this->_node->left != nullptr)
                    _first.push(this->_node->left);
                if (this->_node->right != nullptr)
                    _first.push(this->_node->right);

                if (_second.empty())
                    _isNowFirst = true;
            }
        }
        return oldIter;
    }

    operator WidthIterator<const value_type>() const noexcept
    {
        auto nodeP = (TreeNode<const value_type>*)this->_node;
        return WidthIterator<const value_type>(nodeP);
    }

private:
    Stack<node_pointer> _first;
    Stack<node_pointer> _second;
    bool _isNowFirst = true;
};

template < typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key> >
class AVLTree
{
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
    typedef Allocator allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename std::allocator_traits<Allocator>::pointer pointer;
    typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;

    typedef TreeForwardIterator<Key> iterator;
    typedef TreeForwardIterator<const Key> const_iterator;

    typedef RootLeftRightIterator<Key> root_left_right_iterator;
    typedef RootLeftRightIterator<const Key> const_root_left_right_iterator;

    typedef LeftRootRightIterator<Key> left_root_right_iterator;
    typedef LeftRootRightIterator<const Key> const_left_root_right_iterator;

    typedef LeftRightRootIterator<Key> left_right_root_iterator;
    typedef LeftRightRootIterator<const Key> const_left_right_root_iterator;

    typedef WidthIterator<Key> width_iterator;
    typedef WidthIterator<const Key> const_width_iterator;

private:
    template < typename Alloc >
    using traits = std::allocator_traits<Alloc>;

    typedef typename Allocator::template rebind<TreeNode<Key>>::other NodeAllocator;
    typedef TreeNode<value_type> node_type;
    typedef node_type& node_reference;
    typedef const value_type& const_node_reference;
    typedef typename traits<NodeAllocator>::pointer node_pointer;
    typedef typename traits<NodeAllocator>::const_pointer const_node_pointer;

public:
    AVLTree(): _root(nullptr), _size(0)
    {}
    explicit AVLTree(const Compare& comp, const Allocator& alloc = Allocator()):
        _root(nullptr), _size(0), _cmp(comp), _alloc(alloc)
    {}
    explicit AVLTree(const Allocator& alloc):
        _root(nullptr), _size(0), _alloc(alloc)
    {}
    AVLTree(const AVLTree& other);
    AVLTree(const AVLTree& other, const Allocator& alloc);
    AVLTree(AVLTree&& other):
        _root(other->_root), _size(other->_size), _cmp(other->_cmp), _alloc(other->_alloc)
    {
        other->root = nullptr;
        other->_size = 0;
    }
    AVLTree(AVLTree&& other, const Allocator& alloc):
        _root(other->_root), _size(other->_size), _cmp(other->_cmp), 
        _alloc(alloc)
    {
        other->root = nullptr;
        other->_size = 0;
    }
    AVLTree(std::initializer_list<value_type> init,
        const Compare& comp = Compare(),
        const Allocator& alloc = Allocator()):
        _root(nullptr), _size(0), _cmp(comp), _alloc()
    {
        for (auto i = init.begin(); i != init.end(); i++)
            insert(std::move(*i));
    }

    ~AVLTree()
    {
        clear();
    }

    iterator begin() noexcept
    {
        return iterator(_root);
    }
    const_iterator begin() const noexcept
    {
        return const_iterator(_root);
    }
    const_iterator cbegin() const noexcept
    {
        return const_iterator(_root);
    }

    iterator end() noexcept
    {
        return iterator(nullptr);
    }
    const_iterator end() const noexcept
    {
        return const_iterator(nullptr);
    }
    const_iterator cend() const noexcept
    {
        return const_iterator(nullptr);
    }

    bool empty() const noexcept
    {
        return _size == 0;
    }
    size_type size() const noexcept
    {
        return _size;
    }

    void clear() noexcept
    {
        for (; !empty();)
            erase(begin());
    }

    std::pair<iterator,bool> insert(const value_type& value)
    {
        if (_root == nullptr)
        {
            // creating first node
            _root = _createNode(value);
            _size++;
            return { iterator(_root), true };
        }

        value_compare cmp;
        Stack<node_pointer> path = _getPathToNode(value);
        if (path.top() != nullptr && path.top()->value != value)
            throw std::logic_error("some error in finding path in insertion");

        if (path.top() != nullptr && path.top()->value == value)
            // path.top()->value == value => this value already exists
            return { iterator(path.top()), false };
    
        path.pop();
        // create new node
        auto newNode = _createNode(value);
        _cmp(path.top()->value, value) ? path.top()->right = newNode : path.top()->left = newNode;
        _size++;
        
        for (; !path.empty();)
        {
            auto node = path.top();
            path.pop();
            node->height = std::max(_getHeight(node->left), _getHeight(node->right)) + 1;
            auto balance = _getLeftRightDiff(node);
            node_pointer newSubRoot;
            if (balance > 1 && newNode->value < node->left->value)
                newSubRoot = _rightRotation(node);
            else if (balance < -1 && _cmp(node->right->value, newNode->value))
                newSubRoot = _leftRotation(node);
            else if (balance > 1 && _cmp(node->left->value, newNode->value))
                newSubRoot = _leftRightRotation(node);
            else if (balance < -1 && newNode->value < node->right->value)
                newSubRoot = _rightLeftRotation(node);
            else
                continue;

            if (!path.empty())
                _cmp(path.top()->value, newSubRoot->value) ? 
                    path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            else
                _root = newSubRoot;
        }
        return { iterator(newNode), true };
    }
    std::pair<iterator,bool> insert(value_type&& value)
    {
        if (_root == nullptr)
        {
            // creating first node
            _root = _createNode(std::move(value));
            _size++;
            return { iterator(_root), true };
        }

        value_compare cmp;
        Stack<node_pointer> path = _getPathToNode(value);
        if (path.top() != nullptr && path.top()->value != value)
            throw std::logic_error("some error in finding path in insertion");

        if (path.top() != nullptr && path.top()->value == value)
            // path.top()->value == value => this value already exists
            return { iterator(path.top()), false };
    
        path.pop();
        // create new node
        auto newNode = _createNode(std::move(value));
        _cmp(path.top()->value, value) ? path.top()->right = newNode : path.top()->left = newNode;
        _size++;
        
        for (; !path.empty();)
        {
            auto node = path.top();
            path.pop();
            node->height = std::max(_getHeight(node->left), _getHeight(node->right)) + 1;
            auto balance = _getLeftRightDiff(node);
            node_pointer newSubRoot;
            if (balance > 1 && newNode->value < node->left->value)
                newSubRoot = _rightRotation(node);
            else if (balance < -1 && _cmp(node->right->value, newNode->value))
                newSubRoot = _leftRotation(node);
            else if (balance > 1 && _cmp(node->left->value, newNode->value))
                newSubRoot = _leftRightRotation(node);
            else if (balance < -1 && newNode->value < node->right->value)
                newSubRoot = _rightLeftRotation(node);
            else
                continue;

            if (!path.empty())
                _cmp(path.top()->value, newSubRoot->value) ? 
                    path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            else
                _root = newSubRoot;
        }
        return { iterator(newNode), true };
    }

    template < typename K >
    size_type erase(K&& key)
    {
        // buid the stack
        value_compare cmp;
        Stack<node_pointer> path = _getPathToNode(key);
        if (path.empty())
            return 0;

        node_pointer n = path.top();
        path.pop();
        // n->value == key, we delete this node
        if (n->left == nullptr || n->right == nullptr)
        {
            // deleting leaf or node with one child
            if (n->left == nullptr)
                if (!path.empty())
                    cmp(n->value, path.top()->value) ? 
                        path.top()->left = n->right : path.top()->right = n->right;
                else
                    // deleting root
                    _root = n->right;

            if (n->right == nullptr)
                if (!path.empty())
                    cmp(n->value, path.top()->value) ? 
                        path.top()->left = n->left : path.top()->right = n->left;
                else
                    // deleting root
                    _root = n->left;
        }
        else
        {
            // deleting node with two children
            node_pointer prevNode = path.empty() ? nullptr : path.top();
            // finding nearest value
            Stack<node_pointer> subPath;
            auto minNode = n->right;
            for (; minNode->left;)
            {
                subPath.push(minNode);
                minNode = minNode->left;
            }
            
            if (!subPath.empty())
                subPath.top()->left == minNode ? 
                    subPath.top()->left = minNode->right : subPath.top()->right = minNode->right;
            else
                n->right = minNode->right;
                
            // moving nearest node to this place
            auto node = minNode;
            node->left = n->left;
            node->right = n->right;
            if (prevNode != nullptr)
                cmp(prevNode->value, node->value) ? 
                    prevNode->right = node : prevNode->left = node;
            else
                // deleting root
                _root = node;

            // update heights
            path.push(node);
            for (; !subPath.empty();)
            {
                subPath.top()->height = std::max(_getHeight(subPath.top()->left),
                    _getHeight(subPath.top()->right)) + 1;
                auto newSubRoot = _makeRotation(subPath);
                subPath.pop();
                if (!subPath.empty())
                    _cmp(subPath.top()->value, newSubRoot->value) ? 
                        subPath.top()->right = newSubRoot : subPath.top()->left = newSubRoot;
                else
                    _cmp(path.top()->value, newSubRoot->value) ? 
                        path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            }
        }
        // update heigts
        for (; !path.empty();)
        {
            path.top()->height = std::max(_getHeight(path.top()->left), 
                _getHeight(path.top()->right)) + 1;
            auto newSubRoot = _makeRotation(path);
            path.pop();
            if (!path.empty())
                _cmp(path.top()->value, newSubRoot->value) ? 
                    path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            else
                _root = newSubRoot;
        }
        _deleteNode(n);
        _size--;
        return 1;
    }
    void erase(iterator pos)
    {
        if (pos == end() || _root == nullptr)
            return;
        
        // buid the stack
        value_compare cmp;
        Stack<node_pointer> path = _getPathToNode(*pos);
        if (path.empty())
            return;

        node_pointer n = path.top();
        path.pop();
        // n->value == *pos, we delete this node
        if (n->left == nullptr || n->right == nullptr)
        {
            // deleting leaf or node with one child
            if (n->left == nullptr)
                if (!path.empty())
                    cmp(n->value, path.top()->value) ? 
                        path.top()->left = n->right : path.top()->right = n->right;
                else
                    // deleting root
                    _root = n->right;

            if (n->right == nullptr)
                if (!path.empty())
                    cmp(n->value, path.top()->value) ? 
                        path.top()->left = n->left : path.top()->right = n->left;
                else
                    // deleting root
                    _root = n->left;
        }
        else
        {
            // deleting node with two children
            node_pointer prevNode = path.empty() ? nullptr : path.top();
            // finding nearest value
            Stack<node_pointer> subPath;
            auto minNode = n->right;
            for (; minNode->left;)
            {
                subPath.push(minNode);
                minNode = minNode->left;
            }
            
            if (!subPath.empty())
                subPath.top()->left == minNode ? 
                    subPath.top()->left = minNode->right : subPath.top()->right = minNode->right;
            else
                n->right = minNode->right;
                
            // moving nearest node to this place
            auto node = minNode;
            node->left = n->left;
            node->right = n->right;
            if (prevNode != nullptr)
                cmp(prevNode->value, node->value) ? 
                    prevNode->right = node : prevNode->left = node;
            else
                // deleting root
                _root = node;

            // update heights
            path.push(node);
            for (; !subPath.empty();)
            {
                subPath.top()->height = std::max(_getHeight(subPath.top()->left),
                    _getHeight(subPath.top()->right)) + 1;
                auto newSubRoot = _makeRotation(subPath);
                subPath.pop();
                if (!subPath.empty())
                    _cmp(subPath.top()->value, newSubRoot->value) ? 
                        subPath.top()->right = newSubRoot : subPath.top()->left = newSubRoot;
                else
                    _cmp(path.top()->value, newSubRoot->value) ? 
                        path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            }
        }
        // update heigts
        for (; !path.empty();)
        {
            path.top()->height = std::max(_getHeight(path.top()->left), 
                _getHeight(path.top()->right)) + 1;
            auto newSubRoot = _makeRotation(path);
            path.pop();
            if (!path.empty())
                _cmp(path.top()->value, newSubRoot->value) ? 
                    path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            else
                _root = newSubRoot;
        }
        _deleteNode(n);
        _size--;
    }
    void erase(const_iterator pos)
    {
        if (pos == end() || _root == nullptr)
            return;
        
        // buid the stack
        value_compare cmp;
        Stack<node_pointer> path = _getPathToNode(*pos);
        if (path.empty())
            return;

        node_pointer n = path.top();
        path.pop();
        // n->value == *pos, we delete this node
        if (n->left == nullptr || n->right == nullptr)
        {
            // deleting leaf or node with one child
            if (n->left == nullptr)
                if (!path.empty())
                    cmp(n->value, path.top()->value) ? 
                        path.top()->left = n->right : path.top()->right = n->right;
                else
                    // deleting root
                    _root = n->right;

            if (n->right == nullptr)
                if (!path.empty())
                    cmp(n->value, path.top()->value) ? 
                        path.top()->left = n->left : path.top()->right = n->left;
                else
                    // deleting root
                    _root = n->left;
        }
        else
        {
            // deleting node with two children
            node_pointer prevNode = path.empty() ? nullptr : path.top();
            // finding nearest value
            Stack<node_pointer> subPath;
            auto minNode = n->right;
            for (; minNode->left;)
            {
                subPath.push(minNode);
                minNode = minNode->left;
            }
            
            if (!subPath.empty())
                subPath.top()->left == minNode ? 
                    subPath.top()->left = minNode->right : subPath.top()->right = minNode->right;
            else
                n->right = minNode->right;
                
            // moving nearest node to this place
            auto node = minNode;
            node->left = n->left;
            node->right = n->right;
            if (prevNode != nullptr)
                cmp(prevNode->value, node->value) ? 
                    prevNode->right = node : prevNode->left = node;
            else
                // deleting root
                _root = node;

            // update heights
            path.push(node);
            for (; !subPath.empty();)
            {
                subPath.top()->height = std::max(_getHeight(subPath.top()->left),
                    _getHeight(subPath.top()->right)) + 1;
                auto newSubRoot = _makeRotation(subPath);
                subPath.pop();
                if (!subPath.empty())
                    _cmp(subPath.top()->value, newSubRoot->value) ? 
                        subPath.top()->right = newSubRoot : subPath.top()->left = newSubRoot;
                else
                    _cmp(path.top()->value, newSubRoot->value) ? 
                        path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            }
        }
        // update heigts
        for (; !path.empty();)
        {
            path.top()->height = std::max(_getHeight(path.top()->left), 
                _getHeight(path.top()->right)) + 1;
            auto newSubRoot = _makeRotation(path);
            path.pop();
            if (!path.empty())
                _cmp(path.top()->value, newSubRoot->value) ? 
                    path.top()->right = newSubRoot : path.top()->left = newSubRoot;
            else
                _root = newSubRoot;
        }
        _deleteNode(n);
        _size--;
    }

    iterator find(const Key& key)
    {
        for (auto i = _root;;)
        {
            if (i == nullptr)
                return end();
            else if (cmp(key, i->value))
                i = i->left;
            else if (cmp(i->value, key))
                i = i->right;
            else
                // i->value == value => this value already exists
                return iterator(i);
        }
        return end();
    }
    const_iterator find(const Key& key) const
    {
        for (auto i = _root;;)
        {
            if (i == nullptr)
                return cend();
            else if (cmp(key, i->value))
                i = i->left;
            else if (cmp(i->value, key))
                i = i->right;
            else
                // i->value == value => this value already exists
                return const_iterator(i);
        }
        return cend();
    }

    // node_pointer __getRoot()
    // {
    //     return _root;
    // }

private:
    size_t _getHeight(node_pointer x)
    {
        if (x == nullptr)
            return 0;

        return x->height;
    }
    int _getLeftRightDiff(node_pointer x)
    {
        if (x == nullptr)
            return 0;

        return _getHeight(x->left) - _getHeight(x->right);
    }

    Stack<node_pointer> _getPathToNode(const value_type& key)
    {
        value_compare cmp;
        Stack<node_pointer> path;
        for (auto i = _root;;)
        {
            path.push(i);
            if (i == nullptr)
                // did not find
                return path;
            else if (cmp(key, i->value))
                // go left
                i = i->left;
            else if (cmp(i->value, key))
                // go right
                i = i->right;
            else
                // found this key
                return path;
        }
    }
    Stack<node_pointer> _getPathToNode(iterator it)
    {
        return _getPathToNode(*it);
    }

    node_pointer _leftRotation(node_pointer a)
    {
        auto b = a->right;
        if (b == nullptr)
            throw std::logic_error("left rotation error");

        auto c = b->left;
        // rotation
        b->left = a;
        a->right = c;
        // change heights
        a->height = std::max(_getHeight(a->left), _getHeight(a->right)) + 1;
        b->height = std::max(_getHeight(b->left), _getHeight(b->right)) + 1;
        return b;
    }
    node_pointer _rightRotation(node_pointer a)
    {
        auto b = a->left;
        if (b == nullptr)
            throw std::logic_error("right rotation error");

        auto c = b->right;
        // rotation
        b->right = a;
        a->left = c;
        // change heights
        a->height = std::max(_getHeight(a->left), _getHeight(a->right)) + 1;
        b->height = std::max(_getHeight(b->left), _getHeight(b->right)) + 1;
        return b;
    }
    node_pointer _leftRightRotation(node_pointer a)
    {
        if (a->left == nullptr)
            throw std::logic_error("left right rotation error");

        a->left = _leftRotation(a->left);
        return _rightRotation(a);
    }
    node_pointer _rightLeftRotation(node_pointer a)
    {
        if (a->right == nullptr)
            throw std::logic_error("right left rotation error");

        a->right = _rightRotation(a->right);
        return _leftRotation(a);
    }

    node_pointer _makeRotation(Stack<node_pointer>& path)
    {
        auto root = path.top();
        int balance = _getLeftRightDiff(root);
        if (balance > 1 && _getLeftRightDiff(root->left) >= 0)
            return _rightRotation(root);
        else if (balance > 1 && _getLeftRightDiff(root->left) < 0)
            return _leftRightRotation(root);
        else if (balance < -1 && _getLeftRightDiff(root->right) <= 0)
            return _leftRotation(root);
        else if (balance < -1 && _getLeftRightDiff(root->right) > 0)
            return _rightLeftRotation(root);

        return root;
    }

    node_pointer _createNode(const value_type& value)
    {
        node_pointer nodeP = traits<NodeAllocator>::allocate(_nodeAlloc, 1);
        traits<NodeAllocator>::construct(_nodeAlloc, nodeP, value, nullptr, nullptr, 1);
        return nodeP;
    }
    node_pointer _createNode(value_type&& value)
    {
        node_pointer nodeP = traits<NodeAllocator>::allocate(_nodeAlloc, 1);
        traits<NodeAllocator>::construct(_nodeAlloc, nodeP, value, nullptr, nullptr, 1);
        return nodeP;
    }

    void _deleteNode(node_pointer nodeP)
    {
        traits<NodeAllocator>::destroy(_nodeAlloc, nodeP);
        traits<NodeAllocator>::deallocate(_nodeAlloc, nodeP, 1);
    }

private:
    node_pointer _root;
    size_t _size;
    Compare _cmp;
    Allocator _alloc;
    NodeAllocator _nodeAlloc;
};

#endif // AVL_TREE_HPP_INCLUEDED