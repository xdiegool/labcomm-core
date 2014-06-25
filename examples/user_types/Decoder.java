import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import se.lth.control.labcomm.LabCommDecoderChannel;

public class Decoder
  implements twoLines.Handler

{

  LabCommDecoderChannel decoder;

  public Decoder(InputStream in) 
    throws Exception 
  {
    decoder = new LabCommDecoderChannel(in);
    twoLines.register(decoder, this);

    try {
      System.out.println("Running decoder.");
      decoder.run();
    } catch (java.io.EOFException e) {
	System.out.println("Decoder reached end of file.");
    }
  }

  private String genPoint(point p) {
    return "("+p.x.val+", "+p.y.val+")";
  }

  private String genLine(line l) {
    return "Line from "+genPoint(l.start)+" to "+genPoint(l.end);
  }

  public void handle_twoLines(twoLines d) throws java.io.IOException {
    System.out.print("Got twoLines: ");
    System.out.println("Line l1: "+genLine(d.l1));
    System.out.println("              Line l2: "+genLine(d.l2));
  }


  public static void main(String[] arg) throws Exception {
    Decoder example = new Decoder(
      new FileInputStream(new File(arg[0]))
    );
  }
}

