import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;

import se.lth.control.labcomm.LabCommEncoderChannel;

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

    TwoInts.encode(encoder, x);
    IntString.encode(encoder, y);
  }


  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(new File(arg[0]));
    Encoder example = new Encoder(fos);
    example.doEncode();
    fos.close();
  }

}

