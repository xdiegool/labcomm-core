import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import se.lth.control.labcomm.LabCommDecoderChannel;

public class Decoder
  implements TwoInts.Handler, IntString.Handler 
{

  LabCommDecoderChannel decoder;

  public Decoder(InputStream in) 
    throws Exception 
  {
    decoder = new LabCommDecoderChannel(in);
    TwoInts.register(decoder, this);
    IntString.register(decoder, this);

    try {
      System.out.println("Running decoder.");
      decoder.run();
    } catch (java.io.EOFException e) {
	System.out.println("Decoder reached end of file.");
    }
  }

  public void handle_TwoInts(TwoInts d) throws java.io.IOException {
    System.out.println("Got TwoInts, a="+d.a+", b="+d.b);
  }

  public void handle_IntString(IntString d) throws java.io.IOException {
    System.out.println("Got IntString, x="+d.x+", s="+d.s);
  }


  public static void main(String[] arg) throws Exception {
    Decoder example = new Decoder(
      new FileInputStream(new File(arg[0]))
    );
  }
}

