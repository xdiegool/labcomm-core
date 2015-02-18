import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;

import se.lth.control.labcomm.EncoderChannel;

/**
 * Simple encoder 
 */
public class EncoderIS 
{

  EncoderChannel encoder;

  public EncoderIS(OutputStream out) 
    throws Exception 
  {
    encoder = new EncoderChannel(out);
//    doavoid.register(encoder);
//    theTwoInts.register(encoder);
    IntString.register(encoder);
//    TwoArrays.register(encoder);
  }

  public void doEncodeIS() throws java.io.IOException {
    IntString a = new IntString();
    a.x = 17;
    a.s = "A string";

    IntString b = new IntString();
    b.x = 9;
    b.s = "Hej";

    IntString c = new IntString();
    c.x = 133742;
    c.s = "Thirteenthirtysevenfourtytwo";

    System.out.println("Encoding IntStrings");
    IntString.encode(encoder, a);
    IntString.encode(encoder, b);
    IntString.encode(encoder, c);
  }


  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    EncoderIS example = new EncoderIS(fos);
    example.doEncodeIS();
    fos.close();
  }

}

