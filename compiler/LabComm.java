import java.util.Vector;

public class LabComm {

  public static void main(String[] args) throws Exception {
    String ver = null;
    Vector<String> outargs = new Vector<String>();
    
    for (String s: args) {
      if (s.startsWith("--ver=")) {
        String newver = s.substring(6);
        if (ver != null && !ver.equals(newver)) {
          throw new Exception("Mismatching versions '" + ver +
                              "' != '" + newver);
          
        }
        ver = newver;
      } else {
        outargs.add(s);
      }
    }
    for (String s: outargs) {
      System.out.println(s);
    }
    if (ver != null && ver.equals("2006")) {
      outargs.add(0, "--ver=2006");
      se.lth.control.labcomm2006.compiler.LabComm.main(outargs.toArray(
                                                         new String[0]));
    } else if (ver == null || ver.equals("2014")) {
      outargs.add(0, "--ver=2014");
      se.lth.control.labcomm2014.compiler.LabComm.main(outargs.toArray(
                                                         new String[0]));
    } 
  }

}
