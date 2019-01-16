//
// Created by vlad on 16.01.19.
//

#ifndef BKTREE_LESS_INSERT_ITERATOR_H
#define BKTREE_LESS_INSERT_ITERATOR_H

template <typename _Container>
class less_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void> {
   protected:
    _Container* container;
    typename _Container::iterator iter;

   public:
    /// A nested typedef for the type of whatever container you used.
    typedef _Container container_type;

    /**
     *  The only way to create this %iterator is with a container and an
     *  initial position (a normal %iterator into the container).
     */
    insert_iterator(_Container& __x, typename _Container::iterator __i)
        : container(&__x), iter(__i) {}

    insert_iterator& operator=(const typename _Container::value_type& __value) {
        iter = container->insert(iter, __value);
        ++iter;
        return *this;
    }

    insert_iterator& operator=(typename _Container::value_type&& __value) {
        iter = container->insert(iter, std::move(__value));
        ++iter;
        return *this;
    }

    /// Simply returns *this.
    insert_iterator& operator*() { return *this; }

    /// Simply returns *this.  (This %iterator does not @a move.)
    insert_iterator& operator++() { return *this; }

    /// Simply returns *this.  (This %iterator does not @a move.)
    insert_iterator& operator++(int) { return *this; }
};
#endif  // BKTREE_LESS_INSERT_ITERATOR_H
