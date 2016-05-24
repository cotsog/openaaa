/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.opensec.aaa;

public final class Bind {
  public final static Bind BIND_OBJECT_ID = new Bind("BIND_OBJECT_ID", 1);
  public final static Bind BIND_SESSION_ID = new Bind("BIND_SESSION_ID", 2);
  public final static Bind BIND_KEYING_MATERIAL = new Bind("BIND_KEYING_MATERIAL", 3);
  public final static Bind BIND_SECRET = new Bind("BIND_SECRET", 4);

  public final int swigValue() {
    return swigValue;
  }

  public String toString() {
    return swigName;
  }

  public static Bind swigToEnum(int swigValue) {
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (int i = 0; i < swigValues.length; i++)
      if (swigValues[i].swigValue == swigValue)
        return swigValues[i];
    throw new IllegalArgumentException("No enum " + Bind.class + " with value " + swigValue);
  }

  private Bind(String swigName) {
    this.swigName = swigName;
    this.swigValue = swigNext++;
  }

  private Bind(String swigName, int swigValue) {
    this.swigName = swigName;
    this.swigValue = swigValue;
    swigNext = swigValue+1;
  }

  private Bind(String swigName, Bind swigEnum) {
    this.swigName = swigName;
    this.swigValue = swigEnum.swigValue;
    swigNext = this.swigValue+1;
  }

  private static Bind[] swigValues = { BIND_OBJECT_ID, BIND_SESSION_ID, BIND_KEYING_MATERIAL, BIND_SECRET };
  private static int swigNext = 0;
  private final int swigValue;
  private final String swigName;
}

