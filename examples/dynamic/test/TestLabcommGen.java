package test;

import java.io.ByteArrayInputStream;
import java.io.EOFException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.CharBuffer;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;

import se.lth.control.labcomm2014.Decoder;
import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.Encoder;
import se.lth.control.labcomm2014.EncoderChannel;
import se.lth.control.labcomm2014.compiler.LabCommParser;
import se.lth.control.labcomm2014.compiler.LabCommScanner;
import se.lth.control.labcomm2014.compiler.Program;
import beaver.Parser.Exception;

public class TestLabcommGen {

	private static final String TYPE_NAME_FOO = "foo_t";
	private static final String SAMPLE_NAME_FOO = "foo";
	private static final String SAMPLE_NAME_BAR = "bar";

	static final String handlerClassName= "HandlerContainer";

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
			//dummyTest(irc);

			String tmpFile = args[2];
			System.out.println("*** Testing writing and reading file "+tmpFile);
			encodeTest(irc, tmpFile);
			decodeTest(irc, tmpFile, SAMPLE_NAME_FOO, SAMPLE_NAME_BAR);
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
			lcAST.J_gen(genCode, "labcomm.generated", 2014);
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Generated labcomm code:");

		InRAMCompiler irc = new InRAMCompilerJavax("labcomm.generated", TestLabcommGen.class.getClassLoader(), true);

		StringBuilder handlerClass = new StringBuilder();
		StringBuilder handlerMethods = new StringBuilder();

		handlerClass.append("package labcomm.generated;\n");
		handlerClass.append("import test.TestLabcommGen;\n");
		handlerClass.append("public class "+handlerClassName+" implements ");


		String handlerAttributes = "Object context;\n";
		String handlerConstr = "public "+handlerClassName+"(){ super();}\n";
		String handlerConstrCtxt = "public "+handlerClassName+"(Object context){ this.context=context;}\n";
		
		Iterator<String> i = genCode.keySet().iterator();
		try {
			while(i.hasNext()){
				final String sampleName = i.next();
				System.out.println("sample: "+sampleName);
				final String src = genCode.get(sampleName);
				String hctmp = handlers.get(sampleName);
				if(hctmp != null) {
					handlerClass.append(sampleName+".Handler");
					if(i.hasNext()) {
						handlerClass.append(", ");
					}
					handlerMethods.append(hctmp);
					handlerMethods.append("\n");
				}
				//System.out.println("FOOOO:"+sampleName+"++++"+hctmp);
				//System.out.println("GOOO: "+sampleName+"----"+handlerMethods);
				System.out.println("***"+sampleName+"\n"+src);
				irc.compile(sampleName, src);  // while iterating, compile the labcomm generated code
			}
			handlerClass.append("{\n");
			if(handlerAttributes != null)
				handlerClass.append(handlerAttributes);
			if(handlerConstr != null)
				handlerClass.append(handlerConstr);
			if(handlerConstrCtxt != null)
				handlerClass.append(handlerConstrCtxt);
			if(handlerMethods != null)
				handlerClass.append(handlerMethods.toString());
			handlerClass.append("}\n");


			System.out.println("--- generated code to compile -------"+ handlerClassName);

			final String handlerSrc = handlerClass.toString();
			System.out.println(handlerSrc);
			irc.compile(handlerClassName, handlerSrc); // compile the generated handler class
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

		return irc;
	}

	/** generate labcomm code and compile handlers. Version for separate handler classes 
	 *
	 * @param lcAST - the AST of the labcomm declaration
	 * @param handlers - a map <name, source> of handlers for the types in ast
	 * @return an InRAMCompiler object containing the generated clases
	 */
	private static InRAMCompiler handleAstSeparate(Program lcAST, HashMap<String, String> handlers) {
		Map<String, String> genCode = new HashMap<String, String>();
		try {
			lcAST.J_gen(genCode, "labcomm.generated", 2013);
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
			System.out.println("--- foo -----------------------------");
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
	private static void decodeTest(InRAMCompiler irc, String tmpFile, String... sampleNames) {
		try {
			FileInputStream in = new FileInputStream(tmpFile);
			DecoderChannel dec = new DecoderChannel(in);
			Class handlerClass =  irc.load(handlerClassName);
			Constructor hcc = handlerClass.getDeclaredConstructor(Object.class);
//			Object handler = handlerClass.newInstance(); 
			HandlerContext ctxt = new HandlerContext();
			Object handler = hcc.newInstance(ctxt);

			for (String sampleName : sampleNames) {
				System.out.println("registering handler for "+sampleName);
				Class sampleClass = irc.load(sampleName);
				Class handlerInterface = irc.load(sampleName+"$Handler");

				Method reg = sampleClass.getDeclaredMethod("register", Decoder.class, handlerInterface);
				reg.invoke(sampleClass, dec, handler);
			}

			try{
				System.out.println("*** decoding:");
				dec.run();
			} catch(EOFException e) {
				System.out.println("*** reached EOF ***");
			}
			in.close();
			System.out.println("ctxt.x = "+ctxt.x);
			System.out.println("ctxt.y = "+ctxt.y);
			System.out.println("ctxt.z = "+ctxt.z);
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}
	/** test encoding
	 */
	private static void encodeTest(InRAMCompiler irc, String tmpFile) {
		try {
			Class ft;
			Class fc = irc.load(SAMPLE_NAME_FOO);
			Class bc = irc.load(SAMPLE_NAME_BAR);
			try {
				ft = irc.load(TYPE_NAME_FOO);
			} catch (ClassNotFoundException e) {
				System.out.println("encodeTest: defaulting to ft == fc");
				ft = fc;
			}

			/* create sample class and instance objects */
			Object f = ft.newInstance();

			Field x = ft.getDeclaredField("x");
			Field y = ft.getDeclaredField("y");
			Field z = ft.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);


			FileOutputStream out = new FileOutputStream(tmpFile);
			EncoderChannel enc = new EncoderChannel(out);

			/* register and send foo */
			Method regFoo = fc.getDeclaredMethod("register", Encoder.class);
			regFoo.invoke(fc, enc);

			Method doEncodeFoo = fc.getDeclaredMethod("encode", Encoder.class, ft);
			doEncodeFoo.invoke(fc, enc, f);

			/* register and send bar (NB! uses primitive type int) */
			Method regBar = bc.getDeclaredMethod("register", Encoder.class);
			regBar.invoke(bc, enc);

			Method doEncodeBar = bc.getDeclaredMethod("encode", Encoder.class, Integer.TYPE);
			doEncodeBar.invoke(bc, enc, 42);

			out.close();
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}

	/** dummy test creating instances of sample and handler, and calling handle*/
	private static void dummyTest(InRAMCompiler irc) {
		try {
			Class hc =  irc.load(handlerClassName);
			Constructor hcc = hc.getDeclaredConstructor(Object.class);
//			Object h = hc.newInstance(); 
			Object h = hcc.newInstance(new HandlerContext());
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
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	/** test method
	 */
	private static void decodeTestSeparate(InRAMCompiler irc, String tmpFile, String... sampleNames) {
		try {
			FileInputStream in = new FileInputStream(tmpFile);
			DecoderChannel dec = new DecoderChannel(in);
			for (String sampleName : sampleNames) {
				System.out.println("registering handler for "+sampleName);
				Class sampleClass = irc.load(sampleName);
				Class handlerClass = irc.load("gen_"+sampleName+"Handler");
				Class handlerInterface = irc.load(sampleName+"$Handler");

				Object handler = handlerClass.newInstance(); 

				Method reg = sampleClass.getDeclaredMethod("register", Decoder.class, handlerInterface);
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

	/** dummy test creating instances of sample and handler, and calling handle*/
	private static void dummyTestSeparate(InRAMCompiler irc) {
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
