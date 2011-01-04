/* 
typedef struct {
  struct {
    int a;
  } a;
} struct_struct_t;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommType;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommDecoder;

public class struct_struct_t implements LabCommType {

  public static class struct_a {
    public int a;
  }
  
  public struct_a a;
  
  public static void encode(LabCommEncoder e, struct_struct_t value) throws IOException {
    e.encodeInt(value.a.a);
  }
  
  public static struct_struct_t decode(LabCommDecoder d) throws IOException {
    struct_struct_t result;
    result = new struct_struct_t();
    result.a = new struct_a();
    result.a.a = d.decodeInt();
    return result;
  }
  
}
