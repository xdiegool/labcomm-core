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

    TwoArrays ta = new TwoArrays();
    ta.fixed = new int[] {14, 25};
//    ta.variable = new int[][] {{1,2},{0x11,0x12},{0x21,0x22},{0x31,0x32}};
    ta.variable = new int[][] {{1,2, 3, 4},{0x21,0x22,0x23,0x24}};

    System.out.println("Encoding theTwoInts, a="+x.a+", b="+x.b);
    theTwoInts.encode(encoder, x);

    System.out.println("Encoding IntString, x="+y.x+", s="+y.s);
    IntString.encode(encoder, y);

    System.out.println("Encoding TwoArrays");
    for(int i = 0; i < ta.variable.length; i++) {
	for(int j=0; j < ta.variable[0].length; j++)
		System.out.println(ta.variable[i][j]);
	System.out.println("---");
    }
    TwoArrays.encode(encoder, ta);
  }


  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    Encoder example = new Encoder(fos);
    example.doEncode();
    fos.close();
  }

}

