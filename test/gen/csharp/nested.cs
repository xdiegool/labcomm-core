using System;
using se.lth.control.labcomm;
/* 
sample struct {
  int aa;
  boolean bb;
  int_array_ss ias;
} struct_array_ss[1][_][_];
*/

public class struct_array_ss : LabCommSample {

  public int aa;
  public boolean bb;
  public int[][,,][] ias;
  
  public interface Handler : LabCommHandler {
    void handle(struct_array_ss[][][] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_array_ss);
    }
    
    public String getName() {
      return "struct_array_ss";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_array_ss.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_array_ss[][][] value) {
    e.begin(typeof(struct_array_ss));
    int i_0_max = 1;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].GetLength(0));
      int i_1_max = value[i_0].GetLength(0);
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        e.encodeInt(value[i_0][i_1].GetLength(0));
        int i_2_max = value[i_0][i_1].GetLength(0);
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0][i_1][i_2].aa);
          e.encodeBoolean(value[i_0][i_1][i_2].bb);
          int i_3_max = 1;
          for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
            e.encodeInt(value[i_0][i_1][i_2].ias[i_3].GetLength(0));
            int i_4_max = value[i_0][i_1][i_2].ias[i_3].GetLength(0);
            int i_5_max = 3;
            e.encodeInt(value[i_0][i_1][i_2].ias[i_3].GetLength(2));
            int i_6_max = value[i_0][i_1][i_2].ias[i_3].GetLength(2);
            for (int i_4 = 0 ; i_4 < i_4_max ; i_4++) {
              for (int i_5 = 0 ; i_5 < i_5_max ; i_5++) {
                for (int i_6 = 0 ; i_6 < i_6_max ; i_6++) {
                  int i_7_max = 5;
                  for (int i_7 = 0 ; i_7 < i_7_max ; i_7++) {
                    e.encodeInt(value[i_0][i_1][i_2].ias[i_3][i_4, i_5, i_6][i_7]);
                  }
                }
              }
            }
          }
        }
      }
    }
    e.end(typeof(struct_array_ss));
  }
  
  public static struct_array_ss[][][] decode(LabCommDecoder d) {
    struct_array_ss[][][] result;
    {
      int i_0_max = 1;
      result = new struct_array_ss[i_0_max]
      [][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = d.decodeInt();
          result[i_0] = new struct_array_ss[i_1_max]
          [];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            {
              int i_2_max = d.decodeInt();
              result[i_0][i_1] = new struct_array_ss[i_2_max]
              ;
              for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
                result[i_0][i_1][i_2] = new struct_array_ss();
                result[i_0][i_1][i_2].aa = d.decodeInt();
                result[i_0][i_1][i_2].bb = d.decodeBoolean();
                {
                  int i_3_max = 1;
                  result[i_0][i_1][i_2].ias = new int[i_3_max]
                  [][,,];
                  for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
                    {
                      int i_4_max = d.decodeInt();
                      int i_5_max = 3;
                      int i_6_max = d.decodeInt();
                      result[i_0][i_1][i_2].ias[i_3] = new int[i_4_max, i_5_max, i_6_max]
                      [];
                      for (int i_4 = 0 ; i_4 < i_4_max ; i_4++) {
                        for (int i_5 = 0 ; i_5 < i_5_max ; i_5++) {
                          for (int i_6 = 0 ; i_6 < i_6_max ; i_6++) {
                            {
                              int i_7_max = 5;
                              result[i_0][i_1][i_2].ias[i_3][i_4, i_5, i_6] = new int[i_7_max]
                              ;
                              for (int i_7 = 0 ; i_7 < i_7_max ; i_7++) {
                                result[i_0][i_1][i_2].ias[i_3][i_4, i_5, i_6][i_7] = d.decodeInt();
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [1]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 1, 
      // array [_]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        // array [_]
        0, 0, 0, 16, 
          0, 0, 0, 1, 
          0, 0, 0, 0, 
          // struct { 3 fields
          0, 0, 0, 17, 
            0, 0, 0, 3, 
            // int 'aa'
            0, 0, 0, 2, 
            97, 97, 
            0, 0, 0, 35, 
            // boolean 'bb'
            0, 0, 0, 2, 
            98, 98, 
            0, 0, 0, 32, 
            // int_array_ss 'ias'
            0, 0, 0, 3, 
            105, 97, 115, 
            // array [1]
            0, 0, 0, 16, 
              0, 0, 0, 1, 
              0, 0, 0, 1, 
              // array [_, 3, _]
              0, 0, 0, 16, 
                0, 0, 0, 3, 
                0, 0, 0, 0, 
                0, 0, 0, 3, 
                0, 0, 0, 0, 
                // array [5]
                0, 0, 0, 16, 
                  0, 0, 0, 1, 
                  0, 0, 0, 5, 
                  0, 0, 0, 35, 
                // }
              // }
            // }
          // }
        // }
      // }
    // }
  };

}
/* 
typedef struct {
  int a;
} struct_t;
*/

public class struct_t : LabCommType {

  public int a;
  
  public static void encode(LabCommEncoder e, struct_t value) {
    e.encodeInt(value.a);
  }
  
  public static struct_t decode(LabCommDecoder d) {
    struct_t result;
    result = new struct_t();
    result.a = d.decodeInt();
    return result;
  }
  
}
/* 
typedef struct {
  int a;
} struct_array_t[2];
*/

public class struct_array_t : LabCommType {

  public int a;
  
  public static void encode(LabCommEncoder e, struct_array_t[] value) {
    int i_0_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].a);
    }
  }
  
  public static struct_array_t[] decode(LabCommDecoder d) {
    struct_array_t[] result;
    {
      int i_0_max = 2;
      result = new struct_array_t[i_0_max]
      ;
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = new struct_array_t();
        result[i_0].a = d.decodeInt();
      }
    }
    return result;
  }
  
}
/* 
typedef struct {
  struct {
    int a;
  } a;
} struct_struct_t;
*/

public class struct_struct_t : LabCommType {

  public static class struct_a {
    public int a;
  }
  
  public struct_a a;
  
  public static void encode(LabCommEncoder e, struct_struct_t value) {
    e.encodeInt(value.a.a);
  }
  
  public static struct_struct_t decode(LabCommDecoder d) {
    struct_struct_t result;
    result = new struct_struct_t();
    result.a = new struct_a();
    result.a.a = d.decodeInt();
    return result;
  }
  
}
/* 
sample int int_s;
*/

public class int_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(int_s);
    }
    
    public String getName() {
      return "int_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(int_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int value) {
    e.begin(typeof(int_s));
    e.encodeInt(value);
    e.end(typeof(int_s));
  }
  
  public static int decode(LabCommDecoder d) {
    int result;
    result = d.decodeInt();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    0, 0, 0, 35, 
  };

}
/* 
sample int int_array_s[1][_, 3, _][5];
*/

public class int_array_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[][,,][] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(int_array_s);
    }
    
    public String getName() {
      return "int_array_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(int_array_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[][,,][] value) {
    e.begin(typeof(int_array_s));
    int i_0_max = 1;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].GetLength(0));
      int i_1_max = value[i_0].GetLength(0);
      int i_2_max = 3;
      e.encodeInt(value[i_0].GetLength(2));
      int i_3_max = value[i_0].GetLength(2);
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
            int i_4_max = 5;
            for (int i_4 = 0 ; i_4 < i_4_max ; i_4++) {
              e.encodeInt(value[i_0][i_1, i_2, i_3][i_4]);
            }
          }
        }
      }
    }
    e.end(typeof(int_array_s));
  }
  
  public static int[][,,][] decode(LabCommDecoder d) {
    int[][,,][] result;
    {
      int i_0_max = 1;
      result = new int[i_0_max]
      [][,,];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = d.decodeInt();
          int i_2_max = 3;
          int i_3_max = d.decodeInt();
          result[i_0] = new int[i_1_max, i_2_max, i_3_max]
          [];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
              for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
                {
                  int i_4_max = 5;
                  result[i_0][i_1, i_2, i_3] = new int[i_4_max]
                  ;
                  for (int i_4 = 0 ; i_4 < i_4_max ; i_4++) {
                    result[i_0][i_1, i_2, i_3][i_4] = d.decodeInt();
                  }
                }
              }
            }
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [1]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 1, 
      // array [_, 3, _]
      0, 0, 0, 16, 
        0, 0, 0, 3, 
        0, 0, 0, 0, 
        0, 0, 0, 3, 
        0, 0, 0, 0, 
        // array [5]
        0, 0, 0, 16, 
          0, 0, 0, 1, 
          0, 0, 0, 5, 
          0, 0, 0, 35, 
        // }
      // }
    // }
  };

}
/* 
sample struct {
  int a;
  double bcd;
} struct_s;
*/

public class struct_s : LabCommSample {

  public int a;
  public double bcd;
  
  public interface Handler : LabCommHandler {
    void handle(struct_s value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_s);
    }
    
    public String getName() {
      return "struct_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_s value) {
    e.begin(typeof(struct_s));
    e.encodeInt(value.a);
    e.encodeDouble(value.bcd);
    e.end(typeof(struct_s));
  }
  
  public static struct_s decode(LabCommDecoder d) {
    struct_s result;
    result = new struct_s();
    result.a = d.decodeInt();
    result.bcd = d.decodeDouble();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 2 fields
    0, 0, 0, 17, 
      0, 0, 0, 2, 
      // int 'a'
      0, 0, 0, 1, 
      97, 
      0, 0, 0, 35, 
      // double 'bcd'
      0, 0, 0, 3, 
      98, 99, 100, 
      0, 0, 0, 38, 
    // }
  };

}
/* 
sample struct {
  int a;
} struct_array_s[2];
*/

public class struct_array_s : LabCommSample {

  public int a;
  
  public interface Handler : LabCommHandler {
    void handle(struct_array_s[] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_array_s);
    }
    
    public String getName() {
      return "struct_array_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_array_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_array_s[] value) {
    e.begin(typeof(struct_array_s));
    int i_0_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].a);
    }
    e.end(typeof(struct_array_s));
  }
  
  public static struct_array_s[] decode(LabCommDecoder d) {
    struct_array_s[] result;
    {
      int i_0_max = 2;
      result = new struct_array_s[i_0_max]
      ;
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = new struct_array_s();
        result[i_0].a = d.decodeInt();
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 2, 
      // struct { 1 fields
      0, 0, 0, 17, 
        0, 0, 0, 1, 
        // int 'a'
        0, 0, 0, 1, 
        97, 
        0, 0, 0, 35, 
      // }
    // }
  };

}
/* 
sample struct {
  struct {
    int a;
  } a;
} struct_struct_s;
*/

public class struct_struct_s : LabCommSample {

  public static class struct_a {
    public int a;
  }
  
  public struct_a a;
  
  public interface Handler : LabCommHandler {
    void handle(struct_struct_s value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_struct_s);
    }
    
    public String getName() {
      return "struct_struct_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_struct_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_struct_s value) {
    e.begin(typeof(struct_struct_s));
    e.encodeInt(value.a.a);
    e.end(typeof(struct_struct_s));
  }
  
  public static struct_struct_s decode(LabCommDecoder d) {
    struct_struct_s result;
    result = new struct_struct_s();
    result.a = new struct_a();
    result.a.a = d.decodeInt();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 1 fields
    0, 0, 0, 17, 
      0, 0, 0, 1, 
      // struct 'a'
      0, 0, 0, 1, 
      97, 
      // struct { 1 fields
      0, 0, 0, 17, 
        0, 0, 0, 1, 
        // int 'a'
        0, 0, 0, 1, 
        97, 
        0, 0, 0, 35, 
      // }
    // }
  };

}
/* 
sample int_t int_t_s;
*/

public class int_t_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(int_t_s);
    }
    
    public String getName() {
      return "int_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(int_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int value) {
    e.begin(typeof(int_t_s));
    e.encodeInt(value);
    e.end(typeof(int_t_s));
  }
  
  public static int decode(LabCommDecoder d) {
    int result;
    result = d.decodeInt();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    0, 0, 0, 35, 
  };

}
/* 
sample int_array_t int_array_t_s;
*/

public class int_array_t_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[,][][] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(int_array_t_s);
    }
    
    public String getName() {
      return "int_array_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(int_array_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[,][][] value) {
    e.begin(typeof(int_array_t_s));
    int i_0_max = 1;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      int i_1_max = 2;
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        int i_2_max = 3;
        int i_3_max = 4;
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
            e.encodeInt(value[i_0][i_1][i_2, i_3]);
          }
        }
      }
    }
    e.end(typeof(int_array_t_s));
  }
  
  public static int[,][][] decode(LabCommDecoder d) {
    int[,][][] result;
    {
      int i_0_max = 1;
      result = new int[i_0_max]
      [,][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = 2;
          result[i_0] = new int[i_1_max]
          [,];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            {
              int i_2_max = 3;
              int i_3_max = 4;
              result[i_0][i_1] = new int[i_2_max, i_3_max]
              ;
              for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
                for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
                  result[i_0][i_1][i_2, i_3] = d.decodeInt();
                }
              }
            }
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [1]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 1, 
      // array [2]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 2, 
        // array [3, 4]
        0, 0, 0, 16, 
          0, 0, 0, 2, 
          0, 0, 0, 3, 
          0, 0, 0, 4, 
          0, 0, 0, 35, 
        // }
      // }
    // }
  };

}
/* 
sample struct_t struct_t_s;
*/

public class struct_t_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(struct_t value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_t_s);
    }
    
    public String getName() {
      return "struct_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_t value) {
    e.begin(typeof(struct_t_s));
    struct_t.encode(e, value);
    e.end(typeof(struct_t_s));
  }
  
  public static struct_t decode(LabCommDecoder d) {
    struct_t result;
    result = struct_t.decode(d);
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 1 fields
    0, 0, 0, 17, 
      0, 0, 0, 1, 
      // int 'a'
      0, 0, 0, 1, 
      97, 
      0, 0, 0, 35, 
    // }
  };

}
/* 
sample struct_array_t struct_array_t_s;
*/

public class struct_array_t_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(struct_array_t[] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_array_t_s);
    }
    
    public String getName() {
      return "struct_array_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_array_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_array_t[] value) {
    e.begin(typeof(struct_array_t_s));
    struct_array_t.encode(e, value);
    e.end(typeof(struct_array_t_s));
  }
  
  public static struct_array_t[] decode(LabCommDecoder d) {
    struct_array_t[] result;
    result = struct_array_t.decode(d);
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 2, 
      // struct { 1 fields
      0, 0, 0, 17, 
        0, 0, 0, 1, 
        // int 'a'
        0, 0, 0, 1, 
        97, 
        0, 0, 0, 35, 
      // }
    // }
  };

}
/* 
sample struct_struct_t struct_struct_t_s;
*/

public class struct_struct_t_s : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(struct_struct_t value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(struct_struct_t_s);
    }
    
    public String getName() {
      return "struct_struct_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(struct_struct_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_struct_t value) {
    e.begin(typeof(struct_struct_t_s));
    struct_struct_t.encode(e, value);
    e.end(typeof(struct_struct_t_s));
  }
  
  public static struct_struct_t decode(LabCommDecoder d) {
    struct_struct_t result;
    result = struct_struct_t.decode(d);
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 1 fields
    0, 0, 0, 17, 
      0, 0, 0, 1, 
      // struct 'a'
      0, 0, 0, 1, 
      97, 
      // struct { 1 fields
      0, 0, 0, 17, 
        0, 0, 0, 1, 
        // int 'a'
        0, 0, 0, 1, 
        97, 
        0, 0, 0, 35, 
      // }
    // }
  };

}
