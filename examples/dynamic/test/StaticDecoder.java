package test;

import gen.foo;
import gen.bar;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import se.lth.control.labcomm2014.DecoderChannel;


public class StaticDecoder implements foo.Handler, bar.Handler
{

  DecoderChannel decoder;

  public StaticDecoder(InputStream in) throws Exception {
    decoder = new DecoderChannel(in);
    foo.register(decoder, this);
    bar.register(decoder, this);

  }

  public void run() throws Exception { 
    try {
      System.out.println("Running decoder.");
      decoder.run();
    } catch (java.io.EOFException e) {
	System.out.println("Decoder reached end of file.");
    }
  }


  public void handle_foo(foo d) throws java.io.IOException {
    System.out.println("Got foo, x="+d.x+", y="+d.y+", z="+d.z);
  }

  public void handle_bar(int d) throws java.io.IOException {
    System.out.println("Got bar: "+d);
  }

  public static void main(String[] arg) throws Exception {
    FileInputStream fis = new FileInputStream(new File(arg[0]));
    StaticDecoder dec = new StaticDecoder(fis);
    dec.run();
    fis.close();
  }
}

