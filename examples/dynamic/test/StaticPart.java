package test;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import se.lth.control.labcomm.LabCommEncoderChannel;

import java.io.FileInputStream;
import java.io.InputStream;
import se.lth.control.labcomm.LabCommDecoderChannel;


public class StaticPart {
  /**
   * Simple encoder 
   */
  public static class Encoder {

    LabCommEncoderChannel encoder;

    public Encoder(OutputStream out) 
      throws Exception 
    {
      encoder = new LabCommEncoderChannel(out);
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
  }

  public static class Decoder
  implements foo.Handler, bar.Handler
  {

    LabCommDecoderChannel decoder;

    public Decoder(InputStream in) 
      throws Exception 
    {
      decoder = new LabCommDecoderChannel(in);
      foo.register(decoder, this);
      bar.register(decoder, this);

    }

    public void doDecode() throws java.io.IOException {
      try {
        System.out.println("Running decoder.");
        decoder.run();
      } catch (java.io.EOFException e) {
  	System.out.println("Decoder reached end of file.");
      } catch (Exception e) {
	e.printStackTrace();
      }
    }


    public void handle_foo(foo d) throws java.io.IOException {
      System.out.println("Got foo, x="+d.x+", y="+d.y+", z="+d.z);
    }

    public void handle_bar(int d) throws java.io.IOException {
      System.out.println("Got bar: "+d);
    }

  }

  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    FileInputStream fis = new FileInputStream(new File(arg[0]));
    Encoder enc = new Encoder(fos);
    Decoder dec = new Decoder(fis);
    enc.doEncode();
    dec.doDecode();
    fos.close();
    fis.close();
  }
}

