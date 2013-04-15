import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import se.lth.control.labcomm.LabCommDecoderChannel;

public class Decoder
  implements theTwoInts.Handler, anotherTwoInts.Handler, IntString.Handler, TwoArrays.Handler, TwoFixedArrays.Handler

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

  public void handle_TwoArrays(TwoArrays d) throws java.io.IOException {
    System.out.println("Got TwoArrays:");
    for(int i=0; i<d.fixed.length; i++) {
	System.out.print(d.fixed[i]+" ");
    }
    System.out.println();
    for(int i=0; i<d.variable[0].length; i++) {
	System.out.print(d.variable[0][i]+" ");
	System.out.print(d.variable[1][i]+" ");
    }
    System.out.println();
  }

  public void handle_TwoFixedArrays(TwoFixedArrays d) throws java.io.IOException {
    System.out.println("Got TwoFixedArrays:");
    for(int i=0; i<d.a.length; i++) {
	System.out.print(d.a[i]+" ");
    }
    System.out.println();
    for(int i=0; i<d.b[0].length; i++) {
	System.out.print(d.b[0][i]+" ");
	System.out.print(d.b[1][i]+" ");
    }
    System.out.println();
  }


  public static void main(String[] arg) throws Exception {
    Decoder example = new Decoder(
      new FileInputStream(new File(arg[0]))
    );
  }
}

