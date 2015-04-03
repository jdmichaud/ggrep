/*
 *  Copyright (c) General Electric Medical Systems Europe
 *
 *  Created by Jean-Daniel Michaud - 05-Mar-2014
 *
 */

#include <iostream>

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "XXX_tests.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( XXXTest );

int main(int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( XXXTest::suite()            );
  runner.run();
  return 0;
}