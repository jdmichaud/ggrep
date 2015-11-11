/*! \brief Define type and class to manage text attributes
 *
 * Text attributes are used display special colors or text effect on certain
 * pieces of text
 */

#ifndef __ATTRS_H__
#define __ATTRS_H__

/*
 * This structure describes the text attributes to apply to a certain contiguous
 * text zone
 */
struct tattr_t {
  int attrs_mask;
  uint start_pos;
  uint end_pos;

  tattr_t() : attrs_mask(0), start_pos(0), end_pos(0) {}
  tattr_t(int am, uint sp, uint ep) : attrs_mask(am), start_pos(sp), end_pos(ep) {}
};

/*
 * An array of attrs
 */
typedef tattr_t *attr_list_t;

/*
 * A class implementing the basic attributes APIs commonly used
 */
class AttributeHolder {
public:
  AttributeHolder(uint number_of_attributes) : 
    m_number_of_attributes(number_of_attributes) 
  {
    // Initialize attrs array
    m_attrs = new tattr_t[number_of_attributes + 1];
    (*this).clear_attrs();
  }
  ~AttributeHolder() {
    if (m_attrs != nullptr) 
      delete[] m_attrs;
  }
  /** Clear the attributes */
  void clear_attrs() {
    memset(m_attrs, 0, (m_number_of_attributes + 1) * sizeof (tattr_t));
  }
  /** Return attributes for direct read by the views */
  attr_list_t get_attrs() {
    return m_attrs;
  }
  /** Add an attributes line n */
  void add_attr(uint line_index, uint start_pos, uint end_pos, int mask) {
    if (line_index >= m_number_of_attributes) {
      LOGERR("ERROR: trying to set attribute for line " << line_index 
              << " whereas there are only " << m_number_of_attributes
              << " lines in the buffer")
    } else {
      // Add the matching information as attributes
      m_attrs[line_index].start_pos = start_pos;
      m_attrs[line_index].end_pos = end_pos;
      // Set the attribute at the end as it is the key to determine if an attribute
      // is set and we don't want the view thread to consider the attribute set when
      // the position are not yet set
      m_attrs[line_index].attrs_mask = mask;
    }
  }

protected:
  // Attributes resulting of the filtering
  attr_list_t m_attrs;
  // Number of attributes to hold
  uint m_number_of_attributes;
};

typedef AttributeHolder *pAttributeHolder;

#endif //__ATTRS_H__