package test;
import java.io.ByteArrayInputStream;
import java.io.EOFException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.CharBuffer;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;

import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDecoderChannel;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommEncoderChannel;
import AST.LabCommParser;
import AST.LabCommScanner;
import AST.Program;
import beaver.Parser.Exception;



/** Simple test class that illustrates on-the-fly generation, compilation, and loading
 *   of labcomm handlers.
 *
 *  It reads, modifies and writes the "foo" and "bar" sample types
 */
public class DynamicPart {

	private static final String SAMPLE_NAME_FOO = "foo";
	private static final String SAMPLE_NAME_BAR = "bar";

	/** A class representing the source code for one Labcomm handler */
	static class HandlerSrc {
		private String sampleName;
		private String param;
		private String body;

		private final String proto = "public void handle_";

		public HandlerSrc(String sampleName, String param, String body) {
			this.sampleName = sampleName;
			this.param = param;
			this.body = body;
		}

		public String getSrc() {
			String res = proto+sampleName+"("+param+")"+body;
			return res;
		}
	}

        public void doTest(InRAMCompiler irc, String inFile, String outFile) {
		System.out.println("*** DynamicPart reading from "+inFile);
		decodeTest(irc, tmpFile, SAMPLE_NAME_FOO, SAMPLE_NAME_BAR);
		System.out.println("*** DynamicPart writing to "+outFile);
		encodeTest(irc, tmpFile);
	}

	/**
	 * @param args (types.lc, handlers.txt, inputfile, outputfile) 
	 */
	public static void main(String[] args) {

		/* input data: */
		String labcommStr = readLabcommDecl(args[0]);
		String srcStr = readHandlerDecl(args[1]);

		/*Map<sample name, handler code>*/
		HashMap <String, String> handlers = new HashMap<String, String>();

		generateHandlers(srcStr, handlers);

		System.out.println("*** Generated handler source:");
		handlers.keySet();

		for (String n : handlers.keySet()) {
			System.out.println(n+":");
			System.out.println(handlers.get(n));
		}


		InRAMCompiler irc = generateCode(labcommStr, handlers);

		if(irc != null) {
			String inFile = args[2];
			String outFile = args[3];
			new DynamicPart().doTest(irc, inFile, outFile);
		}
	}

	public static void generateHandlers(String srcStr, HashMap<String,String> handlers) {
		int pos = 0;	
		while(pos < srcStr.length()) {
			//			System.out.println("--------");
			int nameEnd = srcStr.indexOf(':', pos);
			if(nameEnd <0) break;

			String name = srcStr.substring(pos,nameEnd);
			//			System.out.println("Name="+name);

			pos=nameEnd+1;
			String par = "";
			final String handler_decl = "handler(";
			if(srcStr.startsWith(handler_decl, pos)) {
				int endPar = srcStr.indexOf(')', pos);
				par = srcStr.substring(pos+handler_decl.length(), endPar);
				//				System.out.println("param="+par);
				pos = endPar+1;
			} else {
				System.out.println("expeced handler decl:\n"+srcStr.substring(pos));	
			}
			int bodyEnd = srcStr.indexOf("}###", pos); // HERE BE DRAGONS! a bit brittle
			String body = srcStr.substring(pos, bodyEnd+1);
			pos = bodyEnd+5;
			//			System.out.println("body:");
			//			System.out.println(body);

			//			System.out.println("**** generates:");

			HandlerSrc s = new HandlerSrc(name, par, body);
			final String genSrc = s.getSrc();
			//			System.out.println(genSrc);
			handlers.put(name,genSrc);
		}
	}


	public static String readHandlerDecl(String decl) {
		FileReader fr;
		int len=0;;
		CharBuffer buf = CharBuffer.allocate(1024);
		try {
			fr = new FileReader(decl);
			len = fr.read(buf);
			buf.rewind();
		} catch (Throwable e) {
			e.printStackTrace();
			System.exit(1);
		}

		String srcStr = buf.toString().substring(0, len);
		return srcStr;
	}

	private static String readLabcommDecl(String lcfile) {
		FileReader fr;
		int len=0;;
		CharBuffer buf = CharBuffer.allocate(1024);
		try {
			fr = new FileReader(lcfile);
			len = fr.read(buf);
			//			buf.append((char) 0x04);
			buf.rewind();
		} catch (Throwable e) {
			e.printStackTrace();
			System.exit(1);
		}

		String labcommStr = buf.toString().substring(0, len-1);
		return labcommStr;
	}

	public static InRAMCompiler generateCode(String lcDecl, HashMap<String, String> handlers) {
		Program ast = null;
		InputStream in = new ByteArrayInputStream(lcDecl.getBytes());
		LabCommScanner scanner = new LabCommScanner(in);
		LabCommParser parser = new LabCommParser();
		Collection errors = new LinkedList();

		InRAMCompiler irc = null;
		try {
			Program p = (Program)parser.parse(scanner);
			p.errorCheck(errors);
			if (errors.isEmpty()) {
				ast = p;
			} else {
				System.out.println("*** Errors:");
				for (Iterator iter = errors.iterator(); iter.hasNext(); ) {
					String s = (String)iter.next();
					System.out.println(s);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (ast != null) {
			irc = handleAst(ast, handlers);
		} else {
			System.err.println("compilation failed");
		}
		return irc;
	}

	/** generate labcomm code and compile handlers 
	 *
	 * @param lcAST - the AST of the labcomm declaration
	 * @param handlers - a map <name, source> of handlers for the types in ast
	 * @return an InRAMCompiler object containing the generated clases
	 */
	private static InRAMCompiler handleAst(Program lcAST, HashMap<String, String> handlers) {
		Map<String, String> genCode = new HashMap<String, String>();
		try {
			lcAST.J_gen(genCode, "labcomm.generated");
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Generated labcomm code:");

		InRAMCompiler irc = new InRAMCompilerJavax("labcomm.generated", null);

		Iterator<String> i = genCode.keySet().iterator();
		while(i.hasNext()){
			final String sampleName = i.next();
			final String src = genCode.get(sampleName);
			System.out.println("***"+sampleName+"\n"+src);
			StringBuilder sb = new StringBuilder();
			sb.append("package labcomm.generated;\n");
			sb.append("public class gen_"+sampleName+"Handler implements "+sampleName+".Handler {\n");
			sb.append(handlers.get(sampleName));
			sb.append("}\n");
			System.out.println("-------------------------------------");
			System.out.println(sb.toString());
			try {
				irc.compile(sampleName, src);
				irc.compile("gen_"+sampleName+"Handler", sb.toString());
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (SecurityException e) {
				e.printStackTrace();
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				e.printStackTrace();
			}
			System.out.println("================================");
		}
		return irc;
	}
	/** test method
	 */
	private void decodeTest(InRAMCompiler irc, String tmpFile, String... sampleNames) {
		try {
			FileInputStream in = new FileInputStream(tmpFile);
			LabCommDecoderChannel dec = new LabCommDecoderChannel(in);
			for (String sampleName : sampleNames) {
				System.out.println("registering handler for "+sampleName);
				Class sampleClass = irc.load(sampleName);
				Class handlerClass = irc.load("gen_"+sampleName+"Handler");
				Class handlerInterface = irc.load(sampleName+"$Handler");

				Object handler = handlerClass.newInstance(); 

				Method reg = sampleClass.getDeclaredMethod("register", LabCommDecoder.class, handlerInterface);
				reg.invoke(sampleClass, dec, handler);
			}

			try{
				System.out.println("*** decoding:");
				dec.run();
			} catch(EOFException e) {
				System.out.println("*** reached EOF ***");
			}
			in.close();
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}
	/** test encoding
	 */
	private void encodeTest(InRAMCompiler irc, String tmpFile) {
		try {
			Class fc = irc.load(SAMPLE_NAME_FOO);
			Class bc = irc.load(SAMPLE_NAME_BAR);

			/* create sample class and instance objects */
			Object f = fc.newInstance();
			
			Field x = fc.getDeclaredField("x");
			Field y = fc.getDeclaredField("y");
			Field z = fc.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);
			

			FileOutputStream out = new FileOutputStream(tmpFile);
			LabCommEncoderChannel enc = new LabCommEncoderChannel(out);

			/* register and send foo */
			Method regFoo = fc.getDeclaredMethod("register", LabCommEncoder.class);
			regFoo.invoke(fc, enc);

			Method doEncodeFoo = fc.getDeclaredMethod("encode", LabCommEncoder.class, fc);
			doEncodeFoo.invoke(fc, enc, f);

			/* register and send bar (NB! uses primitive type int) */
			Method regBar = bc.getDeclaredMethod("register", LabCommEncoder.class);
			regBar.invoke(bc, enc);

			Method doEncodeBar = bc.getDeclaredMethod("encode", LabCommEncoder.class, Integer.TYPE);
			doEncodeBar.invoke(bc, enc, 42);

			out.close();
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}

	/** dummy test creating instances of sample and handler, and calling handle*/
	private static void dummyTest(InRAMCompiler irc) {
		try {
			Class hc = irc.load("gen_"+SAMPLE_NAME_FOO+"Handler");
			Object h = hc.newInstance(); 
			Class fc = irc.load(SAMPLE_NAME_FOO);
			Object f = fc.newInstance();
			Field x = fc.getDeclaredField("x");
			Field y = fc.getDeclaredField("y");
			Field z = fc.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);
			Method m;
			try {
				m = hc.getDeclaredMethod("handle_"+SAMPLE_NAME_FOO, fc);
				m.invoke(h, f);
			} catch (SecurityException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
