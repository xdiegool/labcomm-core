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

import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDecoderChannel;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommEncoderChannel;
import AST.LabCommParser;
import AST.LabCommScanner;
import AST.Program;
import beaver.Parser.Exception;

public class DynamicPart {

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

 	public void doTest(InRAMCompiler irc, String iFile, String oFile) {
		HandlerContext ctxt = new HandlerContext();
		System.out.println("*** reading file "+iFile);
		decodeTest(irc, ctxt, iFile, SAMPLE_NAME_FOO, SAMPLE_NAME_BAR);
		System.out.println("*** writing "+oFile);
		encodeTest(irc, ctxt, oFile);
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

//		System.out.println("*** Generated handler source:");
		handlers.keySet();

//		for (String n : handlers.keySet()) {
//			System.out.println(n+":");
//			System.out.println(handlers.get(n));
//		}


		InRAMCompiler irc = generateCode(labcommStr, handlers);

		if(irc != null) {
			String iFile = args[2];
			String oFile = args[3];
			new DynamicPart().doTest(irc, iFile, oFile);
		}
	}
	public static void generateHandlers(String srcStr, HashMap<String,String> handlers) {
		int pos = 0;	
		while(pos < srcStr.length()) {
			int nameEnd = srcStr.indexOf(':', pos);
			if(nameEnd <0) break;

			String name = srcStr.substring(pos,nameEnd);

			pos=nameEnd+1;
			String par = "";
			final String handler_decl = "handler(";
			if(srcStr.startsWith(handler_decl, pos)) {
				int endPar = srcStr.indexOf(')', pos);
				par = srcStr.substring(pos+handler_decl.length(), endPar);
				pos = endPar+1;
			} else {
				System.out.println("expeced handler decl:\n"+srcStr.substring(pos));	
			}
			int bodyEnd = srcStr.indexOf("}###", pos); // HERE BE DRAGONS! a bit brittle
			String body = srcStr.substring(pos, bodyEnd+1);
			pos = bodyEnd+5;

			HandlerSrc s = new HandlerSrc(name, par, body);
			final String genSrc = s.getSrc();
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
//		System.out.println("Generated labcomm code:");

		InRAMCompiler irc = new InRAMCompilerJavax("labcomm.generated", TestLabcommGen.class.getClassLoader());

		StringBuilder handlerClass = new StringBuilder();
		StringBuilder handlerMethods = new StringBuilder();

		handlerClass.append("package labcomm.generated;\n");
		handlerClass.append("public class "+handlerClassName+" implements ");


		String handlerAttributes = "Object context;\n";
		String handlerConstr = "public "+handlerClassName+"(){super();}\n";
		String handlerConstrCtxt = "public "+handlerClassName+"(Object context){ this.context=context;}\n";
		
		Iterator<String> i = genCode.keySet().iterator();
		try {
			while(i.hasNext()){
				final String sampleName = i.next();
				final String src = genCode.get(sampleName);
				String handleM = handlers.get(sampleName);
				if(handleM != null) {
					handlerClass.append(sampleName+".Handler");
					if(i.hasNext()) {
						handlerClass.append(", ");
					}
					handlerMethods.append(handleM);
					handlerMethods.append("\n");
				}
//				System.out.println("***"+sampleName+"\n"+src);
				irc.compile(sampleName, src);  // while iterating, compile the labcomm generated code
			}
			handlerClass.append("{\n");
			handlerClass.append(handlerAttributes);
			handlerClass.append(handlerConstr);
			handlerClass.append(handlerConstrCtxt);
			handlerClass.append(handlerMethods.toString());
			handlerClass.append("}\n");


			System.out.println("-------------------------------------");

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

	/** test method
	 */
	private void decodeTest(InRAMCompiler irc, HandlerContext ctxt, String tmpFile, String... sampleNames) {
		try {
			FileInputStream in = new FileInputStream(tmpFile);
			LabCommDecoderChannel dec = new LabCommDecoderChannel(in);
			Class handlerClass =  irc.load(handlerClassName);
			Constructor hcc = handlerClass.getDeclaredConstructor(Object.class);
			Object handler = hcc.newInstance(ctxt);

			for (String sampleName : sampleNames) {
				System.out.println("registering handler for "+sampleName);
				Class sampleClass = irc.load(sampleName);
				Class handlerInterface = irc.load(sampleName+"$Handler");

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
	private void encodeTest(InRAMCompiler irc, HandlerContext ctxt, String tmpFile) {
		try {
			Class fc = irc.load(SAMPLE_NAME_FOO);
			Class bc = irc.load(SAMPLE_NAME_BAR);
			Class ft; // hack for both cases with and w/o typedef
			try {
				ft = irc.load(TYPE_NAME_FOO);
			} catch (ClassNotFoundException e) {
				System.out.println("** encodeTest: defaulting to sample==type");
				ft = fc;
			}

			/* create sample class and instance objects */
			Object fv = ft.newInstance();

			Field x = ft.getField("x");
			Field y = ft.getField("y");
			Field z = ft.getField("z");
			x.setInt(fv, ctxt.x);
			y.setInt(fv, ctxt.y);
			z.setInt(fv, ctxt.z);


			FileOutputStream out = new FileOutputStream(tmpFile);
			LabCommEncoderChannel enc = new LabCommEncoderChannel(out);

			/* register and send foo */
			Method regFoo = fc.getDeclaredMethod("register", LabCommEncoder.class);
			regFoo.invoke(fc, enc);

			Method doEncodeFoo = fc.getDeclaredMethod("encode", LabCommEncoder.class, ft);
			doEncodeFoo.invoke(fc, enc, fv);

			/* register and send bar (NB! uses primitive type int) */
			Method regBar = bc.getDeclaredMethod("register", LabCommEncoder.class);
			regBar.invoke(bc, enc);

			Method doEncodeBar = bc.getDeclaredMethod("encode", LabCommEncoder.class, Integer.TYPE);
			doEncodeBar.invoke(bc, enc, ctxt.bar);

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
			Class ft = irc.load(TYPE_NAME_FOO);
			Object f = ft.newInstance();
			Field x = ft.getDeclaredField("x");
			Field y = ft.getDeclaredField("y");
			Field z = ft.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);
			Method m;
			try {
				m = hc.getDeclaredMethod("handle_"+SAMPLE_NAME_FOO, ft);
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
}
