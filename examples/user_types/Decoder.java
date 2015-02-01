import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import se.lth.control.labcomm.DecoderChannel;
import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeDefParser;
//import se.lth.control.labcomm.TypeBinding;

public class Decoder
  implements twoLines.Handler,
//             TypeDef.Handler,
//             TypeBinding.Handler,
             TypeDefParser.TypeDefListener,
             twoInts.Handler,
             theFirstInt.Handler,
             theSecondInt.Handler
{

  private DecoderChannel decoder;
  private TypeDefParser tdp;

  public Decoder(InputStream in) 
    throws Exception 
  {
    decoder = new DecoderChannel(in);
    twoInts.register(decoder, this);
    twoLines.register(decoder, this);
    theFirstInt.register(decoder, this);
    theSecondInt.register(decoder, this);
    this.tdp = TypeDefParser.registerTypeDefParser(decoder); 
 //   TypeDef.register(decoder, this);
 //   TypeBinding.register(decoder, this);

        
    tdp.addListener(this);
    
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

//  public void handle_TypeDef(TypeDef d) throws java.io.IOException {
//    System.out.println("Got TypeDef: "+d.getName()+"("+d.getIndex()+")");
//  }
//
//  public void handle_TypeBinding(TypeBinding d) throws java.io.IOException {
//    System.out.println("Got TypeBinding: "+d.getSampleIndex()+" --> "+d.getTypeIndex()+"");
//  }

  public void onTypeDef(TypeDef d) {
    System.out.println("onTypeDef: "+d.getName()+"("+d.getIndex()+")");
  }

  public void handle_twoInts(twoInts d) throws java.io.IOException {
    System.out.print("Got twoInts: ");
    System.out.println(d.a +", "+d.b);
  }

  public void handle_theFirstInt(int d) throws java.io.IOException {
    System.out.println("Got theFirstInt: "+d);
  }

  public void handle_theSecondInt(int d) throws java.io.IOException {
    System.out.println("Got theSecondInt: "+d);
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

