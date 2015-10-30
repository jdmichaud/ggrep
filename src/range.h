/*! \brief A minimal C++ range implementation
 *
 */

#ifndef __RANGE_H__
#define __RANGE_H__

/*
 * Represent a set of unique ever increasing integers
 */
class range {
private:
    int m_last;
    int m_iter;

public:
    range(int end):
        m_last(end),
        m_iter(0)
    {}
    range(int begin, int end):
        m_last(end),
        m_iter(begin)
    {}

    // Iterable functions
    const range& begin() const { return *this; }
    const range& end() const { return *this; }

    // Iterator functions
    bool operator!=(const range&) const { return m_iter < m_last; }
    void operator++() { ++m_iter; }
    int operator*() const { return m_iter; }
};

/*
inline range &begin(range &r) { return r.begin(); }
inline range &end(range &r) { return r.end(); }
*/
#endif // __RANGE_H__