/* 
typedef struct {
  int a;
} struct_array_t[2];
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommType;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommDecoder;

public class struct_array_t implements LabCommType {

  public int a;
  
  public static void encode(LabCommEncoder e, struct_array_t[] value) throws IOException {
    int i_0_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].a);
    }
  }
  
  public static struct_array_t[] decode(LabCommDecoder d) throws IOException {
    struct_array_t[] result;
    {
      int i_0_max = 2;
      result = new struct_array_t[i_0_max];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = new struct_array_t();
        result[i_0].a = d.decodeInt();
      }
    }
    return result;
  }
  
}
