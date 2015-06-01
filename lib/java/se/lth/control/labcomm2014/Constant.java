package se.lth.control.labcomm2014;

public class Constant {

  public static final String CURRENT_VERSION = "LabComm2014.v1";

  /*
   * Allowed packet tags
   */
  /*
   * Predeclared aggregate type indices
   */
  public static final int VERSION          = 0x01;
  public static final int SAMPLE_DEF       = 0x02;
  public static final int SAMPLE_REF       = 0x03;
  public static final int TYPE_DEF         = 0x04;
  public static final int TYPE_BINDING     = 0x05;
  public static final int PRAGMA           = 0x3f;
  public static final int FIRST_USER_INDEX = 0x40; /* ..0xffffffff */

  /*
   * Predefined aggregate type indices
   */
  public static final int ARRAY            = 0x10;
  public static final int STRUCT           = 0x11;
  
  /*
   * Predeclared primitive type indices
   */
  public static final int BOOLEAN          = 0x20;
  public static final int BYTE             = 0x21;
  public static final int SHORT            = 0x22;
  public static final int INT              = 0x23;
  public static final int LONG             = 0x24;
  public static final int FLOAT            = 0x25;
  public static final int DOUBLE           = 0x26;
  public static final int STRING           = 0x27;
  public static final int SAMPLE           = 0x28;

}
