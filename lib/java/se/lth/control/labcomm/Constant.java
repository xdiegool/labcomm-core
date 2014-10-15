package se.lth.control.labcomm;

public class Constant {

  public static final String CURRENT_VERSION = "LabComm20141009";

  /*
   * Allowed packet tags
   */
  /*
   * Predeclared aggregate type indices
   */
  public static final int VERSION          = 0x01;
  public static final int SAMPLE           = 0x02;
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

}
