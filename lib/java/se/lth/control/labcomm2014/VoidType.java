package se.lth.control.labcomm2014;
import java.io.PrintStream;

public class VoidType extends SigStructType{
    public VoidType() {
        super("void");
    }

    public void print(PrintStream out, String indent) {
        out.print("void");
    }
}
