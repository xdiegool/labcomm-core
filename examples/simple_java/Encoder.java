import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;

import se.lth.control.labcomm.LabCommEncoderChannel;

/**
 * Simple encoder 
 */
public class Encoder 
{

  LabCommEncoderChannel encoder;

  public Encoder(OutputStream out) 
    throws Exception 
  {
    encoder = new LabCommEncoderChannel(out);
    TwoInts.register(encoder);
    IntString.register(encoder);
  }

  public void doEncode() throws java.io.IOException {
    TwoInts x = new TwoInts();
    x.a = 17;
    x.b = 42;

    IntString y = new IntString();
    y.x = 37;
    y.s = "Testing, testing";

    System.out.println("Encoding TwoInts, a="+x.a+", b="+x.b);
    TwoInts.encode(encoder, x);

    System.out.println("Encoding IntString, x="+y.x+", s="+y.s);
    IntString.encode(encoder, y);
  }


  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    Encoder example = new Encoder(fos);
    example.doEncode();
    fos.close();
  }

}

