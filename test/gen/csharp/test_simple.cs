using System;
using System.IO;
using se.lth.control.labcomm;

class x :
an_int.Handler,
a_fixed_int_array.Handler,
a_fixed_int_multi_array.Handler,
a_fixed_int_array_array_array.Handler,
a_variable_int_array.Handler,
a_variable_int_multi_array.Handler,
a_variable_int_array_array_array.Handler,
an_int_struct.Handler,
a_void.Handler
{

  LabCommEncoderChannel encoder;

  void an_int.Handler.handle(int data) {
    Console.Error.WriteLine("an_int");
    an_int.encode(encoder, data);
  }

  void a_fixed_int_array.Handler.handle(int[] data) {
    Console.Error.WriteLine("a_fixed_int_array");
    a_fixed_int_array.encode(encoder, data);
  }

  void a_fixed_int_multi_array.Handler.handle(int[,,] data) {
    Console.Error.WriteLine("a_fixed_int_multi_array");
    a_fixed_int_multi_array.encode(encoder, data);
  }

  void a_fixed_int_array_array_array.Handler.handle(int[][][] data) {
    Console.Error.WriteLine("a_fixed_int_array_array_array");
    a_fixed_int_array_array_array.encode(encoder, data);
  }

  void a_variable_int_array.Handler.handle(int[] data) {
    Console.Error.WriteLine("a_variable_int_array");
    a_variable_int_array.encode(encoder, data);
  }

  void a_variable_int_multi_array.Handler.handle(int[,,] data) {
    Console.Error.WriteLine("a_variable_int_multi_array");
    a_variable_int_multi_array.encode(encoder, data);
  }

  void a_variable_int_array_array_array.Handler.handle(int[][][] data) {
    Console.Error.WriteLine("a_variable_int_array_array_array");
    a_variable_int_array_array_array.encode(encoder, data);
  }

  void an_int_struct.Handler.handle(an_int_struct data) {
    Console.Error.WriteLine("an_int_struct");
    an_int_struct.encode(encoder, data);
  }

  void a_void.Handler.handle() {
    Console.Error.WriteLine("a_void");
    a_void.encode(encoder);
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

    an_int.register(d, this);
    a_fixed_int_array.register(d, this);
    a_fixed_int_multi_array.register(d, this);
    a_fixed_int_array_array_array.register(d, this);
    a_variable_int_array.register(d, this);
    a_variable_int_multi_array.register(d, this);
    a_variable_int_array_array_array.register(d, this);
    an_int_struct.register(d, this);
    a_void.register(d, this);

    an_int.register(encoder);
    a_fixed_int_array.register(encoder);
    a_fixed_int_multi_array.register(encoder);
    a_fixed_int_array_array_array.register(encoder);
    a_variable_int_array.register(encoder);
    a_variable_int_multi_array.register(encoder);
    a_variable_int_array_array_array.register(encoder);
    an_int_struct.register(encoder);
    a_void.register(encoder);

    try {
      d.run();
    } catch (EndOfStreamException) {
    }

  }

  static void Main(String[] arg) {
    new x(arg[0], arg[1]);
  }

}
