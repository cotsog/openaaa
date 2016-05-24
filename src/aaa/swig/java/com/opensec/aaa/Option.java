/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.opensec.aaa;

public final class Option {
  public final static Option OPT_USERDATA = new Option("OPT_USERDATA", 1);
  public final static Option OPT_CUSTOMLOG = new Option("OPT_CUSTOMLOG", 2);

  public final int swigValue() {
    return swigValue;
  }

  public String toString() {
    return swigName;
  }

  public static Option swigToEnum(int swigValue) {
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (int i = 0; i < swigValues.length; i++)
      if (swigValues[i].swigValue == swigValue)
        return swigValues[i];
    throw new IllegalArgumentException("No enum " + Option.class + " with value " + swigValue);
  }

  private Option(String swigName) {
    this.swigName = swigName;
    this.swigValue = swigNext++;
  }

  private Option(String swigName, int swigValue) {
    this.swigName = swigName;
    this.swigValue = swigValue;
    swigNext = swigValue+1;
  }

  private Option(String swigName, Option swigEnum) {
    this.swigName = swigName;
    this.swigValue = swigEnum.swigValue;
    swigNext = this.swigValue+1;
  }

  private static Option[] swigValues = { OPT_USERDATA, OPT_CUSTOMLOG };
  private static int swigNext = 0;
  private final int swigValue;
  private final String swigName;
}

