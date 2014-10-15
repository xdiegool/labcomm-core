namespace se.lth.control.labcomm {

  public class Constant {

    public const string CURRENT_VERSION = "LabComm20141009";

    /*
     * Allowed packet tags
     */
    public const int VERSION          = 0x01;
    public const int SAMPLE           = 0x02;
    public const int PRAGMA           = 0x3f;
    public const int FIRST_USER_INDEX = 0x40; /* ..0xffffffff */

    /*
     * Predefined aggregate type indices
     */
    public const int ARRAY            = 0x10;
    public const int STRUCT           = 0x11;
    
    /*
     * Predeclared primitive type indices
     */
    public const int BOOLEAN          = 0x20;
    public const int BYTE             = 0x21;
    public const int SHORT            = 0x22;
    public const int INT              = 0x23;
    public const int LONG             = 0x24;
    public const int FLOAT            = 0x25;
    public const int DOUBLE           = 0x26;
    public const int STRING           = 0x27;
    
  }

}
