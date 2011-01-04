/* 
typedef struct {
  int a;
} struct_t;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommType;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommDecoder;

public class struct_t implements LabCommType {

  public int a;
  
  public static void encode(LabCommEncoder e, struct_t value) throws IOException {
    e.encodeInt(value.a);
  }
  
  public static struct_t decode(LabCommDecoder d) throws IOException {
    struct_t result;
    result = new struct_t();
    result.a = d.decodeInt();
    return result;
  }
  
}
