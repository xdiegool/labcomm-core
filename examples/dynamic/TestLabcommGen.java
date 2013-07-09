import java.io.ByteArrayInputStream;
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



public class TestLabcommGen {

	private static final String SAMPLE_NAME_FOO = "foo";

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
	/**
	 * @param args
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
			System.out.println("*** Testing instantiation and invocation of Handler ");
			dummyTest(irc);

			String tmpFile = args[2];
			System.out.println("*** Testing writing and reading file "+tmpFile);
			encodeTest(irc, SAMPLE_NAME_FOO, tmpFile);
			decodeTest(irc, SAMPLE_NAME_FOO, tmpFile);
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
	private static void decodeTest(InRAMCompiler irc, String sampleName, String tmpFile) {
		try {
			FileInputStream in = new FileInputStream(tmpFile);
			LabCommDecoderChannel dec = new LabCommDecoderChannel(in);

			Class fc = irc.load(sampleName);
			Class hc = irc.load("gen_"+sampleName+"Handler");
			Class hi = irc.load(sampleName+"$Handler");

			Object h = hc.newInstance(); 

			Method reg = fc.getDeclaredMethod("register", LabCommDecoder.class, hi);
			reg.invoke(fc, dec, h);

			dec.runOne();
			in.close();
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}
	/** test encoding
	 */
	private static void encodeTest(InRAMCompiler irc, String sampleName, String tmpFile) {
		try {
			Class fc = irc.load(sampleName);
			Object f = fc.newInstance();
			Field x = fc.getDeclaredField("x");
			Field y = fc.getDeclaredField("y");
			Field z = fc.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);

			FileOutputStream out = new FileOutputStream(tmpFile);
			LabCommEncoderChannel enc = new LabCommEncoderChannel(out);
			Method reg = fc.getDeclaredMethod("register", LabCommEncoder.class);
			reg.invoke(fc, enc);

			Method doEncode = fc.getDeclaredMethod("encode", LabCommEncoder.class, fc);
			doEncode.invoke(fc, enc, f);

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
