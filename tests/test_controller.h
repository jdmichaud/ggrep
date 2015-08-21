/*
 *
 *  Created by Jean-Daniel Michaud
 *
 */

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <gmock/gmock.h>

#include "controller.h"

using ::testing::_;
using std::placeholders::_1;

class MockStream {
public:
  MockStream(std::string filepath, ios_base::openmode);
  MOCK_METHOD0(is_open, bool ());
  MOCK_METHOD0(rdbuf, std::filebuf *());
  MOCK_METHOD0(tellg, std::streampos ());
  MOCK_METHOD0(seekg, void ());
  MOCK_METHOD0(gcount, int ());
  MOCK_METHOD0(get, char ());
  static constexpr seekdir beg = 0;
  static constexpr openmode in = 0;
};

class ControllerTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( ControllerTest );
  CPPUNIT_TEST( test_create_buffer );
  CPPUNIT_TEST( get_nth_buffer );
  CPPUNIT_TEST( test_get_active_buffer );
  CPPUNIT_TEST( activate_buffer );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void test_create_buffer()
  {
    TestModel model;
    MockView view;
    // Set some keys in the model
    model.set_integer(1);
    model.set_str("value");
    // Check observer was notified
    EXPECT_CALL(view, notify(_)).Times(0);
    // register the observer
    model.register_observer(std::bind( &MockView::notify, &view, _1 ));
    // Should not trigger a notification
    std::string value = model.get_str();
  }

  void get_nth_buffer()
  {
    TestModel model;
    // Set some keys in the model
    model.set_integer(1);
    model.set_str(std::string("value"));
    // Test with subscript operator
    CPPUNIT_ASSERT_EQUAL ( 1, model.get_integer() );
    CPPUNIT_ASSERT_EQUAL ( std::string("value"), model.get_str() );
  }

  void test_get_active_buffer()
  {
    TestModel model;
    MockView view;
    // Check observer was notified
    EXPECT_CALL(view, notify(_)).Times(2);
    // register the observer
    model.register_observer(std::bind( &MockView::notify, &view, _1 ));
    // Set some keys in the model
    model.set_integer(1);
    model.set_str("value");
  }

  void activate_buffer()
  {
    TestModel model;
    MockView view;
    // Set some keys in the model
    model.set_vector({ 1, 2, 3 });
    // Check observer was notified
    EXPECT_CALL(view, notify(_)).Times(1);
    // register the observer
    model.register_observer(std::bind( &MockView::notify, &view, _1 ));
    // Update the keys
    model.set_vector().update().push_back(4);
  }
};
