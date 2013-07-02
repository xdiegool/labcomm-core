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
  public class Encoder {

    LabCommEncoderChannel encoder;

    public Encoder(OutputStream out) 
      throws Exception 
    {
      encoder = new LabCommEncoderChannel(out);
      theTwoInts.register(encoder);
      IntString.register(encoder);
      TwoArrays.register(encoder);
    }

    public void doEncode() throws java.io.IOException {
      TwoInts x = new TwoInts();
      x.a = 17;
      x.b = 42;
  
      IntString y = new IntString();
      y.x = 37;
      y.s = "Testing, testing";

      System.out.println("Encoding theTwoInts, a="+x.a+", b="+x.b);
      theTwoInts.encode(encoder, x);

      System.out.println("Encoding IntString, x="+y.x+", s="+y.s);
      IntString.encode(encoder, y);
    }
  }

  public class Decoder
  implements theTwoInts.Handler, anotherTwoInts.Handler, IntString.Handler
  {

    LabCommDecoderChannel decoder;

    public Decoder(InputStream in) 
      throws Exception 
    {
      decoder = new LabCommDecoderChannel(in);
      theTwoInts.register(decoder, this);
      anotherTwoInts.register(decoder, this);
      IntString.register(decoder, this);
      TwoArrays.register(decoder, this);
      TwoFixedArrays.register(decoder, this);

      try {
        System.out.println("Running decoder.");
        decoder.run();
      } catch (java.io.EOFException e) {
  	System.out.println("Decoder reached end of file.");
      }
    }

    public void printTwoInts(TwoInts d) throws java.io.IOException {
      System.out.println("a="+d.a+", b="+d.b);
    }

    public void handle_theTwoInts(TwoInts d) throws java.io.IOException {
      System.out.print("Got theTwoInts: ");
      printTwoInts(d);
    }

    public void handle_anotherTwoInts(TwoInts d) throws java.io.IOException {
      System.out.print("Got anotherheTwoInts: ");
      printTwoInts(d);
    }

    public void handle_IntString(IntString d) throws java.io.IOException {
      System.out.println("Got IntString, x="+d.x+", s="+d.s);
    }

  }

  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    FileInputStream fis = new FileInputStream(new File(arg[0]))
    Encoder example = new Encoder(fos);
    Decoder example = new Decoder(fis);
    example.doEncode();
    fos.close();
    fis.close();
  }
}

