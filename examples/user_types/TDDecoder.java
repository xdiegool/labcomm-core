import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;

import se.lth.control.labcomm.DecoderChannel;
import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeDefParser;
import se.lth.control.labcomm.TypeDefVisitor;
//import se.lth.control.labcomm.TypeBinding;

import se.lth.control.labcomm2014.compiler.Program;
import java.io.FileOutputStream;
import java.io.PrintStream;

import java.util.Vector;
import java.util.LinkedList;
import java.util.Iterator;


public class TDDecoder
  implements twoLines.Handler,
//             TypeDef.Handler,
//             TypeBinding.Handler,
             TypeDefParser.TypeDefListener,
             twoInts.Handler,
             theFirstInt.Handler,
             theSecondInt.Handler,
             doavoid.Handler
{

  private DecoderChannel decoder;
  private TypeDefParser tdp;

  public TDDecoder(InputStream in) 
    throws Exception 
  {
    decoder = new DecoderChannel(in);
    twoInts.register(decoder, this);
    twoLines.register(decoder, this);
    theFirstInt.register(decoder, this);
    theSecondInt.register(decoder, this);
    doavoid.register(decoder, this);
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

  public void onTypeDef(TypeDefParser.ParsedTypeDef d) {
    if(d.isSampleDef()){
        System.out.println("onTypeDef (sample): ");
        TypeDefVisitor v = new TypeDefVisitor();
        Program p = v.makeProgram((TypeDefParser.ParsedSampleDef) d);
        LinkedList errors = new LinkedList();
        p.errorCheck(errors);
        if(errors.isEmpty()) {
            try {
                //FileOutputStream f = new FileOutputStream("/tmp/foopp"+d.getName()+".txt");
                //PrintStream out = new PrintStream(f);
                p.pp(System.out);
                p.C_genC(System.out, new Vector(), "lcname", "prefix", 2014);
                //p.J_gen(out, "testpackage", 2014);
                //out.close();
            } catch (Throwable e) {
                System.err.println("Exception: " + e);
                e.printStackTrace();
            }
        } else {
            for (Iterator iter = errors.iterator(); iter.hasNext(); ) {
                String s = (String)iter.next();
                System.out.println(s);
            }
        }
    }
    //System.out.println(" "+d.getName()+";");
    //for(byte b: d.getSignature()) {
    //   System.out.print(Integer.toHexString(b)+" ");
    //}
    //System.out.println(); 
    //try {
    //   tdp.parseSignature(d.getIndex());
    //} catch(IOException ex) { ex.printStackTrace();}   
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

  public void handle_doavoid() throws java.io.IOException {
    System.out.println("Got a void.");
  }

  public void handle_twoLines(twoLines d) throws java.io.IOException {
    System.out.print("Got twoLines: ");
    System.out.println("Line l1: "+genLine(d.l1));
    System.out.println("              Line l2: "+genLine(d.l2));
  }


  public static void main(String[] arg) throws Exception {
    TDDecoder example = new TDDecoder(
      new FileInputStream(new File(arg[0]))
    );
  }
}

