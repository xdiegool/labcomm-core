import AST.*;
import java.io.*;
import java.util.*;

public class LabComm {

  private static void println(String s) {
    System.out.println(s);
  }

  private static void print_help() {
    println("\n Usage: java -jar labcom.jar [options*] FILE");
    println("");
    println(" --help                  Shows this help text");
    println(" -v                      Be verbose");
    println(" --ver=VERSION           Generate code for labcomm VERSION (=2006 or 2013)");
    println("[ C options ]");
    println(" -C                      Generates C/H code in FILE.[ch]");
    println(" --cprefix=PREFIX        Prefixes C types with PREFIX");
    println(" --cinclude=FILE         Include FILE in generated .c");
    println(" --c=CFILE               Generates C code in CFILE");
    println(" --h=HFILE               Generates H code in HFILE");
    println("[ C# options]");
    println(" --cs                    Generates C# code in FILE.cs");
    println(" --cs=CSFILE             Generates C# code in CSFILE");
    println(" --csnamespace=NAMESPACE Place C# classes in NAMESPACE");
    println("[ Java options ]");
    println(" --java=JDIR             Generates Java files in JDIR");
    println(" --javapackage=PACKAGE   Place Java classes in PACKAGE");
    println("[ Python options ]");
    println(" -P                      Generates Python code in FILE.py");
    println(" --python=PFILE          Generates Python code in PFILE");
    println("[ RAPID options ]");
    println(" --rapid                 Generates RAPID code in FILE.sys");
    println("[ Misc options ]");
    println(" --pretty                Pretty prints on standard output");
    println(" --typeinfo=TIFILE       Generates typeinfo in TIFILE");
  }
    
  /** To be cleaned up.
   */
  private static void checkVersion(int v) {
     if(! (v == 2006 || v == 2013) ) {
	System.err.println(" Unknown version: " + v);
	System.err.println(" Supported versions: 2006, 2013 ");
	System.exit(2);
     }
  }

  private static void genH(Program p, String hName, 
			   Vector cIncludes, String coreName, String prefix, int ver) {
    try {
      FileOutputStream f;
      PrintStream out;
      
      f = new FileOutputStream(hName);
      out = new PrintStream(f);
      p.C_genH(out, cIncludes, coreName, prefix, ver);
      out.close();
    } catch (IOException e) {
      System.err.println("IOException: " + hName + " " + e);
    }
  }

  private static void genC(Program p, String cName, 
			   Vector cIncludes, String coreName, String prefix, int ver) {
    try {
      FileOutputStream f;
      PrintStream out;

      f = new FileOutputStream(cName);
      out = new PrintStream(f);
      p.C_genC(out, cIncludes, coreName, prefix, ver);
      out.close();
    } catch (IOException e) {
      System.err.println("IOException: " + cName + " " + e);
    }
  }

  private static void genCS(Program p, String csName, String csNamespace, int ver) {
    try {
      p.CS_gen(csName, csNamespace, ver);
    } catch (IOException e) {
      System.err.println("IOException: " + csName + " " + 
			 csNamespace + " " + e);
    }
  }

  private static void genJava(Program p,  String dirName, String packageName, int ver) {
    try {
      p.J_gen(dirName, packageName, ver);
    } catch (IOException e) {
      System.err.println("IOException: " + dirName + " " + 
			 packageName + " " + e);
    }
  }

  private static void genPython(Program p, String filename, String prefix, int ver) {
    try {
      FileOutputStream f;
      PrintStream out;

      f = new FileOutputStream(filename);
      out = new PrintStream(f);
      p.Python_gen(out, prefix, ver);
      out.close();
    } catch (IOException e) {
      System.err.println("IOException: " + filename + " " + e);
    }
  }

  private static void genRAPID(Program p, String filename, String prefix, int ver) {
    try {
      p.RAPID_gen(filename, prefix, ver);
    } catch (IOException e) {
      System.err.println("IOException: " + filename + " " + e);
    }
  }

  /** Helper class to contain command line options 
      and their associated behaviour
   **/
  private static class Opts {
    final String[] args;
    String coreName = null;
    String prefix = null;
    boolean verbose = false;
    int ver = 2013; //Version 2013 as default
    String cFile = null;
    String hFile = null;
    Vector cIncludes = new Vector();
    String cPrefix; // gets default value (prefix) in processFilename
    String csFile = null;
    String csNamespace = null;
    String javaDir = null;
    String javaPackage = "";
    String pythonFile = null;
    String prettyFile = null;
    String typeinfoFile = null;
    String rapidFile = null;
    String fileName = null;

   Opts(String[] args) {
     this.args = args;
   }

    private static String getCoreName(String s) {
      int i = s.lastIndexOf('.');
      return s.substring(0, i > 0 ? i : s.length());
    }
  
    private static String getFileName(String s) {
      return s.substring(s.lastIndexOf('/') + 1, s.length());
    }
  
    private static String getBaseName(String s) {
      s = getFileName(s);
      int i = s.lastIndexOf('.');
      return s.substring(0, i > 0 ? i : s.length());
    }
  
    private static String getPrefix(String s) {
      return s.substring(s.lastIndexOf('/') + 1, s.length());
    }
  
    boolean processFilename(){
      // Scan for first non-option
      for (int i = 0 ; i < args.length ; i++) {
        if (! args[i].startsWith("-")) {
  	fileName = args[i];
  	break;
        }
      }
      if (fileName != null) {
        coreName = getBaseName(fileName);
        prefix = getPrefix(coreName);
       cPrefix = prefix;
      }
      return fileName != null;
    }
    
    void processArgs(){
      for (int i = 0 ; i < args.length ; i++) {
        if (fileName == null ||
  	  args[i].equals("-help") || 
  	  args[i].equals("-h") || 
  	  args[i].equals("--help")) {
  	print_help();
  	System.exit(0);
        } else if (args[i].equals("-v")) {
  	verbose=true;
        } else if (args[i].startsWith("--ver=")) {
  	ver = Integer.parseInt(args[i].substring(6));
          checkVersion(ver);
        } else if (args[i].equals("-C")) {
  	cFile = coreName + ".c";
  	hFile = coreName + ".h";
        } else if (args[i].startsWith("--cinclude=")) {
  	cIncludes.add(args[i].substring(11));
        } else if (args[i].startsWith("--cprefix=")) {
  	cPrefix = args[i].substring(10);
        } else if (args[i].startsWith("--c=")) {
  	cFile = args[i].substring(4);
        } else if (args[i].startsWith("--h=")) {
  	hFile = args[i].substring(4);
        } else if (args[i].equals("--cs")) {
  	csFile = coreName + ".cs";
        } else if (args[i].startsWith("--cs=")) {
  	csFile = args[i].substring(5);
        } else if (args[i].startsWith("--csnamespace=")) {
  	csNamespace = args[i].substring(14);
        } else if (args[i].startsWith("--java=")) {
  	javaDir = args[i].substring(7);
        } else if (args[i].startsWith("--javapackage=")) {
  	javaPackage = args[i].substring(14);
        } else if (args[i].equals("-P")) {
  	pythonFile = coreName + ".py";
        } else if (args[i].startsWith("--python=")) {
  	pythonFile = args[i].substring(9);
        } else if (args[i].startsWith("--pretty=")) {
  	prettyFile = args[i].substring(9);
        } else if (args[i].startsWith("--typeinfo=")) {
  	typeinfoFile = args[i].substring(11);
        } else if (args[i].equals("--rapid")) {
  	rapidFile = coreName + ".sys";
        } else if (i == args.length - 1) {
  	fileName = args[i];
        } else {
  	System.err.println(" Unknown argument " + args[i]);
  	print_help();
  	System.exit(2);
        }
      }
     if(prefix==null){
  	System.err.println("   WARNING! prefix==null");
        prefix="";
     }
   }

   Program parseFile(){
     Program ast = null;
     try {
       // Check for errors
       LabCommScanner scanner = new LabCommScanner(
                                  new FileReader(fileName));
       LabCommParser parser = new LabCommParser();
       Program p = (Program)parser.parse(scanner);
       Collection errors = new LinkedList();
       p.errorCheck(errors);
         
       if (errors.isEmpty()) {
         ast = p;
       } else {
         for (Iterator iter = errors.iterator(); iter.hasNext(); ) {
           String s = (String)iter.next();
           System.out.println(s);
         }
       }
     } catch (FileNotFoundException e) {
       System.err.println("Could not find file: " + fileName);
     } catch (IOException e) {
       System.err.println("IOException: " + fileName + " " + e);
     } catch (beaver.Parser.Exception e) {
       System.err.println(e.getMessage());
     }
     return ast;
   }

   boolean generateC(Program ast) {
     boolean wroteFile = false; 
     Vector hIncludes = new Vector(cIncludes);
     if (hFile != null) {
       cIncludes.add(hFile);
     }
     if (cFile != null) {
       printStatus("C: " , cFile);
       genC(ast, cFile, cIncludes, coreName, cPrefix, ver);
       wroteFile = true;
     }
     if (hFile != null) {
       printStatus("H: " , hFile);
       genH(ast, hFile, hIncludes, coreName, cPrefix, ver);
       wroteFile = true;
     }
     return wroteFile;
   }
  
   boolean generateCS(Program ast) {
     boolean wroteFile = false; 
     if (csFile != null) {
       printStatus("C#: " , csFile); 
       genCS(ast, csFile, csNamespace, ver);
       wroteFile = true;
     }
     return wroteFile;
   }
  
   boolean generateJava(Program ast) {
     boolean wroteFile = false; 
     if (javaDir != null) {
       printStatus("Java: " , javaDir);
       genJava(ast, javaDir, javaPackage, ver);
       wroteFile = true;
     }
     return wroteFile;
   }
  
   boolean generatePython(Program ast) {
     boolean wroteFile = false; 
     if (pythonFile != null) {
       printStatus("Python: " , pythonFile); 
       genPython(ast, pythonFile, prefix, ver);
       wroteFile = true;
     }
     return wroteFile;
   }
  
   boolean generateRAPID(Program ast) {
     boolean wroteFile = false; 
     if (rapidFile != null) {
       printStatus("RAPID: " , rapidFile);
       genRAPID(ast, rapidFile, coreName, ver);
       wroteFile = true;
     }
     return wroteFile;
   }
   boolean generatePrettyPrint(Program ast) {
     boolean wroteFile = false; 
     if (prettyFile != null) {
       printStatus("Pretty: " , prettyFile); 
       try {
         FileOutputStream f = new FileOutputStream(prettyFile);
         PrintStream out = new PrintStream(f);
         ast.pp(out);
         out.close();
         wroteFile = true;
       } catch (IOException e) {
         System.err.println("IOException: " + prettyFile + " " + e);
       } 
     }
     return wroteFile;
   }
  
   boolean generateTypeinfo(Program ast) {
     boolean wroteFile = false; 
     if (typeinfoFile != null) {
       printStatus("TypeInfo: " , typeinfoFile); 
       try {
         FileOutputStream f = new FileOutputStream(typeinfoFile);
         PrintStream out = new PrintStream(f);
         ast.C_info(out, cPrefix, ver);
         ast.Java_info(out, ver);
         ast.CS_info(out, csNamespace, ver);
         wroteFile = true;
       } catch (IOException e) {
         System.err.println("IOException: " + typeinfoFile + " " + e);
       }
     }
     return wroteFile;
    }

    private void printStatus(String kind, String filename){
       if (verbose) { 
         System.err.println("Generating "+kind+": " + filename); 
       }
    }
  }


  public static void main(String[] args) {
    Opts opts = new Opts(args);
    if(!opts.processFilename()) {
      print_help();
      System.exit(1);
    } else {
      opts.processArgs();
      Program ast =  opts.parseFile();

      if (ast != null) {
	
	boolean fileWritten = false;

        fileWritten |= opts.generateC(ast);
        fileWritten |= opts.generateCS(ast);
        fileWritten |= opts.generateJava(ast);
        fileWritten |= opts.generatePython(ast);
        fileWritten |= opts.generateRAPID(ast);
        fileWritten |= opts.generatePrettyPrint(ast);
        fileWritten |= opts.generateTypeinfo(ast);

        // if no output to files, prettyprint on stdout
	if (!fileWritten) {
	  ast.pp(System.out);
	}
      } else {
          // Catch-all for compilation errors
          System.err.println("Error in specification");
          System.exit(3);
      }
    }
  } 
}
