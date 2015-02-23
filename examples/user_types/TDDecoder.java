import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;

import se.lth.control.labcomm.DecoderChannel;
import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeDefParser;
import se.lth.control.labcomm.ASTbuilder;
//import se.lth.control.labcomm.TypeBinding;

import se.lth.control.labcomm2014.compiler.Program;
import se.lth.control.labcomm2014.compiler.Decl;
import java.io.FileOutputStream;
import java.io.PrintStream;

import java.util.Vector;
import java.util.LinkedList;
import java.util.Iterator;

import org.jastadd.tinytemplate.TinyTemplate;
import org.jastadd.tinytemplate.TemplateContext;
import org.jastadd.tinytemplate.SimpleContext;
import org.jastadd.tinytemplate.TemplateParser.SyntaxError;


public class TDDecoder
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
  private final ASTbuilder astBuilder;
  private Program curAST;

  public TDDecoder(InputStream in) 
    throws Exception 
  {
    decoder = new DecoderChannel(in);
    twoInts.register(decoder, this);
    twoLines.register(decoder, this);
    theFirstInt.register(decoder, this);
    theSecondInt.register(decoder, this);
    doavoid.register(decoder, this);
    intAndRef.register(decoder, this);
    doavoid.registerSampleRef(decoder);
    this.tdp = TypeDefParser.registerTypeDefParser(decoder); 
    this.astBuilder = new ASTbuilder(tdp);
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


  private void doTT(TemplateContext ctx, String name) {
        ctx.expand(name+".decl", System.out);
        System.out.println();
  }

  private void doTestTT(Program p, TinyTemplate tt, String name){
    for(Decl d : p.getDecls()) {
        doTT(new SimpleContext(tt, d), name);
    }
  }

  public void testTT(Program p, InputStream templates, String name) {
        try {
            doTestTT(p, new TinyTemplate(templates), name);
        } catch(SyntaxError e) {
            System.err.println("SyntaxError in template: "+e);
            e.printStackTrace();
        }
    }


  public void onTypeDef(TypeDefParser.ParsedTypeDef d) {
    if(d.isSampleDef()){
        System.out.println("onTypeDef (sample): ");
        Program p = astBuilder.makeProgram(d, curAST);
        try {
                //FileOutputStream f = new FileOutputStream("/tmp/foopp"+d.getName()+".txt");
                //PrintStream out = new PrintStream(f);
                //p.pp(System.out);
                //p.C_genC(System.out, new Vector(), "lcname", "prefix", 2014);
                //p.J_gen(out, "testpackage", 2014);
                //out.close();
                //p.testTT("foo.type = [[#getTypeName ]] foo.name = [[ #getName ]] foo.decl = [[ $if(#isSampleDecl) SAMPLE_DECL $else TYPE_DECL $endif : $include(foo.name) ::TYPE:: $include(foo.type) ]]");
                FileInputStream fis1 = new FileInputStream(new File("Test.tt"));
                testTT(p, fis1, "foo");
                FileInputStream fis2 = new FileInputStream(new File("Test.tt"));
                testTT(p, fis2, "bar");
        } catch (Throwable e) {
                System.err.println("Exception: " + e);
                e.printStackTrace();
        }
        curAST = p;
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

  public void handle_intAndRef(intAndRef d) throws java.io.IOException {
    System.out.println("Got intAndRef: "+d.x+", "+d.reference);
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

