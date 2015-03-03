package test;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import se.lth.control.labcomm2014.EncoderChannel;

import gen.foo;
import gen.bar;

public class StaticEncoder {

  EncoderChannel encoder;

  public StaticEncoder(OutputStream out) 
    throws Exception 
  {
    encoder = new EncoderChannel(out);
    foo.register(encoder);
    bar.register(encoder);
  }

  public void doEncode() throws java.io.IOException {
    foo f = new foo();
    f.x = 17;
    f.y = 42;
    f.z = 37;

    int b = 13;

    System.out.println("Encoding foo, x="+f.x+", y="+f.y+", z="+f.z);
    foo.encode(encoder, f);

    System.out.println("Encoding bar: "+b);
    bar.encode(encoder, b);
  }

  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    StaticEncoder enc = new StaticEncoder(fos);
    enc.doEncode();
    fos.close();
  }
}

