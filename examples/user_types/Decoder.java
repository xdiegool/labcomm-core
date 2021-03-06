import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;

import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.TypeDef;
import se.lth.control.labcomm2014.TypeDefParser;
import se.lth.control.labcomm2014.SigTypeDef;
//import se.lth.control.labcomm2014.TypeBinding;

public class Decoder
  implements twoLines.Handler,
//             TypeDef.Handler,
//             TypeBinding.Handler,
             TypeDefParser.TypeDefListener,
             twoInts.Handler,
             theFirstInt.Handler,
             theSecondInt.Handler,
             doavoid.Handler,
             intAndRef.Handler
{

  private DecoderChannel decoder;
  private TypeDefParser tdp;

  public Decoder(InputStream in) 
    throws Exception 
  {
    decoder = new DecoderChannel(in);
    doavoid.register(decoder, this);
    twoInts.register(decoder, this);
    twoLines.register(decoder, this);
    theFirstInt.register(decoder, this);
    theSecondInt.register(decoder, this);
    intAndRef.register(decoder, this);
    doavoid.registerSampleRef(decoder);
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

  public void onTypeDef(SigTypeDef d) {
    System.out.println("ontype_def: ");
    if(d != null) {
        System.out.print((d.isSampleDef()?"sample ":"typedef ")+d);
        System.out.println(" "+d.getName()+";");
    } else {
        System.out.println(" null???");
    }
    //for(byte b: d.getSignature()) {
    //   System.out.print(Integer.toHexString(b)+" ");
    //}
    //System.out.println(); 
    //try {
    //   tdp.parseSignature(d.getIndex());
    //} catch(IOException ex) { ex.printStackTrace();}   
  }

  public void handle_doavoid() throws java.io.IOException {
    System.out.println("Got a void.");
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

  public void handle_intAndRef(intAndRef d) throws java.io.IOException {
    System.out.println("Got intAndRef: "+d.x+", "+d.reference);
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

