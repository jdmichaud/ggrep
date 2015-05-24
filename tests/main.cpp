/*
 *  Copyright (c) General Electric Medical Systems Europe
 *
 *  Created by Jean-Daniel Michaud - 05-Mar-2014
 *
 */

#include <iostream>

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "tests_model.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ModelTest );

int main(int argc, char **argv)
{
  // Init CppUnit
  CppUnit::TextUi::TestRunner runner;
  // Init Google Mock
  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);
  runner.addTest( ModelTest::suite()            );
  runner.run();
  return 0;
}