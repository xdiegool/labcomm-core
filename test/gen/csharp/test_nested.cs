using System;
using System.IO;
using se.lth.control.labcomm;

class x :
struct_array_ss.Handler,
int_s.Handler,
int_array_s.Handler,
struct_s.Handler,
struct_array_s.Handler,
struct_struct_s.Handler,
int_t_s.Handler,
int_array_t_s.Handler,
struct_t_s.Handler,
struct_array_t_s.Handler,
struct_struct_t_s.Handler
{

  LabCommEncoderChannel encoder;

  void struct_array_ss.Handler.handle(struct_array_ss[][][] data) {
    Console.Error.WriteLine("struct_array_ss");
    struct_array_ss.encode(encoder, data);
  }

  void int_s.Handler.handle(int data) {
    Console.Error.WriteLine("int_s");
    int_s.encode(encoder, data);
  }

  void int_array_s.Handler.handle(int[][,,][] data) {
    Console.Error.WriteLine("int_array_s");
    int_array_s.encode(encoder, data);
  }

  void struct_s.Handler.handle(struct_s data) {
    Console.Error.WriteLine("struct_s");
    struct_s.encode(encoder, data);
  }

  void struct_array_s.Handler.handle(struct_array_s[] data) {
    Console.Error.WriteLine("struct_array_s");
    struct_array_s.encode(encoder, data);
  }

  void struct_struct_s.Handler.handle(struct_struct_s data) {
    Console.Error.WriteLine("struct_struct_s");
    struct_struct_s.encode(encoder, data);
  }

  void int_t_s.Handler.handle(int data) {
    Console.Error.WriteLine("int_t_s");
    int_t_s.encode(encoder, data);
  }

  void int_array_t_s.Handler.handle(int[,][][] data) {
    Console.Error.WriteLine("int_array_t_s");
    int_array_t_s.encode(encoder, data);
  }

  void struct_t_s.Handler.handle(struct_t data) {
    Console.Error.WriteLine("struct_t_s");
    struct_t_s.encode(encoder, data);
  }

  void struct_array_t_s.Handler.handle(struct_array_t[] data) {
    Console.Error.WriteLine("struct_array_t_s");
    struct_array_t_s.encode(encoder, data);
  }

  void struct_struct_t_s.Handler.handle(struct_struct_t data) {
    Console.Error.WriteLine("struct_struct_t_s");
    struct_struct_t_s.encode(encoder, data);
  }

  public x(String InName, String OutName) {
    FileStream InFile = new FileStream(InName,
                                       FileMode.Open, 
                                       FileAccess.Read);
    LabCommDecoderChannel d = new LabCommDecoderChannel(InFile);
    FileStream OutFile = new FileStream(OutName, 
                                        FileMode.OpenOrCreate, 
                                        FileAccess.Write);
    encoder = new LabCommEncoderChannel(OutFile);

    struct_array_ss.register(d, this);
    int_s.register(d, this);
    int_array_s.register(d, this);
    struct_s.register(d, this);
    struct_array_s.register(d, this);
    struct_struct_s.register(d, this);
    int_t_s.register(d, this);
    int_array_t_s.register(d, this);
    struct_t_s.register(d, this);
    struct_array_t_s.register(d, this);
    struct_struct_t_s.register(d, this);

    struct_array_ss.register(encoder);
    int_s.register(encoder);
    int_array_s.register(encoder);
    struct_s.register(encoder);
    struct_array_s.register(encoder);
    struct_struct_s.register(encoder);
    int_t_s.register(encoder);
    int_array_t_s.register(encoder);
    struct_t_s.register(encoder);
    struct_array_t_s.register(encoder);
    struct_struct_t_s.register(encoder);

    try {
      d.run();
    } catch (EndOfStreamException) {
    }

  }

  static void Main(String[] arg) {
    new x(arg[0], arg[1]);
  }

}
