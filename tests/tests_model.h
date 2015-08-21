/*
 *
 *  Created by Jean-Daniel Michaud
 *
 */

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <gmock/gmock.h>

#include "observable.hpp"
#include "model.h"

using ::testing::_;
using std::placeholders::_1;

class MockView {
 public:
  MOCK_METHOD1(notify, void(IObservable &observable));
};

class TestModel : public Model {
  DECLARE_ENTRY( TestModel, integer, int );
  DECLARE_ENTRY( TestModel, str, std::string );
  DECLARE_ENTRY( TestModel, vector, std::vector<int> );
};

class ModelTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( ModelTest );
  CPPUNIT_TEST( test_subscript_operator );
  CPPUNIT_TEST( test_set );
  CPPUNIT_TEST( test_set_notification );
  CPPUNIT_TEST( test_update );
  CPPUNIT_TEST( test_update_notification );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void test_subscript_operator()
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

  void test_set()
  {
    TestModel model;
    // Set some keys in the model
    model.set_integer(1);
    model.set_str(std::string("value"));
    // Test with subscript operator
    CPPUNIT_ASSERT_EQUAL ( 1, model.get_integer() );
    CPPUNIT_ASSERT_EQUAL ( std::string("value"), model.get_str() );
  }

  void test_set_notification()
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

  void test_update()
  {
    TestModel model;
    // Set a vector in the model
    model.set_vector({ 1, 2, 3 });
    // Change the value
    model.set_vector().update().push_back(4);
    // Test with subscript operator
    CPPUNIT_ASSERT_EQUAL ( 4, (int) model.get_vector().size() );
  }

  void test_update_notification()
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
