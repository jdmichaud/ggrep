/*
 *
 *  Created by Jean-Daniel Michaud
 *
 */

#include <curses.h>

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <gmock/gmock.h>

#include "input.h"

class InputTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( InputTest );
  CPPUNIT_TEST( test_constructor );
  CPPUNIT_TEST( test_printable_is_a );
  CPPUNIT_TEST( test_arrow_is_a );
  CPPUNIT_TEST( test_equal_operator );
  CPPUNIT_TEST( test_isgeneric );

  CPPUNIT_TEST( test_factory );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void test_constructor()
  {
    // Test default constructor leads to KLEENE
    Input i;
    CPPUNIT_ASSERT_EQUAL ( (uint) KLEENE, i.get_keycode() );
    // Test construcor with keycode
    Printable p(45);
    CPPUNIT_ASSERT_EQUAL ( (uint) 45, p.get_keycode() );
  }

  void test_printable_is_a()
  {
    Printable p;
    // Test Printable::is_a
    CPPUNIT_ASSERT_EQUAL ( false, p.is_a(31)  );
    CPPUNIT_ASSERT_EQUAL ( true,  p.is_a(32)  );
    CPPUNIT_ASSERT_EQUAL ( true,  p.is_a(64)  );
    CPPUNIT_ASSERT_EQUAL ( true,  p.is_a(126) );
    CPPUNIT_ASSERT_EQUAL ( false, p.is_a(127) );
  }

  void test_arrow_is_a()
  {
    Arrow a;
    // Test Arrow::is_a
    CPPUNIT_ASSERT_EQUAL ( false, a.is_a(31)  );
    CPPUNIT_ASSERT_EQUAL ( true, a.is_a(01040)  );
    CPPUNIT_ASSERT_EQUAL ( true, a.is_a(KEY_DOWN)  );
    CPPUNIT_ASSERT_EQUAL ( true, a.is_a(KEY_LEFT)  );
    CPPUNIT_ASSERT_EQUAL ( false, a.is_a(0123)  );
  }

  void test_equal_operator()
  {
    Input a(012);
    Input b(012);
    Input c(0564);
    // Test equal operator
    CPPUNIT_ASSERT_EQUAL ( true, (a == b) );
    CPPUNIT_ASSERT_EQUAL ( false, (a == c) );
    CPPUNIT_ASSERT_EQUAL ( true, (a == 012) );
    CPPUNIT_ASSERT_EQUAL ( false, (a == 0547) );
  }

  void test_isgeneric()
  {
    Input a;
    Input b(KLEENE);
    Input c(0564);
    // Test equal operator
    CPPUNIT_ASSERT_EQUAL ( true, a.isgeneric() );
    CPPUNIT_ASSERT_EQUAL ( true, b.isgeneric() );
    CPPUNIT_ASSERT_EQUAL ( false, c.isgeneric() );
  }

  void test_factory() {
    /* Don't know how to test for types
    Input i = InputFactory::build_input(45);
    CPPUNIT_ASSERT_EQUAL ( (uint) 45, i.get_keycode() );
    CPPUNIT_ASSERT_NO_THROW ( dynamic_cast<Arrow *>(&i) );
    */
  }

};
